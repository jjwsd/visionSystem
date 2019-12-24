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
    m_pOpcua->ua_update_node_double("position_x",result.centerPt.x);
    m_pOpcua->ua_update_node_double("position_y",result.centerPt.y);
    m_pOpcua->ua_update_node_double("theta", result.dAngle);

    m_pOpcua->ua_update_node_int32("trigger_response",1);
}


void UAWorker::startServerSeq()
{
    m_pOpcua->serverSequnece(pollingCallback);
}


