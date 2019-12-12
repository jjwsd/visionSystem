#include "ImageProcessWorker.h"

#include <QDebug>

#include <time.h>

ImageProcessWorker::ImageProcessWorker(QObject *parent) : QObject(parent)
{
    m_pWorkerModule = nullptr;
}

#include <QFile>
#include <QDateTime>

void ImageProcessWorker::doProcess(_MatImg mat)
{
    clock_t start;
    clock_t end;

    _MatImg dispImg;
    if(m_pWorkerModule != nullptr)
    {
        start = clock();
        CVisionAgentResult result = m_pWorkerModule->RunVision(mat,dispImg);
        end = clock();
        result.dTaktTime = ((double)(end - start) / CLOCKS_PER_SEC);
        if(result.dTaktTime > 0.6)
        {
            result.dTaktTime -= 0.6;
        }
        emit resultReady(dispImg, result);
    }
}

void ImageProcessWorker::doTestProcess(_MatImg mat)
{
    clock_t start;
    clock_t end;

    _MatImg dispImg;
    if(m_pWorkerModule != nullptr)
    {
        start = clock();
        CVisionAgentResult result = m_pWorkerModule->RunVision(mat,dispImg);
        end = clock();
        result.dTaktTime = (double)(end - start) / CLOCKS_PER_SEC;
        emit sendTestResultReady(dispImg, result);
    }
}


