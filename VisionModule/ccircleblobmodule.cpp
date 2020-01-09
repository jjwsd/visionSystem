#include "ccircleblobmodule.h"

CCircleBlobModule::CCircleBlobModule()
{

}

CCircleBlobModule::~CCircleBlobModule()
{

}

CVisionAgentResult CCircleBlobModule::RunVision(Mat srcImg, Mat &dispImg)
{
    CVisionAgentResult result;
    CVisionResult tmpResult;
    int iMaxArea, iMinArea;
    m_Param.GetHoleArea(&iMinArea, &iMaxArea);
    tmpResult = m_baseVision.FindHole_23GHousing(srcImg, dispImg, m_Param.iThresholdLow, m_Param.iThresholdHigh,
                                     iMinArea, iMaxArea);

    result.SetCenterPoint(cv::Point2f(tmpResult.dX, tmpResult.dY));
    result.bOk = tmpResult.bIsOk;
    return result;
}

string CCircleBlobModule::GetName()
{
    return "CircleBlobFinder";
}

void CCircleBlobModule::TestName()
{

}

void CCircleBlobModule::SetParams(CCircleBlobParams params)
{
    m_Param = params;
}

void CCircleBlobParams::GetHoleArea(int* iMinArea, int* iMaxArea)
{
    int iWidth = this->iRadius * 2;
    int iMinWidth = static_cast<int>(iWidth * (1 - this->dTolerance));
    int iMaxWidth = static_cast<int>(iWidth * (1 + this->dTolerance));
    *iMinArea = iMinWidth * iMinWidth;
    *iMaxArea = iMaxWidth * iMaxWidth;
}
