#ifndef CIMAGEPROCESS_H
#define CIMAGEPROCESS_H

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <list>

using namespace std;
using namespace cv;

struct AlignResult
{
    bool bFindOK;
    double dPosX;
    double dPosY;
    double dPosZ;
    double dAngle;
};

struct ImgProcParam
{
    int iLightVal[2];
    int iThresholdValLow[2];
    int iThresholdValHigh[2];
    int iHoleWidth[2];
    Rect rectRoi;
    double dBlobArea[2];
    double dBlobDist[2];
    unsigned int iModelBlobSize;
};

enum colorType
{
    COLOR_BLUE = 0,
    COLOR_GREEN,
    COLOR_RED,
    COLOR_YELLOW,
};

enum ContourType
{
    CONT_DIST = 0,
    CONT_AREA,
    CONT_RECTAREA
};

enum Direction
{
    LEFT = 0,
    RIGHT = 1,
    UP = 2,
    DOWN = 3,
};

class CNewBlob
{
public:
    CNewBlob();
    cv::Rect m_blobRect;
    int      m_iBlobSize;
};

class CLtePbaResult
{
public:
    CLtePbaResult():dDist(100000),ptCenter(0,0) {}
    double dDist;
    Point ptCenter;

    bool operator < (CLtePbaResult & temp)
    {
        return (this->dDist < temp.dDist);
    }
};

class CHolePoint
{
public:
    CHolePoint() :pCenter(0, 0), dDist(0.0), iRadius(0) {}
    Point pCenter;
    double dDist;
    unsigned int iRadius;
    bool operator < (CHolePoint & temp)
    {
        return (this->dDist < temp.dDist);
    }
};

class CVisionResult
{
public:
    CVisionResult() :dX(0.0), dY(0.0), bIsOk(false) {}
    CVisionResult(double dXPos, double dYpos, bool bISOk) :dX(dXPos), dY(dYpos), bIsOk(bISOk) {}
    double dX;
    double dY;
    bool   bIsOk;
};

class CImageProcess
{
public:
    CImageProcess();
    ~CImageProcess();

    std::vector<CNewBlob *> m_vBlobArray;
    bool m_bIsDebugMode;
    bool m_bSaveImage;

    const int WHITE = 255;
    const int BLACK = 0;
    const int IMAGE_WIDTH = 1920;

    double CalcDistance(int startX, int startY, int endX, int endY);
    double CalcDistance(float startX, float startY, float endX, float endY);
    double CalcDistance(double startX, double startY, double endX, double endY);
    double CalcDistance(Point& ptStart, Point& ptEnd);
    // Blob analysis
    cv::Rect FindRoiBlobAnalysis(Mat srcImg, int iBlobMaxSize, int iBlobMinSize);
    void labeling(const Mat img, int threshold, int* table1, int* table2, int tsize, int* pass1, int* pass2, int* cnt_label);
    CvScalar get_color(int i);
    int getBlob(Mat img, int iMaxSize, int iMinSize);
    int getCircleBlob(Mat img, int iMaxSize, int iMinSize);

    void BinarizeImage(Mat srcImage, Mat& destImage, double dThresValLow, double dThresValUp);

    // template matching
    Point2d FindRoiTemplateMatching(Mat srcImg, Mat templateImg, double dMatchingRate);

    Point2d FindHole(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize, int blurVal);
    CVisionResult LTE23_FindHole_PBA(cv::Mat srcImg, cv::Mat& dispImg, Rect& roi, int iThresholdValLow, int iThresholdValHigh, int minAreaSize, int maxAreaSize);
    CVisionResult FindCircleUsingHough(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize);
    CVisionResult FindHole_23GHousing(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minAreaSize, int maxAreaSize);

    Point DuPM_FindHole(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize);
    void DuPBA_FindHole(cv::Mat srcImg, cv::Mat& dispImg, Point* ptCircle, int iArraySize, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize);
    Point2d FindHalfCircle(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize, int blurVal, int iModuleType = 0);
    Point2d FindPinCenter(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, double dBlobArea, double dBlobDist);
    Point2d PowerModuleFindHalfCircle(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize, int blurVal, cv::Rect& roi, bool isFirstTime);

