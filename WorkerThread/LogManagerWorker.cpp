#include "LogManagerWorker.h"

#include <QString>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

LogManagerWorker::LogManagerWorker(QObject *parent) : QObject(parent)
{

}

void LogManagerWorker::saveLog(const QtMsgType &type, const QString &tag, const QString &msg)
{
    QFile file("/tmp/log.txt");
    if(!file.open(QIODevice::Append | QIODevice::Text))
    {
        return;
    }

    QTextStream out(&file);
    QString curtime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    //QString strMsg;
    switch (type) {
    case QtDebugMsg:        
        out << "[Debug]" << curtime << " : "<< tag << " | " << msg << "\n";
        break;
    case QtInfoMsg:
        out << "[Info]" << curtime << " : "<< tag << " | " << msg << "\n";
        break;
    case QtWarningMsg:
        out << "[Warning]" << curtime << " : "<< tag << " | " << msg << "\n";
        break;
    case QtCriticalMsg:
        out << "[Critical]" << curtime << " : "<< tag << " | " << msg << "\n";
        break;
    case QtFatalMsg:
        out << "[Fatal]" << curtime << " : "<< tag << " | " << msg << "\n";
        abort();
    }
}

