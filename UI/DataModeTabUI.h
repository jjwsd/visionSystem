#ifndef DATAMODETABUI_H
#define DATAMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>
#include <WorkerThread/UAWorker.h>


class DataModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit DataModeTabUI(QObject *parent = 0);

signals:

public slots:
    void cbDataConnectBtnClicked();
    void cbDataReadBtnClicked();
    void cbDataWriteBtnClicked();
    void cbDataDisconnectBtnClicked();
    void cbDataLibLoadUserModule();
    void cbDataLibMakeUserModule();
    void cbDataServerCreateBtn();
    void cbDataServerDeleteBtn();

};

#endif // DATAMODETABUI_H
