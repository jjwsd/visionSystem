#ifndef MANUALMODETABUI_H
#define MANUALMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>

class ManualModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit ManualModeTabUI(QObject *parent = 0);

signals:

public slots:
};

#endif // MANUALMODETABUI_H
