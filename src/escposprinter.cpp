#include "escposprinter.h"

#include <QIODevice>
#include <QDataStream>

#include <QLoggingCategory>

#include <QTextCodec>

Q_LOGGING_CATEGORY(EPP, "esc_pos")

namespace {
static const char ESC = 0x1B;
static const char GS = 0x1D;
}

using namespace EscPosQt;

EscPosPrinter::EscPosPrinter(QIODevice *device, QObject *parent) : QObject(parent)
  , m_device(device)
{
    m_codec = QTextCodec::codecForLocale();
    connect(m_device, &QIODevice::readyRead, this, [=] {
        const QByteArray data = m_device->readAll();
        qCDebug(EPP) << "GOT" << data << data.toHex();
    });
}

EscPosPrinter::EscPosPrinter(QIODevice *device, const QByteArray &codecName, QObject *parent) : QObject(parent)
  , m_device(device)
{
    m_codec = QTextCodec::codecForName(codecName);
    connect(m_device, &QIODevice::readyRead, this, [=] {
        const QByteArray data = m_device->readAll();
        qCDebug(EPP) << "GOT" << data << data.toHex();
    });
}

EscPosPrinter &EscPosPrinter::operator<<(PrintModes bc)
{
    return mode(bc);
}

EscPosPrinter &EscPosPrinter::operator<<(EscPosPrinter::Justification i)
{
    return align(i);
}

EscPosPrinter &EscPosPrinter::operator<<(EscPosPrinter::Encoding i)
{
    return encode(i);
}

EscPosPrinter &EscPosPrinter::operator<<(EscPosPrinter::_feed lines)
{
    return paperFeed(lines._lines);
}

