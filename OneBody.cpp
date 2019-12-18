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
    ui(new Ui::OneBody), m_pRectRoi(nullptr), pRectModel(nullptr), pRect(nullptr)
{
    ui->setupUi(this);

    initWorkerThreadConnect();
    initTimer();
    init_graphics_view();
    Init_UI_State();
    initDataTab();
    initLogTab();
    load_settings();
    p_ModelData = new CModelData;

    // auto Mode
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
    QObject::connect(ui->teachTempSaveBtn,SIGNAL(clicked()),&m_TeachModeTab,SLOT(cbTeachPatternImageSaveBtnClicked();));
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
    QObject::connect(ui->manualFileLoadBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbTeachModelLoadBtnClicked()));
    QObject::connect(ui->manualFileCancelBtn, SIGNAL(clicked()), &m_TeachModeTab, SLOT(cbManualModelCancelBtnClicked()));
    QObject::connect(ui->manualLoadBtn, SIGNAL(clicked()), &m_ManualModeTab, SLOT(cbManualImageLoadBtnClicked()));
    QObject::connect(ui->testFileClearBtn,SIGNAL(clicked()),&m_ManualModeTab,SLOT(cbManualImageClearBtnClicked()));
    QObject::connect(ui->testTestBtn,SIGNAL(clicked()),&m_ManualModeTab,SLOT(cbManualRunBtnClicked()));
    QObject::connect(ui->testResultTable,SIGNAL(cellPressed(int,int)),&m_ManualModeTab, SLOT(cbManualResultTableCellPressed(int, int)));

    //data
    QObject::connect(ui->dataConnectBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataConnectBtnClicked()));
    QObject::connect(ui->dataReadBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataReadBtnClicked()));
    QObject::connect(ui->dataDisconBtn, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataDisconnectBtnClicked()));
    QObject::connect(ui->dataLibLoadUserModule, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataLibLoadUserModule()));
    QObject::connect(ui->dataLibMakeUserModule, SIGNAL(clicked()), &m_DataModeTab, SLOT(cbDataLibMakeUserModule()));
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

int g_iResizeRatio = 4;

void OneBody::pattern_matching()
{
    setCamStreamMode(CAM::LIVE_STOP);

    testTemplate = QFileInfo(p_ModelData->m_qsTemplate).fileName();
    testTemplatePath = QFileInfo(p_ModelData->m_qsTemplate).path();
    testTemplatePath += "/";

    //cv::String imgs_folder(testFilePath.toStdString());
    cv::String templateFolder(testTemplatePath.toStdString());
    cv::String templateName(testTemplate.toStdString());

    std::vector<cv::String> filenames;
    filenames.clear();
    for(int j=0; j<testFilenames.size(); j++)
    {
        QString temp;
        temp = testFilenames.at(j);
        filenames.push_back(temp.toStdString());
    }

    CVisionAgentResult visionResult;

    m_cPatternModule.SetResizeRatio(g_iResizeRatio);
    m_cPatternModule.InitPath(templateFolder, templateName);

    QFile file(testTemplatePath + QString(m_cPatternModule.GetContourName().c_str()));
    if(!file.exists())
    {
        QMessageBox::information(this, tr("Pattern Matching Model GUIDE"),
                                 tr("1. Erase Model Area - Mouse L-Button Down & Drag \n"
                                    "2. Save Model - Mouse L-Button Double Click! "),
                                 QMessageBox::Ok);
        m_cPatternModule.MakeNewTemplate(g_iResizeRatio);
    }
    m_ImgProcessWorker->m_pWorkerModule = &m_cPatternModule;

    qDebug() << "image file size " << filenames.size();

    cv::Mat input_img;
    clock_t start;
    clock_t end;

    double img_preprocess_t = 0.0;
    Mat output_img;
    int i=0;
    m_vTestTabImages.clear();

    ui->testResultTable->clear();
    ui->testResultTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Center Point"));
    ui->testResultTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Angle"));
    ui->testResultTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Tact Time"));
    //ui->tableWidgetAutoModuleList->setRowCount(m_cVisionModuleMgr.m_VisionModuleMap.size());
    ui->testResultTable->setRowCount(filenames.size());
    for(unsigned int j=0; j<filenames.size(); j++)
    {
        for(int k=0; k<=2; k++)
        {
            QTableWidgetItem *init = new QTableWidgetItem();
            init->setText("0");
            ui->testResultTable->setItem(j,k,init);
        }
    }

    foreach (std::string var, filenames)
    {
        ui->testListWidget->item(i)->setSelected(true);
        ui->testListWidget->setFocus();

        qDebug() << var.c_str();
        input_img = cv::imread(var,IMREAD_COLOR);
        if(input_img.empty())
        {
            qDebug() << "input image null Error";
            break;
        }

#if 0
        emit sigSendTestMatImgToWorkerThread(input_img);
        qApp->processEvents();
#else
        start = clock();
        visionResult = m_cPatternModule.RunVision(input_img, output_img);
        end = clock();

        img_preprocess_t += (double)(end - start) / CLOCKS_PER_SEC;

        QString strCPX = QString::number(visionResult.centerPt.x);
        QString strCPY = QString::number(visionResult.centerPt.y);
        QString strCP = strCPX + ", " + strCPY;
        QString strAngle = QString::number(visionResult.dAngle* (180/3.14),'f',2);
        QString tt = QString::number(((double)(end - start) / CLOCKS_PER_SEC),'f',2);

        QTableWidgetItem *centerPoint = new QTableWidgetItem();
        QTableWidgetItem *angle = new QTableWidgetItem();
        QTableWidgetItem *tactTime = new QTableWidgetItem();

        centerPoint->setText(strCP);
        angle->setText(strAngle);
        tactTime->setText(tt);

        ui->testResultTable->setItem(i,0,centerPoint);
        ui->testResultTable->setItem(i,1,angle);
        ui->testResultTable->setItem(i,2,tactTime);

        cvtColor(output_img, output_img, CV_BGR2RGB);

        QImage img((uchar*)output_img.data,
                   output_img.cols,
                   output_img.rows,
                   output_img.step,
                   QImage::Format_RGB888);

        QPixmap tmpPixmap = QPixmap::fromImage(img);
        m_vTestTabImages.push_back(tmpPixmap);

        emit sigUpdateMainViewer(tmpPixmap);
        ui->graphicsView->scene()->setSceneRect(m_Pixmap->boundingRect());
        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
        qApp->processEvents();
#endif
        i++;
    }
    qDebug("TIME : %5.2fs ", img_preprocess_t);
}

