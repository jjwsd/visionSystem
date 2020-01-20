#include <QPushButton>
#include <QApplication>
#include <QMessageBox>
#include <QIntValidator>
#include <QFileDialog>
#include <QDir>
#include <QListWidget>
#include <QTableWidget>
#include <QDebug>
#include <QPixmap>
#include <QThread>
#include <QTimer>
#include <QTime>

#include <ctime>

#include "OneBody.h"
#include "ui_OneBody.h"
#include "S_Vision/matchingAPI.h"
#include <VisionModule/ccirclemodule.h>
#include <VisionModule/ccircleblobmodule.h>
#include <S_Vision/CImageConverter.h>
#include <patternMatch.h>

#include <WorkerThread/ImageSaveWorker.h>
#include <WorkerThread/ImageProcessWorker.h>
#include <WorkerThread/UAWorker.h>

#define shutterMax 2000000
#define ShutterMin 500

//Init Static
eIMIFormat OneBody::m_eIMIFormat = BAYERGR8;
QMutex * OneBody::m_GrabImg = nullptr;
CameraData * OneBody::pFrameData = nullptr;
bool   OneBody::bIsAutoMode = false;
bool   OneBody::bFirstTimeRun = false;
CAM::CamStreamMode   OneBody::m_eCamStreamMode = CAM::LIVE_STREAM;

OneBody::OneBody(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint),
    ui(new Ui::OneBody), m_pRectRoi(nullptr), pRect(nullptr)
{
    ui->setupUi(this);

    initWorkerThreadConnect();
    initTimer();
    init_graphics_view();
    Init_UI_State();
    initDataTab();
    initLogTab();
    load_settings();

    //this
    QObject::connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    //auto Mode
    QObject::connect(ui->autoBtnOpenVisionModule,SIGNAL(clicked()),&m_AutoModeTab,SLOT(cbOpenAutoModuleBtnClicked()));
    QObject::connect(ui->autoRunStopBtn, SIGNAL(clicked()),&m_AutoModeTab, SLOT(cbRunAutoModuleBtnClicked()));
    QObject::connect(ui->autoTriggerBtn, SIGNAL(clicked()),&m_AutoModeTab, SLOT(cbSWTriggerBtnClicked()));
    QObject::connect(ui->autoTriggerBtn, SIGNAL(clicked()),&m_AutoModeTab, SLOT(cbSWTriggerBtnEnabled()));

    //teach
    QObject::connect(ui->inspLiveBtn,SIGNAL(clicked()),&m_TeachModeTab,SLOT(cbTeachLiveBtnClicked()));
    QObject::connect(ui->inspGripBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachGrabBtnClicked()));
    QObject::connect(ui->inspSaveBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachImageSaveBtnClicked()));
    QObject::connect(ui->teachLoadBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachImageLoadBtnClicked()));
    QObject::connect(ui->inspROIOkBtn,SIGNAL(clicked()),&m_TeachModeTab,SLOT(cbTeachROIShowBtnClicked()));
    QObject::connect(ui->inspROICancelBtn,SIGNAL(clicked()),&m_TeachModeTab,SLOT(cbTeachROICancelBtnClicked()));
    QObject::connect(ui->inspAlgoCombo, SIGNAL(currentIndexChanged(int)), &m_TeachModeTab, SLOT(cbTeachSelectAlgoCombo(int)));
    QObject::connect(ui->inspPatternOKBtn,SIGNAL(clicked()),&m_TeachModeTab,SLOT(cbTeachPatternRectShowBtnClicked()));
    QObject::connect(ui->teachTempSaveBtn,SIGNAL(clicked()),&m_TeachModeTab,SLOT(cbTeachPatternImageSaveBtnClicked()));
    QObject::connect(ui->teachPatternResize, SIGNAL(textChanged(QString)), &m_TeachModeTab, SLOT(cbTeachPatternResizeValueChanged(QString)));
    QObject::connect(ui->teachCircleThreshLowSlider, SIGNAL(sliderPressed()), &m_TeachModeTab, SLOT(cbTeachThresholdSliderPressed()));
    QObject::connect(ui->teachCircleThreshLowSlider, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdLowSliderValueChanged(int)));
    QObject::connect(ui->teachCircleThreshLowSpin, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdLowSpinValueChanged(int)));
    QObject::connect(ui->teachCircleThreshHighSlider, SIGNAL(sliderPressed()), &m_TeachModeTab, SLOT(cbTeachThresholdSliderPressed()));
    QObject::connect(ui->teachCircleThreshHighSlider, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdHighSliderValueChanged(int)));
    QObject::connect(ui->teachCircleThreshHighSpin, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdHighSpinValueChanged(int)));
    QObject::connect(ui->teachCircleCreateBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachCircleCreateBtnClicked()));
    QObject::connect(ui->teachCircleCancelBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachCircleCancelBtnClicked()));
    QObject::connect(ui->teachCircleGetBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachCircleGetBtnClicked()));
    QObject::connect(ui->teachRectThreshLowSlider, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdLowSliderValueChanged(int)));
    QObject::connect(ui->teachRectThreshLowSpin, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdLowSpinValueChanged(int)));
    QObject::connect(ui->teachRectThreshHighSlider, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdHighSliderValueChanged(int)));
    QObject::connect(ui->teachRectThreshHighSpin, SIGNAL(valueChanged(int)), &m_TeachModeTab, SLOT(cbTeachCircleThresholdHighSpinValueChanged(int)));
    QObject::connect(ui->teachFileSaveBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachModelSaveBtnClicked()));
    QObject::connect(ui->teachFileLaodBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachModelLoadBtnClicked()));
    QObject::connect(ui->teachModelTestBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachModelTestBtnClicked()));
    QObject::connect(ui->stackedWidget, SIGNAL(currentChanged(int)), &m_TeachModeTab, SLOT(cbTeachROICancelBtnClicked()));
    QObject::connect(ui->teachSettingBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachSettingBtnClicked()));
    QObject::connect(ui->teachCheckOriginBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachCheckOriginBtnClicked()));


    //teach setting
    QObject::connect(ui->settingWidthSlider, SIGNAL(valueChanged(int)), this, SLOT(set_width_slider(int)));
    QObject::connect(ui->settingHeightSlider, SIGNAL(valueChanged(int)), this, SLOT(set_height_slider(int)));
    QObject::connect(ui->settingOffsetXSlider, SIGNAL(valueChanged(int)), this, SLOT(set_offset_x_slider(int)));
    QObject::connect(ui->settingOffsetYSlider, SIGNAL(valueChanged(int)), this, SLOT(set_offset_y_slider(int)));
    QObject::connect(ui->settingFormSizeApplyBtn, SIGNAL(clicked()), this, SLOT(set_format_size()));
    QObject::connect(ui->settingExpAutoCheck, SIGNAL(stateChanged(int)),this,SLOT(on_exposure_auto_check()));
    QObject::connect(ui->settingGainAutoCheck, SIGNAL(stateChanged(int)),this,SLOT(on_gain_auto_check()));
    QObject::connect(ui->settingExpSlider, SIGNAL(valueChanged(int)), this, SLOT(set_exposure_slider(int)));
    QObject::connect(ui->settingExpSpin,SIGNAL(valueChanged(int)),this,SLOT(set_exposure_spin(int)));
    QObject::connect(ui->settingGainSlider, SIGNAL(valueChanged(int)), this, SLOT(set_gain_slider(int)));
    QObject::connect(ui->settingGainSpin,SIGNAL(valueChanged(int)),this,SLOT(set_gain_spin(int)));
    QObject::connect(ui->settingBackBtn, SIGNAL(clicked()), this, SLOT(settingBackBtnClicked()));

    //manual
    QObject::connect(ui->manualFileLoadBtn, SIGNAL(clicked()), &m_ManualModeTab, SLOT(cbManualModelLoadBtnClicked()));
    QObject::connect(ui->manualFileCancelBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbManualModelCancelBtnClicked()));
    QObject::connect(ui->manualLoadBtn, SIGNAL(clicked()), &m_ManualModeTab, SLOT(cbManualImageLoadBtnClicked()));
    QObject::connect(ui->testFileClearBtn,SIGNAL(clicked()),&m_ManualModeTab,SLOT(cbManualImageClearBtnClicked()));
    QObject::connect(ui->testTestBtn,SIGNAL(clicked()),&m_ManualModeTab,SLOT(cbManualRunBtnClicked()));
    QObject::connect(ui->testResultTable,SIGNAL(cellPressed(int,int)),&m_ManualModeTab, SLOT(cbManualResultTableCellPressed(int, int)));

    //data
    QObject::connect(ui->dataConnectBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataConnectBtnClicked()));
    QObject::connect(ui->dataReadBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataReadBtnClicked()));
    QObject::connect(ui->dataDisconBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataDisconnectBtnClicked()));
    //QObject::connect(ui->dataLibLoadUserModule, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataLibLoadUserModule()));
    //QObject::connect(ui->dataUserLibraryOpenBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataLibLoadUserModule()));
    QObject::connect(ui->dataLibMakeUserModule, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataLibMakeUserModule()));
    QObject::connect(ui->dataCreatetBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataServerCreateBtn()));
    QObject::connect(ui->dataDelete, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataServerDeleteBtn()));
    QObject::connect(ui->listWidget, SIGNAL(currentRowChanged(int)), &m_DataModeTab, SLOT(cbDataOpcuaSelected(int)));
    QObject::connect(ui->dataOutput1ONBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataOutput1ONBtnClicked()));
    QObject::connect(ui->dataOutput1OFFBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataOutput1OFFBtnClicked()));
    QObject::connect(ui->dataOutput2ONBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataOutput2ONBtnClicked()));
    QObject::connect(ui->dataOutput2OFFBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataOutput2OFFBtnClicked()));
    QObject::connect(ui->dataOutput3ONBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataOutput3ONBtnClicked()));
    QObject::connect(ui->dataOutput3OFFBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataOutput3OFFBtnClicked()));

}

