#ifndef IMAGEPROCESSWORKER_H
#define IMAGEPROCESSWORKER_H

//Qt Header
#include <QObject>

//OneBody Header
#include <VisionModule/CVisionModule.h>
#include <Data/definedata.h>

class ImageProcessWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessWorker(QObject *parent = 0);
    CVisionModule * m_pWorkerModule;

signals:
    void resultReady(_MatImg mat, CVisionAgentResult result);
    void sendTestResultReady(_MatImg mat, CVisionAgentResult result);

public slots:
    void doProcess(_MatImg mat);
    void doTestProcess(_MatImg mat);

};

#endif // IMAGEPROCESSWORKER_H
