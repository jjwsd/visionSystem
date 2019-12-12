#ifndef IMAGESAVEWORKER_H
#define IMAGESAVEWORKER_H

#include <QObject>
#include <QPixmap>

class ImageSaveWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImageSaveWorker(QObject *parent = 0);

signals:
    void resultReady(bool bResultOk);

public slots:
    void doSave(QPixmap img, const QString &fileName);
};

#endif // IMAGESAVEWORKER_H
