#include "ManualModeTabUI.h"
#include "ui_OneBody.h"
#include "OneBody.h"

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
    m_MainWindow->LoadModelData(m_ModelData);
    //ui->inspAlgoCombo->setCurrentIndex(p_ModelData->m_iAlgoType);
    //ui->lightOnCheckBox->setChecked((bool)p_ModelData->m_ilightEnable);
    //ui->lightValueEdit->setText(QString::number(p_ModelData->m_ilightValue));

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
    m_MainWindow->testFilenames.clear();
    QFileDialog *fileDlg = new QFileDialog(m_MainWindow);
    m_MainWindow->testFilenames = fileDlg->getOpenFileNames(m_MainWindow, "Select file",
                                              "/home/nvidia/Pictures",
                                              tr("Image File(*.png *.bmp *.jpg)"));
    if(m_MainWindow->testFilenames.empty())
        return;
    for(int i=0; i<m_MainWindow->testFilenames.size(); i++)
    {
        ui->testListWidget->addItem(QFileInfo(m_MainWindow->testFilenames.at(i)).fileName());
    }
    m_MainWindow->testFilePath = QFileInfo(m_MainWindow->testFilenames.at(0)).path();
}

void ManualModeTabUI::cbManualImageClearBtnClicked()
{
    ui->testListWidget->clear();
    m_MainWindow->testFilenames.clear();
    m_MainWindow->testFilePath.clear();
}

void ManualModeTabUI::cbManualRunBtnClicked()
{
    ui->testResultTable->clear();

    if(m_ModelData.m_iAlgoType == 0)
        m_MainWindow->pattern_matching();
    else if(m_ModelData.m_iAlgoType == 1)
        m_MainWindow->circle_algorithm();
    else if(m_ModelData.m_iAlgoType == 2)
        m_MainWindow->rect_algorithm();
}

void ManualModeTabUI::cbManualResultTableCellPressed(int row, int col)
{
    m_MainWindow->sigUpdateMainViewer(m_MainWindow->m_vTestTabImages.at(row));
    qApp->processEvents();
}

