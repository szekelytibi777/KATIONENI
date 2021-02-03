
#include <QApplication>
#include <QCommandLineParser>
#include "katitoneni.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QGuiApplication::setApplicationDisplayName(KatitoNeni::tr("Interaktív Játékház"));
	QCommandLineParser commandLineParser;
	commandLineParser.addHelpOption();
	commandLineParser.addPositionalArgument(KatitoNeni::tr("[file]"), KatitoNeni::tr("Image file to open."));
	commandLineParser.process(QCoreApplication::arguments());
	KatitoNeni imageViewer;
	imageViewer.show();
    return a.exec();
}
