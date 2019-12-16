#-------------------------------------------------
#
# Project created by QtCreator 2019-07-26T11:08:37
#
#-------------------------------------------------

QT       += core gui xml
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OneBody
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        CXmlParser.cpp \
        main.cpp \
        OneBody.cpp \
    H_Camera/cameradata.cpp \
    UI/userrectitem.cpp \
    CModelData.cpp \    
    S_Vision/CImageConverter.cpp \
    S_Vision/CImageProcess.cpp \
    VisionModule/CVisionCommon.cpp \
    VisionModule/CVisionAgent.cpp \
    VisionModule/CVisionModule.cpp \
    VisionModule/ccirclemodule.cpp \
    VisionModule/cPatternMatchModule.cpp \
    COpcUa.cpp \
    VisionModule/ccircleblobmodule.cpp \
    WorkerThread/ImageProcessWorker.cpp \
    WorkerThread/ImageSaveWorker.cpp \
    VisionModule/CVisionModuleMgr.cpp \
    WorkerThread/LogManagerWorker.cpp \
    logger.cpp \
    UI/AutoModeTabUI.cpp \
    UI/TabUI.cpp \
    UI/TeachModeTabUI.cpp \
    UI/ManualModeTabUI.cpp \
    UI/DataModeTabUI.cpp

HEADERS += \
        CXmlParser.h \
        OneBody.h \
    H_Camera/cameradata.h \
    UI/userrectitem.h \
    CModelData.h \    
    S_Vision/CImageConverter.h \
    S_Vision/CImageProcess.h \
    VisionModule/CVisionCommon.h \
    VisionModule/CVisionAgent.h \
    VisionModule/CVisionModule.h \
    VisionModule/ccirclemodule.h \
    VisionModule/cPatternMatchModule.h \
    COpcUa.h \
    VisionModule/ccircleblobmodule.h \
    VisionModule/cusermodule.h \
    abc.h \
    Data/definedata.h \
    WorkerThread/ImageProcessWorker.h \
    WorkerThread/ImageSaveWorker.h \
    VisionModule/CVisionModuleMgr.h \
    WorkerThread/LogManagerWorker.h \
    logger.h \
    UI/AutoModeTabUI.h \
    UI/TabUI.h \
    UI/TeachModeTabUI.h \
    UI/ManualModeTabUI.h \
    UI/DataModeTabUI.h

FORMS += \
        OneBody.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

unix{
    #INCLUDEPATH += "/usr/include/gstreamer-1.0"
    #INCLUDEPATH += "/usr/lib/aarch64-linux-gnu/gstreamer-1.0/include"
    #INCLUDEPATH += "/usr/include/glib-2.0"
    #INCLUDEPATH += "/usr/lib/aarch64-linux-gnu/glib-2.0/include"
    INCLUDEPATH += /usr/local/include/neptune
    INCLUDEPATH += "/usr/include"
    INCLUDEPATH += "/usr/include/opencv"
    INCLUDEPATH += "/usr/include/opencv2"
    INCLUDEPATH += "$${PWD}/include"
    INCLUDEPATH += "/usr/include/open62541"    
}

unix{
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lgirepository-1.0
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lgobject-2.0
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lglib-2.0
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lgstapp-1.0
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lgstvideo-1.0
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lgstbase-1.0
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lgstreamer-1.0
    #LIBS += "-L/usr/lib/aarch64-linux-gnu/gstreamer-1.0/" -lgstclutter-3.0
    LIBS += "-L/usr/lib/" -lopencv_calib3d
    LIBS += "-L/usr/lib/" -lopencv_core
    LIBS += "-L/usr/lib/" -lopencv_features2d
    LIBS += "-L/usr/lib/" -lopencv_flann
    LIBS += "-L/usr/lib/" -lopencv_highgui
    LIBS += "-L/usr/lib/" -lopencv_imgcodecs
    LIBS += "-L/usr/lib/" -lopencv_imgproc
    LIBS += "-L/usr/lib/" -lopencv_ml
    LIBS += "-L/usr/lib/" -lopencv_objdetect
    LIBS += "-L/usr/lib/" -lopencv_photo
    LIBS += "-L/usr/lib/" -lopencv_shape
    LIBS += "-L/usr/lib/" -lopencv_stitching
    LIBS += "-L/usr/lib/" -lopencv_superres
    LIBS += "-L/usr/lib/" -lopencv_video
    LIBS += "-L/usr/lib/" -lopencv_videoio
    LIBS += "-L/usr/lib/" -lopencv_videostab
    LIBS += "-L/usr/local/lib" -lNeptuneAPI
    LIBS += "-L/usr/lib/aarch64-linux-gnu/" -lopen62541
    LIBS += "-L/usr/local/lib" -lPatternMatch
    LIBS += -ldl
}
