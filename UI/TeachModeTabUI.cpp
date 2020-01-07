#include "TeachModeTabUI.h"
#include "ui_OneBody.h"
#include "OneBody.h"
#include <S_Vision/CImageConverter.h>

#include <QFileDialog>
#include <QDir>
#include <QPixmap>
#include <QMessageBox>

int iResizeRatio = 4;

TeachModeTabUI::TeachModeTabUI(QObject *parent) : QObject(parent), m_PatternRect(nullptr),
    m_RoiRect(nullptr)
{

}

void TeachModeTabUI::cbTeachLiveBtnClicked()
{
    m_MainWindow->setCamStreamMode(CAM::LIVE_STREAM);
}

void TeachModeTabUI::cbTeachGrabBtnClicked()
{
    m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);
}

void TeachModeTabUI::cbTeachImageSaveBtnClicked()
{
    QString fileName = QFileDialog::getSaveFileName(m_MainWindow, tr("Save Image"),
                                                    "/home/nvidia/Pictures",
                                                    tr("Image File(*.png *.bmp *.jpg)"));
    QPixmap pixmap;
    if(m_MainWindow->grabQPixmap(pixmap))
    {
        emit m_MainWindow->sigSaveImgToWorkerThread(pixmap, fileName);
    }
}

void TeachModeTabUI::cbTeachImageLoadBtnClicked()
{
    m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);
    QPixmap temp;
    m_MainWindow->inspLoadfileName.clear();
    m_MainWindow->inspLoadfileName = QFileDialog::getOpenFileName(m_MainWindow, tr("Load Image"),
                                                                  "/home/nvidia/Pictures",
                                                                  tr("Image File(*.png *.bmp *.jpg)"));
    if (m_MainWindow->inspLoadfileName.isEmpty())
        return;
    temp.load(m_MainWindow->inspLoadfileName);
    m_MainWindow->g_Pixmap.load(m_MainWindow->inspLoadfileName);

    m_MainWindow->m_capturedImg = CImageConverter::QPixmapToCvMat(temp);
    m_MainWindow->m_Pixmap->setPixmap(temp);
    ui->graphicsView->scene()->setSceneRect(m_MainWindow->m_Pixmap->boundingRect());
    ui->graphicsView->fitInView(ui->graphicsView->scene()->sceneRect(), Qt::KeepAspectRatio);
}

void TeachModeTabUI::cbTeachROIShowBtnClicked()
{
#if 1
    NeptuneGetSizeInfo(m_MainWindow->m_CamHandle, &m_MainWindow->m_CamSizeInfo);

    int sizeWidth = m_MainWindow->m_CamSizeInfo.width, sizeHeight = m_MainWindow->m_CamSizeInfo.height;
    QRectF roiRect(sizeWidth/10, sizeHeight/10, (sizeWidth-sizeWidth/5), (sizeHeight-sizeHeight/5));

    if(m_MainWindow->m_pRectRoi == nullptr)
    {
        m_MainWindow->m_pRectRoi = new UserRectItem();
        qDebug() << "rectRoi = " << m_MainWindow->m_pRectRoi->rect().left();
        m_MainWindow->m_pRectRoi->setRect(roiRect);
        m_MainWindow->m_pRectRoi->SetLineColor(Qt::darkGreen);
        m_MainWindow->m_pRectRoi->setFlag(QGraphicsItem::ItemIsMovable);
        ui->graphicsView->scene()->addItem(m_MainWindow->m_pRectRoi);
        m_MainWindow->m_pRectRoi->show();
    }
#else
    int sizeWidth = 0;
    int sizeHeight = 0;
    if(m_MainWindow->m_Pixmap != nullptr)
    {
        sizeWidth = m_MainWindow->m_Pixmap->boundingRect().width();
        sizeHeight= m_MainWindow->m_Pixmap->boundingRect().height();
    }

    if(sizeWidth <=0 || sizeHeight <= 0 )
    {
        qWarning() << "main view pixmap is null!";
        return;
    }

    if(m_RoiRect == nullptr)
    {
        m_RoiRect = new CDragBox(sizeWidth/4, sizeHeight/4, Qt::red, QPoint(sizeWidth, sizeHeight));
        m_RoiRect->setPos(0,0);
        ui->graphicsView->scene()->addItem(m_RoiRect);
    }
#endif
}

