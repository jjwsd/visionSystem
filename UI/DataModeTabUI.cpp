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

    //void * libHandle = dlopen("/home/nvidia/QT_Project/abcChild/libabc2.so", RTLD_LAZY);
    void * libHandle = dlopen(soFileName.toStdString().c_str(), RTLD_LAZY);
    if(libHandle  == NULL)
    {
        std::cout << "library Load Error!" << std::endl;
        std::cout << dlerror() << std::endl;
        return;
    }

    ABC* (*creator)() = (ABC*(*)())dlsym(libHandle, "create");
    ABC* p = (*creator)();
    p->show_message();

    void (*destructor)(ABC*) = (void(*)(ABC*))dlsym(libHandle, "destroy");
    (*destructor)(p);
    dlclose(libHandle);
}

#include <QCoreApplication>
#include <QTextStream>
void DataModeTabUI::cbDataLibMakeUserModule()
{
    QTextStream stream(stdout);
    stream << "void OneBody::on_dataLibMakeModuleBtn_clicked()";
}

