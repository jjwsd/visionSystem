#include "UAWorker.h"

#include <QDebug>

UAWorker * UAWorker::m_pWokerThread = nullptr;

UAWorker::UAWorker(QObject *parent) : QObject(parent)
{
    m_pOpcua = nullptr;
    m_pOpcua = new COpcUa;
    UAWorker::m_pWokerThread = this;
}

void UAWorker::pollingCallback(UA_Server *server, void *data)
{
    //if(b_running == UA_TRUE)
    //{
        //polling 하며 trigger_request, trigger_response 값 확인
        UA_Int32 request = COpcUa::ua_read_node_int32("trigger_request");
        UA_Int32 response = COpcUa::ua_read_node_int32("trigger_response");
        //_MatImg img;
        //Robot 이 trigger_request 요청
        if(request == 1 && response == 0)
        {           
           emit UAWorker::m_pWokerThread->send_event_by_polling();
           request = 0;
           response = 0;
        }
        //}
}

void UAWorker::cbSendData(_MatImg mat, CVisionAgentResult result)
{
    if(m_pOpcua->server != nullptr)
    {
        cv::Point2f tempPT;
        double tempAngle;
        result.GetCenterPoint(tempPT);
        result.GetAngle(tempAngle);
        QString x = QString::number(tempPT.x,'f',2);
        QString y = QString::number(tempPT.y,'f',2);
        QString a = QString::number(tempAngle,'f',2);

        m_pOpcua->ua_update_node_int32("trigger_request", 0);
        m_pOpcua->ua_update_node_double("position_x", x.toDouble());
        m_pOpcua->ua_update_node_double("position_y", y.toDouble());
        m_pOpcua->ua_update_node_double("theta", a.toDouble());

        m_pOpcua->ua_update_node_int32("trigger_response",1);
    }
}


void UAWorker::startServerSeq()
{
    m_pOpcua->serverSequnece(pollingCallback);
}


