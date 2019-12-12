#include "logger.h"

#include <WorkerThread/LogManagerWorker.h>

Logger::Logger(QObject *parent) : QObject(parent)
{
    //LogManagerWorker * worker = new LogManagerWorker();
    //worker->moveToThread(&m_logSaveThread);
    //QObject::connect(this, &Logger::sigSendMsg, worker, &LogManagerWorker::saveLog);
    //m_logSaveThread.start();
}

Logger::~Logger()
{
    //m_logSaveThread.quit();
    //m_logSaveThread.wait();
}

void Logger::d(const QString &tag, const QString &msg)
{
    emit sigSendMsg(QtDebugMsg, tag, msg);
}

void Logger::i(const QString &tag, const QString &msg)
{
    emit sigSendMsg(QtInfoMsg, tag, msg);
}

void Logger::w(const QString &tag, const QString &msg)
{
    emit sigSendMsg(QtWarningMsg, tag, msg);
}

void Logger::e(const QString &tag, const QString &msg)
{
    emit sigSendMsg(QtFatalMsg, tag, msg);
}

