#ifndef ESCPOSPRINTER_H
#define ESCPOSPRINTER_H

#include <QObject>

class QIODevice;
class EscPosPrinter : public QObject
{
    Q_OBJECT
public:
    explicit EscPosPrinter(QIODevice *device, QObject *parent = nullptr);
    explicit EscPosPrinter(QIODevice *device, const QByteArray &codecName, QObject *parent = nullptr);

    struct QRCode {
        enum Model {
            Model1 = 49,
            Model2,
            MicroQRCode,
        };
        enum ErrorCorrection {
            L = 48, // 7%
            M, // 15%
            Q, // 25%
            H, // 30%
        };
        QRCode(EscPosPrinter::QRCode::Model model, int moduleSize, EscPosPrinter::QRCode::ErrorCorrection erroCorrection, const QByteArray &_data);

        QByteArray data;
    };

    enum PrintMode {
        PrintModeNone = 0x00, // 32char on mini, 48 on 80mm
        PrintModeFont2 = 0x01,
        PrintModeEmphasized = 0x08,
        PrintModeDoubleHeight = 0x10,
        PrintModeDoubleWidth = 0x20, // 16char on mini, 24 on 80mm
        PrintModeUnderline = 0x80,
    };
    Q_ENUM(PrintMode)
    Q_DECLARE_FLAGS(PrintModes, PrintMode)

    enum Justification {
        JustificationLeft = 0x30,
        JustificationCenter = 0x31,
        JustificationRight = 0x32,
    };
    Q_ENUM(Justification)

    enum HriPosition {
        HriNotPrinted = 0x00,
        HriNotAbove = 0x01,
        HriNotBelow = 0x02,
        HriNotAboveAndBelow = 0x03,
    };
    Q_ENUM(HriPosition)

    enum Encoding {
        EncodingPC437 = 0,
        EncodingKatakana = 1,
        EncodingPC850 = 2,// Qt supported
        EncodingPC860 = 3,
        EncodingPC866 = 17,// Qt supported
        EncodingPC852 = 18,
        EncodingISO8859_2 = 39,// Qt supported
        EncodingISO8859_15 = 40,// Qt supported
    };
    Q_ENUM(Encoding)

    struct _feed { int _lines; };
    inline static _feed feed(int __lines) { return { __lines }; }

    EscPosPrinter &operator<<(PrintModes i);
    EscPosPrinter &operator<<(Justification i);
    EscPosPrinter &operator<<(Encoding i);
    EscPosPrinter &operator<<(_feed lines);
    EscPosPrinter &operator<<(const char *s);
    EscPosPrinter &operator<<(const QByteArray &s);
    EscPosPrinter &operator<<(const QRCode &qr);
    /*!
     * The UTF-8 string will be encoded with QTextCodec
     * if one of the Qt supported encodings is selected.
     */
    EscPosPrinter &operator<<(const QString &s);
    EscPosPrinter &operator<<(void (*pf) ());

    static void init() {}
    static void eol() {}
    static void standardMode() {}
    static void pageMode() {}

    void debug();

    void write(const QByteArray &data);
    void write(const char *data, int size);

    EscPosPrinter &initialize();
    EscPosPrinter &encode(Encoding codec);
    EscPosPrinter &mode(PrintModes pm);
    EscPosPrinter &modeStandard();
    EscPosPrinter &modePage();
    EscPosPrinter &partialCut();
    EscPosPrinter &printAndFeedPaper(quint8 n = 1);
    EscPosPrinter &align(Justification i);
    EscPosPrinter &paperFeed(int lines = 1);

    EscPosPrinter &text(const QString &text);
    EscPosPrinter &raw(const QByteArray &data);
    EscPosPrinter &raw(const char *s);
    EscPosPrinter &raw(const char *s, int size);

    EscPosPrinter &qr(const QRCode &code);

public Q_SLOTS:
    void getStatus();

private:
    QByteArray m_buffer;
    QTextCodec *m_codec = nullptr;
    QIODevice *m_device;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(EscPosPrinter::PrintModes)

#endif // ESCPOSPRINTER_H