void OneBody::settingBackBtnClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}



void OneBody::load_model()
{
    ui->lightOnCheckBox->setChecked((bool)p_ModelData->m_ilightEnable);
    if(p_ModelData->m_iAlgoType == 0)
    {
        QString testTemplate = QFileInfo(p_ModelData->m_qsTemplate).fileName();
        QString testTemplatePath = QFileInfo(p_ModelData->m_qsTemplate).path();
        testTemplatePath += "/";

        ui->teachPatternResize->setText(QString::number(p_ModelData->m_iResize));
        g_iResizeRatio = p_ModelData->m_iResize;

        //ui->manualAlgoTab->setCurrentIndex(p_ModelData->m_iAlgoType);
        ui->inspAlgoCombo->setCurrentIndex(p_ModelData->m_iAlgoType);
        QPixmap pixmap;
        QPixmap canny;
        pixmap.load(p_ModelData->m_qsTemplate);
        canny.load(testTemplatePath + QString("contour_def_canny_totally.bmp"));
        //ui->inspPatternImage->setPixmap(pixmap);
        //ui->teachTempLabel->setText(p_ModelData->m_qsTemplate);

        //ui->manualTempLabel->setText(p_ModelData->m_qsTemplate);
        ui->manualPatternImage->setPixmap(pixmap);
        ui->manualCannyImage->setPixmap(canny);

        cv::String templateFolder(testTemplatePath.toStdString());
        cv::String templateName(testTemplate.toStdString());

        CVisionAgentResult visionResult;
        std::vector<cv::String> filenames;

        m_cPatternModule.SetResizeRatio(g_iResizeRatio);
        m_cPatternModule.InitPath(templateFolder, templateName);

        m_Roi = QRect(p_ModelData->m_iStartX, p_ModelData->m_iStartY, p_ModelData->m_iEndX, p_ModelData->m_iEndY);
    }
    else if(p_ModelData->m_iAlgoType == 1)
    {
        //ui->manualAlgoTab->setCurrentIndex(p_ModelData->m_iAlgoType);
        ui->inspAlgoCombo->setCurrentIndex(p_ModelData->m_iAlgoType);
        ui->teachCircleThreshLowSlider->setValue(p_ModelData->m_iThresholdLow);
        ui->teachCircleThreshHighSlider->setValue(p_ModelData->m_iThresholdHigh);
        ui->teachCircleTolSpin->setValue(p_ModelData->m_iTolerance);
        ui->teachCircleNoCombo->setCurrentText(QString::number(p_ModelData->m_iTargetNo));
        ui->teachCircleRadEdit->setText(QString::number(p_ModelData->m_iRadius));

        ui->manualLabel1->setText("Radius");
        ui->manualRadWidthLabel->setText(QString::number(p_ModelData->m_iRadius));
        ui->manualThreshLowLabel->setText(QString::number(p_ModelData->m_iThresholdLow));
        ui->manualThreshHighLabel->setText(QString::number(p_ModelData->m_iThresholdHigh));
        ui->manualTolLabel->setText(QString::number(p_ModelData->m_iTolerance));
        ui->manualNoLabel->setText(QString::number(p_ModelData->m_iTargetNo));
    }
    else if(p_ModelData->m_iAlgoType == 2)
    {
        //ui->manualAlgoTab->setCurrentIndex(p_ModelData->m_iAlgoType);
        ui->inspAlgoCombo->setCurrentIndex(p_ModelData->m_iAlgoType);
        ui->teachRectThreshLowSlider->setValue(p_ModelData->m_iThresholdLow);
        ui->teachRectThreshHighSlider->setValue(p_ModelData->m_iThresholdHigh);
        ui->teachRectTolSpin->setValue(p_ModelData->m_iTolerance);
        ui->teachRectNoCombo->setCurrentText(QString::number(p_ModelData->m_iTargetNo));
        ui->teachRectWidthEdit->setText(QString::number(p_ModelData->m_iWidth));
        ui->teachRectHeightEdit->setText(QString::number(p_ModelData->m_iHeight));

        ui->manualLabel1->setText("Width");
        ui->manualLabel2->setText("Height");
        ui->manualRadWidthLabel->setText(QString::number(p_ModelData->m_iWidth));
        ui->manualHeightLabel->setText(QString::number(p_ModelData->m_iHeight));
        ui->manualThreshLowLabel->setText(QString::number(p_ModelData->m_iThresholdLow));
        ui->manualThreshHighLabel->setText(QString::number(p_ModelData->m_iThresholdHigh));
        ui->manualTolLabel->setText(QString::number(p_ModelData->m_iTolerance));
        ui->manualNoLabel->setText(QString::number(p_ModelData->m_iTargetNo));
    }

}