OneBody::~OneBody()
{
    releaseWorkerThread();
    releaseCameraResource();
    delete ui;
}

bool OneBody::event(QEvent * e)
{
    if(e->type() == QEvent::WindowActivate && OneBody::bFirstTimeRun == false){
        OneBody::bFirstTimeRun = true;
        qDebug() << QEvent::WindowActivate;
        m_AutoModeTab.SetMainUi(ui);
        m_ManualModeTab.SetMainUi(ui);
        m_TeachModeTab.SetMainUi(ui);
        m_DataModeTab.SetMainUi(ui);
    }
    return QWidget::event(e);
}

void OneBody::Init_UI_State()
{
    //init state setting
    //ui->inspROICombo->addItem("Rectangle");
    //ui->inspPatternCombo->addItem("Rectangle");

    ui->settingExpAutoCheck->setChecked(true);
    ui->settingGainAutoCheck->setChecked(true);
    ui->settingExpSlider->setEnabled(false);
    ui->settingExpSpin->setEnabled(false);
    ui->settingGainSlider->setEnabled(false);
    ui->settingGainSpin->setEnabled(false);
    ui->settingFormatCombo->setCurrentText("RGBx");
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(cam_connect_status()));
    timer->start(1000);

    m_iLowValue=0;
    m_iHighValue=0;
    resultCount = 0;

    ui->runResultTable->resizeColumnsToContents();
    //ui->runResultTable->horizontalHeader()->setStretchLastSection(true);

    ui->tableWidgetAutoModuleList->resizeColumnsToContents();
    ui->tableWidgetAutoModuleList->horizontalHeader()->setStretchLastSection(true);
}

