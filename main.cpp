#include "OneBody.h"
#include <QApplication>
#include <QDateTime>
#include <QTextStream>

const QString g_strLogPath("/tmp/onebody_systemlog/");

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QFile file(g_strLogPath + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh") + "hour_SystemLog.txt");
    if(!file.open(QIODevice::Append | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);
    QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
        out << "[Debug]'" << curtime << "'" << msg << "\n";
        break;
    case QtInfoMsg:
        out << "[Info]'" << curtime << "'"<< msg << "\n";
        break;
    case QtWarningMsg:
        out << "[Warning]'" << curtime << "'"<< msg << "\n";
        break;
    case QtCriticalMsg:
        out << "[Critical]'" << curtime << "'"<< msg << "\n";
        break;
    case QtFatalMsg:
        out << "[Fatal]'" << curtime << "'"<< msg << " :("<< context.file << ")" << context.line << context.function << "\n";
        abort();
    }
}


int main(int argc, char *argv[])
{
    QDir dir(g_strLogPath);
    if(!dir.exists())
    {
        dir.mkpath(g_strLogPath);
    }
    qInstallMessageHandler(myMessageOutput);

    QApplication a(argc, argv);
    OneBody w;
    w.setMaximumSize(1920,1080);
    w.show();

    return a.exec();
}