void TeachModeTabUI::cbTeachROICancelBtnClicked()
{
#if 1
    if(m_MainWindow->m_pRectRoi != nullptr)
    {
        qDebug() << "!pRectROI->IsEmpty()";
        ui->graphicsView->scene()->removeItem(m_MainWindow->m_pRectRoi);

        delete m_MainWindow->m_pRectRoi;
        m_MainWindow->m_pRectRoi = nullptr;
    }
    else
    {
        qDebug() << "pRectROI->IsEmpty()";
    }
#else
    if(m_RoiRect != nullptr)
    {
        ui->graphicsView->scene()->removeItem(m_RoiRect);
        delete m_RoiRect;
        m_RoiRect = nullptr;
    }
    else
    {
        qDebug() << "pRectROI->IsEmpty()";
    }
#endif
}

void TeachModeTabUI::cbTeachSelectAlgoCombo(int value)
{
    ui->teachOptionTab->setCurrentIndex(value);
    m_MainWindow->p_ModelData->m_iAlgoType = value;
}

#include <Utility/dragbox.h>

void TeachModeTabUI::cbTeachPatternRectShowBtnClicked()
{
    int sizeWidth = 0;
    int sizeHeight = 0;
    if(m_MainWindow->m_Pixmap != nullptr)
    {
        sizeWidth = m_MainWindow->m_Pixmap->boundingRect().width();
        sizeHeight= m_MainWindow->m_Pixmap->boundingRect().height();
    }

    if(sizeWidth <=0 || sizeHeight <= 0 )
    {
        qWarning() << "main view pixmap is null!";
        return;
    }

    if(m_PatternRect == nullptr)
    {
        m_PatternRect = new CDragBox(sizeWidth/4, sizeHeight/4, Qt::green, QPoint(sizeWidth, sizeHeight));
        m_PatternRect->setPos(0,0);
        ui->graphicsView->scene()->addItem(m_PatternRect);
    }
}

void TeachModeTabUI::cbTeachPatternImageSaveBtnClicked()
{
    QPixmap temp;
    if(m_PatternRect != nullptr)
    {
        temp = m_MainWindow->m_Pixmap->pixmap().copy(m_PatternRect->getRectPosBySceneCoord().toRect());

        QString fileName = QFileDialog::getSaveFileName(m_MainWindow, tr("Save Image"),
                                                        "/home/nvidia/Pictures",
                                                        tr("Image File(*.png *.bmp *.jpg)"));
        QString name = QFileInfo(fileName).fileName();
        QDir path("/home/nvidia/Pictures");

        if(path.exists(name) == false)
        {
            path.mkdir(name);
            fileName += "/" + name;
        }

        qDebug() << fileName;

        if(fileName.contains(".png", Qt::CaseInsensitive))
        {
            temp.save(fileName,"png");
            m_MainWindow->p_ModelData->m_qsTemplate = fileName;
        }
        else
        {
            temp.save(fileName+".png","png");
            m_MainWindow->p_ModelData->m_qsTemplate = fileName+".png";
        }

        m_MainWindow->testTemplate = QFileInfo(m_MainWindow->p_ModelData->m_qsTemplate).fileName();
        m_MainWindow->testTemplatePath = QFileInfo(m_MainWindow->p_ModelData->m_qsTemplate).path();
        m_MainWindow->testTemplatePath += "/";

        cv::String tpl_folder(m_MainWindow->testTemplatePath.toStdString());
        cv::String template_name(m_MainWindow->testTemplate.toStdString());

        m_MainWindow->m_cPatternModule.InitPath(tpl_folder, template_name, iResizeRatio);

        QFile file(m_MainWindow->testTemplatePath + QString(m_MainWindow->m_cPatternModule.GetContourName().c_str()));
        if(file.exists())
        {
            qDebug() << "pattern Matching file is already exist!";
            return;
        }
        QMessageBox::information(m_MainWindow, tr("Pattern Matching Model GUIDE"),
                                 tr("1. Erase Model Area - Mouse L-Button Down & Drag \n"
                                    "2. Save Model - Mouse L-Button Double Click! "),
                                 QMessageBox::Ok);

        m_MainWindow->m_cPatternModule.MakeNewTemplate(iResizeRatio);
    }
    else
    {
        QMessageBox::information(m_MainWindow, tr("information"), "Check Pattern Area", QMessageBox::Close);
        return;
    }

    if(m_PatternRect != nullptr)
    {
        ui->graphicsView->scene()->removeItem(m_PatternRect);
        delete m_PatternRect;
        m_PatternRect = nullptr;
    }
}

void TeachModeTabUI::cbTeachPatternResizeValueChanged(QString rValue)
{
    iResizeRatio = rValue.toInt();
}