void OneBody::load_settings()
{
    err = NeptuneInit();
    if(err != eNeptuneSuccess)
    {
        qDebug() << "NeptuneInit() == " << err;
    }

    int iNum = 0;
    NeptuneNumberOfCameras(&iNum);

    qDebug() << "Camera Nums = " << iNum;
    if(iNum <= 0 )
    {
        return;
    }

    char tmpChar[100];
    NeptuneGetDisplayName(0, tmpChar);

    std::string strDisplayName(tmpChar);
    qDebug() << "NeptuneGetDisplayName = " << strDisplayName.c_str();

    NeptuneGetCameraID(0, tmpChar);
    strDisplayName = tmpChar;
    qDebug() << "NeptuneGetCameraID = " << strDisplayName.c_str();


    NCamInfo pCamInfo;
    err = NeptuneOpenCamera(tmpChar, &m_CamHandle);
    if(err != eNeptuneSuccess)
    {
        qDebug() << "NeptuneOpenCamera() == " << err;
    }

    NeptuneGetCamInfo(m_CamHandle, &pCamInfo);
    qDebug() << "pCamInfo.strDevcieID = " << pCamInfo.strDevcieID;

    ui->infoCamName->setText(pCamInfo.strModel);
    ui->infoCamSerial->setText(pCamInfo.strDevcieID);

    // pixel format
    int pnNumbers = 0;
    NeptuneGetNumAvailablePixelFormats(m_CamHandle, &pnNumbers);

    for (int i = 0; i < pnNumbers; ++i)
    {
        NeptuneGetAvailablePixelFormatsIdxToStr(m_CamHandle, i, tmpChar);
        QString tmp = QString::fromLocal8Bit(tmpChar);
        if(tmp == "BayerGR8" || tmp == "Mono8")
            ui->settingFormatCombo->addItem(tmpChar);
    }
    NeptuneGetSizeInfo(m_CamHandle, &m_CamSizeInfo);

    pFrameData = new CameraData;
    pFrameData->m_pMainWindow = this;

    NeptuneSetPixelFormatStr(m_CamHandle, "BayerGR8");
    NeptuneSetBayerConversion(m_CamHandle, eNeptuneBayerNearest);
    OneBody::m_eIMIFormat = BAYERGR8;
    ui->settingFormatCombo->setCurrentIndex(1);

    float pfValue;
    NeptuneGetFrameRate(m_CamHandle, &pfValue);
    //ui->settingFrameLabel->setText(QString::number(pfValue));

    err = NeptuneStartAcquisition(m_CamHandle, imi_cam_frame_cb);
    m_bAcqStart = true;
    if(err != eNeptuneSuccess)
    {
        qDebug() << "NeptuneStartAcquisition() == " << err;
    }

    int expMin, expMax;
    NeptuneGetAutoExposureMinMax(m_CamHandle, &expMin, &expMax);
    qDebug() << expMin << ", " << expMax;

    ui->settingExpSlider->setMinimum(expMin);
    ui->settingExpSlider->setMaximum(expMax);
    ui->settingExpSpin->setMinimum(expMin);
    ui->settingExpSpin->setMaximum(expMax);


    int gainMin, gainMax;
    NeptuneGetGainMinMax(m_CamHandle, &gainMin, &gainMax);
    qDebug() << gainMin << ", " << gainMax;

    ui->settingGainSlider->setMinimum(gainMin);
    ui->settingGainSlider->setMaximum(gainMax);
    ui->settingGainSpin->setMinimum(gainMin);
    ui->settingGainSpin->setMaximum(gainMax);

    ui->settingWidthSlider->setMinimum(0);
    ui->settingHeightSlider->setMinimum(0);
    ui->settingOffsetXSlider->setMinimum(0);
    ui->settingOffsetYSlider->setMinimum(0);
    ui->settingWidthSlider->setMaximum(m_CamSizeInfo.maxWidth);
    ui->settingHeightSlider->setMaximum(m_CamSizeInfo.maxHeight);
    ui->settingOffsetXSlider->setMaximum(0);
    ui->settingOffsetYSlider->setMaximum(0);

    //    m_CamSizeInfo.width = 1944;
    //    m_CamSizeInfo.height = 2264;
    //    m_CamSizeInfo.x = 1880;
    //    m_CamSizeInfo.y = 792;

    m_CamSizeInfo.width = 5184;
    m_CamSizeInfo.height = 3888;
    m_CamSizeInfo.x = 0;
    m_CamSizeInfo.y = 0;

    ui->settingWidthSlider->setValue(m_CamSizeInfo.width);
    ui->settingHeightSlider->setValue(m_CamSizeInfo.height);
    ui->settingOffsetXSlider->setValue(m_CamSizeInfo.x);
    ui->settingOffsetYSlider->setValue(m_CamSizeInfo.y);
    ui->settingWidthLabel->setText(QString::number(m_CamSizeInfo.width));
    ui->settingHeightLabel->setText(QString::number(m_CamSizeInfo.height));
    ui->settingOffsetXLabel->setText(QString::number(m_CamSizeInfo.x));
    ui->settingOffsetYLabel->setText(QString::number(m_CamSizeInfo.y));

    set_format_size();
}

void OneBody::set_width_slider(int value)
{
    ui->settingWidthLabel->setText(QString::number(value));
    ui->settingOffsetXSlider->setMaximum(m_CamSizeInfo.maxWidth-value);
}

void OneBody::set_height_slider(int value)
{
    ui->settingHeightLabel->setText(QString::number(value));
    ui->settingOffsetYSlider->setMaximum(m_CamSizeInfo.maxHeight-value);
}

void OneBody::set_offset_x_slider(int value)
{
    ui->settingOffsetXLabel->setText(QString::number(value));
}

void OneBody::set_offset_y_slider(int value)
{
    ui->settingOffsetYLabel->setText(QString::number(value));
}

void OneBody::set_format_size()
{
    setCamStreamMode(CAM::LIVE_STOP);
    NErr status =  eNeptuneSuccess;
    m_bStop = false;
    if(m_bAcqStart == true)
    {
        qDebug() << "NeptuneStopAcquisition";
        NeptuneStopAcquisition(m_CamHandle);
        m_bStop = true;
    }
    //setting size
    NeptuneGetSizeInfo(m_CamHandle, &m_CamSizeInfo);
    m_CamSizeInfo.width = ui->settingWidthLabel->text().toInt();
    m_CamSizeInfo.height = ui->settingHeightLabel->text().toInt();
    m_CamSizeInfo.x = ui->settingOffsetXLabel->text().toInt();
    m_CamSizeInfo.y = ui->settingOffsetYLabel->text().toInt();
    //set Format
    if(ui->settingFormatCombo->currentIndex()==1)
        OneBody::m_eIMIFormat = BAYERGR8;
    else
        OneBody::m_eIMIFormat = MONO8;
    //get FrameRate
    float pfValue;
    NeptuneGetFrameRate(m_CamHandle, &pfValue);
    //ui->settingFrameLabel->setText(QString::number(pfValue));
    //set Size
    status = NeptuneSetSizeInfo(m_CamHandle, m_CamSizeInfo);
    NeptuneGetSizeInfo(m_CamHandle, &m_CamSizeInfo);
    ui->settingWidthLabel->setText(QString("%1").arg(m_CamSizeInfo.width));
    ui->settingHeightLabel->setText(QString("%1").arg(m_CamSizeInfo.height));
    ui->settingOffsetXLabel->setText(QString("%1").arg(m_CamSizeInfo.x));
    ui->settingOffsetYLabel->setText(QString("%1").arg(m_CamSizeInfo.y));
    if(status == eNeptuneSuccess)
    {

    }
    else
        QMessageBox::critical(this, "Error", QString("NeptuneSetSizeInfo error : err[%1]").arg(status));

    if(m_bStop == true){
        pFrameData->m_iFrameCnt = 0;
        setCamStreamMode(CAM::LIVE_STREAM);
        NeptuneStartAcquisition(m_CamHandle, imi_cam_frame_cb);
        qDebug() << "NeptuneStartAcquisition";
    }
}

