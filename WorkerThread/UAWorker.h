#ifndef UAWORKER_H
#define UAWORKER_H

//Qt Header
#include <QObject>

//OneBody Header
#include <COpcUa.h>
#include <VisionModule/CVisionModule.h>
#include <Data/definedata.h>

class COpcUa;

class UAWorker : public QObject
{
    Q_OBJECT
public:
    explicit UAWorker(QObject *parent = 0);
    COpcUa * m_pOpcua;

    static UAWorker * m_pWokerThread;


signals:
    void send_event_by_polling();

public slots:
    static void pollingCallback(UA_Server *server, void *data);
    void cbSendData(double xPos, double Angle);
    void startServerSeq();
};

#endif