void TeachModeTabUI::cbTeachThresholdSliderPressed()
{
    if(m_MainWindow->m_eCamStreamMode == CAM::LIVE_STREAM)
    {
        m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);
        if(m_MainWindow->grabMat(m_MainWindow->m_capturedImg))
        {

        }
    }
}

void TeachModeTabUI::cbTeachCircleThresholdLowSliderValueChanged(int value)
{
    ui->teachCircleThreshLowSpin->setValue(value);
    ui->teachRectThreshLowSpin->setValue(value);
    m_MainWindow->p_ModelData->m_iThresholdLow = value;
    m_MainWindow->m_iLowValue = value;

    //cv::Mat matImage = CImageConverter::QPixmapToCvMat(g_Pixmap);

    CImageProcess imgProcess;

    cv::Mat destImg;
    imgProcess.BinarizeImage(m_MainWindow->m_capturedImg, destImg, m_MainWindow->m_iLowValue, m_MainWindow->m_iHighValue);
    cv::cvtColor(destImg, destImg, CV_GRAY2RGB);

    QImage img((uchar*)destImg.data,
               destImg.cols,
               destImg.rows,
               QImage::Format_RGB888);

    m_MainWindow->m_Pixmap->setPixmap(QPixmap::fromImage(img));
    ui->graphicsView->resize(ui->graphicsView->size());
    ui->graphicsView->scene()->setSceneRect(m_MainWindow->m_Pixmap->boundingRect());
    ui->graphicsView->update();
}

void TeachModeTabUI::cbTeachCircleThresholdLowSpinValueChanged(int value)
{
    ui->teachCircleThreshLowSlider->setValue(value);
    ui->teachRectThreshLowSlider->setValue(value);
}

void TeachModeTabUI::cbTeachCircleCreateBtnClicked()
{
    NeptuneGetSizeInfo(m_MainWindow->m_CamHandle, &m_MainWindow->m_CamSizeInfo);
    int sizeWidth = m_MainWindow->m_CamSizeInfo.width, sizeHeight=m_MainWindow->m_CamSizeInfo.height;
    QRectF roiRect(sizeWidth/4, sizeHeight/4, (sizeWidth-sizeWidth/2), (sizeHeight-sizeHeight/2));

    if(m_MainWindow->pRect == nullptr)
    {
        m_MainWindow->pRect = new UserRectItem();
        qDebug() << "rectRoi = " << m_MainWindow->pRect->rect().left();
        m_MainWindow->pRect->setRect(roiRect);
        m_MainWindow->pRect->SetLineColor(Qt::green);
        m_MainWindow->pRect->setFlag(QGraphicsItem::ItemIsMovable);
        ui->graphicsView->scene()->addItem(m_MainWindow->pRect);
        m_MainWindow->pRect->show();
    }
}

void TeachModeTabUI::cbTeachCircleCancelBtnClicked()
{
    if(m_MainWindow->pRect != nullptr)
    {
        qDebug() << "!pRect->IsEmpty()";
        ui->graphicsView->scene()->removeItem(m_MainWindow->pRect);

        delete m_MainWindow->pRect;
        m_MainWindow->pRect = nullptr;
    }
    else
    {
        qDebug() << "pRect->IsEmpty()";
    }
}

void TeachModeTabUI::cbTeachCircleGetBtnClicked()
{
    QRectF temp;
    temp = m_MainWindow->pRect->getRectPosBySceneCoord().toRect();


    qDebug() << temp.width();

    ui->teachCircleRadEdit->setText(QString::number(temp.width()/2));
}

void TeachModeTabUI::cbTeachCircleThresholdHighSliderValueChanged(int value)
{
    ui->teachCircleThreshHighSpin->setValue(value);
    ui->teachRectThreshHighSpin->setValue(value);
    m_MainWindow->p_ModelData->m_iThresholdHigh = value;
    m_MainWindow->m_iHighValue = value;

    CImageProcess imgProcess;

    cv::Mat destImg;
    imgProcess.BinarizeImage(m_MainWindow->m_capturedImg, destImg, m_MainWindow->m_iLowValue, m_MainWindow->m_iHighValue);
    cv::cvtColor(destImg, destImg, CV_GRAY2RGB);

    QImage img((uchar*)destImg.data,
               destImg.cols,
               destImg.rows,
               QImage::Format_RGB888);

    m_MainWindow->m_Pixmap->setPixmap(QPixmap::fromImage(img));
    ui->graphicsView->resize(ui->graphicsView->size());
    ui->graphicsView->scene()->setSceneRect(m_MainWindow->m_Pixmap->boundingRect());
    ui->graphicsView->update();
}

