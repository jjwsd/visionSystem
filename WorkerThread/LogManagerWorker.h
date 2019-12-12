#ifndef LOGMANAGERWORKER_H
#define LOGMANAGERWORKER_H

#include <QObject>

class LogManagerWorker : public QObject
{
    Q_OBJECT
public:
    explicit LogManagerWorker(QObject *parent = 0);

signals:

public slots:
    void saveLog(const QtMsgType& type, const QString & tag, const QString & msg);
};

#endif // LOGMANAGERWORKER_H
