#ifndef AUTOMODETABUI_H
#define AUTOMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>

class AutoModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit AutoModeTabUI(QObject *parent = 0);    

signals:

public slots:
    void cbOpenAutoModuleBtnClicked();
    void cbRunAutoModuleBtnClicked();
    void cbSWTriggerBtnClicked();
    void cbSWTriggerBtnEnabled();
};

#endif // AUTOMODETABUI_H