void OneBody::on_exposure_auto_check()
{
    bool bAuto = ui->settingExpAutoCheck->isChecked();
    unsigned int value;

    if(ui->settingExpAutoCheck->checkState())
    {
        NeptuneSetShutter(m_CamHandle, shutterMax/20, !bAuto);
        NeptuneGetShutter(m_CamHandle, &value, &bAuto);
        qDebug() << "Auto Exp: " << value;
        ui->settingExpSlider->setEnabled(false);
        ui->settingExpSpin->setEnabled(false);
    }
    else
    {
        unsigned int value = ui->settingExpSpin->text().toInt();
        value *= shutterMax/100;
        if(value==0)
            value = ShutterMin;
        NeptuneSetShutter(m_CamHandle, value, bAuto);
        NeptuneGetShutter(m_CamHandle, &value, &bAuto);
        qDebug() << "Auto off Exp: " << value;
        ui->settingExpSlider->setEnabled(true);
        ui->settingExpSpin->setEnabled(true);
    }
}

void OneBody::set_exposure_slider(int value)
{
    unsigned int uValue;
    bool bAuto = ui->settingExpAutoCheck->isChecked();
    uValue = value*(shutterMax/100);
    if(value==0)
        uValue = 500;

    ui->settingExpSpin->setValue(value);
    qDebug() << "uValue: "<<uValue;
    NeptuneSetShutter(m_CamHandle, uValue, bAuto);
    NeptuneGetShutter(m_CamHandle, &uValue, &bAuto);
    qDebug() << "shutter: "<<uValue;
}

void OneBody::set_exposure_spin(int value)
{
    unsigned int uValue;
    bool bAuto = ui->settingExpAutoCheck->isChecked();
    uValue = value*(shutterMax/100);
    if(value==0)
        uValue = 500;

    ui->settingExpSlider->setValue(value);
    qDebug() << "uValue: "<<uValue;
    NeptuneSetShutter(m_CamHandle, uValue, bAuto);
    NeptuneGetShutter(m_CamHandle, &uValue, &bAuto);
    qDebug() << "shutter: "<<uValue;
}

void OneBody::on_gain_auto_check()
{
    bool bAuto = ui->settingGainAutoCheck->isChecked();
    int value;
    if(ui->settingGainAutoCheck->checkState())
    {
        ui->settingGainSlider->setEnabled(false);
        ui->settingGainSpin->setEnabled(false);
        NeptuneSetGain(m_CamHandle, 1920/2, !bAuto);
    }
    else
    {
        value = ui->settingGainSpin->text().toInt();
        ui->settingGainSlider->setEnabled(true);
        ui->settingGainSpin->setEnabled(true);
        NeptuneSetGain(m_CamHandle, value, bAuto);
    }
}

void OneBody::set_gain_slider(int value)
{
    bool bAuto = ui->settingGainAutoCheck->isChecked();
    ui->settingGainSpin->setValue(value);
    NeptuneSetGain(m_CamHandle, value, bAuto);
}

void OneBody::set_gain_spin(int value)
{
    bool bAuto = ui->settingGainAutoCheck->isChecked();
    ui->settingGainSlider->setValue(value);
    NeptuneSetGain(m_CamHandle, value, bAuto);
}

void OneBody::on_clicked_resolution_applyBtn()
{
    m_FrameData->m_iFrameCnt = 0;

    QString form, size, frame;
    form = ui->settingFormatCombo->currentText();

    QRegExp rx("[/]");
    QStringList sizeList = size.split(rx, QString::SkipEmptyParts);
    QStringList frameList = frame.split(rx, QString::SkipEmptyParts);

    int sizeWidth = sizeList.front().toInt(), sizeHeight=sizeList.back().toInt();
    int frameNum = frameList.front().toInt(), frameDen = frameList.back().toInt();

    if(!form.isEmpty() && !sizeList.isEmpty() && !frameList.isEmpty())
    {

    }
}



void OneBody::settingBackBtnClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void OneBody::UpdatePatternModelUI(CModelData modelData)
{
    if(modelData.m_iAlgoType == VISION::PATTERN)
    {
        QString testTemplate = QFileInfo(modelData.m_qsTemplate).fileName();
        QString testTemplatePath = QFileInfo(modelData.m_qsTemplate).path();
        testTemplatePath += "/";

        QPixmap pixmap;
        QPixmap canny;
        pixmap.load(modelData.m_qsTemplate);
        canny.load(testTemplatePath + QString("contour_def_canny_totally.bmp"));        
        ui->manualPatternImage->setPixmap(pixmap);
        ui->manualCannyImage->setPixmap(canny);

        cv::String templateFolder(testTemplatePath.toStdString());
        cv::String templateName(testTemplate.toStdString());
    }
}



void OneBody::automode_triggerBtn_enable()
{
    ui->autoTriggerBtn->setStyleSheet("color: rgb(255, 255, 255);"
                                      "background-color: rgb(40, 0, 140);"
                                      "border-width: 1px;"
                                      "padding: 5px;"
                                      "border-radius: 5px;");
    ui->autoTriggerBtn->setEnabled(true);
}

