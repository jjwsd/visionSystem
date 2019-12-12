#include <QDomDocument>
#include <QFile>
#include <QFileDialog>
#include <QObject>

#include <CModelData.h>

#ifndef CXMLPARSER_H
#define CXMLPARSER_H

class CModelData;
class CXmlParser
{
public:
    CXmlParser();
    void createXmlFile(QString fileName, const CModelData * p_ModelData);
    //void createXmlFile(QString fileName, CModelData p_ModelData);
    void openXmlFile(QString fileNamePath, CModelData * p_ModelData);
    QDomDocument doc;
    QDomElement root;
    QString m_qsPath;
};

#endif // CXMLPARSER_H
