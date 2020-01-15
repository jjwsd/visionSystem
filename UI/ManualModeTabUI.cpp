#include "ManualModeTabUI.h"
#include "ui_OneBody.h"
#include "OneBody.h"

#include <VisionModule/ccirclemodule.h>
#include <VisionModule/ccircleblobmodule.h>
#include <S_Vision/CImageConverter.h>

#include <QDebug>
#include <QMessageBox>

ManualModeTabUI::ManualModeTabUI(QObject *parent) : QObject(parent)
{

}

void ManualModeTabUI::cbManualModelLoadBtnClicked()
{
    QFileDialog *filDlg = new QFileDialog(m_MainWindow);
    QString fileNamePath =
            filDlg->getOpenFileName(m_MainWindow, tr("Open Inspection Model File"),
                                    QDir::currentPath(),
                                    tr("XML File(*.xml)"));
    if (fileNamePath.isEmpty())
        return;

    m_MainWindow->xml.openXmlFile(fileNamePath, &m_ModelData);
    m_MainWindow->UpdatePatternModelUI(m_ModelData);

    updateModelUI(m_ModelData);

    ui->tableWidgetAutoModuleList_2->clear();
    ui->tableWidgetAutoModuleList_2->setHorizontalHeaderItem(0, new QTableWidgetItem("Seq. Num"));
    ui->tableWidgetAutoModuleList_2->setHorizontalHeaderItem(1, new QTableWidgetItem("Vision Type"));
    ui->tableWidgetAutoModuleList_2->setRowCount(1);

    QTableWidgetItem *firstItem = new QTableWidgetItem();
    QTableWidgetItem *secondItem = new QTableWidgetItem();

    firstItem->setText("0");
    if(m_ModelData.m_iAlgoType==0)
        secondItem->setText("Pattern");
    else if(m_ModelData.m_iAlgoType==1)
        secondItem->setText("Circle");
    else if(m_ModelData.m_iAlgoType==2)
        secondItem->setText("Rect");

    ui->tableWidgetAutoModuleList_2->setItem(0, 0, firstItem);
    ui->tableWidgetAutoModuleList_2->setItem(0, 1, secondItem);
}

void ManualModeTabUI::cbManualModelCancelBtnClicked()
{
    m_ModelData.init();
    //m_MainWindow->init_model_ui();
}

void ManualModeTabUI::cbManualImageLoadBtnClicked()
{
    ui->testListWidget->clear();
    m_loadImageNames.clear();
    QFileDialog *fileDlg = new QFileDialog(m_MainWindow);
    m_loadImageNames = fileDlg->getOpenFileNames(m_MainWindow, "Select file",
                                                            "/home/nvidia/Pictures",
                                                            tr("Image File(*.png *.bmp *.jpg)"));
    if(m_loadImageNames.empty())
        return;
    for(int i=0; i<m_loadImageNames.size(); i++)
    {
        ui->testListWidget->addItem(QFileInfo(m_loadImageNames.at(i)).fileName());
    }
    m_MainWindow->testFilePath = QFileInfo(m_loadImageNames.at(0)).path();
}

void ManualModeTabUI::cbManualImageClearBtnClicked()
{
    ui->testListWidget->clear();
    m_loadImageNames.clear();
    m_MainWindow->testFilePath.clear();
}

void ManualModeTabUI::cbManualRunBtnClicked()
{
    ui->testResultTable->clear();

    if(m_ModelData.m_iAlgoType == VISION::PATTERN)
        pattern_matching();
    else if(m_ModelData.m_iAlgoType == VISION::CIRCLE)
        circle_algorithm();
    else if(m_ModelData.m_iAlgoType == VISION::RECTANGLE)
        rect_algorithm();
}


void ManualModeTabUI::rect_algorithm()
{

}

void ManualModeTabUI::cbManualResultTableCellPressed(int row, int col)
{
    m_MainWindow->sigUpdateMainViewer(m_vManualTabImgList.at(row));
    qApp->processEvents();
}

void ManualModeTabUI::updateModelUI(CModelData modelData)
{
    if(m_ModelData.m_iAlgoType == VISION::CIRCLE)
    {
        ui->manualLabel1->setText("Radius");
        ui->manualRadWidthLabel->setText(QString::number(m_ModelData.m_iRadius));
        ui->manualThreshLowLabel->setText(QString::number(m_ModelData.m_iThresholdLow));
        ui->manualThreshHighLabel->setText(QString::number(m_ModelData.m_iThresholdHigh));
        ui->manualTolLabel->setText(QString::number(m_ModelData.m_iTolerance));
        ui->manualNoLabel->setText(QString::number(m_ModelData.m_iTargetNo));
    }
    else if(m_ModelData.m_iAlgoType == VISION::RECTANGLE)
    {
        ui->manualLabel1->setText("Width");
        ui->manualLabel2->setText("Height");
        ui->manualRadWidthLabel->setText(QString::number(m_ModelData.m_iWidth));
        ui->manualHeightLabel->setText(QString::number(m_ModelData.m_iHeight));
        ui->manualThreshLowLabel->setText(QString::number(m_ModelData.m_iThresholdLow));
        ui->manualThreshHighLabel->setText(QString::number(m_ModelData.m_iThresholdHigh));
        ui->manualTolLabel->setText(QString::number(m_ModelData.m_iTolerance));
        ui->manualNoLabel->setText(QString::number(m_ModelData.m_iTargetNo));
    }
}

