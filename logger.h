#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QThread>

class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = 0);
    ~Logger();

    enum eLogLevel {
        DEBUG = 0,
        INFO = 1,
        WARN = 2,
        ERROR = 3,
    };

    void d(const QString & tag, const QString & msg);
    void i(const QString & tag, const QString & msg);
    void w(const QString & tag, const QString & msg);
    void e(const QString & tag, const QString & msg);

signals:
    void sigSendMsg(const QtMsgType& type, const QString & tag, const QString & msg);

public slots:

private:
    QThread m_logSaveThread;

};

#endif // LOGGER_H
