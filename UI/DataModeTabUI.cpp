#include "DataModeTabUI.h"
#include "ui_OneBody.h"
#include "OneBody.h"

DataModeTabUI::DataModeTabUI(QObject *parent) : QObject(parent)
{

}

void DataModeTabUI::cbDataConnectBtnClicked()
{
    int temp = 0;
    QString ipAddresss = "opc.tcp://" + ui->dataIPEdit->toPlainText();
    m_MainWindow->g_opcUA.connect_server(ipAddresss.toUtf8().data());
    ui->dataStatusEdit->setPlainText(ipAddresss + " : Connecting.. ");
    temp = m_MainWindow->g_opcUA.connect_server(ipAddresss.toUtf8().data());
    if(temp == 1)
        ui->dataStatusEdit->setPlainText(ipAddresss + " Connected!!");
    else
        ui->dataStatusEdit->setPlainText(ipAddresss + " Connect Fail");
}

void DataModeTabUI::cbDataReadBtnClicked()
{
    QString tmp = ui->dataNameSpaceEdit->toPlainText();
    int iNamespace = tmp.toInt();
    tmp = ui->dataNodeIDEdit->toPlainText();
    std::string strResult = m_MainWindow->g_opcUA.read_node(iNamespace, tmp.toUtf8().data());
    ui->dataValueEdit->setPlainText(QString::fromStdString(strResult));
}

void DataModeTabUI::cbDataWriteBtnClicked()
{
    QString tmp = ui->dataNodeIDEdit->toPlainText();
    //g_opcUA.write_node(tmp.toUtf8().data());
}

void DataModeTabUI::cbDataDisconnectBtnClicked()
{
    m_MainWindow->g_opcUA.disconnect_server();
    ui->dataStatusEdit->setPlainText("DisConnected");
}

#include <dlfcn.h>
#include <abc.h>

void DataModeTabUI::cbDataLibLoadUserModule()
{
    QString soFileName = QFileDialog::getOpenFileName(m_MainWindow, tr("Load .so File"),
                                                      QDir::currentPath(),
                                                      tr("Library File(*.so)"));

    if(soFileName == QString::null)
    {
        return;
    }

    void * libHandle = dlopen(soFileName.toStdString().c_str(), RTLD_LAZY);
    if(libHandle  == NULL)
    {
        std::cout << "library Load Error!" << std::endl;
        std::cout << dlerror() << std::endl;
        return;
    }

    CVisionModule* (*creator)() = (CVisionModule*(*)())dlsym(libHandle, "create");
    CVisionModule* p = (*creator)();

    QString tmpName = p->GetName().c_str();
    ui->lbGetNameCheck->setStyleSheet("QLabel { background-color : red; color : white; }");
    ui->lbGetNameCheck->setText(QString(tmpName));

    p->TestName();
    cv::Mat temp;
    p->RunVision(temp,temp);

    void (*destructor)(CVisionModule*) = (void(*)(CVisionModule*))dlsym(libHandle, "destroy");
    (*destructor)(p);
    std::cout << "(*destructor)(p)";
    dlclose(libHandle);
}

#include <QCoreApplication>
#include <QTextStream>
void DataModeTabUI::cbDataLibMakeUserModule()
{
    QTextStream stream(stdout);
    stream << "void OneBody::on_dataLibMakeModuleBtn_clicked()";
}

void DataModeTabUI::cbDataServerCreateBtn()
{
    m_MainWindow->m_uaWorker->startServerSeq();
}

void DataModeTabUI::cbDataServerDeleteBtn()
{
    m_MainWindow->g_opcUA.ua_server_stop();
}

void DataModeTabUI::cbDataOpcuaSelected(int index)
{
    if(index == 2)
        ui->stackedWidget->setCurrentIndex(6);
    else if(index == 3)
        ui->stackedWidget->setCurrentIndex(2);
    else
        ui->stackedWidget->setCurrentIndex(5);
}

void DataModeTabUI::cbDataOutput1ONBtnClicked()
{
    jetsonTX2GPIONumber light = gpio388;
    gpioExport(light);
    gpioSetDirection(light,outputPin);
    gpioSetValue(light,on);
    //gpioUnexport(light);
}

void DataModeTabUI::cbDataOutput1OFFBtnClicked()
{
    jetsonTX2GPIONumber light = gpio388;
    gpioExport(light);
    gpioSetDirection(light,outputPin);
    gpioSetValue(light,off);
    //gpioUnexport(light);
}

void DataModeTabUI::cbDataOutput2ONBtnClicked()
{
    jetsonTX2GPIONumber output1 = gpio298;
    gpioExport(output1);
    gpioSetDirection(output1,outputPin);
    gpioSetValue(output1,on);
    //gpioUnexport(output1);
}

void DataModeTabUI::cbDataOutput2OFFBtnClicked()
{
    jetsonTX2GPIONumber output1 = gpio298;
    gpioExport(output1);
    gpioSetDirection(output1,outputPin);
    gpioSetValue(output1,off);
    //gpioUnexport(output1);
}

void DataModeTabUI::cbDataOutput3ONBtnClicked()
{
    jetsonTX2GPIONumber output2 = gpio480;
    gpioExport(output2);
    gpioSetDirection(output2,outputPin);
    gpioSetValue(output2,on);
    //gpioUnexport(output2);
}

void DataModeTabUI::cbDataOutput3OFFBtnClicked()
{
    jetsonTX2GPIONumber output2 = gpio480;
    gpioExport(output2);
    gpioSetDirection(output2,outputPin);
    gpioSetValue(output2,off);
    //gpioUnexport(output2);
}