void OneBody::temp_save_model()
{
    p_ModelData->m_iResize = ui->teachPatternResize->text().toInt();
    if(m_pRectRoi != nullptr)
    {
        p_ModelData->m_iStartX = m_pRectRoi->getRectPosBySceneCoord().toRect().x();
        p_ModelData->m_iStartY = m_pRectRoi->getRectPosBySceneCoord().toRect().y();
        p_ModelData->m_iEndX = m_pRectRoi->getRectPosBySceneCoord().toRect().width();
        p_ModelData->m_iEndY = m_pRectRoi->getRectPosBySceneCoord().toRect().height();
    }
    else
    {
        p_ModelData->m_iStartX = 0;
        p_ModelData->m_iStartY = 0;
        p_ModelData->m_iEndX = 0;
        p_ModelData->m_iEndY = 0;
    }

    if(p_ModelData->m_iAlgoType == 1)
    {
        p_ModelData->m_iTolerance = ui->teachCircleTolSpin->text().toInt();
        p_ModelData->m_iTargetNo = ui->teachCircleNoCombo->currentText().toInt();
        p_ModelData->m_iRadius = ui->teachCircleRadEdit->text().toFloat();
    }
    else
    {
        p_ModelData->m_iTolerance = ui->teachRectTolSpin->text().toInt();
        p_ModelData->m_iTargetNo = ui->teachRectNoCombo->currentText().toInt();
        p_ModelData->m_iWidth = ui->teachRectWidthEdit->text().toInt();
        p_ModelData->m_iHeight = ui->teachRectHeightEdit->text().toInt();
    }
}

