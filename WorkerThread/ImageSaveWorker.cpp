#include "ImageSaveWorker.h"

#include <QImage>
#include <QPixmap>

ImageSaveWorker::ImageSaveWorker(QObject *parent) : QObject(parent)
{

}

void ImageSaveWorker::doSave(QPixmap img, const QString &fileName) {
    bool bIsOkay = false;
    if(fileName.contains(".png", Qt::CaseInsensitive))
    {
        bIsOkay = img.save(fileName,"png");
    }
    else
    {
        bIsOkay = img.save(fileName+".png","png");
    }
    emit resultReady(bIsOkay);
}

