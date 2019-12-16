#ifndef TEACHMODETABUI_H
#define TEACHMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>

class TeachModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit TeachModeTabUI(QObject *parent = 0);

signals:

public slots:
};

#endif // TEACHMODETABUI_H