EscPosPrinter &EscPosPrinter::operator<<(const char *s)
{
    qCDebug(EPP) << "char *s" << QByteArray(s);
    write(s, int(strlen(s)));
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const QByteArray &s)
{
    write(s);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const EscPosPrinter::QRCode &qr)
{
    write(qr.data);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const EscPosPrinter::BarCodeA &bc)
{
    write(bc.data);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const EscPosPrinter::BarCodeB &bc)
{
    write(bc.data);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const EscPosPrinter::DarumaBarCode &bc)
{
    write(bc.data);
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(const QString &text)
{
    qCDebug(EPP) << "string" << text << text.toLatin1() << m_codec->fromUnicode(text);
    if (m_codec) {
        write(m_codec->fromUnicode(text));
    } else {
        write(text.toLatin1());
    }
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(QStringView text)
{
    qCDebug(EPP) << "string" << text << text.toLatin1() << m_codec->fromUnicode(text);
    if (m_codec) {
        write(m_codec->fromUnicode(text));
    } else {
        write(text.toLatin1());
    }
    return *this;
}

EscPosPrinter &EscPosPrinter::operator<<(void (*pf)())
{
    if (pf == EscPosPrinter::eol) {
        write("\n", 1);
    } else if (pf == EscPosPrinter::init) {
        return initialize();
    } else if (pf == EscPosPrinter::standardMode) {
        return modeStandard();
    } else if (pf == EscPosPrinter::pageMode) {
        return modePage();
    }
    return *this;
}

void EscPosPrinter::write(const QByteArray &data)
{
    m_device->write(data);
}

void EscPosPrinter::write(const char *data, int size)
{
    m_device->write(data, size);
}

EscPosPrinter &EscPosPrinter::initialize()
{
    const char str[] = { ESC, '@'};
    write(str, sizeof(str));
    return *this;
}

EscPosPrinter &EscPosPrinter::encode(EscPosPrinter::Encoding codec)
{
    switch (codec) {
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
        qCWarning(EPP) << "Could not find a Qt Codec for" << codec;
    }

    const char str[] = { ESC, 't', char(codec)};
    qCDebug(EPP) << "encoding" << codec << QByteArray(str, sizeof(str)).toHex() << m_codec;
    write(str, sizeof(str));
    return *this;
}

EscPosPrinter &EscPosPrinter::mode(EscPosPrinter::PrintModes pm)
{
    qCDebug(EPP) << "print modes" << pm;
    const char str[] = { ESC, '!', char(pm)};
    write(str, sizeof(str));
    return *this;
}

EscPosPrinter &EscPosPrinter::modeStandard()
{
    const char str[] = { ESC, 'L'};
    write(str, sizeof(str));
    return *this;
}

EscPosPrinter &EscPosPrinter::modePage()
{
    const char str[] = { ESC, 'S'};
    write(str, sizeof(str));
    return *this;
}

EscPosPrinter &EscPosPrinter::partialCut()
{
    const char str[] = { ESC, 'm'};
    write(str, sizeof(str));
    return *this;
}

EscPosPrinter &EscPosPrinter::printAndFeedPaper(quint8 n)
{
    const char str[] = { ESC, 'J', char(n)};
    write(str, sizeof(str));
    return *this;
}

EscPosPrinter &EscPosPrinter::align(EscPosPrinter::Justification i)
{
    const char str[] = { ESC, 'a', char(i)};
    qCDebug(EPP) << "justification" << i << QByteArray(str, 3).toHex();
    write(str, 3);// TODO doesn't work on DR700
    return *this;
}

EscPosPrinter &EscPosPrinter::paperFeed(int lines)
{
    const char str[] = { ESC, 'd', char(lines)};
    qCDebug(EPP) << "line feeds" << lines << QByteArray(str, 3).toHex();
    write(str, 3);
    return *this;
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
    const char mT[] = {
        GS, '(', 'k', 0x04, 0x00, 0x31, 0x41, char(model), 0x00};
    data.append(mT, sizeof(mT));


    // Module Size f167
    const char mS[] = {
        GS, '(', 'k', 0x03, 0x00, 0x31, 0x43, char(moduleSize)};
    data.append(mS, sizeof(mS));

    // Error Level f169
    // L = 0, M = 1, Q = 2, H = 3
    const char eL[] = {
        GS, '(', 'k', 0x03, 0x00, 0x31, 0x45, char(erroCorrection)};
    data.append(eL, sizeof(eL));

    // truncate data f180
    int len = _data.length() + 3;// 3 header bytes
    if (len > 7092) {
        len = 7092;
    }

    // data header
    const char dH[] = {
        GS, '(', 'k', char(len), char(len >> 8), 0x31, 0x50, 0x30};
    data.append(dH, sizeof(dH));

    data.append(_data.mid(0, 7092));

    // Print f181
    const char pT[] = {
        GS, '(', 'k', 0x03, 0x00, 0x31, 0x51, 0x30};
    data.append(pT, sizeof(pT));
}

EscPosPrinter::BarCodeB::BarCodeB(System system, const QByteArray &_data)
{
    qCDebug(EPP) << "BarCodeB" << system << _data;
    data.reserve(_data.size() + 4);

    const char code[] = { GS, 'k'};
    data.append(code, sizeof(code));
    data.append(quint8(system));
    data.append(quint8(_data.size()));
    data.append(_data);
}

EscPosPrinter::BarCodeA::BarCodeA(System system, const QByteArray &_data)
{
    qCDebug(EPP) << "BarCodeA" << system << _data;
    data.reserve(_data.size() + 4);

    const char code[] = { GS, 'k'};
    data.append(code, sizeof(code));
    data.append(quint8(system));
    data.append(_data);
    data.append('\x00');
}

EscPosPrinter::DarumaBarCode::DarumaBarCode(System system, quint8 width, quint8 height, bool printHri, const QByteArray &_data)
{
    qCDebug(EPP) << "BarCode128" << system << width << height << printHri << _data;
    data.reserve(data.size() + 7);

    // Imprime código de barras horizontal
    // ESC b n1 n2 n3 n4 s1...sn NULL
    // n1 – tipo do código a ser impresso
    // EAN13       1
    // EAN8        2
    // S2OF5       3
    // I2OF5       4
    // CODE128     5
    // CODE39      6
    // CODE93      7
    // UPC_A       8
    // CODABAR     9
    // MSI         10
    // CODE11      11
    // n2 – largura da barra. De 2 a 5. Se 0, é usado 2.
    // n3 – altura da barra. De 50 a 200. Se 0, é usado 50.
    // n4 – se 1, imprime o código abaixo das barras
    // s1...sn – string contendo o código.
    // EAN-13: 12 dígitos de 0 a 9
    // EAN–8: 7 dígitos de 0 a 9
    // UPC–A: 11 dígitos de 0 a 9

    const char code[] = {ESC, 'b'};
    data.append(code, sizeof(code));
    data.append(quint8(system));
    data.append(width);
    data.append(height);
    data.append(quint8(printHri));
    data.append(_data);
    data.append('\x00');
}
