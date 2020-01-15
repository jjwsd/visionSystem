#ifndef MANUALMODETABUI_H
#define MANUALMODETABUI_H

#include <QObject>
#include <QPixmap>

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

private:
    void updateModelUI(CModelData modelData);
    void pattern_matching();
    void circle_algorithm();
    void circle_blob_algorithm();
    void rect_algorithm();

    QStringList m_loadImageNames;
    std::vector<QPixmap> m_vManualTabImgList;

};

#endif // MANUALMODETABUI_H