void TeachModeTabUI::cbTeachCircleThresholdHighSpinValueChanged(int value)
{
    ui->teachCircleThreshHighSlider->setValue(value);
    ui->teachRectThreshHighSlider->setValue(value);
}

void TeachModeTabUI::cbTeachModelSaveBtnClicked()
{
    QFileDialog *filDlg = new QFileDialog(m_MainWindow);
    QString fileNamePath =
            filDlg->getSaveFileName(m_MainWindow, tr("Save Inspection Model File"),
                                    QDir::currentPath(),
                                    tr("XML Files(*.xml)"));

    QString fileName = QFileInfo(fileNamePath).fileName();
    if (fileName.isEmpty())
        return;

    m_MainWindow->temp_save_model();
    m_MainWindow->xml.createXmlFile(fileName, m_MainWindow->p_ModelData);
}

void TeachModeTabUI::cbTeachModelLoadBtnClicked()
{
    QFileDialog *filDlg = new QFileDialog(m_MainWindow);
    QString fileNamePath =
            filDlg->getOpenFileName(m_MainWindow, tr("Open Inspection Model File"),
                                    QDir::currentPath(),
                                    tr("XML File(*.xml)"));
    if (fileNamePath.isEmpty())
        return;

    m_MainWindow->xml.openXmlFile(fileNamePath, m_MainWindow->p_ModelData);
    m_MainWindow->load_model();
    //ui->inspAlgoCombo->setCurrentIndex(p_ModelData->m_iAlgoType);
    //ui->lightOnCheckBox->setChecked((bool)p_ModelData->m_ilightEnable);
    //ui->lightValueEdit->setText(QString::number(p_ModelData->m_ilightValue));

    ui->tableWidgetAutoModuleList_2->clear();
    ui->tableWidgetAutoModuleList_2->setHorizontalHeaderItem(0, new QTableWidgetItem("Seq. Num"));
    ui->tableWidgetAutoModuleList_2->setHorizontalHeaderItem(1, new QTableWidgetItem("Vision Type"));
    ui->tableWidgetAutoModuleList_2->setRowCount(1);
    ui->tableWidgetAutoModuleList_3->clear();
    ui->tableWidgetAutoModuleList_3->setHorizontalHeaderItem(0, new QTableWidgetItem("Seq. Num"));
    ui->tableWidgetAutoModuleList_3->setHorizontalHeaderItem(1, new QTableWidgetItem("Vision Type"));
    ui->tableWidgetAutoModuleList_3->setRowCount(1);

    QTableWidgetItem *firstItem = new QTableWidgetItem();
    QTableWidgetItem *secondItem = new QTableWidgetItem();

    firstItem->setText("0");
    if(m_MainWindow->p_ModelData->m_iAlgoType==0)
        secondItem->setText("Pattern");
    else if(m_MainWindow->p_ModelData->m_iAlgoType==1)
        secondItem->setText("Circle");
    else if(m_MainWindow->p_ModelData->m_iAlgoType==2)
        secondItem->setText("Rect");

    ui->tableWidgetAutoModuleList_2->setItem(0, 0, firstItem);
    ui->tableWidgetAutoModuleList_2->setItem(0, 1, secondItem);

    QTableWidgetItem *first = new QTableWidgetItem();
    QTableWidgetItem *second = new QTableWidgetItem();

    first->setText("0");
    if(m_MainWindow->p_ModelData->m_iAlgoType==0)
        second->setText("Pattern");
    else if(m_MainWindow->p_ModelData->m_iAlgoType==1)
        second->setText("Circle");
    else if(m_MainWindow->p_ModelData->m_iAlgoType==2)
        second->setText("Rect");

    ui->tableWidgetAutoModuleList_3->setItem(0, 0, first);
    ui->tableWidgetAutoModuleList_3->setItem(0, 1, second);
}

void TeachModeTabUI::cbTeachModelTestBtnClicked()
{
    m_MainWindow->setCamStreamMode(CAM::LIVE_STOP);
    Mat input_img = cv::imread(m_MainWindow->inspLoadfileName.toStdString(),IMREAD_COLOR);
    Mat output_img;
    if(input_img.empty())
    {
        qDebug() << "input image null Error";
        return;
    }
    m_MainWindow->m_cPatternModule.RunVision(input_img, output_img);
    QImage img((uchar*)output_img.data,
               output_img.cols,
               output_img.rows,
               output_img.step,
               QImage::Format_RGB888);

    QPixmap tmpPixmap = QPixmap::fromImage(img);
    ui->autoImage->setPixmap(tmpPixmap);
}

void TeachModeTabUI::cbTeachSettingBtnClicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}





