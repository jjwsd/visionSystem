#ifndef ONEBODY_H
#define ONEBODY_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QMutex>
#include <QPixmap>
#include <QThread>
#include <QTimer>

#include <NeptuneAPI.h>
#include "CXmlParser.h"
#include "CModelData.h"
#include "opencv2/opencv.hpp"
#include "H_Camera/cameradata.h"
#include "UI/userrectitem.h"
#include <Data/definedata.h>
#include <COpcUa.h>

#include <VisionModule/cPatternMatchModule.h>
#include <VisionModule/CVisionModuleMgr.h>
#include <WorkerThread/ImageProcessWorker.h>
#include <logger.h>
#include <WorkerThread/UAWorker.h>
#include <WorkerThread/IOWorker.h>

#include <UI/AutoModeTabUI.h>
#include <UI/TeachModeTabUI.h>
#include <UI/ManualModeTabUI.h>
#include <UI/DataModeTabUI.h>

class CameraData;
class CModelData;
class CXmlParser;
class COpcUa;
class UAWorker;
class IOWorker;

extern "C"{
    #include <jetsonGPIO.h>
}

namespace Ui {
class OneBody;
}

class OneBody : public QMainWindow
{
    Q_OBJECT

public:
    explicit OneBody(QWidget *parent = nullptr);
    ~OneBody();

    bool event(QEvent * e);

    static void imi_cam_frame_cb(PNImageFrame pImgFrame);

    static CameraData * pFrameData;
    static bool  bIsAutoMode;
    static CAM::CamStreamMode  m_eCamStreamMode;
    static QVector<QRgb> sColorTable;
    static QMutex * m_GrabImg;
    static bool bFirstTimeRun;

    Ui::OneBody* GetUI() const;

    std::list<CVisionAgentResult> m_list;

private:
    void init_graphics_view();
    void initWorkerThreadConnect();
    void initTimer();
    void initLogTab();
    void initDataTab();

    void releaseWorkerThread();
    void releaseCameraResource();

    void delay(const int milliSeconds);

    void convertToQImageRgbFormat(unsigned char *pBuffer, unsigned int nW, unsigned int nH, QImage& image); 

    QThread m_imgSaveThread;
    QThread m_imgProcThread;
    QThread m_logThread;
    QThread m_uaThread;
    QThread m_ioThread;

    AutoModeTabUI m_AutoModeTab;
    TeachModeTabUI m_TeachModeTab;
    ManualModeTabUI m_ManualModeTab;
    DataModeTabUI m_DataModeTab;

    //Logger resultLogger;

public slots:
    void teachSaveImageResult(bool bResult);

    void on_connect_clicked();

    void on_inspection_clicked();

    void on_testRun_clicked();

    void on_data_clicked();

    void on_run_clicked();

    void otherButtonOff();

    void on_setting_clicked();

    void on_lightOnCheckBox_stateChanged(int arg1);

    void on_lightValueEdit_textChanged(const QString &arg1);

    void on_lightValueSlider_valueChanged(int value);

    void on_inspLoadBtn_clicked();

    void on_testLoadBtn_clicked();

    void Init_UI_State();

    void load_settings();

    void set_width_slider(int value);

    void set_height_slider(int value);

    void set_offset_x_slider(int value);

    void set_offset_y_slider(int value);

    void set_format_size();

    void on_exposure_auto_check();

    void set_exposure_slider(int value);

    void set_exposure_spin(int value);

    void on_gain_auto_check();

    void set_gain_slider(int value);

    void set_gain_spin(int value);

    void on_clicked_resolution_applyBtn();

    void pattern_matching();

    void settingBackBtnClicked();

    void LoadModelData(CModelData m_ModelData);

    void SaveModelData(CModelData * m_ModelData, CDragBox * m_RoiRect);

    void circle_algorithm();

    void circle_blob_algorithm();

    void rect_algorithm();

    void init_model_ui();

    void cbAutoTabGetVisionProcessResult(_MatImg mat, CVisionAgentResult result);
    void cbTestTabGetVisionProcessResult(_MatImg mat, CVisionAgentResult result);
    void cbUpdateMainViewer(QPixmap);
    void cbUpdateAutoModeImage();

    void logtabComboBoxSelected(int item);

signals:
    void sigUpdateMainViewer(QPixmap);

    void sigSendMatImgToWorkerThread(_MatImg p);
    void sigSaveImgToWorkerThread(QPixmap img, const QString &fileName);

    void sigSendTestMatImgToWorkerThread(_MatImg p);

    void sigTestMSg(const QtMsgType& type, const QString & tag, const QString & msg);
    void sigCheckTriggerStart();

private:
    void automode_triggerBtn_enable();

public:
    CXmlParser xml;
    CVisionModuleMgr m_cVisionModuleMgr;

    QTimer* m_pTimerAutoMode;

    std::list<CVisionAgentResult> m_lAutoVisionResult;
    //CModelData * p_ModelData;
    QRect m_Roi;
    bool m_bAutoModeStart = false;
    ImageProcessWorker * m_ImgProcessWorker;
    UAWorker * m_uaWorker;
    IOWorker * m_ioWorker;

    void setCamStreamMode(CAM::CamStreamMode streamMode);
    bool grabQPixmap(QPixmap& image);
    bool grabMat(Mat& matImg);

    bool grabQPixmapByRoi(QPixmap& image, QRect& roi);
    bool grabMatByRoi(Mat& matImg, QRect& roi);

    Ui::OneBody *ui;
    COpcUa g_opcUA;

    QPixmap * m_RawImage;
    QGraphicsPixmapItem * m_Pixmap;
    CameraData * m_FrameData;
    QPixmap g_Pixmap;
    cv::Mat m_capturedImg;

    NErr err;
    NCamHandle m_CamHandle;
    NSizeInfo m_CamSizeInfo;
    static eIMIFormat m_eIMIFormat;

    //threshold
    int m_iLowValue;
    int m_iHighValue;

    UserRectItem * m_pRectRoi;
    //UserRectItem * pRectModel;
    UserRectItem * pRect;


    bool newRect = false;

    QMutex m_MutexImg;

    QTimer *timer;
    long tCount;
    long resultCount;

    std::vector<QPixmap> m_vTestTabImages;

    QString testTemplate;
    QString testTemplatePath;
    QString testFilePath;
    QStringList testFilenames;
    QString inspLoadfileName;

    bool m_bAcqStart = false;
    bool m_bStop = false;

    CPatternMatchModule m_cPatternModule;
};

#endif // ONEBODY_H
