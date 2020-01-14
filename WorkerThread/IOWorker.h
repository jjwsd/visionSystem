#ifndef IOWORKER_H
#define IOWORKER_H

//Qt Header
#include <QObject>

//OneBody Header
extern "C"{
    #include <jetsonGPIO.h>
}


class IOWorker : public QObject
{
    Q_OBJECT
public:
    explicit IOWorker(QObject *parent = 0);

signals:
    void send_event_by_polling();

public slots:
    void pollingCallback();

};

#endif
