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

class CameraData;
class CModelData;
class CXmlParser;
class COpcUa;

namespace Ui {
class OneBody;
}

class OneBody : public QMainWindow
{
    Q_OBJECT

public:
    explicit OneBody(QWidget *parent = nullptr);
    ~OneBody();

    static void imi_cam_frame_cb(PNImageFrame pImgFrame);

    static CameraData * pFrameData;
    static bool  bIsAutoMode;
    static CAM::CamStreamMode  m_eCamStreamMode;
    static QVector<QRgb> sColorTable;
    static QMutex * m_GrabImg;    

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

    void setCamStreamMode(CAM::CamStreamMode streamMode);
    void delay(const int milliSeconds);

    void convertToQImageRgbFormat(unsigned char *pBuffer, unsigned int nW, unsigned int nH, QImage& image);

    bool grabQPixmap(QPixmap& image);
    bool grabMat(Mat& matImg);

    bool grabQPixmapByRoi(QPixmap& image, QRect& roi);
    bool grabMatByRoi(Mat& matImg, QRect& roi);

    QThread m_imgSaveThread;
    QThread m_imgProcThread;
    QThread m_logThread;

    ImageProcessWorker * m_ImgProcessWorker;

    QRect m_Roi;

    //Logger resultLogger;

private slots:
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

    void on_clicked_data_save_btn();

    void on_clicked_data_open_btn();

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

    void add_framerate_list(int value);

    void on_clicked_resolution_applyBtn();

    void select_teach_algo_tab(int value);

    void teachGrabBtnClicked();

    void teachImageSaveBtnClicked();

    void teachLiveBtnClicked();

    void teachTestBtnClicked();

    void set_roi();

    void delete_roi();

    void set_pattern_area();

    void delete_pattern_area();

    void set_rect();

    void delete_rect();

    void cam_connect_status();

    void on_test_templete_load_btn();

    void pattern_matching();

    void test_delete_template();

    void test_delete_images();

    void show_out_images(int row, int col);

    void on_teach_temp_save_btn();

    void teachSettingBtnClicked();

    void settingBackBtnClicked();

    void set_threshold_low_slider(int value);

    void set_threshold_low_spin(int value);

    void set_threshold_high_slider(int value);

    void set_threshold_high_spin(int value);

    void get_radius();

    void load_model();

    void temp_save_model();

    void testTabStartTest();

    void circle_algorithm();

    void circle_blob_algorithm();

    void rect_algorithm();

    void cancel_model();

    void init_model_ui();

    void on_dataStartBtn_clicked();

    void on_dataReadBtn_clicked();

    void on_dataDisconnectBtn_clicked();

    void on_dataLibLoadModuleBtn_clicked();

    void on_dataLibMakeModuleBtn_clicked();

    void on_dataWriteBtn_clicked();

    void automode_openModuleBtn_clicked();

    void automode_runAutoBtn_clicked();

    void automode_triggerBtn_clicked();

    void automode_triggerBtn_enable();

    void cbAutoTabGetVisionProcessResult(_MatImg mat, CVisionAgentResult result);
    void cbTestTabGetVisionProcessResult(_MatImg mat, CVisionAgentResult result);
    void cbUpdateMainViewer(QPixmap);
    void cbUpdateAutoModeImage();

    void resize_value(QString rValue);

    void teachThreshHoldSliderPressed();

    void logtabComboBoxSelected(int item);

signals:
    void sigUpdateMainViewer(QPixmap);

    void sigSendMatImgToWorkerThread(_MatImg p);
    void sigSaveImgToWorkerThread(QPixmap img, const QString &fileName);

    void sigSendTestMatImgToWorkerThread(_MatImg p);

    void sigTestMSg(const QtMsgType& type, const QString & tag, const QString & msg);

private:
    Ui::OneBody *ui;
    CXmlParser xml;
    CModelData * p_ModelData;
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
    UserRectItem * pRectModel;
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
    bool m_bAutoModeStart = false;

    CPatternMatchModule m_cPatternModule;

    CVisionModuleMgr m_cVisionModuleMgr;

    QTimer* m_pTimerAutoMode;

    std::list<CVisionAgentResult> m_lAutoVisionResult;
};

#endif // ONEBODY_H