void OneBody::circle_algorithm()
{
    setCamStreamMode(CAM::LIVE_STOP);
    clock_t start;
    clock_t end;
    double img_preprocess_t = 0.0;
    QTableWidgetItem *centerPt = new QTableWidgetItem();
    //ui->graphicsView->scene()->clear();

    for(int i=0; i<testFilenames.size(); i++)
    {
        CCircleModule visionModule;
        CVisionAgentResult visionResult;
        QPixmap temp;
        temp.load(testFilenames.at(i));

        qDebug() << testFilenames.at(i);

        Mat dispImg;
        CCircleParams params;
        params.iRadius = p_ModelData->m_iRadius;
        params.dTolerance = p_ModelData->m_iTolerance * 0.1;
        params.iThresholdHigh = p_ModelData->m_iThresholdHigh;
        params.iThresholdLow = p_ModelData->m_iThresholdLow;

        //        params.iRadius = 172;
        //        params.dTolerance = 0.2;
        //        params.iThresholdHigh = 90;
        //        params.iThresholdLow = 0;

        visionModule.SetParams(params);
        visionModule.m_bDebugMode = true;

        Mat srcImg = CImageConverter::QPixmapToCvMat(temp);
#if 0
        cv::cvtColor(srcImg, srcImg, CV_BGR2RGB);

        QImage img((uchar*)srcImg.data,
                   srcImg.cols,
                   srcImg.rows,
                   QImage::Format_RGB888);
#else
        start = clock();
        visionResult = visionModule.RunVision(srcImg, dispImg);
        end = clock();
        img_preprocess_t += (double)(end - start) / CLOCKS_PER_SEC;

        cv::cvtColor(dispImg, dispImg, CV_BGR2RGB);

        QImage img((uchar*)dispImg.data,
                   dispImg.cols,
                   dispImg.rows,
                   QImage::Format_RGB888);
#endif
        QString tmp = QString::number(visionResult.centerPt.x) + ", "
                + QString::number(visionResult.centerPt.y);
        centerPt->setText(tmp);

        if(visionResult.bOk == true)
        {
            ui->testResultTable->setItem(i,0,centerPt);
        }
        else
            QMessageBox::information(this, tr("information"), "No Search Hole", QMessageBox::Close);

        //m_Pixmap->setPixmap(QPixmap::fromImage(img));
        emit sigUpdateMainViewer(QPixmap::fromImage(img));
        qApp->processEvents();
        qDebug() << "finished";
    }

}

void OneBody::circle_blob_algorithm()
{    
    setCamStreamMode(CAM::LIVE_STOP);

    for(int i=0; i<testFilenames.size(); i++)
    {
        CCircleBlobModule visionModule;
        QPixmap temp;
        temp.load(testFilenames.at(i));

        qDebug() << testFilenames.at(i);

        Mat dispImg;
        CCircleBlobParams params;
        params.iRadius = p_ModelData->m_iRadius;
        params.dTolerance = p_ModelData->m_iTolerance * 0.1;
        params.iThresholdHigh = p_ModelData->m_iThresholdHigh;
        params.iThresholdLow = p_ModelData->m_iThresholdLow;

        visionModule.SetParams(params);
        visionModule.m_bDebugMode = true;

        Mat srcImg = CImageConverter::QPixmapToCvMat(temp);
        visionModule.RunVision(srcImg, dispImg);

        cv::cvtColor(dispImg, dispImg, CV_BGR2RGB);

        QImage img((uchar*)dispImg.data,
                   dispImg.cols,
                   dispImg.rows,
                   QImage::Format_RGB888);

        emit sigUpdateMainViewer(QPixmap::fromImage(img));
        qApp->processEvents();
    }
}

void OneBody::rect_algorithm()
{

}

