#include <QDebug>
#include <QApplication>
#include <QDir>
#include "CXmlParser.h"

CXmlParser::CXmlParser()
{

}

void CXmlParser::createXmlFile(QString fileName, CModelData m_ModelData)
{
    QString temp;

    if(fileName.contains(".xml", Qt::CaseInsensitive))
        temp = fileName;
    else
        temp = fileName + ".xml";

    QFile file(temp);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writting";
    }
    else {
        doc.clear();
        root. clear();

        root = doc.createElement("Model");
        doc.appendChild(root);

        QDomElement algoType = doc.createElement("algoType");
        QDomElement lightEnable = doc.createElement("lightEnable");
        QDomElement templatePath = doc.createElement("templatePath");
        QDomElement thresholdLow = doc.createElement("thresholdLow");
        QDomElement thresholdHigh = doc.createElement("thresholdHigh");
        QDomElement targetNo = doc.createElement("targetNo");
        QDomElement tolerance = doc.createElement("tolerance");
        QDomElement radius = doc.createElement("radius");
        QDomElement width = doc.createElement("width");
        QDomElement height = doc.createElement("height");
        QDomElement resize = doc.createElement("resize");
        QDomElement startx = doc.createElement("startx");
        QDomElement starty = doc.createElement("starty");
        QDomElement endx = doc.createElement("endx");
        QDomElement endy = doc.createElement("endy");
        QDomElement matchRate = doc.createElement("matchRate");
        QDomElement originX = doc.createElement("originX");
        QDomElement originY = doc.createElement("originY");
        QDomElement originAngle = doc.createElement("originAngle");

        algoType.setAttribute("type", (int)m_ModelData.m_iAlgoType);
        lightEnable.setAttribute("status", (int)m_ModelData.m_ilightEnable);
        templatePath.setAttribute("path", (QString)m_ModelData.m_qsTemplate);
        thresholdLow.setAttribute("value", (int)m_ModelData.m_iThresholdLow);
        thresholdHigh.setAttribute("value", (int)m_ModelData.m_iThresholdHigh);
        targetNo.setAttribute("value", (int)m_ModelData.m_iTargetNo);
        tolerance.setAttribute("value", (int)m_ModelData.m_iTolerance);
        radius.setAttribute("value", (float)m_ModelData.m_iRadius);
        width.setAttribute("value", (int)m_ModelData.m_iWidth);
        height.setAttribute("value", (int)m_ModelData.m_iHeight);
        resize.setAttribute("value", (int)m_ModelData.m_iResize);
        startx.setAttribute("value", (int)m_ModelData.m_iStartX);
        starty.setAttribute("value", (int)m_ModelData.m_iStartY);
        endx.setAttribute("value", (int)m_ModelData.m_iEndX);
        endy.setAttribute("value", (int)m_ModelData.m_iEndY);
        matchRate.setAttribute("value", (int)m_ModelData.m_iMatchRate);
        originX.setAttribute("value", (float)m_ModelData.m_fOriginX);
        originY.setAttribute("value", (float)m_ModelData.m_fOriginY);
        originAngle.setAttribute("value", (float)m_ModelData.m_fOriginAngle);

        root.appendChild(algoType);
        root.appendChild(lightEnable);
        root.appendChild(templatePath);
        root.appendChild(thresholdLow);
        root.appendChild(thresholdHigh);
        root.appendChild(targetNo);
        root.appendChild(tolerance);
        root.appendChild(radius);
        root.appendChild(width);
        root.appendChild(height);
        root.appendChild(resize);
        root.appendChild(startx);
        root.appendChild(starty);
        root.appendChild(endx);
        root.appendChild(endy);
        root.appendChild(matchRate);
        root.appendChild(originX);
        root.appendChild(originY);
        root.appendChild(originAngle);

        QTextStream stream(&file);
        stream << doc.toString();

        file.close();
        doc.clear();
        qDebug() << "Finished";
    }
}

void CXmlParser::openXmlFile(QString fileNamePath, CModelData* p_ModelData)
{
    QFile file(fileNamePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open file";
    }
    else
    {
        if(!doc.setContent(&file))
            qDebug() << "Failed to load document";
    }
    file.close();

    //get the root element
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();
    while(!node.isNull()){
        QDomElement element = node.toElement();
        if(!element.isNull())
        {
            if(element.tagName() == "algoType")
                p_ModelData->m_iAlgoType = element.attribute("type").toInt();
            if(element.tagName() == "lightEnable")
                p_ModelData->m_ilightEnable = element.attribute("status").toInt();
            if(element.tagName() == "templatePath")
                p_ModelData->m_qsTemplate = element.attribute("path");
            if(element.tagName() == "thresholdLow")
                p_ModelData->m_iThresholdLow = element.attribute("value").toInt();
            if(element.tagName() == "thresholdHigh")
                p_ModelData->m_iThresholdHigh = element.attribute("value").toInt();
            if(element.tagName() == "targetNo")
                p_ModelData->m_iTargetNo = element.attribute("value").toInt();
            if(element.tagName() == "tolerance")
                p_ModelData->m_iTolerance = element.attribute("value").toInt();
            if(element.tagName() == "radius")
                p_ModelData->m_iRadius = element.attribute("value").toFloat();
            if(element.tagName() == "width")
                p_ModelData->m_iWidth = element.attribute("value").toInt();
            if(element.tagName() == "height")
                p_ModelData->m_iHeight = element.attribute("value").toInt();
            if(element.tagName() == "resize")
                p_ModelData->m_iResize = element.attribute("value").toInt();
            if(element.tagName() == "startx")
                p_ModelData->m_iStartX = element.attribute("value").toInt();
            if(element.tagName() == "starty")
                p_ModelData->m_iStartY = element.attribute("value").toInt();
            if(element.tagName() == "endx")
                p_ModelData->m_iEndX = element.attribute("value").toInt();
            if(element.tagName() == "endy")
                p_ModelData->m_iEndY = element.attribute("value").toInt();
            if(element.tagName() == "matchRate")
                p_ModelData->m_iMatchRate = element.attribute("value").toInt();
            if(element.tagName() == "originX")
                p_ModelData->m_fOriginX = element.attribute("value").toFloat();
            if(element.tagName() == "originY")
                p_ModelData->m_fOriginY = element.attribute("value").toFloat();
            if(element.tagName() == "originAngle")
                p_ModelData->m_fOriginAngle = element.attribute("value").toFloat();
        }
        node = node.nextSibling();
    }
    qDebug() << "Finished";
}
