# escpos-qt
Qt library for sending ESC/POS commands to an ESC/POS (termal) printer

Supports several formatting options via a streaming API:
 * Print encoding with QString codec conversion
 * Print Mode
 * Justification
 * HRI positioning
 * Reset
 * Line feed
 * QRCode printing

## Example Usage
```c++
    // Create a buffer to write the commands
    // an QIODevice can also be used directly but some bad printers might not behave well
    // when receiving partial commands
    QBuffer b;
    b.open(QBuffer::ReadWrite);
    EscPosPrinter p(&b);
    
    // Init(reset) the printer and set some encoding
    p << EscPosPrinter::init << EscPosPrinter::EncodingPC850;
    
    // Print some text with some formatting options, if a plain string "foo" is sent
    // it won't be handled by QCodec, it will send as raw data.
    p << EscPosPrinter::PrintModes(EscPosPrinter::PrintModeDoubleWidth | EscPosPrinter::PrintModeDoubleHeight | EscPosPrinter::PrintModeEmphasized)
      << EscPosPrinter::JustificationCenter
      << QStringLiteral("Some Text");

    p << "\n";

    // Printing QRCodes
    p << EscPosPrinter::JustificationCenter << EscPosPrinter::PrintModes(EscPosPrinter::PrintModeNone)
      << EscPosPrinter::QRCode(EscPosPrinter::QRCode::Model2, 5, EscPosPrinter::QRCode::M, "https://github.com/ceciletti/escpos-qt") << "\n"
      << EscPosPrinter::JustificationLeft;

    // Send data to printer
    ioDevice->write(b.data());
```