void OneBody::cbAutoTabGetVisionProcessResult(_MatImg mat, CVisionAgentResult result)
{
    automode_triggerBtn_enable();
    m_lAutoVisionResult.push_back(result);
    if(result.bOk)
    {
        m_pTimerAutoMode->start(2500);
        m_MutexImg.lock();
        m_Pixmap->setPixmap( CImageConverter::cvMatToQPixmap(mat));
        m_MutexImg.unlock();
        ui->graphicsView->scene()->setSceneRect(m_Pixmap->boundingRect());
        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
        //update result at table

        cv::Point2f centerPt;
        double dAngle;
        if(!result.GetCenterPoint(centerPt) || !result.GetAngle(dAngle))
        {

        }
        QString strCPX = QString::number(centerPt.x);
        QString strCPY = QString::number(centerPt.y);
        QString strAngle = QString::number(dAngle* (180/3.14),'f',2);
        QString strTT = QString::number(result.dTaktTime,'f',2);
        //QString strCP = strCPX + ", " + strCPY;
        QPixmap okimg;
        okimg.load(":/images/OK.PNG");

        ui->autoReultImg->setPixmap(okimg);
        ui->autoResultX->setText(strCPX);
        ui->autoResultY->setText(strCPY);
        ui->autoResultAngle->setText(strAngle+" degree");
        ui->autoResultTact->setText(strTT+"s");

        //        QTableWidgetItem *centerPoint = new QTableWidgetItem();
        //        QTableWidgetItem *angle = new QTableWidgetItem();
        //        QTableWidgetItem *tactTime = new QTableWidgetItem();

        //        centerPoint->setText(strCP);
        //        angle->setText(strAngle);
        //        tactTime->setText(strTT);

        //        ui->runResultTable->setRowCount(resultCount+1);
        //        ui->runResultTable->setItem(resultCount,0,centerPoint);
        //        ui->runResultTable->setItem(resultCount,1,angle);
        //        ui->runResultTable->setItem(resultCount,2,tactTime);
        //        ui->runResultTable->resizeColumnsToContents();
        //        resultCount++;
    }
    else
    {
        QPixmap ngimg;
        ngimg.load(":/images/NG.PNG");
        ui->autoReultImg->setPixmap(ngimg);
        setCamStreamMode(CAM::LIVE_STREAM);

        ui->autoResultX->setText("");
        ui->autoResultY->setText("");
        ui->autoResultAngle->setText("");
        ui->autoResultTact->setText("");
    }
}

void OneBody::imi_cam_frame_cb(PNImageFrame pImgFrame)
{
    pFrameData->m_iFrameCnt++;

    if(m_eCamStreamMode != CAM::LIVE_STOP)
    {
        if(OneBody::m_eIMIFormat == BAYERGR8)//static function only use static variable
        {
            OneBody::pFrameData->m_Frame.create(pImgFrame->uiHeight,pImgFrame->uiWidth,CV_8UC(3));
            memcpy( OneBody::pFrameData->m_Frame.data, pImgFrame->pRGBBuf, pImgFrame->uiHeight * pImgFrame->uiWidth * 3);
            cv::flip(OneBody::pFrameData->m_Frame, OneBody::pFrameData->m_Frame, 0);
            if(m_eCamStreamMode == CAM::LIVE_STREAM)
            {
                QImage img((uchar*)OneBody::pFrameData->m_Frame.data,
                           OneBody::pFrameData->m_Frame.cols,
                           OneBody::pFrameData->m_Frame.rows,
                           QImage::Format_RGB888);
                pFrameData->m_pMainWindow->sigUpdateMainViewer(QPixmap::fromImage(img));
                qApp->processEvents();
            }
            else if(m_eCamStreamMode == CAM::CAPTURE)
            {
                OneBody::m_GrabImg->lock();
                OneBody::m_eCamStreamMode = CAM::LIVE_STOP;
                OneBody::m_GrabImg->unlock();
                //cv::cvtColor(OneBody::pFrameData->m_Frame, OneBody::pFrameData->m_Frame, CV_BGR2RGB);
                //pFrameData->m_pMainWindow->sg_send_one_frame_to_ui(OneBody::pFrameData->m_Frame);
                QImage img((uchar*)OneBody::pFrameData->m_Frame.data,
                           OneBody::pFrameData->m_Frame.cols,
                           OneBody::pFrameData->m_Frame.rows,
                           QImage::Format_RGB888);
                pFrameData->m_pMainWindow->sigUpdateMainViewer(QPixmap::fromImage(img));
                qApp->processEvents();
            }
            else if(m_eCamStreamMode == CAM::AUTO_RUN_MODE)
            {
                OneBody::m_GrabImg->lock();
                OneBody::m_eCamStreamMode = CAM::LIVE_STOP;
                OneBody::m_GrabImg->unlock();
                pFrameData->m_pMainWindow->sigSendMatImgToWorkerThread(OneBody::pFrameData->m_Frame);
            }
        }
        else if(OneBody::m_eIMIFormat == MONO8)
        {
            OneBody::pFrameData->m_Frame.create(pImgFrame->uiHeight,pImgFrame->uiWidth,CV_8UC(1));
            memcpy( OneBody::pFrameData->m_Frame.data, pImgFrame->pRawBuf, pImgFrame->uiHeight * pImgFrame->uiWidth * 1);
            cv::flip(OneBody::pFrameData->m_Frame, OneBody::pFrameData->m_Frame, 0);


            if(m_eCamStreamMode == CAM::LIVE_STREAM)
            {
                QImage img((uchar*)OneBody::pFrameData->m_Frame.data,
                           OneBody::pFrameData->m_Frame.cols,
                           OneBody::pFrameData->m_Frame.rows,
                           static_cast<int>(OneBody::pFrameData->m_Frame.step),
                           QImage::Format_RGB888);
                pFrameData->m_pMainWindow->sigUpdateMainViewer(QPixmap::fromImage(img));
                qApp->processEvents();
            }
            else if(m_eCamStreamMode == CAM::CAPTURE)
            {
                OneBody::m_GrabImg->lock();
                OneBody::m_eCamStreamMode = CAM::LIVE_STOP;
                OneBody::m_GrabImg->unlock();
                QImage img((uchar*)OneBody::pFrameData->m_Frame.data,
                           OneBody::pFrameData->m_Frame.cols,
                           OneBody::pFrameData->m_Frame.rows,
                           QImage::Format_RGB888);
                pFrameData->m_pMainWindow->sigUpdateMainViewer(QPixmap::fromImage(img));
                //                cv::cvtColor(OneBody::pFrameData->m_Frame, OneBody::pFrameData->m_Frame, CV_GRAY2RGB);
                //                pFrameData->m_pMainWindow->sigSendMatImgToWorkerThread(OneBody::pFrameData->m_Frame);
                qApp->processEvents();
            }
        }
    }
}