void OneBody::init_model_ui()
{
    //ui->manualAlgoTab->setCurrentIndex(p_ModelData->m_iAlgoType);
    //ui->manualTempLabel->setText(p_ModelData->m_qsTemplate);

    ui->inspAlgoCombo->setCurrentIndex(p_ModelData->m_iAlgoType);
    //ui->teachTempLabel->setText(p_ModelData->m_qsTemplate);
    ui->teachCircleThreshLowSlider->setValue(p_ModelData->m_iThresholdLow);
    ui->teachCircleThreshHighSlider->setValue(p_ModelData->m_iThresholdHigh);
    ui->teachCircleTolSpin->setValue(p_ModelData->m_iTolerance);
    ui->teachCircleNoCombo->setCurrentIndex(p_ModelData->m_iTargetNo);
    ui->teachCircleRadEdit->setText(QString::number(p_ModelData->m_iRadius));

    //ui->teachTempLabel->setText(p_ModelData->m_qsTemplate);
    ui->teachRectThreshLowSlider->setValue(p_ModelData->m_iThresholdLow);
    ui->teachRectThreshHighSlider->setValue(p_ModelData->m_iThresholdHigh);
    ui->teachRectTolSpin->setValue(p_ModelData->m_iTolerance);
    ui->teachRectNoCombo->setCurrentIndex(p_ModelData->m_iTargetNo);
    ui->teachRectWidthEdit->setText(QString::number(p_ModelData->m_iWidth));
    ui->teachRectHeightEdit->setText(QString::number(p_ModelData->m_iHeight));
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
        QImage img((uchar*)mat.data,
                   mat.cols,
                   mat.rows,
                   QImage::Format_RGB888);

        m_MutexImg.lock();
        m_Pixmap->setPixmap( QPixmap::fromImage(img));
        m_MutexImg.unlock();
        ui->graphicsView->scene()->setSceneRect(m_Pixmap->boundingRect());
        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
        //update result at table
        QString strCPX = QString::number(result.centerPt.x);
        QString strCPY = QString::number(result.centerPt.y);
        QString strAngle = QString::number(result.dAngle* (180/3.14),'f',2);
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

void OneBody::cbTestTabGetVisionProcessResult(_MatImg mat, CVisionAgentResult result)
{
    if(result.bOk)
    {
        //update result at table
        QString strCPX = QString::number(result.centerPt.x);
        QString strCPY = QString::number(result.centerPt.y);
        QString strCP = strCPX + ", " + strCPY;
        QString strAngle = QString::number(result.dAngle* (180/3.14),'f',2);
        QString tt = QString::number(result.dTaktTime,'f',2);

        QTableWidgetItem *centerPoint = new QTableWidgetItem();
        QTableWidgetItem *angle = new QTableWidgetItem();
        QTableWidgetItem *tactTime = new QTableWidgetItem();

        centerPoint->setText(strCP);
        angle->setText(strAngle);
        tactTime->setText(tt);

        int iRowCount = ui->testResultTable->rowCount();
        ui->testResultTable->setItem(iRowCount,0,centerPoint);
        ui->testResultTable->setItem(iRowCount,1,angle);
        ui->testResultTable->setItem(iRowCount,2,tactTime);

        //cvtColor(mat, mat, CV_BGR2RGB);

        QImage img((uchar*)mat.data,
                   mat.cols,
                   mat.rows,
                   QImage::Format_RGB888);

        m_MutexImg.lock();
        m_Pixmap->setPixmap( QPixmap::fromImage(img));
        m_MutexImg.unlock();
        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);

        QPixmap tmpPixmap = QPixmap::fromImage(img);
        m_vTestTabImages.push_back(tmpPixmap);
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
    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

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
    if(ui->lightOnCheckBox->checkState())
        p_ModelData->m_ilightEnable = 1;
    else
        p_ModelData->m_ilightEnable = 0;
}

void OneBody::on_lightValueEdit_textChanged(const QString &arg1)
{
    //set light value on
    //    QRegExp re("\\d*"); // a digit (\d), zero or more times (*)
    //    if (re.exactMatch(arg1)){
    //        int value = arg1.toInt();
    //        if(value >= -127 && value<=127)
    //            ui->lightValueSlider->setValue(value);
    //    }
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

void OneBody::on_testLoadBtn_clicked()
{
    ui->testListWidget->clear();
    testFilenames.clear();
    QFileDialog *fileDlg = new QFileDialog(this);
    testFilenames = fileDlg->getOpenFileNames(this, "Select file",
                                              "/home/nvidia/Pictures",
                                              tr("Image File(*.png *.bmp *.jpg)"));
    if(testFilenames.empty())
        return;
    for(int i=0; i<testFilenames.size(); i++)
    {
        ui->testListWidget->addItem(QFileInfo(testFilenames.at(i)).fileName());
    }
    testFilePath = QFileInfo(testFilenames.at(0)).path();
}

void OneBody::cbUpdateMainViewer(QPixmap img)
{
    m_MutexImg.lock();
    m_Pixmap->setPixmap( img );
    m_MutexImg.unlock();

    //if(pFrameData->m_iFrameCnt >= 3 && pFrameData->m_iFrameCnt <= 6)
    {
        ui->graphicsView->scene()->setSceneRect(m_Pixmap->boundingRect());
        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
        //        ui->graphicsView->setTransform(QTransform());
        //        ui->graphicsView->scene()->update();
        //        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
    }
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

        foreach (CVisionAgentResult v, m_lAutoVisionResult)
        {
            if(v.bOk)
            {
                newItem1 = pTemp->clone();
                newItem1->setText(QString::number(v.centerPt.x,'f',2));

                newItem2 = pTemp->clone();
                newItem2->setText(QString::number(v.centerPt.y,'f',2));

                newItem3 = pTemp->clone();
                newItem3->setText(QString::number(v.dAngle,'f',2));

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

