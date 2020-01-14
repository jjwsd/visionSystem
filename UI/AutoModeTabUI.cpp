#include "AutoModeTabUI.h"
#include "ui_OneBody.h"
#include "OneBody.h"

#include <QFileDialog>
#include <CXmlParser.h>

AutoModeTabUI::AutoModeTabUI(QObject *parent) : QObject(parent)
{

}

void AutoModeTabUI::cbOpenAutoModuleBtnClicked()
{
    //OneBody * m_MainWindow = static_cast<OneBody *>(QApplication::activeWindow());
    QFileDialog *fileDlg = new QFileDialog(m_MainWindow);
    fileDlg->setNameFilter(tr("XML File(*.xml)"));
    fileDlg->setViewMode(QFileDialog::Detail);
    fileDlg->setFileMode(QFileDialog::ExistingFiles);

    if(fileDlg->exec() == QDialog::Accepted)
    {
        m_MainWindow->m_cVisionModuleMgr.m_VisionModuleMap.clear();
        QStringList fileNames = fileDlg->selectedFiles();
        QString xmlName;
        QStringList::const_iterator constIterator;
        int i = 0;
        for (constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd();++constIterator)
        {
            xmlName = (*constIterator).toLocal8Bit().constData();
            m_MainWindow->xml.openXmlFile(xmlName, &m_ModelData);

            m_MainWindow->m_Roi = QRect(m_ModelData.m_iStartX, m_ModelData.m_iStartY, m_ModelData.m_iEndX, m_ModelData.m_iEndY);

            if(m_ModelData.m_iAlgoType == VISION::PATTERN)
            {
                QString testTemplate = QFileInfo(m_ModelData.m_qsTemplate).fileName();
                QString testTemplatePath = QFileInfo(m_ModelData.m_qsTemplate).path();
                testTemplatePath += "/";

                cv::String templateFolder(testTemplatePath.toStdString());
                cv::String templateName(testTemplate.toStdString());

                CPatternMatchModule * pPatternMatch = new CPatternMatchModule();
                pPatternMatch->InitPath(templateFolder, templateName, m_ModelData.m_iResize);

                m_MainWindow->m_cVisionModuleMgr.m_VisionModuleMap.insert(std::make_pair(i++, pPatternMatch));
            }
        }
    }
    QPixmap pixmap;
    QPixmap canny;
    QString tempPath = QFileInfo(m_ModelData.m_qsTemplate).path();
    pixmap.load(m_ModelData.m_qsTemplate);
    canny.load(tempPath + QString("/contour_def_canny_totally.bmp"));

    ui->manualPatternImage->setPixmap(pixmap);
    ui->manualCannyImage->setPixmap(canny);

    ui->tableWidgetAutoModuleList->clear();
    ui->tableWidgetAutoModuleList->setHorizontalHeaderItem(0, new QTableWidgetItem("Seqeunce Num"));
    ui->tableWidgetAutoModuleList->setHorizontalHeaderItem(1, new QTableWidgetItem("Vision Type"));
    ui->tableWidgetAutoModuleList->setRowCount(m_MainWindow->m_cVisionModuleMgr.m_VisionModuleMap.size());

    QTableWidgetItem *firstItem;
    QTableWidgetItem *secondItem;
    unsigned int iCount = 0;
    for (auto var = m_MainWindow->m_cVisionModuleMgr.m_VisionModuleMap.begin(); var != m_MainWindow->m_cVisionModuleMgr.m_VisionModuleMap.end(); ++var) {
        firstItem = new QTableWidgetItem(tr("%1").arg(var->first));
        secondItem = new QTableWidgetItem(tr("%1").arg(var->second->GetName().c_str()));
        ui->tableWidgetAutoModuleList->setItem(iCount, 0, firstItem);
        ui->tableWidgetAutoModuleList->setItem(iCount, 1, secondItem);
        ++iCount;
    }
}

void AutoModeTabUI::cbRunAutoModuleBtnClicked()
{
    m_MainWindow->m_lAutoVisionResult.clear();
    if(m_MainWindow->m_bAutoModeStart == false)
    {
        m_MainWindow->m_bAutoModeStart = true;
        m_MainWindow->m_ImgProcessWorker->m_pWorkerModule = m_MainWindow->m_cVisionModuleMgr.m_VisionModuleMap.begin()->second;
        emit m_MainWindow->sigCheckTriggerStart();
    }
    else
    {
        m_MainWindow->m_bAutoModeStart = false;
    }
}

void AutoModeTabUI::cbSWTriggerBtnClicked()
{
    if(m_MainWindow->m_bAutoModeStart)
    {
        m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);
        Mat img;
        if(m_MainWindow->m_Roi.right() <= 0 || m_MainWindow->m_Roi.bottom() <= 0)
        {
            if(m_MainWindow->grabMat(img))
            {
                emit m_MainWindow->sigSendMatImgToWorkerThread(img);
            }
        }
        else
        {
            if(m_MainWindow->grabMatByRoi(img, m_MainWindow->m_Roi))
            {
                emit m_MainWindow->sigSendMatImgToWorkerThread(img);
            }
        }
        ui->autoTriggerBtn->setStyleSheet("color: rgb(180,180,180);");
        ui->autoTriggerBtn->setEnabled(false);
    }
}

void AutoModeTabUI::cbSWTriggerBtnEnabled()
{
    ui->autoTriggerBtn->setStyleSheet("color: rgb(255, 255, 255);"
                                      "background-color: rgb(40, 0, 140);"
                                      "border-width: 1px;"
                                      "padding: 5px;"
                                      "border-radius: 5px;");
    ui->autoTriggerBtn->setEnabled(true);
}