Ui::OneBody *OneBody::GetUI() const
{
    return ui;
}

void OneBody::init_graphics_view()
{
    m_Pixmap = new QGraphicsPixmapItem();

    ui->graphicsView->setScene(new QGraphicsScene(this));
    ui->graphicsView->scene()->addItem(m_Pixmap);
    //ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    m_FrameData = new CameraData();
    m_FrameData->m_pMainWindow = this;

    QObject::connect(this, SIGNAL(sigUpdateMainViewer(QPixmap)),
                     this, SLOT(cbUpdateMainViewer(QPixmap)), Qt::QueuedConnection);

    m_GrabImg = new QMutex();
}

#include <WorkerThread/LogManagerWorker.h>

void OneBody::initWorkerThreadConnect()
{
    qRegisterMetaType<_MatImg>("_MatImg");
    qRegisterMetaType<_VisionResult>("CVisionAgentResult");

    ImageSaveWorker *imgSaveWorker = new ImageSaveWorker();
    imgSaveWorker->moveToThread(&m_imgSaveThread);
    connect(&m_imgSaveThread, &QThread::finished, imgSaveWorker, &QObject::deleteLater);
    connect(this, &OneBody::sigSaveImgToWorkerThread, imgSaveWorker, &ImageSaveWorker::doSave);
    connect(imgSaveWorker, &ImageSaveWorker::resultReady, this, &OneBody::teachSaveImageResult);
    m_imgSaveThread.start();

    m_ImgProcessWorker = new ImageProcessWorker();
    m_ImgProcessWorker->moveToThread(&m_imgProcThread);
    connect(&m_imgProcThread, &QThread::finished, m_ImgProcessWorker, &QObject::deleteLater);
    connect(this, &OneBody::sigSendMatImgToWorkerThread, m_ImgProcessWorker, &ImageProcessWorker::doProcess);
    connect(m_ImgProcessWorker, &ImageProcessWorker::resultReady, this, &OneBody::cbAutoTabGetVisionProcessResult);

    connect(this, &OneBody::sigSendTestMatImgToWorkerThread, m_ImgProcessWorker, &ImageProcessWorker::doTestProcess);
    connect(m_ImgProcessWorker, &ImageProcessWorker::sendTestResultReady, this, &OneBody::cbAutoTabGetVisionProcessResult);
    m_imgProcThread.start();

    LogManagerWorker *Logworker = new LogManagerWorker();
    Logworker->moveToThread(&m_logThread);
    connect(&m_logThread, &QThread::finished, Logworker, &QObject::deleteLater);
    connect(this, &OneBody::sigTestMSg, Logworker, &LogManagerWorker::saveLog);
    m_logThread.start();

    m_uaWorker = new UAWorker();
    m_uaWorker->moveToThread(&m_uaThread);
    connect(&m_uaThread, &QThread::finished, m_uaWorker, &QObject::deleteLater);
    connect(m_uaWorker, &UAWorker::send_event_by_polling, &m_AutoModeTab, &AutoModeTabUI::cbSWTriggerBtnClicked);
    connect(m_ImgProcessWorker, &ImageProcessWorker::resultReady, m_uaWorker, &UAWorker::cbSendData);
    m_uaThread.start();

    m_ioWorker = new IOWorker();
    m_ioWorker->moveToThread(&m_ioThread);
    connect(&m_ioThread, &QThread::finished, m_ioWorker, &QObject::deleteLater);
    connect(m_ioWorker, &IOWorker::send_event_by_polling, &m_AutoModeTab, &AutoModeTabUI::cbSWTriggerBtnClicked);
    connect(ui->autoRunStopBtn, SIGNAL(clicked()), m_ioWorker, SLOT(pollingCallback()));
    connect(m_ImgProcessWorker, &ImageProcessWorker::resultReady, m_ioWorker, &IOWorker::pollingCallback);
    m_ioThread.start();
}

void OneBody::initTimer()
{
    m_pTimerAutoMode = new QTimer(this);
    connect(m_pTimerAutoMode, SIGNAL(timeout()), this, SLOT(cbUpdateAutoModeImage()));
}

void OneBody::initLogTab()
{
    ui->logcomboBox->addItem("System Log");
    ui->logcomboBox->addItem("Vision Result");
    connect(ui->logcomboBox, SIGNAL(activated(int)), this,  SLOT(logtabComboBoxSelected(int)));

    ui->logTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Log Level"));
    ui->logTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Date"));
    ui->logTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Tag"));
    ui->logTableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Message"));
}

void OneBody::initDataTab()
{
    ui->listWidget->setStyleSheet("QListWidget::item { border-bottom: 2px solid black; }");
}

void OneBody::releaseWorkerThread()
{
    m_imgSaveThread.quit();
    m_imgSaveThread.wait();

    m_imgProcThread.quit();
    m_imgProcThread.wait();
}

void OneBody::releaseCameraResource()
{
    NeptuneStopAcquisition(m_CamHandle);
    NeptuneCloseCamera(m_CamHandle);
}

void OneBody::setCamStreamMode(CAM::CamStreamMode streamMode)
{
    m_GrabImg->lock();
    m_eCamStreamMode = streamMode;
    m_GrabImg->unlock();
}

