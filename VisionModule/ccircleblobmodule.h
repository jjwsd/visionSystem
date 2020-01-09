#ifndef CCIRCLEBLOBMODULE_H
#define CCIRCLEBLOBMODULE_H

#include <S_Vision/CImageProcess.h>
#include <VisionModule/CVisionModule.h>

class CCircleBlobParams
{
public:
    CCircleBlobParams():iThresholdHigh(255), iThresholdLow(0), iRadius(50), dTolerance(0.1){}
public:
    int iThresholdLow;
    int iThresholdHigh;
    int iRadius;
    double dTolerance;

    void GetHoleArea(int* iMinArea, int* iMaxArea);

};

class CCircleBlobModule : public CVisionModule
{
public:
    CCircleBlobModule();
    virtual ~CCircleBlobModule();
    virtual CVisionAgentResult RunVision(Mat srcImg, Mat& dispImg);
    virtual std::string GetName();
    virtual void TestName();

    CCircleBlobParams m_Param;
    CImageProcess   m_baseVision;
    void SetParams(CCircleBlobParams params);
};

#endif // CCIRCLEBLOBMODULE_H
