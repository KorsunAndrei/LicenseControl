#include "licensecontrol.h"
#include <QApplication>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   Q_UNUSED(context);

   QString dt = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss");
   QString txt = QString("[%1] ").arg(dt);

   switch (type)
   {
      case QtDebugMsg:
         txt += QString("{Debug} \t\t %1").arg(msg);
         break;
      case QtWarningMsg:
         txt += QString("{Warning} \t %1").arg(msg);
         break;
      case QtCriticalMsg:
         txt += QString("{Critical} \t %1").arg(msg);
         break;
      case QtFatalMsg:
         txt += QString("{Fatal} \t\t %1").arg(msg);
         abort();
         break;
      case QtInfoMsg:
         txt += QString("{Info} \t\t %1").arg(msg);
         break;
   }

   QFile outFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/LicenseControl.log");
   outFile.open(QIODevice::WriteOnly | QIODevice::Append);
   QTextStream textStream(&outFile);
   textStream << txt << endl;
}

int main(int argc, char *argv[])
{
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("imageformats");
    paths.append("platforms");
    paths.append("sqldrivers");
    QCoreApplication::setLibraryPaths(paths);
    QApplication a(argc, argv);
    QString translatorFileName = QLatin1String("translations/qt_ru");
    QTranslator *translator = new QTranslator(&a);
    if (translator->load(translatorFileName, a.applicationDirPath()))
         a.installTranslator(translator);
    qInstallMessageHandler(customMessageHandler);
    a.setQuitOnLastWindowClosed(false);
    LicenseControl * LC = new LicenseControl(&a);
    LC->showTrayIcon();
    return a.exec();
}

