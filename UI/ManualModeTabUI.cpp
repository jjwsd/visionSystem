#include "ManualModeTabUI.h"
#include "ui_OneBody.h"
#include "OneBody.h"

ManualModeTabUI::ManualModeTabUI(QObject *parent) : QObject(parent)
{

}

void ManualModeTabUI::cbManualModelCancelBtnClicked()
{
    m_MainWindow->p_ModelData->init();
    m_MainWindow->init_model_ui();
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

    if(m_MainWindow->p_ModelData->m_iAlgoType == 0)
        m_MainWindow->pattern_matching();
    else if(m_MainWindow->p_ModelData->m_iAlgoType == 1)
        m_MainWindow->circle_algorithm();
    else if(m_MainWindow->p_ModelData->m_iAlgoType == 2)
        m_MainWindow->rect_algorithm();
}

void ManualModeTabUI::cbManualResultTableCellPressed(int row, int col)
{
    m_MainWindow->sigUpdateMainViewer(m_MainWindow->m_vTestTabImages.at(row));
    qApp->processEvents();
}

