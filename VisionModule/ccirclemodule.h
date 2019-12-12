#ifndef CCIRCLEMODULE_H
#define CCIRCLEMODULE_H

#include <VisionModule/CVisionModule.h>

class CCircleParams
{
public:
    CCircleParams():iThresholdHigh(255), iThresholdLow(0), iRadius(50), dTolerance(0.1){}
public:
    int iThresholdLow;
    int iThresholdHigh;
    int iRadius;
    double dTolerance;

    double dDp;
    double dMinDist;
    double dParam1;
    double dParam2;
};

class CCircleModule : public CVisionModule
{
public:
    CCircleModule();
    virtual CVisionAgentResult RunVision(Mat srcImg, Mat& dispImg);
    virtual std::string GetName();

    CCircleParams m_Param;
    void SetParams(CCircleParams params);

};

#endif // CCIRCLEMODULE_H