    // find regulator position
    //AlignResult FindRegulator(Mat srcImg, Mat& dispImg, ImgProcParam param, int templateNum, bool bAutomode, bool bUseTemplateMatching);
    AlignResult FindCornerPt(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode, int iModuleType = -1);
    AlignResult PowerModuleFindCornerPt(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode, bool isFirstCheck);
    Point2d GetIntersectionPt(Point2d AP1, Point2d AP2, Point2d BP1, Point2d BP2);

    AlignResult FindCenterPtFromWhole(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode, int iModuleType);
    AlignResult DUPBA_FindCenterPtFromEdge(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode);
    AlignResult LTE_23G_BOXPBA_Find(Mat srcImg, Mat& dispImg, ImgProcParam param);
    AlignResult LTE_23GShieldCan_BOX_FindCenterPt(Mat srcImg, Mat& dispImg, ImgProcParam param);
    Point LTE_23GPBA_FindHole(cv::Mat srcImg, cv::Mat& dispImg, ImgProcParam param, int iThresholdValLow, int iThresholdValHigh, int minArea, int maxArea);
    Point LTE_23GPBA_FindSlotHole(cv::Mat srcImg, cv::Mat& dispImg, ImgProcParam param, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize);
    AlignResult LTE_23GShielCan_FindHole(cv::Mat srcImg, cv::Mat& dispImg, ImgProcParam param, int iThresholdValLow, int iThresholdValHigh, int minArea, int maxArea);

    Point2d ShowAndFindRotatePtFromCenter(Mat& dispImg, Point2d centerPt, Point2d& originPt, double dAngle);
    Point2d FindRotatePtFromCenter(Point2d centerPt, Point2d& originPt, double dAngle);

    AlignResult FindCircleFromBox(Mat srcImg, Mat& dispImg, ImgProcParam param, int iModuleType);

    // check turn on
    //bool CheckTurnOnArea(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode, int areaNum);  // area: all, lt, lb, rt, rb
    int getNearestPointNum(Point currentPt, std::vector<Point>* belt);

    double GetColor(Mat srcImg, int colorCh);

    uchar GetPixel(Mat & srcImg, Point pt);
    void SetPixel(Mat & srcImg, Point pt, uchar value);
    Vec3b GetPixel3C(Mat & srcImg, Point pt);
    void SetPixel3C(Mat & srcImg, Point pt, Vec3b value);

    Mat FindContours(Mat & srcImg, std::vector<std::vector<Point>> & vContours, double dMin, double dMax, int iAreaType, bool bDraw, Scalar drawColor = Scalar(255, 255, 255));
    Mat FindLargestContour(Mat & srcImg, std::vector<Point> & vContour, double dMin, double dMax, int& cnt, bool bDraw, int iDrawLength = 1, Scalar drawColor = Scalar(255, 255, 255));
    Mat FindLargestContour(Mat & srcImg, std::vector<Point> & vContour, double dMin, double dMax, Rect& bndRect, int& cnt, bool bDraw, int iDrawLength = 1, Scalar drawColor = Scalar(255, 255, 255));
    Mat FindLargestContour2(Mat & srcImg, std::vector<Point> & vContour, double dMin, double dMax, Rect& bndRect, int& cnt, bool bDraw, int iDrawLength, Scalar drawColor = Scalar(255, 255, 255));

    Point2d LinePtByX(Vec4f line, double x);
    Point2d LinePtByY(Vec4f line, double y);
    void SetOption(bool bSaveImage);

private:
    double radiansToDegree(const double dRadians);
    double degreeToRadians(const double Degree);

    template <class T>
    double calcDistance(T startX, T startY, T endX, T endY );
};

#endif // CIMAGEPROCESS_H
