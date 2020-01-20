#ifndef CMODELDATA_H
#define CMODELDATA_H

#include <QString>

enum eIMIFormat{
    BAYERGR8 = 0,
    MONO8 = 1
};


class CModelData
{
public:
    CModelData();

    int m_iAlgoType;
    int m_ilightEnable;
    QString m_qsTemplate;
    int m_iThresholdLow;
    int m_iThresholdHigh;
    int m_iTargetNo;
    float m_iTolerance;
    float m_iRadius;
    int m_iWidth;
    int m_iHeight;
    int m_iResize;
    int m_iStartX;
    int m_iStartY;
    int m_iEndX;
    int m_iEndY;
    int m_iMatchRate;
    float m_fOriginX;
    float m_fOriginY;
    float m_fOriginAngle;

    void init();
};

#endif // CMODELDATA_H
