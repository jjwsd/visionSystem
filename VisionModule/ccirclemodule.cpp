#include "ccirclemodule.h"

#include <opencv2/imgproc.hpp>

CCircleModule::CCircleModule()
{
    this->m_eVisionType = VISION::CIRCLE;
}

CVisionAgentResult CCircleModule::RunVision(Mat srcImg, Mat& dispImg)
{    
    CVisionAgentResult result;
    cv::Mat procImg, grayImg, binImg;
    bool bIsOk = false;
    int iWriteCount = 0;
    std::string strFileName;


    procImg = srcImg.clone();
    dispImg = srcImg.clone();

    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    if(m_bDebugMode)
    {
        strFileName = to_string(++iWriteCount) + "_cvtColor_Gray.jpg";
        cv::imwrite(strFileName,grayImg);
    }
    m_baseVision.BinarizeImage(grayImg, binImg, m_Param.iThresholdLow, m_Param.iThresholdHigh);

    if(m_bDebugMode)
    {
        strFileName = to_string(++iWriteCount) + "_Binarize.jpg";
        cv::imwrite(strFileName, binImg);
    }

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    cv::morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    if (binImg.channels() == 3) {
        cvtColor(binImg, binImg, CV_BGR2GRAY);
    }
    vector<Vec3f> circles;
    Mat gaussianImg;
    cv::GaussianBlur(binImg, gaussianImg, Size(3, 3), 1, 1);

    if(m_bDebugMode)
    {
        strFileName = to_string(++iWriteCount) + "Gaussian.jpg";
        cv::imwrite(strFileName, gaussianImg);
    }

    double dMinR = m_Param.iRadius*(1.0-m_Param.dTolerance);
    double dMaxR = m_Param.iRadius*(1.0+m_Param.dTolerance);

    cv::HoughCircles(gaussianImg, circles, CV_HOUGH_GRADIENT, 1.0, 300, 100, 70, (int)dMinR, (int)dMaxR);

    unsigned int circleRadius = 0;
    Point2d tempCenter;

    int circleCnt = circles.size();

    if (circleCnt == 1)
    {
        tempCenter = Point2d(circles[0][0], circles[0][1]);
        circleRadius = (unsigned int)circles[0][2];
        bIsOk = true;
    }
    else if(circleCnt > 1)
    {
        CHolePoint  * cHolePoint = new CHolePoint[circleCnt];
        Point ptImgCenter(binImg.size().width / 2, binImg.size().height / 2);

        for (int i = 0; i< circleCnt; i++)
        {
            tempCenter = Point2d(circles[i][0], circles[i][1]);
            cHolePoint[i].pCenter = Point((int)tempCenter.x, (int)tempCenter.y);
            cHolePoint[i].dDist = m_baseVision.CalcDistance(ptImgCenter, cHolePoint[i].pCenter);
            cHolePoint[i].iRadius = (unsigned int)circles[0][2];

            if (m_bDebugMode)
            {
                cv::circle(dispImg, tempCenter, (int)circles[0][2], CV_RGB(255, 0, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 1);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 1);
            }
        }

        std::sort(cHolePoint, cHolePoint + circleCnt);

        tempCenter = cHolePoint[0].pCenter;
        circleRadius = cHolePoint[0].iRadius;
        bIsOk = true;

        delete[] cHolePoint;
    }
    else
    {

    }

    if (m_bDebugMode)
    {
        cv::circle(dispImg, tempCenter, circleRadius, CV_RGB(255, 0, 0), 10);
        cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 5);
        cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 5);
    }

    result.SetCenterPoint(cv::Point2f(tempCenter.x, tempCenter.y));
    result.bOk = bIsOk;
    return result;
}

string CCircleModule::GetName()
{
    return "CircleFinder";
}

void CCircleModule::SetParams(CCircleParams params)
{
    m_Param = params;
}

