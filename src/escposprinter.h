#ifndef ESCPOSPRINTER_H
#define ESCPOSPRINTER_H

#include <QObject>
#include <escposexports.h>

class QIODevice;
class QTextCodec;

namespace EscPosQt {

class ESC_POS_QT_EXPORT EscPosPrinter : public QObject
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

    struct BarCodeA {
        enum System {
            UpcA = 0x00,
            UpcB = 0x01,
            Jan13 = 0x02,
            Jan8 = 0x03,
            Code39 = 0x04,
            Itf = 0x05,
            Codabar = 0x06,
        };
        BarCodeA(EscPosPrinter::BarCodeA::System system, const QByteArray &_data);

        QByteArray data;
    };

    struct BarCodeB {
        enum System {
            UpcA = 0x41,
            UpcB = 0x42,
            Jan13 = 0x43,
            Jan8 = 0x44,
            Code39 = 0x45,
            Itf = 0x46,
            Codabar = 0x47,
            Code93 = 0x48,
            Code128 = 0x49,
            GS1_128 = 0x4A,
            GS1_DataBar_Omnidirectional = 0x4B,
            GS1_DataBar_Truncated = 0x4C,
            GS1_DataBar_Limited = 0x4D,
            GS1_DataBar_Expanded = 0x4E,
            Code128Auto = 0x4F,
        };
        BarCodeB(EscPosPrinter::BarCodeB::System system, const QByteArray &_data);

        QByteArray data;
    };

    /**
     * @brief Old termal printers non-standard...
     *
     * Beaware that sending justification commands will demand a printer reboot
     *
     * HRI stands for Human Readable Interpretation characters
     * width range 2-5
     * height range 50-200
     */
    struct DarumaBarCode {
        enum System {
            EAN13 = 1,
            EAN8,
            S2OF5,
            I2OF5,
            Code128,
            Code39,
            Code93,
            UPC_A,
            CODABAR,
            MSI,
            Code11,
        };
        DarumaBarCode(EscPosPrinter::DarumaBarCode::System system, quint8 width, quint8 height, bool printHri, const QByteArray &_data);

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
    EscPosPrinter &operator<<(const BarCodeA &bc);
    EscPosPrinter &operator<<(const BarCodeB &bc);
    EscPosPrinter &operator<<(const DarumaBarCode &bc);
    /*!
     * The UTF-8 string will be encoded with QTextCodec
     * if one of the Qt supported encodings is selected.
     */
    EscPosPrinter &operator<<(const QString &text);
    EscPosPrinter &operator<<(QStringView text);
    EscPosPrinter &operator<<(void (*pf) ());

    static void init() {}
    static void eol() {}
    static void standardMode() {}
    static void pageMode() {}

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

public Q_SLOTS:
    void getStatus();

private:
    QTextCodec *m_codec = nullptr;
    QIODevice *m_device;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(EscPosQt::EscPosPrinter::PrintModes)

#endif // ESCPOSPRINTER_H
