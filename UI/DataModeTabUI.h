#ifndef DATAMODETABUI_H
#define DATAMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>

class DataModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit DataModeTabUI(QObject *parent = 0);

signals:

public slots:
};

#endif // DATAMODETABUI_H
