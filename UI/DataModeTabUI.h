#ifndef DATAMODETABUI_H
#define DATAMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>
#include <WorkerThread/UAWorker.h>
extern "C"
{
    #include <jetsonGPIO.h>
}

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
    void cbDataOpcuaSelected(int index);
    void cbDataOutput1ONBtnClicked();
    void cbDataOutput1OFFBtnClicked();
    void cbDataOutput2ONBtnClicked();
    void cbDataOutput2OFFBtnClicked();
    void cbDataOutput3ONBtnClicked();
    void cbDataOutput3OFFBtnClicked();
};

#endif // DATAMODETABUI_H