void ManualModeTabUI::circle_blob_algorithm()
{
    m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);

    for(int i=0; i<m_loadImageNames.size(); i++)
    {
        CCircleBlobModule visionModule;
        QPixmap temp;
        temp.load(m_loadImageNames.at(i));

        qDebug() << m_loadImageNames.at(i);

        Mat dispImg;
        CCircleBlobParams params;
        params.iRadius = m_ModelData.m_iRadius;
        params.dTolerance = m_ModelData.m_iTolerance * 0.1;
        params.iThresholdHigh = m_ModelData.m_iThresholdHigh;
        params.iThresholdLow = m_ModelData.m_iThresholdLow;

        visionModule.SetParams(params);
        visionModule.m_bDebugMode = true;

        Mat srcImg = CImageConverter::QPixmapToCvMat(temp);
        visionModule.RunVision(srcImg, dispImg);

        cv::cvtColor(dispImg, dispImg, CV_BGR2RGB);

        QImage img((uchar*)dispImg.data,
                   dispImg.cols,
                   dispImg.rows,
                   QImage::Format_RGB888);

        emit m_MainWindow->sigUpdateMainViewer(QPixmap::fromImage(img));
        qApp->processEvents();
    }
}

void ManualModeTabUI::circle_algorithm()
{
    m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);
    clock_t start;
    clock_t end;
    double img_preprocess_t = 0.0;
    QTableWidgetItem *centerPt = new QTableWidgetItem();
    //ui->graphicsView->scene()->clear();

    QStringList testFilenames;

    for(int i=0; i<testFilenames.size(); i++)
    {
        CCircleModule visionModule;
        CVisionAgentResult visionResult;
        QPixmap temp;
        temp.load(testFilenames.at(i));

        qDebug() << testFilenames.at(i);

        Mat dispImg;
        CCircleParams params;
        params.iRadius = m_ModelData.m_iRadius;
        params.dTolerance = m_ModelData.m_iTolerance * 0.1;
        params.iThresholdHigh = m_ModelData.m_iThresholdHigh;
        params.iThresholdLow = m_ModelData.m_iThresholdLow;

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
        cv::Point2f tmpCenterPt;
        if(!visionResult.GetCenterPoint(tmpCenterPt))
        {
            continue;
        }


        QString tmp = QString::number(tmpCenterPt.x) + ", "
                + QString::number(tmpCenterPt.y);
        centerPt->setText(tmp);

        if(visionResult.bOk == true)
        {
            ui->testResultTable->setItem(i,0,centerPt);
        }
        else
            QMessageBox::information(m_MainWindow, tr("information"), "No Search Hole", QMessageBox::Close);

        //m_Pixmap->setPixmap(QPixmap::fromImage(img));
        emit m_MainWindow->sigUpdateMainViewer(QPixmap::fromImage(img));
        qApp->processEvents();
        qDebug() << "finished";
    }

}

void ManualModeTabUI::pattern_matching()
{
    m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);
    QString testTemplate;
    QString testTemplatePath;
    QStringList testFilenames;
    testTemplate = QFileInfo(m_ModelData.m_qsTemplate).fileName();
    testTemplatePath = QFileInfo(m_ModelData.m_qsTemplate).path();
    testTemplatePath += "/";

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
    CPatternMatchModule patternModule;
    patternModule.InitPath(templateFolder, templateName, m_ModelData.m_iResize);
    // must be fixed by value!

    QFile file(testTemplatePath + QString(patternModule.GetContourName().c_str()));
    if(!file.exists())
    {
        QMessageBox::information(m_MainWindow, tr("Pattern Matching Model GUIDE"),
                                 tr("1. Erase Model Area - Mouse L-Button Down & Drag \n"
                                    "2. Save Model - Mouse L-Button Double Click! "),
                                 QMessageBox::Ok);
        patternModule.MakeNewTemplate(m_ModelData.m_iResize);
    }
    //m_ImgProcessWorker->m_pWorkerModule = &m_cPatternModule;

    qDebug() << "image file size " << filenames.size();

    cv::Mat input_img;
    clock_t start;
    clock_t end;

    double img_preprocess_t = 0.0;
    Mat output_img;
    int i=0;
    m_vManualTabImgList.clear();

    ui->testResultTable->clear();
    ui->testResultTable->setHorizontalHeaderItem(0, new QTableWidgetItem("Center Point"));
    ui->testResultTable->setHorizontalHeaderItem(1, new QTableWidgetItem("Angle"));
    ui->testResultTable->setHorizontalHeaderItem(2, new QTableWidgetItem("Tact Time"));
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
        visionResult = patternModule.RunVision(input_img, output_img);
        end = clock();

        img_preprocess_t += (double)(end - start) / CLOCKS_PER_SEC;

        cv::Point2f centerPt;
        double dAngle;


        if(!visionResult.GetCenterPoint(centerPt))
        {
            continue;
        }

        QString strCPX = QString::number(centerPt.x);
        QString strCPY = QString::number(centerPt.y);
        QString strCP = strCPX + ", " + strCPY;

        if(!visionResult.GetAngle(dAngle))
        {
            continue;
        }

        QString strAngle = QString::number(dAngle* (180/3.14),'f',2);
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
        m_vManualTabImgList.push_back(tmpPixmap);

        emit m_MainWindow->sigUpdateMainViewer(tmpPixmap);
        ui->graphicsView->scene()->setSceneRect(m_MainWindow->m_Pixmap->boundingRect());
        ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
        qApp->processEvents();
#endif
        i++;
    }
    qDebug("TIME : %5.2fs ", img_preprocess_t);
}

