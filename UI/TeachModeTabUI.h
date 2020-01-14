#ifndef TEACHMODETABUI_H
#define TEACHMODETABUI_H

#include <QObject>
#include <UI/TabUI.h>
#include <Data/definedata.h>
#include <Utility/dragbox.h>
#include <CModelData.h>
#include <VisionModule/cPatternMatchModule.h>
#include "UI/userrectitem.h"

class TeachModeTabUI : public QObject, public TabUI
{
    Q_OBJECT
public:
    explicit TeachModeTabUI(QObject *parent = 0);

    CModelData m_ModelData;
    CPatternMatchModule m_cPatternModule;
    QRect m_Roi;

signals:

public slots:
    //
    void cbTeachLiveBtnClicked();
    void cbTeachGrabBtnClicked();
    void cbTeachImageSaveBtnClicked();
    void cbTeachImageLoadBtnClicked();
    void cbTeachROIShowBtnClicked();
    void cbTeachROICancelBtnClicked();
    void cbTeachSelectAlgoCombo(int value);
    void cbTeachPatternRectShowBtnClicked();
    void cbTeachPatternImageSaveBtnClicked();
    void cbTeachPatternResizeValueChanged(QString rValue);
    void cbTeachThresholdSliderPressed();
    void cbTeachCircleThresholdLowSliderValueChanged(int value);
    void cbTeachCircleThresholdLowSpinValueChanged(int value);
    void cbTeachCircleCreateBtnClicked();
    void cbTeachCircleCancelBtnClicked();
    void cbTeachCircleGetBtnClicked();
    void cbTeachCircleThresholdHighSliderValueChanged(int value);
    void cbTeachCircleThresholdHighSpinValueChanged(int value);
    void cbTeachModelSaveBtnClicked();
    void cbTeachModelLoadBtnClicked();
    void cbTeachModelTestBtnClicked();
    void cbTeachSettingBtnClicked();
    void cbTabChanged();

private:
    CDragBox * m_PatternRect;
    CDragBox * m_RoiRect;

public:
    void InitModelUI();

};

#endif // TEACHMODETABUI_H
