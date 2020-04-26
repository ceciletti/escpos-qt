#include "escposprinter.h"

#include <QIODevice>
#include <QDataStream>

#include <QLoggingCategory>

#include <QTextCodec>

Q_LOGGING_CATEGORY(EPP, "esc_pos")

static const char ESC = 0x1B;
static const char GS = 0x1D;

EscPosPrinter::EscPosPrinter(QIODevice *device, QObject *parent) : QObject(parent)
  , m_device(device)
{
    m_codec = QTextCodec::codecForLocale();
    connect(m_device, &QIODevice::readyRead, this, [=] {
        const QByteArray data = m_device->readAll();
        qCDebug(EPP) << "GOT" << data << data.toHex();
    });
}

EscPosPrinter &EscPosPrinter::operator<<(PrintModes i)
{
    qCDebug(EPP) << "print modes" << i;
    const char str[3] = { ESC, '!', char(i)};
    write(str, 3);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(EscPosPrinter::Justification i)
{
    const char str[3] = { ESC, 'a', char(i)};
    qCDebug(EPP) << "justification" << i << QByteArray(str, 3).toHex();
    write(str, 3);// TODO doesn't work on DR700
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(EscPosPrinter::Encoding i)
{
    switch (i) {
    case EncodingPC850:
        m_codec = QTextCodec::codecForName("IBM 850");
        break;
    case EncodingPC866:
        m_codec = QTextCodec::codecForName("IBM 866");
        break;
    case EncodingISO8859_2:
        m_codec = QTextCodec::codecForName("ISO8859-2");
        break;
    case EncodingISO8859_15:
        m_codec = QTextCodec::codecForName("ISO8859-15");
        break;
    default:
        m_codec = nullptr;
    }

    if (!m_codec) {
        qCWarning(EPP) << "Could not find a Qt Codec for" << i;
    }

    const char str[3] = { ESC, 't', char(i)};
    qCDebug(EPP) << "encoding" << i << QByteArray(str, 3).toHex() << m_codec;
    write(str, 3);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(EscPosPrinter::_feed lines)
{
    const char str[3] = { ESC, 'd', char(lines._lines)};
    qCDebug(EPP) << "line feeds" << lines._lines << QByteArray(str, 3).toHex();
    write(str, 3);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const char *s)
{
    qCDebug(EPP) << "char *s" << QByteArray(s);
    write(s, int(strlen(s)));
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const QByteArray &s)
{
    qCDebug(EPP) << "bytearray" << s;
    write(s);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const EscPosPrinter::QRCode &qr)
{
    write(qr.data);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const QString &s)
{
    qCDebug(EPP) << "string" << s << s.toLatin1() << m_codec->fromUnicode(s);
    if (m_codec) {
        write(m_codec->fromUnicode(s));
    } else {
        write(s.toLatin1());
    }
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(void (*pf)())
{
    if (pf == EscPosPrinter::eol) {
        write("\n", 1);
    } else if (pf == EscPosPrinter::init) {
        write("\x1B\x40", 2);// ESC @
    } else if (pf == EscPosPrinter::standardMode) {
        write("\x1B\x53", 2);// ESC L
    } else if (pf == EscPosPrinter::pageMode) {
        write("\x1B\x4C", 2);// ESC S
    }
    return *this;
}

void EscPosPrinter::debug()
{
    qCDebug(EPP) << "DEBUG" << m_buffer;
}

void EscPosPrinter::write(const QByteArray &data)
{
    m_device->write(data);
}

void EscPosPrinter::write(const char *data, int size)
{
    m_device->write(data, size);
}

void EscPosPrinter::getStatus()
{

}

EscPosPrinter::QRCode::QRCode(EscPosPrinter::QRCode::Model model, int moduleSize, EscPosPrinter::QRCode::ErrorCorrection erroCorrection, const QByteArray &_data)
{
    qCDebug(EPP) << "QRCode" << model << moduleSize << erroCorrection << _data;

    // Model f165
    // 49 - model1
    // 50 - model2
    // 51 - micro qr code
    const char mT[9] = {
        GS, '(', 'k', 0x04, 0x00, 0x31, 0x41, char(model), 0x00};
    data.append(mT, 9);


    // Module Size f167
    const char mS[8] = {
        GS, '(', 'k', 0x03, 0x00, 0x31, 0x43, char(moduleSize)};
    data.append(mS, 8);

    // Error Level f169
    // L = 0, M = 1, Q = 2, H = 3
    const char eL[8] = {
        GS, '(', 'k', 0x03, 0x00, 0x31, 0x45, char(erroCorrection)};
    data.append(eL, 8);

    // truncate data f180
    int len = _data.length() + 3;// 3 header bytes
    if (len > 7092) {
        len = 7092;
    }

    // data header
    const char dH[8] = {
        GS, '(', 'k', char(len), char(len >> 8), 0x31, 0x50, 0x30};
    data.append(dH, 8);

    data.append(_data.mid(0, 7092));

    // Print f181
    const char pT[8] = {
        GS, '(', 'k', 0x03, 0x00, 0x31, 0x51, 0x30};
    data.append(pT, 8);
}
