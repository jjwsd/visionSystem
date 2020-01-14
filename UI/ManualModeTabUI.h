#ifndef MANUALMODETABUI_H
#define MANUALMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>
#include <CModelData.h>

class ManualModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit ManualModeTabUI(QObject *parent = 0);

    CModelData m_ModelData;

signals:

public slots:
    void cbManualModelLoadBtnClicked();
    void cbManualModelCancelBtnClicked();
    void cbManualImageLoadBtnClicked();
    void cbManualImageClearBtnClicked();
    void cbManualRunBtnClicked();
    void cbManualResultTableCellPressed(int row, int col);

};

#endif // MANUALMODETABUI_H