void OneBody::delay(const int milliSeconds)
{
    QTime dieTime = QTime::currentTime().addMSecs(milliSeconds);
    while (QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void OneBody::convertToQImageRgbFormat(unsigned char *pBuffer, unsigned int nW, unsigned int nH, QImage &image)
{
    image = QImage(pBuffer, nW, nH, QImage::Format_RGB888);
}

bool OneBody::grabQPixmap(QPixmap &image)
{
    NErr status = eNeptuneSuccess;
    NImageFrame data;
    bool bResult = true;

    data.pRGBBuf = new unsigned char[m_CamSizeInfo.width*m_CamSizeInfo.height*3];
    status = NeptuneGrabRGBData(m_CamHandle, &data, 1000);
    if(status != eNeptuneSuccess)
    {
        qDebug() << "error = " << status;
        bResult = false;
        return bResult;
    }

    QImage img(data.pRGBBuf, data.uiWidth, data.uiHeight, QImage::Format_RGB888);
    image = QPixmap::fromImage(img.mirrored());

    delete [] data.pRGBBuf;
    return bResult;
}

bool OneBody::grabMat(Mat &matImg)
{
    NErr status = eNeptuneSuccess;
    NImageFrame data;
    bool bResult = true;

    data.pRGBBuf = new unsigned char[m_CamSizeInfo.width*m_CamSizeInfo.height*3];
    status = NeptuneGrabRGBData(m_CamHandle, &data, 1000);
    if(status != eNeptuneSuccess)
    {
        qDebug() << "error = " << status;
        bResult = false;
        return bResult;
    }

    matImg.create(data.uiHeight,data.uiWidth,CV_8UC(3));
    memcpy(matImg.data, data.pRGBBuf, data.uiHeight * data.uiWidth * 3);
    cv::flip(matImg, matImg, 0);
    cv::cvtColor(matImg,matImg,CV_BGR2RGB);

    delete [] data.pRGBBuf;
    return bResult;
}

bool OneBody::grabQPixmapByRoi(QPixmap &image, QRect &roi)
{
    NErr status = eNeptuneSuccess;
    NImageFrame data;
    bool bResult = true;

    data.pRGBBuf = new unsigned char[m_CamSizeInfo.width*m_CamSizeInfo.height*3];
    status = NeptuneGrabRGBData(m_CamHandle, &data, 1000);
    if(status != eNeptuneSuccess)
    {
        qDebug() << "error = " << status;
        bResult = false;
        return bResult;
    }

    QImage img(data.pRGBBuf, data.uiWidth, data.uiHeight, QImage::Format_RGB888);

    if((data.uiWidth < roi.width()) || (data.uiHeight < roi.height())
            || (roi.x() < 0) || (roi.y() < 0))
    {
        bResult = false;
        return bResult;
    }

    image = QPixmap::fromImage(img.mirrored().copy(roi));

    delete [] data.pRGBBuf;
    return bResult;
}

bool OneBody::grabMatByRoi(Mat &matImg, QRect &roi)
{
    NErr status = eNeptuneSuccess;
    NImageFrame data;
    bool bResult = true;

    data.pRGBBuf = new unsigned char[m_CamSizeInfo.width*m_CamSizeInfo.height*3];
    status = NeptuneGrabRGBData(m_CamHandle, &data, 1000);
    if(status != eNeptuneSuccess)
    {
        qDebug() << "error = " << status;
        bResult = false;
        return bResult;
    }

    matImg.create(data.uiHeight,data.uiWidth,CV_8UC(3));
    memcpy(matImg.data, data.pRGBBuf, data.uiHeight * data.uiWidth * 3);
    cv::flip(matImg, matImg, 0);

    matImg = matImg(cv::Rect(roi.x(), roi.y(), roi.width(), roi.height()));
    cv::cvtColor(matImg,matImg,CV_BGR2RGB);

    delete [] data.pRGBBuf;
    return bResult;
}

void OneBody::teachSaveImageResult(bool bResult)
{
    if(!bResult)
    {
        QMessageBox::warning(this, tr("Warning!"), "Image Save Incomplete", QMessageBox::Close);
    }
}

void OneBody::on_connect_clicked()
{

}

void OneBody::on_inspection_clicked()
{
    otherButtonOff();
    ui->inspection->setStyleSheet("border-image: url(:/images/but_04teach_select.JPG);");
    ui->stackedWidget->setCurrentIndex(0);
    ui->MainstackedWidget->setCurrentIndex(0);
}

void OneBody::on_testRun_clicked()
{
    otherButtonOff();
    ui->testRun->setStyleSheet("border-image: url(:/images/but_02manual_select.JPG);");
    ui->stackedWidget->setCurrentIndex(1);
    ui->MainstackedWidget->setCurrentIndex(0);
}

void OneBody::on_data_clicked()
{
    otherButtonOff();
    ui->data->setStyleSheet("border-image: url(:/images/but_03data_select.JPG);");
    ui->stackedWidget->setCurrentIndex(5);
    ui->MainstackedWidget->setCurrentIndex(1);
}

void OneBody::on_run_clicked()
{
    otherButtonOff();
    ui->run->setStyleSheet("border-image: url(:/images/but_01auto_select.JPG);");
    ui->stackedWidget->setCurrentIndex(3);
    ui->MainstackedWidget->setCurrentIndex(0);
}

void OneBody::on_setting_clicked()
{
    otherButtonOff();
    ui->setting->setStyleSheet("border-image: url(:/images/but_setting_select.JPG);");
    ui->stackedWidget->setCurrentIndex(5);
    ui->MainstackedWidget->setCurrentIndex(2);
}

void OneBody::otherButtonOff()
{
    //ui->connect->setStyleSheet("border-image: url(:/images/Check_Off.PNG);");
    ui->inspection->setStyleSheet("border-image: url(:/images/but_04teach_click.JPG);");
    ui->testRun->setStyleSheet("border-image: url(:/images/but_02manual_click.JPG);");
    ui->data->setStyleSheet("border-image: url(:/images/but_03data_click.JPG);");
    ui->run->setStyleSheet("border-image: url(:/images/but_01auto_click.JPG);");
    ui->setting->setStyleSheet("border-image: url(:/images/but_setting_click.JPG);");
}


void OneBody::on_lightOnCheckBox_stateChanged(int arg1)
{
//    if(ui->lightOnCheckBox->checkState())
//        p_ModelData->m_ilightEnable = 1;
//    else
//        p_ModelData->m_ilightEnable = 0;
}

void OneBody::on_lightValueEdit_textChanged(const QString &arg1)
{

}

void OneBody::on_lightValueSlider_valueChanged(int value)
{
    QString valString;
    valString = QString::number(value);
    //ui->lightValueEdit->setText(valString);
}

void OneBody::on_inspLoadBtn_clicked()
{
    setCamStreamMode(CAM::LIVE_STOP);
    QPixmap temp;
    inspLoadfileName.clear();
    inspLoadfileName = QFileDialog::getOpenFileName(this, tr("Load Image"),
                                                    "/home/nvidia/Pictures",
                                                    tr("Image File(*.png *.bmp *.jpg)"));
    if (inspLoadfileName.isEmpty())
        return;
    temp.load(inspLoadfileName);
    g_Pixmap.load(inspLoadfileName);

    m_capturedImg = CImageConverter::QPixmapToCvMat(temp);
    m_Pixmap->setPixmap(temp);
    ui->graphicsView->scene()->setSceneRect(m_Pixmap->boundingRect());
    ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void OneBody::cbUpdateMainViewer(QPixmap img)
{
    m_MutexImg.lock();
    m_Pixmap->setPixmap( img );
    m_MutexImg.unlock();

    ui->graphicsView->scene()->setSceneRect(m_Pixmap->boundingRect());
    ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);

}

void OneBody::cbUpdateAutoModeImage()
{
    ui->autoImage->setPixmap(m_Pixmap->pixmap());
    m_pTimerAutoMode->stop();
    setCamStreamMode(CAM::LIVE_STREAM);
}

void OneBody::logtabComboBoxSelected(int item)
{
    if(item == 0)
    {
        ui->logTableWidget->clear();
        ui->logTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Log Level"));
        ui->logTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Date"));
        ui->logTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Message"));
        ui->logTableWidget->setRowCount(50);
        //ui->logTableWidget->setHorizontalHeaderItem(m_cVisionModuleMgr.m_VisionModuleMap.size());

        unsigned int iCount = 0;
        QDir directory("/tmp/onebody_systemlog/");
        QStringList fileList = directory.entryList(QStringList() << "*.txt", QDir::Files);
        foreach (QString fileName, fileList)
        {
            QFile file(directory.absoluteFilePath(fileName));
            if(!file.open(QIODevice::ReadOnly))
            {
                qWarning() << "Cannot file Open";
                return;
            }

            QTextStream in(&file);

            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList fields = line.split("'");
                int i =0;
                foreach (QString msg, fields) {
                    ui->logTableWidget->setItem(iCount, i, new QTableWidgetItem(msg));
                    i++;
                }
                iCount++;
            }
            file.close();
        }
        ui->logTableWidget->resizeColumnsToContents();
        ui->logTableWidget->horizontalHeader()->setStretchLastSection(true);
    }
    else if(item == 1)
    {
        ui->logTableWidget->clear();
        ui->logTableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("X"));
        ui->logTableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Y"));
        ui->logTableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Angle"));
        ui->logTableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Tact Time"));
        ui->logTableWidget->setRowCount(50);

        unsigned int iCount = 0;

        QTableWidgetItem * pTemp = new QTableWidgetItem();
        pTemp->setTextAlignment(Qt::AlignCenter);

        QTableWidgetItem * newItem1;
        QTableWidgetItem * newItem2;
        QTableWidgetItem * newItem3;
        QTableWidgetItem * newItem4;

        cv::Point2f centerPt;
        double dAngle;
        QString tempStr;
        QString tempStr2;

        foreach (CVisionAgentResult v, m_lAutoVisionResult)
        {
            if(v.bOk)
            {
                newItem1 = pTemp->clone();
                newItem2 = pTemp->clone();
                if(v.GetCenterPoint(centerPt))
                {
                    tempStr = QString::number(centerPt.x,'f',2);
                    tempStr2 = QString::number(centerPt.y,'f',2);
                }
                else
                {
                    tempStr = "X";
                    tempStr2 = "X";
                }
                newItem1->setText(tempStr);
                newItem2->setText(tempStr2);

                newItem3 = pTemp->clone();
                if(v.GetAngle(dAngle))
                {
                    tempStr = QString::number(dAngle,'f',2);
                }
                else
                {
                    tempStr = "X";
                }

                newItem3->setText(tempStr);

                newItem4 = pTemp->clone();
                newItem4->setText(QString::number(v.dTaktTime,'f',2));

                ui->logTableWidget->setItem(iCount, 0, newItem1);
                ui->logTableWidget->setItem(iCount, 1, newItem2);
                ui->logTableWidget->setItem(iCount, 2, newItem3);
                ui->logTableWidget->setItem(iCount, 3, newItem4);
            }
            else
            {
                ui->logTableWidget->setItem(iCount, 0, new QTableWidgetItem("Failed"));
                ui->logTableWidget->setItem(iCount, 1, new QTableWidgetItem("-"));
                ui->logTableWidget->setItem(iCount, 2, new QTableWidgetItem("-"));
                ui->logTableWidget->setItem(iCount, 3, new QTableWidgetItem("-"));
            }
            iCount++;
        }
    }
}

