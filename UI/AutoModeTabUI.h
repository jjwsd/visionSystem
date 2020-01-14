#ifndef AUTOMODETABUI_H
#define AUTOMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>
#include <CModelData.h>

class AutoModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit AutoModeTabUI(QObject *parent = 0);    

    CModelData m_ModelData;

signals:

public slots:
    void cbOpenAutoModuleBtnClicked();
    void cbRunAutoModuleBtnClicked();
    void cbSWTriggerBtnClicked();
    void cbSWTriggerBtnEnabled();
};

#endif // AUTOMODETABUI_H