void OneBody::tabChanged(int value)
{
    if(m_TeachModeTab.m_RoiRect != nullptr)
    {
        ui->graphicsView->scene()->removeItem(m_TeachModeTab.m_RoiRect);
        delete m_TeachModeTab.m_RoiRect;
        m_TeachModeTab.m_RoiRect = nullptr;
    }
    else
    {
        qDebug() << "m_RoiRect->IsEmpty()";
    }
    if(m_TeachModeTab.m_PatternRect != nullptr)
    {
        ui->graphicsView->scene()->removeItem(m_TeachModeTab.m_PatternRect);
        delete m_TeachModeTab.m_PatternRect;
        m_TeachModeTab.m_PatternRect = nullptr;
    }
    else
    {
        qDebug() << "m_PatternRect->IsEmpty()";
    }
    //clear model picture
    ui->manualPatternImage->clear();
    ui->manualCannyImage->clear();
    //clear model data
    m_AutoModeTab.m_ModelData.init();
    m_ManualModeTab.m_ModelData.init();
    m_TeachModeTab.m_ModelData.init();
    //clear Auto table
    ui->tableWidgetAutoModuleList->clear();
    ui->tableWidgetAutoModuleList->setHorizontalHeaderItem(0, new QTableWidgetItem("Seq. Num"));
    ui->tableWidgetAutoModuleList->setHorizontalHeaderItem(1, new QTableWidgetItem("Vision Type"));
    ui->tableWidgetAutoModuleList->setRowCount(m_cVisionModuleMgr.m_VisionModuleMap.size());
    //clear mannual table
    ui->tableWidgetAutoModuleList_2->clear();
    ui->tableWidgetAutoModuleList_2->setHorizontalHeaderItem(0, new QTableWidgetItem("Seq. Num"));
    ui->tableWidgetAutoModuleList_2->setHorizontalHeaderItem(1, new QTableWidgetItem("Vision Type"));
    //clear teach table
    ui->tableWidgetAutoModuleList_3->clear();
    ui->tableWidgetAutoModuleList_3->setHorizontalHeaderItem(0, new QTableWidgetItem("Seq. Num"));
    ui->tableWidgetAutoModuleList_3->setHorizontalHeaderItem(1, new QTableWidgetItem("Vision Type"));

}

