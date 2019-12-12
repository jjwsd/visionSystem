#include "S_Vision/CImageProcess.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define POINT1 0
#define POINT2 1

int compare(const void* val1, const void* val2);
int compare(const void* val1, const void* val2)
{
    return (*(int*)val1 - *(int*)val2);
}

CImageProcess::CImageProcess() :m_bIsDebugMode(false)
{

}

CImageProcess::~CImageProcess()
{
    for (auto element : m_vBlobArray) {
        delete element;
    }
    m_vBlobArray.clear();
}

double CImageProcess::CalcDistance(int startX, int startY, int endX, int endY)
{
    return sqrt(pow(startX - endX, 2) + pow(startY - endY, 2));
}

double CImageProcess::CalcDistance(float startX, float startY, float endX, float endY)
{
    return sqrt(pow(startX - endX, 2) + pow(startY - endY, 2));
}

double CImageProcess::CalcDistance(double startX, double startY, double endX, double endY)
{
    return sqrt(pow(startX - endX, 2) + pow(startY - endY, 2));
}

double CImageProcess::CalcDistance(Point& ptStart, Point& ptEnd)
{
    return sqrt(pow(ptStart.x - ptEnd.x, 2) + pow(ptStart.y- ptEnd.y, 2));
}

cv::Rect CImageProcess::FindRoiBlobAnalysis(Mat srcImg, int iBlobMaxSize, int iBlobMinSize)
{
    Rect retRect;
    Rect rect;

    int iBlobCnt;

    Mat dispImg;
    dispImg = srcImg.clone();

    iBlobCnt = getBlob(srcImg, iBlobMaxSize, iBlobMinSize);

    int iMaxSize = 0;//srcImg.cols * srcImg.rows;
    int iMaxNum = 0;

    CNewBlob* newblob;

    int left = srcImg.cols;
    int right = 0;
    int top = srcImg.rows;
    int bottom = 0;

    if (iBlobCnt > 0)
    {
        int iSize;
        for (int i = 0; i < m_vBlobArray.size(); ++i)
        {
            iSize = m_vBlobArray.at(i)->m_iBlobSize;
            if (iMaxSize < iSize)
            {
                iMaxSize = iSize;
                iMaxNum = i;
            }
        }

        newblob = m_vBlobArray.at(iMaxNum);

        // find start pos, end pos
        if (newblob->m_blobRect.x < left)
            left = newblob->m_blobRect.x;
        if (newblob->m_blobRect.width> right)
            right = newblob->m_blobRect.width;
        if (newblob->m_blobRect.y < top)
            top = newblob->m_blobRect.y;
        if (newblob->m_blobRect.height > bottom)
            bottom = newblob->m_blobRect.height;

        rect = cv::Rect(left, top, right, bottom);
    }
    return rect;
}


CvScalar CImageProcess::get_color(int i)
{
    CvScalar res;
    switch (i % 6) {

    case 0:
        res = CV_RGB(255, 0, 0);
        break;
    case 1:
        res = CV_RGB(0, 255, 0);
        break;
    case 2:
        res = CV_RGB(0, 0, 255);
        break;
    case 3:
        res = CV_RGB(255, 255, 0);
        break;
    case 4:
        res = CV_RGB(255, 0, 255);
        break;
    case 5:
        res = CV_RGB(0, 255, 255);
        break;
    }
    return res;
}

int CImageProcess::getBlob(Mat img, int iMaxSize, int iMinSize)
{
    int* pass1;
    int* pass2;
    int* table1;
    int* table2;

    int size = img.cols * img.rows;

    int i, j;

    pass1 = (int*)malloc(size * sizeof(int));
    pass2 = (int*)malloc(size * sizeof(int));
    table1 = (int*)malloc(size / 2 * sizeof(int));
    table2 = (int*)malloc(size / 2 * sizeof(int));

    int count = 0;

    labeling(img, 50, table1, table2, size / 2, pass1, pass2, &count);

    for (auto element : m_vBlobArray)
    {
        delete element;
    }
    m_vBlobArray.clear();

    CNewBlob* pB = new CNewBlob[count];
    for (int i = 0; i<count; i++)
    {
        pB[i].m_blobRect.x = img.cols;
        pB[i].m_blobRect.width = 0;
        pB[i].m_blobRect.y = img.rows;
        pB[i].m_blobRect.height = 0;
        pB[i].m_iBlobSize = 0;
    }

    for (i = 0; i < (img.rows); i++)
    {
        for (j = 0; j < (img.cols); j++)
        {
            int label = pass2[i * img.cols + j];

            if (label)
            {
                if (j < pB[label - 1].m_blobRect.x)
                    pB[label - 1].m_blobRect.x = j;

                if (j > pB[label - 1].m_blobRect.width)
                    pB[label - 1].m_blobRect.width = j;

                if (i < pB[label - 1].m_blobRect.y)
                    pB[label - 1].m_blobRect.y = i;

                if (i > pB[label - 1].m_blobRect.height)
                    pB[label - 1].m_blobRect.height = i;

                pB[label - 1].m_iBlobSize = (pB[label - 1].m_blobRect.width - pB[label - 1].m_blobRect.x) *
                        (pB[label - 1].m_blobRect.height - pB[label - 1].m_blobRect.y);
            }
        }
    }

    for (int i = 0; i<count; i++)
    {
        if (pB[i].m_iBlobSize <= iMaxSize && pB[i].m_iBlobSize >= iMinSize)
        {
            CNewBlob* pBlob = new CNewBlob;
            pBlob->m_blobRect = pB[i].m_blobRect;
            pBlob->m_iBlobSize = pB[i].m_iBlobSize;
            m_vBlobArray.push_back(pBlob);
        }
    }

    free(pass1);
    free(pass2);
    free(table1);
    free(table2);

    delete[] pB;

    count = (int)m_vBlobArray.size();

    return count;
}

int CImageProcess::getCircleBlob(Mat img, int iMaxSize, int iMinSize)
{
    int* pass1;
    int* pass2;
    int* table1;
    int* table2;

    int size = img.cols * img.rows;

    int i, j;

    pass1 = (int*)malloc(size * sizeof(int));
    pass2 = (int*)malloc(size * sizeof(int));
    table1 = (int*)malloc(size / 2 * sizeof(int));
    table2 = (int*)malloc(size / 2 * sizeof(int));

    int count = 0;

    labeling(img, 50, table1, table2, size / 2, pass1, pass2, &count);

    for (auto var: m_vBlobArray) {
        delete var;
    }
    m_vBlobArray.clear();

    CNewBlob* pB = new CNewBlob[count];
    for (int i = 0; i<count; i++)
    {
        pB[i].m_blobRect.x = img.cols;
        pB[i].m_blobRect.width = 0;
        pB[i].m_blobRect.y = img.rows;
        pB[i].m_blobRect.height = 0;
        pB[i].m_iBlobSize = 0;
    }

    for (i = 0; i < (img.rows); i++)
    {
        for (j = 0; j < (img.cols); j++)
        {
            int label = pass2[i * img.cols + j];
            if (label)
            {
                if (j < pB[label - 1].m_blobRect.x)
                    pB[label - 1].m_blobRect.x = j;

                if (j > pB[label - 1].m_blobRect.width)
                    pB[label - 1].m_blobRect.width = j;

                if (i < pB[label - 1].m_blobRect.y)
                    pB[label - 1].m_blobRect.y = i;

                if (i > pB[label - 1].m_blobRect.height)
                    pB[label - 1].m_blobRect.height = i;

                pB[label - 1].m_iBlobSize = (pB[label - 1].m_blobRect.width - pB[label - 1].m_blobRect.x) *
                                             (pB[label - 1].m_blobRect.height - pB[label - 1].m_blobRect.y);
            }
        }
    }

    for (int i = 0; i<count; i++)
    {
        double width = (pB[i].m_blobRect.width - pB[i].m_blobRect.x);
        double height = (pB[i].m_blobRect.height - pB[i].m_blobRect.y);

        if (pB[i].m_iBlobSize <= iMaxSize && pB[i].m_iBlobSize >= iMinSize
            && width / height < 1.2 && width / height > 0.8)
        {
            CNewBlob* pBlob = new CNewBlob;

            pBlob->m_blobRect = pB[i].m_blobRect;
            pBlob->m_iBlobSize = pB[i].m_iBlobSize;
            m_vBlobArray.push_back(pBlob);
        }
    }

    free(pass1);
    free(pass2);
    free(table1);
    free(table2);

    delete[] pB;

    count = (int)m_vBlobArray.size();

    return count;
}

void CImageProcess::labeling(const Mat img, int threshold, int* table1, int* table2, int tsize, int* pass1, int* pass2, int* cnt_label)
{
    int x, y; // pass 1,2
    int i;  // initialize
    int cnt = 0; // pass 1
    int label = 0; // pass 2

                   // initialize , table1 table1
    int sizeWidth = img.cols;
    int sizeHeight = img.rows;

    memset(pass1, 0, (sizeWidth * sizeHeight)* sizeof(int));
    memset(pass2, 0, (sizeWidth * sizeHeight)* sizeof(int));

    for (i = 0; i < tsize; i++) {
        table1[i] = i;
    }
    memset(table2, 0, tsize * sizeof(int));

    // pass 1
    for (y = 1; y < (sizeHeight); y++) {
        for (x = 1; x < (sizeWidth); x++) {

            uchar* data = (uchar*)img.data;
            uchar val = data[y * sizeWidth + x];

            if (val > threshold * 3) { // fore ground??
                int up, le;

                up = pass1[(y - 1)*(sizeWidth)+(x)]; // up  index
                le = pass1[(y)*(sizeWidth)+(x - 1)]; // left index

                                                     // case
                if (up == 0 && le == 0) {
                    ++cnt;
                    pass1[y * sizeWidth + x] = cnt;

                }
                else if (up != 0 && le != 0) {
                    if (up > le) {
                        pass1[y * sizeWidth + x] = le;
                        table1[up] = table1[le]; // update table1 table1
                    }
                    else {
                        pass1[y * sizeWidth + x] = up;
                        table1[le] = table1[up]; // update table1 table1
                    }
                }
                else {
                    pass1[y * sizeWidth + x] = up + le;
                }

            }/*if*/

        }/*for*/
    }/*for*/


     // pass 2
    for (y = 1; y < (sizeHeight); y++) {
        for (x = 1; x < (sizeWidth); x++) {

            if (pass1[y * sizeWidth + x]) {
                int v = table1[pass1[y * sizeWidth + x]];

                if (table2[v] == 0) {
                    ++label;
                    table2[v] = label;
                }

                pass2[y * sizeWidth + x] = table2[v];
            }
        }
    }

    printf("labels: %d\n", label);
    *cnt_label = label;
}

void CImageProcess::BinarizeImage(Mat srcImage, Mat& destImage, double dThresValLow, double dThresValUp)
{
    if (srcImage.data == NULL)
        return;

    Mat LowerImg, UpperImg;

    LowerImg = srcImage.clone();
    UpperImg = srcImage.clone();

    if (LowerImg.channels() != 1)
    {
        cvtColor(LowerImg, LowerImg, CV_BGR2GRAY);
    }
    if (UpperImg.channels() != 1)
    {
        cvtColor(UpperImg, UpperImg, CV_BGR2GRAY);
    }
    threshold(LowerImg, LowerImg, dThresValLow, 255.0, THRESH_BINARY);
    threshold(UpperImg, UpperImg, dThresValUp, 255.0, THRESH_BINARY);
    bitwise_xor(LowerImg, UpperImg, destImage);
}

Point2d CImageProcess::FindRoiTemplateMatching(Mat srcImg, Mat templateImg, double dMatchingRate)
{
    Point2d resultPt(-999.0, -999.0);
    std::string strPath, strFileName;
    Mat procImg, procImgTemplate;

    if (srcImg.empty())
        return resultPt;

    if (srcImg.channels() != 1)
        cvtColor(srcImg, srcImg, CV_BGR2GRAY);

    procImg = srcImg.clone();
    resize(procImg, procImg, Size(procImg.cols / 10, procImg.rows / 10));

    if (templateImg.channels() != 1)
        cvtColor(templateImg, templateImg, CV_BGR2GRAY);


    /// Create the result matrix
    int result_cols;
    int result_rows;
    Mat result;

    int match_method = TM_CCOEFF_NORMED;

    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    procImgTemplate = templateImg.clone();
    resize(procImgTemplate, procImgTemplate, Size(procImgTemplate.cols / 10, procImgTemplate.rows / 10));
    result_cols = srcImg.cols - templateImg.cols + 1;
    result_rows = srcImg.rows - templateImg.rows + 1;
    result.create(result_rows, result_cols, CV_32FC1);

    /// Do the Matching and Normalize
    matchTemplate(procImg, procImgTemplate, result, match_method);

    /// Localizing the best match with minMaxLoc
    minMaxLoc(result, &minVal, &maxVal, NULL, &maxLoc, Mat());

    if (maxVal > dMatchingRate / 100)
    {
        matchLoc = maxLoc;

        matchLoc.x *= 10;
        matchLoc.y *= 10;
    }
    else
    {
        matchLoc.x = -1;
        matchLoc.y = -1;
    }

    return matchLoc;
}

AlignResult CImageProcess::PowerModuleFindCornerPt(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode, bool isFirstCheck)
{
    AlignResult result;

    result.bFindOK = false;
    result.dPosX = 0;
    result.dPosY = 0;
    result.dPosZ = 0;
    result.dAngle = -999.0;

    Mat procImg, binImg, grayImg;
    Rect roi = param.rectRoi;

    dispImg = srcImg.clone();

    int sizeTolerance = int(param.iHoleWidth[POINT1] * 0.1);
    int minR, maxR;
    minR = (param.iHoleWidth[POINT1]) - sizeTolerance;
    maxR = (param.iHoleWidth[POINT1]) + sizeTolerance;
    Point2d circlecenter(-100, -100);

    int iOffestValue = maxR * 2;

    circlecenter = PowerModuleFindHalfCircle(srcImg, dispImg, param.iThresholdValLow[POINT1], param.iThresholdValHigh[POINT1], minR, maxR, 3, param.rectRoi, isFirstCheck);
    if (circlecenter.x < 0 || circlecenter.y < 0) {
        return result;
    }
    double iLeftX = ((circlecenter.x - iOffestValue) < 0) ? 0 : circlecenter.x - iOffestValue;
    double iTopY = ((circlecenter.y - iOffestValue) < 0) ? 0 : circlecenter.y - iOffestValue;
    double iRightX = ((circlecenter.x + (iOffestValue)) > srcImg.size().width) ? srcImg.size().width : circlecenter.x + (iOffestValue);
    double iBottomY = ((circlecenter.y + (iOffestValue)) > srcImg.size().height) ? srcImg.size().height : circlecenter.y + (iOffestValue);

    Rect newRoi(Point((int)iLeftX, (int)iTopY), Point((int)iRightX, (int)iBottomY));
    rectangle(dispImg, newRoi, CV_RGB(255, 255, 0), 2);
    roi = newRoi;

    procImg = srcImg(roi).clone();

    std::string strPath, strFileName;
    strPath = ".\\Img";
    strFileName = "srcImg";

    if (procImg.channels() == 3) {
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    }
    else {
        grayImg = procImg.clone();
    }
    int thresholdLow, thresholdHigh;

    thresholdLow = param.iThresholdValLow[POINT1];
    thresholdHigh = param.iThresholdValHigh[POINT1];

    BinarizeImage(grayImg, binImg, thresholdLow, thresholdHigh);
    //threshold(grayImg, binImg, 220, 255, CVHRESH_BINARY_INV);

    std::vector<Point> vContour;
    Rect bndRect;
    int cnt = 0;
    int minSize = (binImg.cols * binImg.rows) / 16;
    int maxSize = (binImg.cols * binImg.rows) / 4;

    Mat largestblob = FindLargestContour(binImg, vContour, minSize, maxSize, bndRect, cnt, true, CV_FILLED);

    if (cnt < 1) {
        return result;
    }
    strPath = (".\\img");
    strFileName = "largestblob";

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_CLOSE;
    morphologyEx(largestblob, largestblob, morphMode, kernel, Point(-1, -1), iteration);

    strFileName = "morphologyEx";

    Canny(largestblob, largestblob, 128, 255);

    strFileName = "Canny";

    Mat horImg, vertImg;
    std::vector<Point> vhorLine, vvertLine;

    horImg.create(largestblob.size(), CV_8UC1);
    vertImg.create(largestblob.size(), CV_8UC1);
    horImg = 0;
    vertImg = 0;

    int tempCenterX = (int)(circlecenter.x - roi.x);
    int tempCenterY = (int)(circlecenter.y - roi.y);


    for (int y = tempCenterY; y < (tempCenterY + (maxR)); y++)
    {
        for (int x = bndRect.x; x < (tempCenterX + maxR); x++)
        {
            Point pt(x, y);
            uchar pixel = GetPixel(largestblob, pt);
            if (pixel == WHITE)
            {
                vvertLine.push_back(pt);
                break;
            }
        }
    }

    int iMaxX = tempCenterX + (maxR);
    iMaxX = (iMaxX > largestblob.size().width) ? largestblob.size().width : iMaxX;

    int iMaxY = tempCenterY + (maxR);
    iMaxY = (iMaxY > largestblob.size().height) ? largestblob.size().height : iMaxY;

    for (int x = tempCenterX; x < iMaxX; x++)
    {
        for (int y = bndRect.y; y < iMaxY; y++)
        {
            Point pt(x, y);
            uchar pixel = GetPixel(largestblob, pt);
            if (pixel == WHITE)
            {
                vhorLine.push_back(pt);
                break;
            }
        }
    }

    Vec4f horFitLine, vertFitLine;

    if (vvertLine.size() == 0 || vhorLine.size() == 0)
    {
        result.bFindOK = false;
        return result;
    }

    fitLine(vhorLine, horFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);
    fitLine(vvertLine, vertFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

    Point2d horPt0 = LinePtByX(horFitLine, 0.0);
    Point2d horPt1 = LinePtByX(horFitLine, horImg.rows);

    horPt0.x = horPt0.x + roi.x;
    horPt0.y = horPt0.y + roi.y;
    horPt1.x = horPt1.x + roi.x;
    horPt1.y = horPt1.y + roi.y;

    line(dispImg, horPt0, horPt1, Scalar(0, 0, 255), 4);

    Point2d vertPt0 = LinePtByY(vertFitLine, 0.0);
    Point2d vertPt1 = LinePtByY(vertFitLine, vertImg.cols);

    vertPt0.x = vertPt0.x + roi.x;
    vertPt0.y = vertPt0.y + roi.y;
    vertPt1.x = vertPt1.x + roi.x;
    vertPt1.y = vertPt1.y + roi.y;

    line(dispImg, vertPt0, vertPt1, Scalar(0, 0, 255), 4);

    Point2d intersectionPt = GetIntersectionPt(horPt0, horPt1, vertPt0, vertPt1);
    circle(dispImg, intersectionPt, 5, CV_RGB(0, 255, 0), 10);

    result.dPosX = intersectionPt.x; // ±³Á¡À» ÀúÀå.
    result.dPosY = intersectionPt.y;
    result.dAngle = radiansToDegree(atan2(vertPt1.y - vertPt0.y, vertPt1.x - vertPt0.x));
    // confirm
    double vertAngle = fabs(result.dAngle);
    double horAngle = radiansToDegree(atan2(horPt1.y - horPt0.y, horPt1.x - horPt0.x));
    horAngle = fabs(horAngle);
    horAngle = horAngle + vertAngle;
    if (horAngle >= 86 && horAngle <= 100)
    {
        result.bFindOK = true;
    }
    return result;
}

AlignResult CImageProcess::FindCornerPt(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode, int iModuleType)
{
    AlignResult result;

    result.bFindOK = false;

    result.dPosX = 0;
    result.dPosY = 0;
    result.dPosZ = 0;
    result.dAngle = -999.0;

    Mat procImg, binImg, grayImg;
    Mat tempHorImg, tempVerImg;
    Rect roi = param.rectRoi;

    dispImg = srcImg.clone();

    int sizeTolerance = int(param.iHoleWidth[POINT1] * 0.2);
    int minR, maxR;
    minR = (param.iHoleWidth[POINT1]) - sizeTolerance;
    maxR = (param.iHoleWidth[POINT1]) + sizeTolerance;
    Point2d circlecenter(-100, -100);

    //if (iModuleType == PBA)
    {
        int iOffestValue = maxR * 2;
        circlecenter = FindHalfCircle(srcImg, dispImg, param.iThresholdValLow[POINT1], param.iThresholdValHigh[POINT1], minR, maxR, 3);
        if (circlecenter.x < 0 || circlecenter.y < 0) {
            return result;
        }
        double iLeftX = ((circlecenter.x - iOffestValue) < 0) ? 0 : circlecenter.x - iOffestValue;
        double iTopY = ((circlecenter.y - iOffestValue) < 0) ? 0 : circlecenter.y - iOffestValue;
        double iRightX = ((circlecenter.x + (iOffestValue)) > srcImg.size().width) ? srcImg.size().width : circlecenter.x + (iOffestValue);
        double iBottomY = ((circlecenter.y + (iOffestValue)) > srcImg.size().height) ? srcImg.size().height : circlecenter.y + (iOffestValue);

        Rect newRoi(Point((int)iLeftX, (int)iTopY), Point((int)iRightX, (int)iBottomY));
        roi = newRoi;
    }

    procImg = srcImg(roi).clone();

    //line(dispImg, Point(roi.x, roi.y), Point(roi.x + roi.width, roi.y), Scalar(80, 155, 80), 2);
    //line(dispImg, Point(roi.x, roi.y), Point(roi.x, roi.y + roi.height), Scalar(80, 155, 80), 2);
    //line(dispImg, Point(roi.x + roi.width, roi.y), Point(roi.x + roi.width, roi.y + roi.height), Scalar(80, 155, 80), 2);
    //line(dispImg, Point(roi.x, roi.y + roi.height), Point(roi.x + roi.width, roi.y + roi.height), Scalar(80, 155, 80), 2);

    if (procImg.channels() == 3) {
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    }
    else {
        grayImg = procImg.clone();
    }
    int thresholdLow, thresholdHigh;

    thresholdLow = param.iThresholdValLow[POINT1];
    thresholdHigh = param.iThresholdValHigh[POINT1];

    BinarizeImage(grayImg, binImg, thresholdLow, thresholdHigh);
    //threshold(grayImg, binImg, 220, 255, CVHRESH_BINARY_INV);

    std::vector<Point> vContour;
    Rect bndRect;
    int cnt = 0;
    int minSize = (binImg.cols * binImg.rows) / 16;
    int maxSize = (binImg.cols * binImg.rows) / 4;

    Mat largestblob = FindLargestContour(binImg, vContour, minSize, maxSize, bndRect, cnt, true, CV_FILLED);

    if (cnt < 1) {
        return result;
    }

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_CLOSE;
    morphologyEx(largestblob, largestblob, morphMode, kernel, Point(-1, -1), iteration);

    Canny(largestblob, largestblob, 128, 255);


    Mat horImg, vertImg;
    std::vector<Point> vhorLine, vvertLine;

    horImg.create(largestblob.size(), CV_8UC1);
    vertImg.create(largestblob.size(), CV_8UC1);
    horImg = 0;
    vertImg = 0;

    int iStartY, iStartX, iEndY, iEndX;
    if (iModuleType == 0)
    {
        int tempCenterX = (int)(circlecenter.x - roi.x);
        int tempCenterY = (int)(circlecenter.y - roi.y);
        for (int y = bndRect.y; y < tempCenterY - (maxR / 2); y++)
        {
            for (int x = bndRect.x + bndRect.width; x> tempCenterX; x--)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(largestblob, pt);
                if (pixel == WHITE)
                {
                    vvertLine.push_back(pt);
                    break;
                }
            }
        }

        int iMinX = tempCenterX - (maxR / 2);
        iMinX = (iMinX < 0) ? 0 : iMinX;

        int iMaxY = tempCenterY - (maxR / 2);
        iMaxY = (iMaxY < 0) ? 0 : iMaxY;
        for (int x = bndRect.x; x < iMinX; x++)
        {
            for (int y = bndRect.y + bndRect.height; y > iMaxY; y--)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(largestblob, pt);
                if (pixel == WHITE)
                {
                    vhorLine.push_back(pt);
                    break;
                }
            }
        }
    }
    else {
        iStartY = bndRect.y + 100;
        iEndY = bndRect.y + (bndRect.height / 2);
        iStartX = bndRect.x + 100;
        iEndX = bndRect.x + (bndRect.width / 2);

        for (int y = iStartY; y<iEndY; y++)
        {
            for (int x = bndRect.x-100; x< iEndX; x++)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(largestblob, pt);

                if (pixel == WHITE)
                {
                    vvertLine.push_back(pt);
                    break;
                }
            }
        }

        for (int x = iStartX; x < iEndX; x++)
        {
            for (int y = bndRect.y; y < iEndY; y++)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(largestblob, pt);
                if (pixel == WHITE)
                {
                    vhorLine.push_back(pt);
                    break;
                }
            }
        }
    }

    Vec4f horFitLine, vertFitLine;

    if (vvertLine.size() == 0 || vhorLine.size() == 0)
    {
        result.bFindOK = false;
        return result;
    }

    fitLine(vhorLine, horFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);
    fitLine(vvertLine, vertFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

    Point2d horPt0 = LinePtByX(horFitLine, 0.0);
    Point2d horPt1 = LinePtByX(horFitLine, horImg.rows);

    horPt0.x = horPt0.x + roi.x;
    horPt0.y = horPt0.y + roi.y;
    horPt1.x = horPt1.x + roi.x;
    horPt1.y = horPt1.y + roi.y;

    line(dispImg, horPt0, horPt1, Scalar(0, 0, 255), 4);

    Point2d vertPt0 = LinePtByY(vertFitLine, 0.0);
    Point2d vertPt1 = LinePtByY(vertFitLine, vertImg.cols);

    vertPt0.x = vertPt0.x + roi.x;
    vertPt0.y = vertPt0.y + roi.y;
    vertPt1.x = vertPt1.x + roi.x;
    vertPt1.y = vertPt1.y + roi.y;

    cv::line(dispImg, vertPt0, vertPt1, Scalar(0, 0, 255), 4);

    Point2d intersectionPt = GetIntersectionPt(horPt0, horPt1, vertPt0, vertPt1);
    cv::circle(dispImg, intersectionPt, 5, CV_RGB(0, 255, 0), 10);

    //line(dispImg, Point(0, 0), Point(result.dPosX, result.dPosY), Scalar(0, 255, 0), 4);

    if (iModuleType == 0)
    {
        result.dPosX = intersectionPt.x; // ±³Á¡À» ÀúÀå.
        result.dPosY = intersectionPt.y;
    }
    else
    {
        result.dPosX = intersectionPt.x - srcImg.cols / 2; // ÁßœÉÁ¡¿¡Œ­ÀÇ Â÷žŠ °è»ê
        result.dPosY = intersectionPt.y - srcImg.rows / 2;
    }
    result.dAngle = radiansToDegree(atan2(vertPt1.y - vertPt0.y, vertPt1.x - vertPt0.x));
    // confirm
    double vertAngle = radiansToDegree(atan2(vertPt1.y - vertPt0.y, vertPt1.x - vertPt0.x));
    if (iModuleType == 0)
    {
        if (fabs(vertAngle + 90 - result.dAngle) > 3.0)
        {
            result.bFindOK = false;
        }
        else
        {
            result.bFindOK = true;
        }
    }
    else if (iModuleType == 1)
    {
        double horAngle = radiansToDegree(atan2(horPt1.y - horPt0.y, horPt1.x - horPt0.x));
        vertAngle = fabs(vertAngle);
        horAngle = fabs(horAngle);
        horAngle = horAngle + vertAngle;
        if (horAngle >= 86 && horAngle <= 100)
        {
            result.bFindOK = true;
        }
    }
    else
    {
        result.bFindOK = true;
    }
    return result;
}

AlignResult CImageProcess::FindCircleFromBox(Mat srcImg, Mat& dispImg, ImgProcParam param, int iModuleType)
{
    AlignResult result;
    result.bFindOK = false;
    result.dPosX = 0;
    result.dPosY = 0;
    result.dPosZ = 0;
    result.dAngle = -999.0;

    Mat binImg, gaussianImg;
    Rect roi = param.rectRoi;

    Mat procImg = srcImg(roi).clone();
    dispImg = srcImg.clone();

    cvtColor(procImg, procImg, CV_BGR2GRAY);

    Scalar avg = mean(procImg);
    double dThreshHold = avg.val[0];
    BinarizeImage(procImg, binImg, dThreshHold, 255.0);
    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    std::vector<Point> vContour;
    Rect bndRect;
    int cnt = 0;
    int iBoardMinSize = (1250 * 800);
    int iBoardMaxSize = (1580 * 1220);

    if (iModuleType == 2)
        iBoardMinSize /= 4;

    Mat largestblob = FindLargestContour(binImg, vContour, iBoardMinSize, iBoardMaxSize, bndRect, cnt, true, CV_FILLED);

    // test start
    vector<Vec3f> circles;
    if (largestblob.channels() == 3) {
        cvtColor(largestblob, largestblob, CV_BGR2GRAY);
    }

    if (iModuleType == 1)
    {
        GaussianBlur(largestblob, gaussianImg, Size(5, 5), 5, 5);
        int minRSize = 40;
        int maxRSize = 100;
        HoughCircles(gaussianImg, circles, CV_HOUGH_GRADIENT, 2.0, 100, 100, 50, minRSize, maxRSize);

        Point2d circleCenter(-999.0, -999.0);
        float circleRadius = 0.0;
        Point2d tempCenter;
        uchar pixelVal;
        double lowerYCenter = -999.0;
        //1202, 779, 1682, 1159
        Rect tempCircleBnd(1188, 805, 1666, 1153);
        Point CircleleftTop(1188, 805);
        Point CircleRightBottom(1666, 1153);
        if (circles.size() != 0)
        {
            for (int i = 0; i<int(circles.size()); i++)
            {
                tempCenter = Point2d(circles[i][0], circles[i][1]);
                pixelVal = GetPixel(gaussianImg, circleCenter);
                if (tempCenter.y > lowerYCenter)
                {
                    lowerYCenter = tempCenter.y;
                    circleCenter = tempCenter;
                    circleRadius = circles[i][2];
                }

                if (m_bIsDebugMode)
                {
                    tempCenter.x += roi.x;
                    tempCenter.y += roi.y;
                    cv::circle(dispImg, tempCenter, (int)circles[i][2], CV_RGB(255, 0, 0), 1);
                    cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 1);
                    cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 1);
                }
            }

            tempCenter = circleCenter;
            tempCenter.x += roi.x;
            tempCenter.y += roi.y;

            if (tempCenter.x >= CircleleftTop.x && tempCenter.x <= CircleRightBottom.x
                && tempCenter.y >= CircleleftTop.y && tempCenter.y <= CircleRightBottom.y)
            {
                cv::circle(dispImg, tempCenter, (int)circleRadius, CV_RGB(255, 0, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 3);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 3);
            }
            else
            {
                cv::circle(dispImg, tempCenter, (int)circleRadius, CV_RGB(0, 0, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 255), 3);
                return result;
            }
        }
        else
        {
            //AfxMessageBox(("¿øÀ» Ã£À»Œö ŸøÀœ"));
            return result;
        }

        // test end
        Mat tmpImg;
        Canny(largestblob, tmpImg, 128, 255);

        std::vector<Point> vhorLine, vvertLine;

        int iStartY, iStartX, iEndY, iEndX;
        int iCenterY = bndRect.y + bndRect.height / 2;
        int iCenterX = bndRect.x + bndRect.width / 2;

        // horz
        iStartY = (int)(circleCenter.y + 150);
        iEndY = (int)(circleCenter.y - 150);
        iStartX = (int)(circleCenter.x + 150);
        iEndX = (int)(iStartX + 100);

        for (int x = iStartX; x < iEndX; x++)
        {
            for (int y = iStartY; y > iEndY; y--)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(tmpImg, pt);
                if (pixel == WHITE)
                {
                    vhorLine.push_back(pt);
                    break;
                }
            }
        }

        Vec4f horzFitLine;

        if (vhorLine.size() == 0)
        {
            result.bFindOK = false;
            return result;
        }

        fitLine(vhorLine, horzFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

        Point2d horzPt0 = LinePtByX(horzFitLine, 0);
        Point2d horzPt1 = LinePtByX(horzFitLine, tmpImg.cols);

        horzPt0.x = horzPt0.x + roi.x;
        horzPt0.y = horzPt0.y + roi.y;
        horzPt1.x = horzPt1.x + roi.x;
        horzPt1.y = horzPt1.y + roi.y;

        line(dispImg, horzPt0, horzPt1, Scalar(0, 0, 255), 3);

        double dHorAngle = atan2(horzPt1.y - horzPt0.y, horzPt1.x - horzPt0.x);
        double dChangeAngle = degreeToRadians(53.0) - dHorAngle;

        double dDist = 764.60;//distance(745, 1005, 155, 560);
        double dX = std::cos(dChangeAngle) * dDist;
        double dY = std::sin(dChangeAngle) * dDist;
        double dVerCircleX = circleCenter.x - fabs(dY);
        double dVerCircleY = circleCenter.y - fabs(dX);

        iStartY = (int)dVerCircleY - 220;
        iEndY = (int)iStartY + 100;
        iStartX = (((int)dVerCircleX - 200) < 0) ? 0 : ((int)dVerCircleX - 200);
        iEndX = iStartX + 300;

        for (int y = iStartY; y < iEndY; y++)
        {
            for (int x = iStartX; x < iEndX; x++)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(tmpImg, pt);
                if (pixel == WHITE)
                {
                    vvertLine.push_back(pt);
                    break;
                }
            }
        }

        Vec4f vertFitLine;

        if (vvertLine.size() == 0)
        {
            result.bFindOK = false;
            return result;
        }

        fitLine(vvertLine, vertFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

        Point2d vertPt0 = LinePtByY(vertFitLine, 0);
        Point2d vertPt1 = LinePtByY(vertFitLine, tmpImg.rows);

        vertPt0.x = vertPt0.x + roi.x;
        vertPt0.y = vertPt0.y + roi.y;
        vertPt1.x = vertPt1.x + roi.x;
        vertPt1.y = vertPt1.y + roi.y;

        line(dispImg, vertPt0, vertPt1, Scalar(0, 0, 255), 3);
        ///////////////
        Point2d intersectionPt = GetIntersectionPt(horzPt0, horzPt1, vertPt0, vertPt1);
        circle(dispImg, intersectionPt, 5, CV_RGB(0, 255, 0), 10);

        result.dPosX = intersectionPt.x; // ±³Á¡À» ÀúÀå.
        result.dPosY = intersectionPt.y;
        result.dAngle = radiansToDegree(atan2(vertPt1.y - vertPt0.y, vertPt1.x - vertPt0.x));

        dHorAngle = radiansToDegree(dHorAngle);
        double dVertAngle = result.dAngle;
        double dTotalAngle = fabs(dHorAngle) + fabs(dVertAngle);

        if (dHorAngle > 0.0)
        {
            dTotalAngle = (180.0 - dVertAngle) + dHorAngle;
        }
        else
        {
            dTotalAngle = 180.0 - (dVertAngle - dHorAngle);
        }

        double dMinAngle = 89.0;
        double dMaxAngle = 91.0;
        if (dTotalAngle > dMinAngle && dTotalAngle < dMaxAngle)
        {
            result.bFindOK = true;
        }
        else
        {
            result.bFindOK = false;
        }
    }
    return result;
}

AlignResult CImageProcess::FindCenterPtFromWhole(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode, int iModuleType)
{
    AlignResult result;
    result.bFindOK = false;

    result.dPosX = 0;
    result.dPosY = 0;
    result.dPosZ = 0;
    result.dAngle = -999.0;

    Mat binImg, gaussianImg;
    Rect roi = param.rectRoi;

    Mat procImg = srcImg(roi).clone();
    dispImg = srcImg.clone();

    cvtColor(procImg, procImg, CV_BGR2GRAY);

    Scalar avg = mean(procImg);
    double dThreshHoldMin = avg.val[0];
    double dThreshHoldMax = 255.0;
    if (iModuleType == 1)
    {
        dThreshHoldMin = 6.0; // º¯°æ ÇÊ¿ä.
    }
    else if (iModuleType == 2)
    {
        dThreshHoldMin = 8.45; // º¯°æ ÇÊ¿ä.
    }
    BinarizeImage(procImg, binImg, dThreshHoldMin, dThreshHoldMax);
    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    std::vector<Point> vContour;
    Rect bndRect;
    int cnt = 0;
    // ÁŠÇ°ž¶ŽÙ º¯°æ ÇÊ¿ä.
    int iBoardMinSize = (1250 * 800);
    int iBoardMaxSize = (1580 * 1220);

    if (iModuleType == 1)
    {
        iBoardMinSize /= 4;
    }

    Mat largestblob = FindLargestContour(binImg, vContour, iBoardMinSize, iBoardMaxSize, bndRect, cnt, true, CV_FILLED);

    // test start
    vector<Vec3f> circles;
    if (largestblob.channels() == 3) {
        cvtColor(largestblob, largestblob, CV_BGR2GRAY);
    }

    if (iModuleType == 1)
    {
        GaussianBlur(largestblob, gaussianImg, Size(5, 5), 5, 5);
        int minRSize = 40;
        int maxRSize = 100;
        HoughCircles(gaussianImg, circles, CV_HOUGH_GRADIENT, 2.0, 100, 100, 50, minRSize, maxRSize);

        Point2d circleCenter(-999.0, -999.0);
        float circleRadius = 0.0;
        Point2d tempCenter;
        uchar pixelVal;
        double lowerYCenter = -999.0;
        //1202, 779, 1682, 1159
        Rect tempCircleBnd(1188,805,1666,1153);
        Point CircleleftTop(1188, 805);
        Point CircleRightBottom(1666, 1153);
        if (circles.size() != 0)
        {
            for (int i = 0; i<int(circles.size()); i++)
            {
                tempCenter = Point2d(circles[i][0], circles[i][1]);
                pixelVal = GetPixel(gaussianImg, circleCenter);
                if (tempCenter.y > lowerYCenter)
                {
                    lowerYCenter = tempCenter.y;
                    circleCenter = tempCenter;
                    circleRadius = circles[i][2];
                }

                if (m_bIsDebugMode)
                {
                    tempCenter.x += roi.x;
                    tempCenter.y += roi.y;
                    cv::circle(dispImg, tempCenter, (int)circles[i][2], CV_RGB(255, 0, 0), 1);
                    cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 1);
                    cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 1);
                }
            }

            tempCenter = circleCenter;
            tempCenter.x += roi.x;
            tempCenter.y += roi.y;

            if (tempCenter.x >= CircleleftTop.x && tempCenter.x <= CircleRightBottom.x
                && tempCenter.y >= CircleleftTop.y && tempCenter.y <= CircleRightBottom.y)
            {
                cv::circle(dispImg, tempCenter, (int)circleRadius, CV_RGB(255, 0, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 3);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 3);
            }
            else
            {
                cv::circle(dispImg, tempCenter, (int)circleRadius, CV_RGB(0, 0, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 255), 3);
                return result;
            }
        }
        else
        {
            //AfxMessageBox(("¿øÀ» Ã£À»Œö ŸøÀœ"));
            return result;
        }

        // test end
        Mat tmpImg;
        Canny(largestblob, tmpImg, 128, 255);

        std::vector<Point> vhorLine, vvertLine;

        int iStartY, iStartX, iEndY, iEndX;
        int iCenterY = bndRect.y + bndRect.height / 2;
        int iCenterX = bndRect.x + bndRect.width / 2;

        // horz
        iStartY = (int)circleCenter.y + 150;
        iEndY = (int)circleCenter.y - 150;
        iStartX = (int)circleCenter.x + 150;
        iEndX = iStartX + 100;

        for (int x = iStartX; x < iEndX; x++)
        {
            for (int y = iStartY; y > iEndY; y--)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(tmpImg, pt);
                if (pixel == WHITE)
                {
                    vhorLine.push_back(pt);
                    break;
                }
            }
        }

        Vec4f horzFitLine;

        if (vhorLine.size() == 0)
        {
            result.bFindOK = false;
            return result;
        }

        fitLine(vhorLine, horzFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

        Point2d horzPt0 = LinePtByX(horzFitLine, 0);
        Point2d horzPt1 = LinePtByX(horzFitLine, tmpImg.cols);

        horzPt0.x = horzPt0.x + roi.x;
        horzPt0.y = horzPt0.y + roi.y;
        horzPt1.x = horzPt1.x + roi.x;
        horzPt1.y = horzPt1.y + roi.y;

        line(dispImg, horzPt0, horzPt1, Scalar(0, 0, 255), 3);

        double dHorAngle = atan2(horzPt1.y - horzPt0.y, horzPt1.x - horzPt0.x);
        double dChangeAngle = degreeToRadians(53.0) - dHorAngle;

        double dDist = 764.60;//distance(745, 1005, 155, 560);
        double dX = std::cos(dChangeAngle) * dDist;
        double dY = std::sin(dChangeAngle) * dDist;
        int iVerCircleX = (int)(circleCenter.x - fabs(dY));
        int iVerCircleY = (int)(circleCenter.y - fabs(dX));

        iStartY = (int)iVerCircleY - 220;
        iEndY = iStartY + 100;
        iStartX = ((iVerCircleX - 200) < 0) ? 0 : (iVerCircleX - 200);
        iEndX = iStartX + 300;

        for (int y = iStartY; y < iEndY; y++)
        {
            for (int x = iStartX; x < iEndX; x++)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(tmpImg, pt);
                if (pixel == WHITE)
                {
                    vvertLine.push_back(pt);
                    break;
                }
            }
        }

        Vec4f vertFitLine;

        if (vvertLine.size() == 0)
        {
            result.bFindOK = false;
            return result;
        }

        fitLine(vvertLine, vertFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

        Point2d vertPt0 = LinePtByY(vertFitLine, 0);
        Point2d vertPt1 = LinePtByY(vertFitLine, tmpImg.rows);

        vertPt0.x = vertPt0.x + roi.x;
        vertPt0.y = vertPt0.y + roi.y;
        vertPt1.x = vertPt1.x + roi.x;
        vertPt1.y = vertPt1.y + roi.y;

        line(dispImg, vertPt0, vertPt1, Scalar(0, 0, 255), 3);
        ///////////////
        Point2d intersectionPt = GetIntersectionPt(horzPt0, horzPt1, vertPt0, vertPt1);
        circle(dispImg, intersectionPt, 5, CV_RGB(0, 255, 0), 10);

        result.dPosX = intersectionPt.x; // ±³Á¡À» ÀúÀå.
        result.dPosY = intersectionPt.y;
        result.dAngle = radiansToDegree(atan2(vertPt1.y - vertPt0.y, vertPt1.x - vertPt0.x));

        dHorAngle = radiansToDegree(dHorAngle);
        double dVertAngle = result.dAngle;
        double dTotalAngle = fabs(dHorAngle) + fabs(dVertAngle);

        if (dHorAngle > 0.0)
        {
            dTotalAngle = (180.0 - dVertAngle) + dHorAngle;
        }
        else
        {
            dTotalAngle = 180.0 - (dVertAngle - dHorAngle);
        }

        double dMinAngle = 89.0;
        double dMaxAngle = 91.0;
        if (dTotalAngle > dMinAngle && dTotalAngle < dMaxAngle)
        {
            result.bFindOK = true;
        }
        else
        {
            result.bFindOK = false;
        }
    }
    else
    {
        // roi (power module ŸÈ¿¡Œ­ ŽÙœÃ blob Ã£±â)
        Mat cannyImg;
        Canny(largestblob, cannyImg, 128, 255);

        Mat pmImg = procImg(bndRect).clone();
        BinarizeImage(pmImg, pmImg, 50, 255.0);
        morphologyEx(pmImg, pmImg, morphMode, kernel, Point(-1, -1), iteration);

        std::vector<Point> vContourPM;
        Rect bndRectPM;
        int cntPM = 0;

        int iPMMinSize = int(bndRect.area() * 0.02);
        int iPMMaxSize = iPMMinSize * 10;
        Mat connectorBlob = FindLargestContour(pmImg, vContourPM, iPMMinSize, iPMMaxSize, bndRectPM, cntPM, true, CV_FILLED);

        // check angle
        Point2d pmCenter = Point2d(bndRect.x + bndRect.width / 2 + roi.x, bndRect.y + bndRect.height / 2 + roi.y);
        Point2d connectCenter = Point2d(bndRectPM.x + bndRectPM.width / 2 + roi.x + bndRect.x, bndRectPM.y + bndRectPM.height / 2 + roi.y + bndRect.y);

        double distY = pmCenter.y - connectCenter.y;
        double distX = pmCenter.x - connectCenter.x;
        float th = float(atan2(distY, distX));
        double dDist = calcDistance(bndRect.x, bndRect.y, bndRectPM.x, bndRectPM.y) / 2;

        int stX, endX, stY, endY;
        // ³ªÁß¿¡ ÆÄ¶ó¹ÌÅÍ·Î »©ÀÚ
        stX = -100;  stY = -50; endX = 500; endY = 100;
        stX = int(stX + pmCenter.x + (cos(th)) * dDist);
        stY = int(stY + pmCenter.y + (sin(th)) * dDist);
        endX = int(endX + pmCenter.x + (cos(th)) * dDist);
        endY = int(endY + pmCenter.y + (sin(th)) * dDist);

        std::vector<Point> vvertLine;

        for (int y = stY - roi.y; y < endY - roi.y; y++)
        {
            for (int x = stX - roi.x; x < endX - roi.x; x++)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(cannyImg, pt);
                if (pixel == WHITE)
                {
                    vvertLine.push_back(pt);
                    break;
                }
            }
        }

        Vec4f vertFitLine;

        if (vvertLine.size() == 0)
        {
            result.bFindOK = false;
            return result;
        }

        fitLine(vvertLine, vertFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

        Point2d vertPt0 = LinePtByY(vertFitLine, 0);
        Point2d vertPt1 = LinePtByY(vertFitLine, cannyImg.rows);

        vertPt0.x = vertPt0.x + roi.x;
        vertPt0.y = vertPt0.y + roi.y;
        vertPt1.x = vertPt1.x + roi.x;
        vertPt1.y = vertPt1.y + roi.y;

        line(dispImg, vertPt0, vertPt1, Scalar(0, 0, 255), 3);

        // ³ªÁß¿¡ ÆÄ¶ó¹ÌÅÍ·Î »©ÀÚ
        stX = -200;  stY = -100; endX = 300; endY = 200;
        th = float(atan2(distY, distX)) - 90;
        stX = int(stX + pmCenter.x + (cos(th)) * dDist);
        stY = int(stY + pmCenter.y + (sin(th)) * dDist);
        endX = int(endX + pmCenter.x + (cos(th)) * dDist);
        endY = int(endY + pmCenter.y + (sin(th)) * dDist);

        std::vector<Point> vhorzLine;

        for (int x = stX - roi.x; x < endX - roi.x; x++)
        {
            for (int y = stY - roi.y; y < endY - roi.y; y++)
            {
                Point pt(x, y);
                uchar pixel = GetPixel(cannyImg, pt);
                if (pixel == WHITE)
                {
                    vhorzLine.push_back(pt);
                    break;
                }
            }
        }

        Vec4f horzFitLine;

        if (vhorzLine.size() == 0)
        {
            result.bFindOK = false;
            return result;
        }

        fitLine(vhorzLine, horzFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

        Point2d horzPt0 = LinePtByX(horzFitLine, 0);
        Point2d horzPt1 = LinePtByX(horzFitLine, cannyImg.cols);

        horzPt0.x = horzPt0.x + roi.x;
        horzPt0.y = horzPt0.y + roi.y;
        horzPt1.x = horzPt1.x + roi.x;
        horzPt1.y = horzPt1.y + roi.y;

        line(dispImg, horzPt0, horzPt1, Scalar(0, 0, 255), 3);

        ///////////////
        Point2d intersectionPt = GetIntersectionPt(horzPt0, horzPt1, vertPt0, vertPt1);
        circle(dispImg, intersectionPt, 5, CV_RGB(0, 255, 0), 10);

        result.dPosX = intersectionPt.x; // ±³Á¡À» ÀúÀå.
        result.dPosY = intersectionPt.y;

        result.dAngle = radiansToDegree(atan2(vertPt1.y - vertPt0.y, vertPt1.x - vertPt0.x));

        double dHorAngle = radiansToDegree(atan2(horzPt1.y - horzPt0.y, horzPt1.x - horzPt0.x));
        //dHorAngle = radiansToDegree(dHorAngle);
        double dVertAngle = result.dAngle;
        double dTotalAngle = fabs(dHorAngle) + fabs(dVertAngle);

        if (dHorAngle > 0.0)
        {
            dTotalAngle = (180.0 - dVertAngle) + dHorAngle;
        }
        else
        {
            dTotalAngle = 180.0 - (dVertAngle - dHorAngle);
        }

        double dMinAngle = 88.5;
        double dMaxAngle = 91;
        if (dTotalAngle > dMinAngle && dTotalAngle < dMaxAngle)
        {
            result.bFindOK = true;
        }
        else
        {
            result.bFindOK = false;
        }
    }
    return result;
}

AlignResult CImageProcess::DUPBA_FindCenterPtFromEdge(Mat srcImg, Mat& dispImg, ImgProcParam param, bool bAutomode)
{
    AlignResult result;
    result.bFindOK = false;

    result.dPosX = 0;
    result.dPosY = 0;
    result.dPosZ = 0;
    result.dAngle = -999.0;

    Mat binImg, gaussianImg;
    Rect roi = param.rectRoi;

    Mat procImg = srcImg(roi).clone();
    dispImg = srcImg.clone();

    cvtColor(procImg, procImg, CV_BGR2GRAY);

    Scalar avg = mean(procImg);
    int iThreshHoldMin = param.iThresholdValLow[POINT1];
    int iThreshHoldMax = param.iThresholdValHigh[POINT1];

    //dThreshHoldMin = (double)(param.iThresholdValLow[POINT1]);
    //dThreshHoldMax = (double)(param.iThresholdValHigh[POINT1]);
    //iThreshHoldMin = 6.0; // º¯°æ ÇÊ¿ä.

    BinarizeImage(procImg, binImg, iThreshHoldMin, iThreshHoldMax);
    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    std::vector<Point> vContour;
    Rect bndRect;
    int cnt = 0;
    // ÁŠÇ°ž¶ŽÙ º¯°æ ÇÊ¿ä.
    int iBoardMinSize = (1250 * 800);
    int iBoardMaxSize = (1350 * 950);

    Mat largestblob = FindLargestContour(binImg, vContour, iBoardMinSize, iBoardMaxSize, bndRect, cnt, true, CV_FILLED);

    // test start
    vector<Vec3f> circles;
    if (largestblob.channels() == 3) {
        cvtColor(largestblob, largestblob, CV_BGR2GRAY);
    }

    GaussianBlur(largestblob, gaussianImg, Size(5, 5), 5, 5);

    int minRSize = 50;
    int maxRSize = 80;
    //int minRSize = 40;
    //int maxRSize = 80;
    HoughCircles(gaussianImg, circles, CV_HOUGH_GRADIENT, 2.0, 100, 100, 50, minRSize, maxRSize);

    Point2d circleCenter(-999.0, -999.0);
    float circleRadius = 0.0;
    Point2d tempCenter;
    uchar pixelVal;
    double lowerYCenter = -999.0;
    //1202, 779, 1682, 1159
    Rect tempCircleBnd(1188, 805, 1666, 1153);
    Point CircleleftTop(1188, 805);
    Point CircleRightBottom(1666, 1153);
    if (circles.size() != 0)
    {
        for (int i = 0; i<int(circles.size()); i++)
        {
            tempCenter = Point2d(circles[i][0], circles[i][1]);
            pixelVal = GetPixel(gaussianImg, circleCenter);
            if (tempCenter.y > lowerYCenter)
            {
                lowerYCenter = tempCenter.y;
                circleCenter = tempCenter;
                circleRadius = circles[i][2];
            }

            if (m_bIsDebugMode)
            {
                tempCenter.x += roi.x;
                tempCenter.y += roi.y;
                cv::circle(dispImg, tempCenter, (int)circles[i][2], CV_RGB(255, 0, 0), 1);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 1);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 1);

            }
        }

        tempCenter = circleCenter;
        tempCenter.x += roi.x;
        tempCenter.y += roi.y;

        if (tempCenter.x >= CircleleftTop.x && tempCenter.x <= CircleRightBottom.x
            && tempCenter.y >= CircleleftTop.y && tempCenter.y <= CircleRightBottom.y)
        {
            cv::circle(dispImg, tempCenter, (int)circleRadius, CV_RGB(255, 0, 255), 3);
            cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 3);
            cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 3);
        }
        else
        {
            cv::circle(dispImg, tempCenter, (int)circleRadius, CV_RGB(0, 0, 255), 3);
            cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 255), 3);
            cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 255), 3);
            //return result;
        }
    }
    else
    {
        //AfxMessageBox(("¿øÀ» Ã£À»Œö ŸøÀœ"));
        return result;
    }

    // test end
    Mat tmpImg;
    Canny(largestblob, tmpImg, 128, 255);

    std::vector<Point> vhorLine, vvertLine;

    int iStartY, iStartX, iEndY, iEndX;
    int iCenterY = bndRect.y + bndRect.height / 2;
    int iCenterX = bndRect.x + bndRect.width / 2;

    // horz
    iStartY = (int)(circleCenter.y - 55);
    iEndY = (int)(circleCenter.y + 55);
    iStartX = (int)(circleCenter.x - 200);
    iEndX = iStartX + 100;

    for (int x = iStartX; x < iEndX; x++)
    {
        for (int y = iStartY; y < iEndY; y++)
        {
            Point pt(x, y);
            uchar pixel = GetPixel(tmpImg, pt);
            if (pixel == WHITE)
            {
                vhorLine.push_back(pt);
                break;
            }
        }
    }

    Vec4f horzFitLine;

    if (vhorLine.size() == 0)
    {
        result.bFindOK = false;
        return result;
    }

    fitLine(vhorLine, horzFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

    Point2d horzPt0 = LinePtByX(horzFitLine, 0);
    Point2d horzPt1 = LinePtByX(horzFitLine, tmpImg.cols);

    horzPt0.x = horzPt0.x + roi.x;
    horzPt0.y = horzPt0.y + roi.y;
    horzPt1.x = horzPt1.x + roi.x;
    horzPt1.y = horzPt1.y + roi.y;

    line(dispImg, horzPt0, horzPt1, Scalar(0, 0, 255), 3);

    double dHorAngle = atan2(horzPt1.y - horzPt0.y, horzPt1.x - horzPt0.x);
    double dChangeAngle = degreeToRadians(53.0) - dHorAngle;

    double dDist = 764.60;//distance(745, 1005, 155, 560);
    double dX = std::cos(dChangeAngle) * dDist;
    double dY = std::sin(dChangeAngle) * dDist;
    int dVerCircleX = (int)(circleCenter.x - fabs(dY));
    int dVerCircleY = (int)(circleCenter.y - fabs(dX));

    iStartY = dVerCircleY - 220;
    iEndY = iStartY + 100;
    iStartX = ((dVerCircleX - 200) < 0) ? 0 : (dVerCircleX - 200);
    iEndX = iStartX + 300;

    for (int y = iStartY; y < iEndY; y++)
    {
        for (int x = iStartX; x < iEndX; x++)
        {
            Point pt(x, y);
            uchar pixel = GetPixel(tmpImg, pt);
            if (pixel == WHITE)
            {
                vvertLine.push_back(pt);
                break;
            }
        }
    }

    Vec4f vertFitLine;

    if (vvertLine.size() == 0)
    {
        result.bFindOK = false;
        return result;
    }

    fitLine(vvertLine, vertFitLine, CV_DIST_FAIR, 0, 0.01, 0.01);

    Point2d vertPt0 = LinePtByY(vertFitLine, 0);
    Point2d vertPt1 = LinePtByY(vertFitLine, tmpImg.rows);

    vertPt0.x = vertPt0.x + roi.x;
    vertPt0.y = vertPt0.y + roi.y;
    vertPt1.x = vertPt1.x + roi.x;
    vertPt1.y = vertPt1.y + roi.y;

    line(dispImg, vertPt0, vertPt1, Scalar(0, 0, 255), 3);
    ///////////////
    Point2d intersectionPt = GetIntersectionPt(horzPt0, horzPt1, vertPt0, vertPt1);
    circle(dispImg, intersectionPt, 5, CV_RGB(0, 255, 0), 10);

    result.dPosX = intersectionPt.x; // ±³Á¡À» ÀúÀå.
    result.dPosY = intersectionPt.y;
    result.dAngle = radiansToDegree(atan2(vertPt1.y - vertPt0.y, vertPt1.x - vertPt0.x));

    dHorAngle = radiansToDegree(dHorAngle);
    double dVertAngle = result.dAngle;
    double dTotalAngle = fabs(dHorAngle) + fabs(dVertAngle);

    if (dHorAngle > 0.0)
    {
        dTotalAngle = (180.0 - dVertAngle) + dHorAngle;
    }
    else
    {
        dTotalAngle = 180.0 - (dVertAngle - dHorAngle);
    }

    double dMinAngle = 89.0;
    double dMaxAngle = 91.0;
    if (dTotalAngle > dMinAngle && dTotalAngle < dMaxAngle)
    {
        result.bFindOK = true;
    }
    else
    {
        result.bFindOK = false;
    }
    return result;
}

AlignResult CImageProcess::LTE_23G_BOXPBA_Find(Mat srcImg, Mat& dispImg, ImgProcParam param)
{
    AlignResult result;
    result.bFindOK = false;

    result.dPosX = 0;
    result.dPosY = 0;
    result.dPosZ = 0;
    result.dAngle = -999.0;

    Mat binImg, binImgForBoard;
    Rect roi = param.rectRoi;
    Mat procImg;

    if (roi.width > 0 && roi.height > 0)
    {
        procImg = srcImg(roi).clone();
    }
    else
    {
        procImg = srcImg.clone();
    }
    dispImg = srcImg.clone();

    cvtColor(procImg, procImg, CV_BGR2GRAY);

    int iThreshHoldMinForBoard = 90;
    int iThreshHoldMaxForBoard = 255;
    BinarizeImage(procImg, binImgForBoard, iThreshHoldMinForBoard, iThreshHoldMaxForBoard);

    int iThreshHoldMin = param.iThresholdValLow[POINT1];
    int iThreshHoldMax = param.iThresholdValHigh[POINT1];
    BinarizeImage(procImg, binImg, iThreshHoldMin, iThreshHoldMax);

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImgForBoard, binImgForBoard, morphMode, kernel, Point(-1, -1), iteration);
    morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    // BoardÀÇ ÁßœÉÀ» ±žÇÏ±â À§ÇÑ ºÎºÐ
    std::vector<Point> vContourBoard;
    Rect bndRect;
    int cnt = 0;
    // ÁŠÇ° ž¶ŽÙ º¯°æ ÇÊ¿ä.
    int iBoardWidth = 2150;
    int iBoardHeight = 2150;
    int iBoardSize = iBoardWidth * iBoardHeight;
    //int iBoardSize = param.iModelBlobSize;
    double iSizeTolerance = 0.2;

    double iBoardMinSize = iBoardSize * (1 - iSizeTolerance);
    double iBoardMaxSize = iBoardSize * (1 + iSizeTolerance);
    Mat largestblob = FindLargestContour(binImgForBoard, vContourBoard, iBoardMinSize, iBoardMaxSize, bndRect, cnt, true, CV_FILLED);

    if (cnt != 1)
    {
        // PBA ºžµå ¹ß°ßÇÏÁö žøÇÔ.
        // ¿¡·¯ žÞŒŒÁö Ÿ÷·Îµå
        return result;
    }

    Point ptTemp(roi.x, roi.y);
    Point2f ptBoardCenter(bndRect.width / 2.0f,  bndRect.height / 2.0f);
    ptBoardCenter.x += bndRect.x;
    ptBoardCenter.y += bndRect.y;
    circle(dispImg, (Point)ptBoardCenter + ptTemp, 10, CV_RGB(255, 0, 255), 6);

    // Board »óÀÇ CircleÀ» Ã£ŽÂ ¹ý
    std::vector<std::vector<Point>> vAllContours;
    std::vector<Vec4i> hierarchy;
    findContours(binImg, vAllContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    // Barcode <-> WhiteChip ±žºÐÀ» À§ÇÑ ºÎºÐ.
    std::vector<std::vector<Point>>::iterator it_blob = vAllContours.begin();
    int iMaxCnt = 10;
    CLtePbaResult* lteResult = new CLtePbaResult[iMaxCnt];
    int iBlobCnt= 0;
    Scalar color = NULL;

    int iDiagonalMin = 140;
    int iDiagonalMax = 200;
    while (it_blob != vAllContours.end())
    {
        double dArea = contourArea(*it_blob);
        Rect tempRect = boundingRect(*it_blob);
        RotatedRect rotatedRect = cv::minAreaRect(*it_blob);
        Rect tempRect2 = rotatedRect.boundingRect();

        double dDist = calcDistance(0, 0, tempRect.width, tempRect.height);

        if (dDist > iDiagonalMin && dDist < iDiagonalMax)
        {
            color = CV_RGB(255, 0, 0);
            if (iBlobCnt < iMaxCnt)
            {
                lteResult[iBlobCnt].ptCenter = rotatedRect.center;
                lteResult[iBlobCnt].dDist = CalcDistance(rotatedRect.center.x, rotatedRect.center.y, ptBoardCenter.x, ptBoardCenter.y);
                iBlobCnt++;
            }

            if (m_bIsDebugMode)
            {
                Point ptTemp = rotatedRect.center;
                ptTemp += Point(roi.x, roi.y);
                Point2f vertices[4];
                rotatedRect.points(vertices);
                for (int i = 0; i < 4; i++)
                {
                    line(dispImg, (Point)vertices[i] + Point(roi.x, roi.y), (Point)vertices[(i + 1) % 4] + Point(roi.x, roi.y), Scalar(100, 100, 100), 3);
                }
            }
        }
        else
        {
            color = CV_RGB(0, 0, 0);
        }

        //if (m_bIsDebugMode)
        {
            line(dispImg, Point(roi.x + tempRect.x, roi.y + tempRect.y), Point(roi.x + tempRect.x, roi.y + tempRect.y + tempRect.height), color, 2);
            line(dispImg, Point(roi.x + tempRect.x, roi.y + tempRect.y), Point(roi.x + tempRect.x + tempRect.width, roi.y + tempRect.y), color, 2);
            line(dispImg, Point(roi.x + tempRect.x + tempRect.width, roi.y + tempRect.y), Point(roi.x + tempRect.x + tempRect.width, roi.y + tempRect.y + tempRect.height), color, 2);
            line(dispImg, Point(roi.x + tempRect.x, roi.y + tempRect.y + tempRect.height), Point(roi.x + tempRect.x + tempRect.width, roi.y + tempRect.y + tempRect.height), color, 2);
        }
        it_blob++;
    }

    // White BlobÀ» BoardÀÇ ÁßœÉ¿¡Œ­ °¡±î¿î ŒøÀž·Î Á€·Ä.
    sort(lteResult, lteResult + iBlobCnt);
    for (int i = 0; i < iBlobCnt; ++i)
    {
        if (i < 2) {
            circle(dispImg, ptTemp + lteResult[i].ptCenter, 3, CV_RGB(255, 0, 0), 2);
            line(dispImg, (Point)ptBoardCenter + ptTemp, ptTemp + lteResult[i].ptCenter, CV_RGB(150, 0, 0), 6);
        }
        else
        {
            circle(dispImg, ptTemp + lteResult[i].ptCenter, 3, CV_RGB(0, 255, 255), 2);
        }
    }

    // ÁÂ, ¿ì, »ó, ÇÏ ±žºÐ
    Direction iDirection = LEFT;
    Point temp1;
    if (ptBoardCenter.x < lteResult[0].ptCenter.x && ptBoardCenter.x < lteResult[1].ptCenter.x)
    {
        iDirection = RIGHT;
        if (lteResult[0].ptCenter.y > lteResult[1].ptCenter.y)
        {
            temp1 = lteResult[1].ptCenter;
            lteResult[1].ptCenter = lteResult[0].ptCenter;
            lteResult[0].ptCenter = temp1;
        }
    }
    else if (ptBoardCenter.x > lteResult[0].ptCenter.x && ptBoardCenter.x > lteResult[1].ptCenter.x)
    {
        iDirection = LEFT;
        if (lteResult[0].ptCenter.y < lteResult[1].ptCenter.y)
        {
            temp1 = lteResult[1].ptCenter;
            lteResult[1].ptCenter = lteResult[0].ptCenter;
            lteResult[0].ptCenter = temp1;
        }
    }
    else if (ptBoardCenter.y > lteResult[0].ptCenter.y && ptBoardCenter.y > lteResult[1].ptCenter.y)
    {
        iDirection = UP;
        if (lteResult[0].ptCenter.x > lteResult[1].ptCenter.x)
        {
            temp1 = lteResult[1].ptCenter;
            lteResult[1].ptCenter = lteResult[0].ptCenter;
            lteResult[0].ptCenter = temp1;
        }
    }
    else
    {
        iDirection = DOWN;
        if (lteResult[0].ptCenter.x < lteResult[1].ptCenter.x)
        {
            temp1 = lteResult[1].ptCenter;
            lteResult[1].ptCenter = lteResult[0].ptCenter;
            lteResult[0].ptCenter = temp1;
        }
    }

    result.dPosX = lteResult[0].ptCenter.x + roi.x;
    result.dPosY = lteResult[0].ptCenter.y + roi.y;
    // CW Angle
    double dCWAngle = radiansToDegree(atan2(lteResult[0].ptCenter.y - lteResult[1].ptCenter.y, lteResult[0].ptCenter.x - lteResult[1].ptCenter.x));
    dCWAngle = floor(dCWAngle * 100) / 100;
    result.dAngle = dCWAngle;
    result.bFindOK = true;
    delete[] lteResult;

    return result;
}
// ShieldCanÀÇ Center¿¡Œ­ -> ÁßœÉ »óŽÜÀÇ Circle ÁßœÉ°£ÀÇ °¢µµ & Circle ÁßœÉÀÇ °ªÀ» ¹ÝÈ¯.
AlignResult CImageProcess::LTE_23GShieldCan_BOX_FindCenterPt(Mat srcImg, Mat& dispImg, ImgProcParam param)
{
    AlignResult result;
    result.bFindOK = false;

    result.dPosX = 0;
    result.dPosY = 0;
    result.dPosZ = 0;
    result.dAngle = -999.0;

    Mat binImg, gaussianImg, binImgForBoard;
    Rect roi = param.rectRoi;

    Mat procImg = srcImg(roi).clone();
    dispImg = srcImg.clone();

    cvtColor(procImg, procImg, CV_BGR2GRAY);

    int iThreshHoldMinForBoard = 90;
    int iThreshHoldMaxForBoard = 255;
    BinarizeImage(procImg, binImgForBoard, iThreshHoldMinForBoard, iThreshHoldMaxForBoard);

    int iThreshHoldMin = param.iThresholdValLow[POINT1];
    int iThreshHoldMax = param.iThresholdValHigh[POINT1];

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImgForBoard, binImgForBoard, morphMode, kernel, Point(-1, -1), iteration);

    std::vector<Point> vContour;
    Rect bndRect;
    int cnt = 0;
    // ÁŠÇ°ž¶ŽÙ º¯°æ ÇÊ¿ä.
    int iBoardWidth = 1307;
    int iBoardHeight = 1323;
    int iBoardSize = iBoardWidth * iBoardHeight;
    double iSizeTolerance = 0.2;

    double iBoardMinSize = iBoardSize * (1 - iSizeTolerance);
    double iBoardMaxSize = iBoardSize * (1 + iSizeTolerance);
    Mat largestblob = FindLargestContour(binImgForBoard, vContour, iBoardMinSize, iBoardMaxSize, bndRect, cnt, true, CV_FILLED);

    RotatedRect rotatedRect = cv::minAreaRect(vContour);

    Point2d ptBoardCenter = rotatedRect.center;
    Point2f vertices[4];
    rotatedRect.points(vertices);
    for (int i = 0; i < 4; i++)
    {
        line(dispImg, (Point)vertices[i] + Point(roi.x, roi.y), (Point)vertices[(i + 1) % 4] + Point(roi.x, roi.y), Scalar(100, 100, 100), 3);
    }
    circle(dispImg, ptBoardCenter + Point2d(roi.x, roi.y), 6, CV_RGB(255, 0, 0), 6);

    if (m_bIsDebugMode)
    {
        Point2d ptTempBoardCenter(bndRect.width / 2, bndRect.height / 2);
        ptTempBoardCenter.x += bndRect.x;
        ptTempBoardCenter.y += bndRect.y;
        circle(dispImg, (Point)ptTempBoardCenter + Point(roi.x, roi.y), 6, CV_RGB(0, 0, 255), 5);
    }


    // test start
    vector<Vec3f> circles;
    if (largestblob.channels() == 3) {
        cvtColor(largestblob, largestblob, CV_BGR2GRAY);
    }

    GaussianBlur(largestblob, gaussianImg, Size(5, 5), 5, 5);

    int minRSize = 90;
    int maxRSize = 110;
    HoughCircles(gaussianImg, circles, CV_HOUGH_GRADIENT, 2.0, 100, 100, 50, minRSize, maxRSize);

    float circleRadius = 0.0;
    Point2d tempCenter;
    double dTempDist = 0.0;
    CLtePbaResult lteResult;
    if (circles.size() != 0)
    {
        for (int i = 0; i<int(circles.size()); i++)
        {
            tempCenter = Point2d(circles[i][0], circles[i][1]);

            dTempDist = CalcDistance(ptBoardCenter.x, ptBoardCenter.y, tempCenter.x, tempCenter.y);
            if (dTempDist < lteResult.dDist)
            {
                lteResult.dDist = dTempDist;
                lteResult.ptCenter = (Point)tempCenter;
                circleRadius = circles[i][2];
            }

            if (m_bIsDebugMode)
            {
                tempCenter.x += roi.x;
                tempCenter.y += roi.y;
                cv::circle(dispImg, tempCenter, (int)circles[i][2], CV_RGB(255, 0, 0), 1);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 1);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 1);
            }
        }

        tempCenter = lteResult.ptCenter;
        tempCenter.x += roi.x;
        tempCenter.y += roi.y;

        cv::circle(dispImg, tempCenter, (int)circleRadius, CV_RGB(255, 0, 255), 3);
        cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 3);
        cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 3);
    }

    Direction iDirection = LEFT;
    if (lteResult.ptCenter.x > ptBoardCenter.x)
    {
        iDirection = RIGHT;
    }
    else if (lteResult.ptCenter.x < ptBoardCenter.x)
    {
        iDirection = LEFT;
    }

    result.dPosX = lteResult.ptCenter.x + roi.x;
    result.dPosY = lteResult.ptCenter.y + roi.y;
    // CW Angle
    double dCWAngle = radiansToDegree(atan2(ptBoardCenter.y - lteResult.ptCenter.y, ptBoardCenter.x - lteResult.ptCenter.x));
    dCWAngle = floor(dCWAngle * 100) / 100;
    result.dAngle = dCWAngle;
    result.bFindOK = true;
    return result;
}

Point CImageProcess::LTE_23GPBA_FindHole(cv::Mat srcImg, cv::Mat& dispImg, ImgProcParam param, int iThresholdValLow, int iThresholdValHigh, int minArea, int maxArea)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;
    Rect roi = param.rectRoi;

    dispImg = srcImg.clone();
    procImg = srcImg(roi).clone();

    // blobÀž·Î half circle À§Ä¡ÇÑ °÷À» ÁŠ¿ÜÇÑ ºÎºÐÀ» ŸøŸÖ ¹öž°ŽÙ
    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    if (binImg.channels() == 3) {
        cvtColor(binImg, binImg, CV_BGR2GRAY);
    }

    GaussianBlur(binImg, binImg, Size(5, 5), 5, 5);

    //int cnt = getBlob(binImg, maxRSize, minRSize);
    int cnt = getCircleBlob(binImg, maxArea, minArea);
    Point ptResult(0, 0);
    int iRadius = 0;
    Point center;
    double dDist = 100000;
    Point ptImgCenter(binImg.size().width / 2, binImg.size().height / 2);
    CNewBlob* newblob = NULL;
    Point ptTemp;

    if (cnt > 0)
    {
        for (int i = 0; i < cnt; ++i)
        {
            newblob = m_vBlobArray.at(i);
            center = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
            if (calcDistance(center.x, center.y, ptImgCenter.x, ptImgCenter.y) < dDist)
            {
                dDist = calcDistance(center.x, center.y, ptImgCenter.x, ptImgCenter.y);
                ptResult = center;
                iRadius = newblob->m_blobRect.size().width / 2;
            }

            if (m_bIsDebugMode)
            {
                center.x += roi.x;
                center.y += roi.y;
                circle(dispImg, center, (newblob->m_blobRect.size().width) / 2, CV_RGB(0, 0, 255), 3);
                cv::line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(255, 0, 0), 3);
                cv::line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(255, 0, 0), 3);
            }
        }
        ptResult.x += roi.x;
        ptResult.y += roi.y;
        circle(dispImg, ptResult, iRadius, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, Point(ptResult.x - 10, ptResult.y), Point(ptResult.x + 10, ptResult.y), CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(ptResult.x, ptResult.y - 10), Point(ptResult.x, ptResult.y + 10), CV_RGB(0, 255, 0), 2);
    }
    return ptResult;
}

Point CImageProcess::LTE_23GPBA_FindSlotHole(cv::Mat srcImg, cv::Mat& dispImg, ImgProcParam param, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;
    Rect roi = param.rectRoi;
    Point ptResult(-999,-999);
    procImg = srcImg(roi).clone();

    // blobÀž·Î half circle À§Ä¡ÇÑ °÷À» ÁŠ¿ÜÇÑ ºÎºÐÀ» ŸøŸÖ ¹öž°ŽÙ
    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    if (binImg.channels() == 3) {
        cvtColor(binImg, binImg, CV_BGR2GRAY);
    }

    GaussianBlur(binImg, binImg, Size(3, 3), 3, 3);
    std::vector<Point> vContourSlot;
    Rect bndRect;
    int cnt = 0;
    // ÁŠÇ°ž¶ŽÙ º¯°æ ÇÊ¿ä.
    int iBoardWidth = 110;
    int iBoardHeight = 110;
    int iBoardSize = iBoardWidth * iBoardHeight;
    double iSizeTolerance = 0.2;

    double iBoardMinSize = iBoardSize * (1 - iSizeTolerance);
    double iBoardMaxSize = iBoardSize * (1 + iSizeTolerance);
    std::vector<std::vector<cv::Point>> vAllContours;
    FindContours(binImg, vAllContours, iBoardMinSize, iBoardMaxSize, CONT_AREA, true);
    int iIdx = 0;
    std::vector<std::vector<cv::Point>>::iterator it_blob = vAllContours.begin();
    Point2f ptImgCenter(binImg.size().width / 2.0f, binImg.size().height / 2.0f);
    double dDist = 100000.0;
    Point ptTemp;
    RotatedRect rotatedRect2;
    Point2f vertices[4];
    while (it_blob != vAllContours.end())
    {
        rotatedRect2 = cv::minAreaRect(*it_blob);

        if (CalcDistance(ptImgCenter.x, ptImgCenter.y, rotatedRect2.center.x, rotatedRect2.center.y) < dDist)
        {
            dDist = CalcDistance(ptImgCenter.x, ptImgCenter.y, rotatedRect2.center.x, rotatedRect2.center.y);
            ptResult = rotatedRect2.center;
            rotatedRect2.points(vertices);
        }

        if (m_bIsDebugMode)
        {
            ptTemp = rotatedRect2.center;
            ptTemp += Point(roi.x, roi.y);
            Point2f vertices[4];
            rotatedRect2.points(vertices);
            for (int i = 0; i < 4; i++)
            {
                line(dispImg, (Point)vertices[i] + Point(roi.x, roi.y), (Point)vertices[(i + 1) % 4] + Point(roi.x, roi.y), Scalar(100, 100, 100), 3);
            }
            circle(dispImg, ptTemp, 6, CV_RGB(0, 0, 255), 6);
        }
        ++it_blob;
    }

    ptResult += Point(roi.x, roi.y);
    for (int i = 0; i < 4; i++)
    {
        line(dispImg, (Point)vertices[i] + Point(roi.x, roi.y), (Point)vertices[(i + 1) % 4] + Point(roi.x, roi.y), Scalar(100, 100, 100), 3);
    }
    circle(dispImg, ptResult, 6, CV_RGB(255, 0, 255), 6);
    return ptResult;
}

AlignResult CImageProcess::LTE_23GShielCan_FindHole(cv::Mat srcImg, cv::Mat& dispImg, ImgProcParam param, int iThresholdValLow, int iThresholdValHigh, int minArea, int maxArea)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;
    Rect roi = param.rectRoi;

    dispImg = srcImg.clone();
    if (roi.width > 0 && roi.height > 0)
    {
        procImg = srcImg(roi).clone();
    }
    else
    {
        procImg = srcImg.clone();
    }

    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    int iteration = 3;
    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    int morphMode = MORPH_OPEN;
    morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    if (binImg.channels() == 3) {
        cvtColor(binImg, binImg, CV_BGR2GRAY);
    }

    GaussianBlur(binImg, binImg, Size(5, 5), 5, 5);

    int cnt = getCircleBlob(binImg, maxArea, minArea);
    AlignResult cResult;
    cResult.bFindOK = false;
    cResult.dAngle = 0.0;
    cResult.dPosX = 0.0;
    cResult.dPosY = 0.0;
    cResult.dPosZ = 0.0;
    int iRadius = 0;
    Point center;
    double dDist = 100000;
    Point ptImgCenter(binImg.size().width / 2, binImg.size().height / 2);
    CNewBlob* newblob = NULL;
    Point ptTemp;
    CHolePoint * cHolePoint = new CHolePoint[cnt];

    if (cnt > 1)
    {
        for (int i = 0; i < cnt; ++i)
        {
            newblob = m_vBlobArray.at(i);
            center = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
            cHolePoint[i].dDist = CalcDistance(center.x, center.y, ptImgCenter.x, ptImgCenter.y);
            cHolePoint[i].iRadius = newblob->m_blobRect.size().width / 2;
            cHolePoint[i].pCenter = center;
            cHolePoint[i].pCenter += Point(roi.x, roi.y);

            if (m_bIsDebugMode)
            {
                center.x += roi.x;
                center.y += roi.y;
                circle(dispImg, center, (newblob->m_blobRect.size().width) / 2, CV_RGB(0, 0, 255), 3);
                cv::line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(255, 0, 0), 3);
                cv::line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(255, 0, 0), 3);
            }
        }
        sort(cHolePoint, cHolePoint + cnt);

        if (cHolePoint[0].pCenter.x > cHolePoint[1].pCenter.x)
        {
            CHolePoint cTemp;
            cTemp = cHolePoint[0];
            cHolePoint[0] = cHolePoint[1];
            cHolePoint[1] = cTemp;
        }

        circle(dispImg, cHolePoint[0].pCenter, cHolePoint[0].iRadius, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, Point(cHolePoint[0].pCenter.x - 10, cHolePoint[0].pCenter.y), Point(cHolePoint[0].pCenter.x + 10, cHolePoint[0].pCenter.y), CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(cHolePoint[0].pCenter.x, cHolePoint[0].pCenter.y - 10), Point(cHolePoint[0].pCenter.x, cHolePoint[0].pCenter.y + 10), CV_RGB(0, 255, 0), 2);

        circle(dispImg, cHolePoint[1].pCenter, cHolePoint[1].iRadius, CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(cHolePoint[1].pCenter.x - 10, cHolePoint[1].pCenter.y), Point(cHolePoint[1].pCenter.x + 10, cHolePoint[1].pCenter.y), CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(cHolePoint[1].pCenter.x, cHolePoint[1].pCenter.y - 10), Point(cHolePoint[1].pCenter.x, cHolePoint[1].pCenter.y + 10), CV_RGB(0, 255, 0), 2);

        cv::line(dispImg, cHolePoint[0].pCenter, cHolePoint[1].pCenter, CV_RGB(255, 0, 0), 2);

        cResult.bFindOK = true;
        cResult.dPosX = cHolePoint[0].pCenter.x;
        cResult.dPosY = cHolePoint[0].pCenter.y;
        cResult.dAngle = radiansToDegree(atan2(cHolePoint[0].pCenter.y - cHolePoint[1].pCenter.y, cHolePoint[0].pCenter.x - cHolePoint[1].pCenter.x));
        if (cResult.dAngle < 0)
        {
            cResult.dAngle = 360.0 + cResult.dAngle;
        }
    }
    delete [] cHolePoint;
    return cResult;
}

Point2d CImageProcess::ShowAndFindRotatePtFromCenter(Mat& dispImg, Point2d centerPt, Point2d& originPt, double dAngle)
{
    Point2d rotatePt = FindRotatePtFromCenter(centerPt, originPt, dAngle);
    circle(dispImg, rotatePt, 5, CV_RGB(255, 0, 0), 10);
    circle(dispImg, originPt, 5, CV_RGB(168, 118, 124), 10);
    double dRadius = CalcDistance(centerPt.x, centerPt.y, originPt.x, originPt.y);
    double dStartAn = radiansToDegree(atan2(centerPt.y - originPt.y, centerPt.x - originPt.x));
    double dEndAn = radiansToDegree(atan2(centerPt.y - rotatePt.y, centerPt.x - rotatePt.x));
    ellipse(dispImg, (Point)centerPt, Size((int)dRadius, (int)dRadius), 180.0, dStartAn, dEndAn, CV_RGB(112, 48, 160), 3);
    return rotatePt;
}

Point2d CImageProcess::FindRotatePtFromCenter(Point2d centerPt, Point2d& originPt, double dAngle)
{
    AlignResult pResult;
    pResult.bFindOK = true;
    Point outPoint;
    //CW rotation
    Point2d transPt = originPt - centerPt;
    double x = std::cos((degreeToRadians(dAngle))) * transPt.x - std::sin((degreeToRadians(dAngle))) * transPt.y;
    double y = std::sin((degreeToRadians(dAngle))) * transPt.x + std::cos((degreeToRadians(dAngle))) * transPt.y;
    Point2d changePt(x, y);
    Point2d rotatePt = changePt + centerPt;
    return rotatePt;
}


int CImageProcess::getNearestPointNum(Point currentPt, std::vector<Point>* belt)
{
    int iSize;

    for (int i = 0; i< IMAGE_WIDTH; i++)
    {
        double dX = 0.0;
        iSize = (int)belt[i].size();

        for (int j = 0; j< iSize; j++)
        {
            dX += belt[i].at(j).x;
        }
        if (iSize != 0)
            dX = dX / iSize;

        if (abs(currentPt.x - dX) < 00)
            return i;
    }

    return -1;
}

int compare(void *first, void *second)
{
    if (*(int*)first > *(int*)second)
        return 1;
    else if (*(int*)first < *(int*)second)
        return -1;
    else
        return 0;
}

double CImageProcess::GetColor(Mat srcImg, int colorCh)
{
    if (srcImg.channels() != 3 || srcImg.data == NULL)
        return 0.0;

    std::string strPath, strFileName;
    Mat procImg = srcImg.clone();
    cvtColor(srcImg, procImg, CV_BGR2HSV);

    vector<Mat> hsv_planes;
    split(procImg, hsv_planes);

    Scalar avg = mean(hsv_planes[colorCh]);

    return avg.val[0];
}


uchar CImageProcess::GetPixel(Mat & srcImg, Point pt)
{
    if (pt.x < 0 || pt.y < 0 || pt.x > srcImg.cols || pt.y > srcImg.rows
        || (pt.y*srcImg.cols + pt.x) > (int)srcImg.total())
        return 0;

    return srcImg.data[pt.y*srcImg.cols + pt.x];
}

void CImageProcess::SetPixel(Mat & srcImg, Point pt, uchar value)
{
    if (pt.x < 0 || pt.y < 0 || pt.x > srcImg.cols || pt.y > srcImg.rows
        || (pt.y*srcImg.cols + pt.x) > (int)srcImg.total())
        return;

    srcImg.data[pt.y*srcImg.cols + pt.x] = value;
}

cv::Vec3b CImageProcess::GetPixel3C(cv::Mat & srcImg, cv::Point pt)
{
    cv::Vec3b value;
    value.val[0] = srcImg.data[pt.y*srcImg.cols * 3 + pt.x];
    value.val[1] = srcImg.data[pt.y*srcImg.cols * 3 + pt.x + 1];
    value.val[2] = srcImg.data[pt.y*srcImg.cols * 3 + pt.x + 2];

    return value;
}

void CImageProcess::SetPixel3C(cv::Mat & srcImg, cv::Point pt, cv::Vec3b value)
{
    srcImg.data[pt.y*srcImg.cols * 3 + pt.x] = value.val[0];
    srcImg.data[pt.y*srcImg.cols * 3 + pt.x + 1] = value.val[1];
    srcImg.data[pt.y*srcImg.cols * 3 + pt.x + 2] = value.val[2];
}


Mat CImageProcess::FindContours(Mat & srcImg, std::vector<std::vector<Point>> & vContours, double dMin, double dMax, int iAreaType, bool bDraw, Scalar drawColor)
{
    Mat tempImg;
    if (srcImg.channels() == 3)
    {
        cvtColor(srcImg, tempImg, CV_BGR2GRAY);
    }
    else
    {
        tempImg = srcImg.clone();
    }

    std::vector<std::vector<Point>> vAllContours;
    std::vector<Vec4i> hierarchy;
    findContours(tempImg, vAllContours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

    std::vector<std::vector<Point>> vSelected;

    std::vector<std::vector<Point>>::iterator it_blob = vAllContours.begin();
    while (it_blob != vAllContours.end())
    {
        double dArea = contourArea(*it_blob);
        Rect bndRect = boundingRect(*it_blob);
        double dRectArea = bndRect.area();
        double dDist = calcDistance(0, 0, bndRect.width, bndRect.height);

        double dVal = 0.0;

        if (iAreaType == CONT_DIST) dVal = dDist;
        else if (iAreaType == CONT_AREA) dVal = dArea;
        else dVal = dRectArea;

        if (dVal >= dMin && dVal <= dMax)
        {
            vSelected.push_back(*it_blob);
        }

        ++it_blob;
    }

    Mat contImg;
    if (bDraw)
    {
        contImg.create(srcImg.size(), CV_8UC3);
        contImg = 0;
        drawContours(contImg, vSelected, -1, drawColor, CV_FILLED);
    }

    vContours = vSelected;

    return contImg;
}


Mat CImageProcess::FindLargestContour(Mat & srcImg, std::vector<Point> & vContour, double dMin, double dMax, int& cnt, bool bDraw, int iDrawLength, Scalar drawColor)
{
    Mat tempImg;
    if (srcImg.channels() == 3)
    {
        cvtColor(srcImg, tempImg, CV_BGR2GRAY);
    }
    else
    {
        tempImg = srcImg.clone();
    }

    std::vector<std::vector<Point>> vAllContours;
    std::vector<Vec4i> hierarchy;
    findContours(tempImg, vAllContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    int iMaxIdx = 0;
    double dMaxArea = 0.0;

    int iIdx = 0;
    std::vector<std::vector<Point>>::iterator it_blob = vAllContours.begin();
    while (it_blob != vAllContours.end())
    {
        double dArea = contourArea(*it_blob);
        Rect bndRect = boundingRect(*it_blob);
        double dDist = calcDistance(0, 0, bndRect.width, bndRect.height);

        if (dArea>dMaxArea)
        {
            iMaxIdx = iIdx;
            dMaxArea = dArea;
        }

        ++it_blob;
        iIdx++;

        if (dArea > dMin)
            cnt++;
    }

    Mat contImg;
    if (bDraw)
    {
        contImg.create(srcImg.size(), CV_8UC3);
        contImg = 0;
        drawContours(contImg, vAllContours, iMaxIdx, drawColor, iDrawLength);
    }

    if (iMaxIdx<(int)vAllContours.size())
        vContour = vAllContours[iMaxIdx];

    return contImg;
}

Mat CImageProcess::FindLargestContour(Mat & srcImg, std::vector<Point> & vContour, double dMin, double dMax, Rect& bndRect, int& cnt, bool bDraw, int iDrawLength, Scalar drawColor)
{
    Mat tempImg;
    Mat tempDspImg;
    if (srcImg.channels() == 3)
    {
        cvtColor(srcImg, tempImg, CV_BGR2GRAY);
    }
    else
    {
        tempImg = srcImg.clone();
        tempDspImg = srcImg.clone();
    }

    std::vector<std::vector<Point>> vAllContours;
    std::vector<Vec4i> hierarchy;
    findContours(tempImg, vAllContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    int iMaxIdx = 0;
    double dMaxArea = 0.0;

    int iIdx = 0;
    std::vector<std::vector<Point>>::iterator it_blob = vAllContours.begin();
    while (it_blob != vAllContours.end())
    {
        double dArea = contourArea(*it_blob);
        Rect tempRect = boundingRect(*it_blob);
        double dDist = calcDistance(0, 0, tempRect.width, tempRect.height);

        if (dArea>dMaxArea)
        {
            iMaxIdx = iIdx;
            dMaxArea = dArea;
            bndRect = tempRect;
            if (m_bIsDebugMode)
            {
                line(tempDspImg, Point(tempRect.x, tempRect.y), Point(tempRect.x + tempRect.width, tempRect.y), Scalar(255, 0, 0), 2);
                line(tempDspImg, Point(tempRect.x, tempRect.y), Point(tempRect.x, tempRect.y + tempRect.height), Scalar(255, 0, 0), 2);
                line(tempDspImg, Point(tempRect.x + tempRect.width, tempRect.y), Point(tempRect.x + tempRect.width, tempRect.y + tempRect.height), Scalar(255, 0, 0), 4);
                line(tempDspImg, Point(tempRect.x, tempRect.y + tempRect.height), Point(tempRect.x + tempRect.width, tempRect.y + tempRect.height), Scalar(255, 0, 0), 4);
            }
        }

        ++it_blob;
        iIdx++;

        if (dArea > dMin)
            cnt++;
    }
    Mat contImg;
    if (bDraw)
    {
        contImg.create(srcImg.size(), CV_8UC3);
        contImg = 0;
        drawContours(contImg, vAllContours, iMaxIdx, drawColor, iDrawLength);
    }

    if (iMaxIdx<(int)vAllContours.size())
        vContour = vAllContours[iMaxIdx];

    return contImg;
}

Mat CImageProcess::FindLargestContour2(Mat & srcImg, std::vector<Point> & vContour, double dMin, double dMax, Rect& bndRect, int& cnt, bool bDraw, int iDrawLength, Scalar drawColor)
{
    Mat tempImg;
    Mat tempDspImg;
    if (srcImg.channels() == 3)
    {
        cvtColor(srcImg, tempImg, CV_BGR2GRAY);
    }
    else
    {
        tempImg = srcImg.clone();
        tempDspImg = srcImg.clone();
    }

    std::vector<std::vector<Point>> vAllContours;
    std::vector<Vec4i> hierarchy;
    findContours(tempImg, vAllContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    int iMaxIdx = 0;
    double dMaxArea = 0.0;

    int iIdx = 0;
    std::vector<std::vector<Point>>::iterator it_blob = vAllContours.begin();
    while (it_blob != vAllContours.end())
    {
        double dArea = contourArea(*it_blob);
        Rect tempRect = boundingRect(*it_blob);
        double dDist = calcDistance(0, 0, tempRect.width, tempRect.height);

        //if (dArea>dMaxArea)
        {
            iMaxIdx = iIdx;
            dMaxArea = dArea;
            bndRect = tempRect;
            if (m_bIsDebugMode)
            {
                line(tempDspImg, Point(tempRect.x, tempRect.y), Point(tempRect.x + tempRect.width, tempRect.y), Scalar(255, 0, 0), 2);
                line(tempDspImg, Point(tempRect.x, tempRect.y), Point(tempRect.x, tempRect.y + tempRect.height), Scalar(255, 0, 0), 2);
                line(tempDspImg, Point(tempRect.x + tempRect.width, tempRect.y), Point(tempRect.x + tempRect.width, tempRect.y + tempRect.height), Scalar(255, 0, 0), 2);
                line(tempDspImg, Point(tempRect.x, tempRect.y + tempRect.height), Point(tempRect.x + tempRect.width, tempRect.y + tempRect.height), Scalar(255, 0, 0), 2);
            }
        }

        ++it_blob;
        iIdx++;

        if (dArea > dMin && dArea < dMax) {
            cnt++;
        }
    }

    Mat contImg;
    if (bDraw)
    {
        contImg.create(srcImg.size(), CV_8UC3);
        contImg = 0;
        drawContours(contImg, vAllContours, iMaxIdx, drawColor, iDrawLength);
    }

    if (iMaxIdx<(int)vAllContours.size())
        vContour = vAllContours[iMaxIdx];

    return contImg;
}


Point2d CImageProcess::LinePtByX(Vec4f line, double x)
{
    double vx = line.val[0];
    double vy = line.val[1];
    double x0 = line.val[2];
    double y0 = line.val[3];

    if (vx == 0.0f) vx = 0.00001f;
    double y = (vy * (x - x0) / vx) + y0;

    return Point2d(x, y);
}

Point2d CImageProcess::LinePtByY(Vec4f line, double y)
{
    double vx = line.val[0];
    double vy = line.val[1];
    double x0 = line.val[2];
    double y0 = line.val[3];

    if (vy == 0.0f) vy = 0.00001f;
    double x = (vx * (y - y0) / vy) + x0;

    return Point2d(x, y);
}

Point2d CImageProcess::GetIntersectionPt(Point2d AP1, Point2d AP2, Point2d BP1, Point2d BP2)
{
    double t, s;
    double under = (BP2.y - BP1.y)*(AP2.x - AP1.x) - (BP2.x - BP1.x)*(AP2.y - AP1.y);

    if (under == 0)
        return Point2d(0, 0);

    double _t= (BP2.x - BP1.x)*(AP1.y - BP1.y) - (BP2.y - BP1.y)*(AP1.x - BP1.x);
    double _s = (AP2.x - AP1.x)*(AP1.y - BP1.y) - (AP2.y - AP1.y)*(AP1.x - BP1.x);

    t = _t / under;
    s = _s / under;

    if (t<0.0 || t>1.0 || s<0.0 || s>1.0)
        return Point2d(0, 0);

    if (_t == 0 && _s == 0)
        return Point2d(0, 0);

    double x = AP1.x + t*(double)(AP2.x - AP1.x);
    double y = AP1.y + t*(double)(AP2.y - AP1.y);

    return Point2d(x, y);
}

void CImageProcess::SetOption(bool bSaveImage)
{
    m_bSaveImage = bSaveImage;
}

double CImageProcess::radiansToDegree(const double dRadians)
{
    return dRadians * (180.0/3.141592653589793238463);
}

double CImageProcess::degreeToRadians(const double Degree)
{
    return Degree / (180.0/3.141592653589793238463);
}

Point2d CImageProcess::FindHole(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize, int blurVal)
{
    Mat procImg, grayImg, binImg;

    procImg = srcImg.clone();
    dispImg = procImg.clone();

    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    int cnt = getCircleBlob(binImg, maxRSize, minRSize);
    Point center;

    if (cnt == 1)
    {
        CNewBlob* newblob = m_vBlobArray.at(0);
        center = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
        circle(dispImg, center, (newblob->m_blobRect.size().width) / 2, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(0, 255, 0), 2);
    }
    else
    {
        for (int i = 0; i < cnt; ++i)
        {
            CNewBlob* newblob = m_vBlobArray.at(i);
            center = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
            circle(dispImg, center, (newblob->m_blobRect.size().width) / 2, CV_RGB(0, 0, 255), 3);
            cv::line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(255, 0, 0), 3);
            cv::line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(255, 0, 0), 3);
        }
        center.x = -999;
        center.y = -999;
    }
    return Point2d(center.x, center.y);
}

CVisionResult CImageProcess::FindHole_23GHousing(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minAreaSize, int maxAreaSize)
{
    Mat procImg, grayImg, binImg;
    bool bIsOk = false;

    procImg = srcImg.clone();
    dispImg = procImg.clone();

    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    int cnt = getCircleBlob(binImg, maxAreaSize, minAreaSize);
    Point center(0,0);

    if (cnt == 1)
    {
        CNewBlob* newblob = m_vBlobArray.at(0);
        center = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
        circle(dispImg, center, (newblob->m_blobRect.width - newblob->m_blobRect.x) / 2, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(0, 255, 0), 2);
        bIsOk = true;
    }
    else if(cnt > 1)
    {
        CHolePoint  * cHolePoint = new CHolePoint[cnt];
        Point ptImgCenter(binImg.size().width / 2, binImg.size().height / 2);
        CNewBlob* newblob = NULL;

        for (int i = 0; i < cnt; ++i)
        {
            newblob = m_vBlobArray.at(i);
            cHolePoint[i].pCenter = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
            cHolePoint[i].dDist = pow(ptImgCenter.x - cHolePoint[i].pCenter.x, 2) + pow(ptImgCenter.y - cHolePoint[i].pCenter.y, 2);
            cHolePoint[i].iRadius = newblob->m_blobRect.size().width / 2;

            //if (m_bIsDebugMode)
            {
                center = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
                circle(dispImg, center, (newblob->m_blobRect.size().width) / 2, CV_RGB(0, 0, 255), 2);
                cv::line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(255, 0, 0), 3);
                cv::line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(255, 0, 0), 3);
            }
            delete newblob;
        }

        m_vBlobArray.clear();

        sort(cHolePoint, cHolePoint + cnt);

        center = cHolePoint[0].pCenter;
        circle(dispImg, center, cHolePoint[0].iRadius, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(255, 0, 0), 3);
        cv::line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(255, 0, 0), 3);

        delete[] cHolePoint;
        bIsOk = true;
    }
    return CVisionResult(center.x, center.y, bIsOk);
}

CVisionResult CImageProcess::LTE23_FindHole_PBA(cv::Mat srcImg, cv::Mat& dispImg, Rect& roi, int iThresholdValLow, int iThresholdValHigh, int minAreaSize, int maxAreaSize)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;
    bool bIsOk = false;

    dispImg = srcImg.clone();

    if (roi.width > 0 && roi.height > 0)
        procImg = srcImg(roi).clone();
    else
        procImg = srcImg.clone();

    // blobÀž·Î half circle À§Ä¡ÇÑ °÷À» ÁŠ¿ÜÇÑ ºÎºÐÀ» ŸøŸÖ ¹öž°ŽÙ
    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    // blob °¡·Î ŒŒ·Î Å©±â°¡ °°Àº°Å..
    int cnt = getCircleBlob(binImg, maxAreaSize, minAreaSize);
    Point ptCenterInRoi;
    Point ptCenterReal;
    Point ptOffsetRoi(roi.x, roi.y);

    if (cnt == 1)
    {
        CNewBlob* newblob = m_vBlobArray.at(0);
        ptCenterInRoi = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
        circle(dispImg, ptCenterInRoi + ptOffsetRoi, (newblob->m_blobRect.size().width) / 2, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, Point(ptCenterInRoi.x - 10, ptCenterInRoi.y) + ptOffsetRoi, Point(ptCenterInRoi.x + 10, ptCenterInRoi.y) + ptOffsetRoi, CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(ptCenterInRoi.x, ptCenterInRoi.y - 10) + ptOffsetRoi, Point(ptCenterInRoi.x, ptCenterInRoi.y + 10) + ptOffsetRoi, CV_RGB(0, 255, 0), 2);
        bIsOk = true;
    }
    else
    {
        CHolePoint  * cHolePoint = new CHolePoint[cnt];
        Point ptRealImgCenter(dispImg.size().width / 2, dispImg.size().height / 2);
        CNewBlob* newblob = NULL;

        for (int i = 0; i < cnt; ++i)
        {
            newblob = m_vBlobArray.at(i);
            cHolePoint[i].pCenter = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
            Point temp = cHolePoint[i].pCenter + ptOffsetRoi;
            cHolePoint[i].dDist = CalcDistance(ptRealImgCenter, temp);
            cHolePoint[i].iRadius = newblob->m_blobRect.size().width / 2;

            if (m_bIsDebugMode)
            {
                ptCenterInRoi = cHolePoint[i].pCenter;
                circle(dispImg, ptOffsetRoi + ptCenterInRoi, cHolePoint[i].iRadius , CV_RGB(0, 255, 255), 3);
                cv::line(dispImg, ptOffsetRoi + Point(ptCenterInRoi.x - 10, ptCenterInRoi.y), ptOffsetRoi + Point(ptCenterInRoi.x + 10, ptCenterInRoi.y), CV_RGB(255, 0, 0), 3);
                cv::line(dispImg, ptOffsetRoi + Point(ptCenterInRoi.x, ptCenterInRoi.y - 10), ptOffsetRoi + Point(ptCenterInRoi.x, ptCenterInRoi.y + 10), CV_RGB(255, 0, 0), 3);
            }
            delete newblob;
        }

        sort(cHolePoint, cHolePoint + cnt);

        ptCenterInRoi = cHolePoint[0].pCenter;
        circle(dispImg, ptOffsetRoi + ptCenterInRoi, cHolePoint[0].iRadius, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, ptOffsetRoi + Point(ptCenterInRoi.x - 10, ptCenterInRoi.y), ptOffsetRoi + Point(ptCenterInRoi.x + 10, ptCenterInRoi.y), CV_RGB(255, 0, 0), 3);
        cv::line(dispImg, ptOffsetRoi + Point(ptCenterInRoi.x, ptCenterInRoi.y - 10), ptOffsetRoi + Point(ptCenterInRoi.x, ptCenterInRoi.y + 10), CV_RGB(255, 0, 0), 3);

        bIsOk = true;

        delete[] cHolePoint;
    }
    ptCenterReal = ptOffsetRoi + ptCenterInRoi;
    return CVisionResult(ptCenterReal.x, ptCenterReal.y, bIsOk);
}

CVisionResult CImageProcess::FindCircleUsingHough(cv::Mat srcImg, cv::Mat& dispImg,int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize)
{
    Mat procImg, grayImg, binImg;
    bool bIsOk = false;

    procImg = srcImg.clone();
    dispImg = procImg.clone();

    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    cv::imwrite("1_cvtColor Gray.jpg",grayImg);
    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);


    cv::imwrite("2_Binarize Image.jpg", binImg);

    //int iteration = 3;
    //Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);
    //int morphMode = MORPH_OPEN;
    //morphologyEx(binImg, binImg, morphMode, kernel, Point(-1, -1), iteration);

    if (binImg.channels() == 3) {
        cvtColor(binImg, binImg, CV_BGR2GRAY);
    }
    vector<Vec3f> circles;
    Mat gaussianImg;
    cv::GaussianBlur(binImg, gaussianImg, Size(3, 3), 1, 1);

    cv::imwrite("3_Gaussian Blur.jpg", gaussianImg);

    cv::HoughCircles(gaussianImg, circles, CV_HOUGH_GRADIENT, 2.0, 300, 100, 70, minRSize/2, maxRSize/2);

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
            cHolePoint[i].dDist = CalcDistance(ptImgCenter, cHolePoint[i].pCenter);
            cHolePoint[i].iRadius = (unsigned int)circles[0][2];

            if (m_bIsDebugMode)
            {
                cv::circle(dispImg, tempCenter, (int)circles[0][2], CV_RGB(255, 0, 255), 3);
                cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 1);
                cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 1);
            }
        }

        sort(cHolePoint, cHolePoint + circleCnt);

        tempCenter = cHolePoint[0].pCenter;
        circleRadius = cHolePoint[0].iRadius;
        bIsOk = true;

        delete[] cHolePoint;
    }
    else
    {

    }

    //if (m_bIsDebugMode)
    {
        cv::circle(dispImg, tempCenter, circleRadius, CV_RGB(255, 0, 0), 10);
        cv::line(dispImg, Point((int)tempCenter.x - 10, (int)tempCenter.y), Point((int)tempCenter.x + 10, (int)tempCenter.y), CV_RGB(0, 255, 0), 5);
        cv::line(dispImg, Point((int)tempCenter.x, (int)tempCenter.y - 10), Point((int)tempCenter.x, (int)tempCenter.y + 10), CV_RGB(0, 255, 0), 5);
    }
    return CVisionResult(tempCenter.x, tempCenter.y, bIsOk);
}
Point CImageProcess::DuPM_FindHole(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;

    procImg = srcImg.clone();
    //dispImg = procImg.clone();

    // blobÀž·Î half circle À§Ä¡ÇÑ °÷À» ÁŠ¿ÜÇÑ ºÎºÐÀ» ŸøŸÖ ¹öž°ŽÙ
    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    // blob °¡·Î ŒŒ·Î Å©±â°¡ °°Àº°Å..
    int cnt = getCircleBlob(binImg, maxRSize, minRSize);
    Point ptResult(0, 0);
    int iRadius = 0;
    Point center;
    Point ptImgCenter(binImg.size().width / 2, binImg.size().height / 2);
    CNewBlob* newblob = NULL;

    if (cnt > 0)
    {
        for (int i = 0; i < cnt; ++i)
        {
            newblob = m_vBlobArray.at(i);
            center = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
            long dist1 = (long)pow(ptImgCenter.x - ptResult.x, 2) + (long)pow(ptImgCenter.y - ptResult.y, 2);
            long dist2 = (long)pow(ptImgCenter.x - center.x, 2) + (long)pow(ptImgCenter.y - center.y, 2);
            if (dist1 > dist2)
            {
                ptResult = center;
                iRadius = newblob->m_blobRect.size().width / 2;
            }
        }

        circle(dispImg, ptResult, (newblob->m_blobRect.size().width) / 2, CV_RGB(255, 0, 0), 2);
        cv::line(dispImg, Point(ptResult.x - 10, ptResult.y), Point(ptResult.x + 10, ptResult.y), CV_RGB(0, 255, 0), 2);
        cv::line(dispImg, Point(ptResult.x, ptResult.y - 10), Point(ptResult.x, ptResult.y + 10), CV_RGB(0, 255, 0), 2);

        if (m_bIsDebugMode)
        {
            CNewBlob* tempBlob;
            Point ptTemp;
            for (int i = 0; i < cnt; ++i)
            {
                tempBlob = m_vBlobArray.at(i);
                ptTemp = Point((tempBlob->m_blobRect.x + tempBlob->m_blobRect.width) / 2, (tempBlob->m_blobRect.y + tempBlob->m_blobRect.height) / 2);
                if (ptTemp.x != ptResult.x && ptTemp.y == ptResult.y)
                {
                    circle(dispImg, ptTemp, (tempBlob->m_blobRect.size().width) / 2, CV_RGB(0, 0, 255), 3);
                    cv::line(dispImg, Point(ptTemp.x - 10, ptTemp.y), Point(ptTemp.x + 10, ptTemp.y), CV_RGB(255, 0, 0), 3);
                    cv::line(dispImg, Point(ptTemp.x, ptTemp.y - 10), Point(ptTemp.x, ptTemp.y + 10), CV_RGB(255, 0, 0), 3);
                }
            }
        }
    }
    return ptResult;
}

void CImageProcess::DuPBA_FindHole(cv::Mat srcImg, cv::Mat& dispImg, Point* ptCircle, int iArraySize, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;

    procImg = srcImg.clone();
    if (procImg.channels() != 1)
    {
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    }
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    // blob °¡·Î ŒŒ·Î Å©±â°¡ °°Àº°Å..
    int cnt = getCircleBlob(binImg, maxRSize, minRSize);
    CHolePoint  * cHolePoint = new CHolePoint[cnt];
    Point ptImgCenter(binImg.size().width / 2, binImg.size().height / 2);

    for (int i = 0; i < iArraySize; ++i)
    {
        ptCircle[i].x = 0;
        ptCircle[i].y = 0;
    }

    CNewBlob* newblob = NULL;
    if (cnt > 0)
    {
        for (int i = 0; i < cnt; ++i)
        {
            newblob = m_vBlobArray.at(i);
            cHolePoint[i].pCenter = Point((newblob->m_blobRect.x + newblob->m_blobRect.width) / 2, (newblob->m_blobRect.y + newblob->m_blobRect.height) / 2);
            cHolePoint[i].dDist = pow(ptImgCenter.x - cHolePoint[i].pCenter.x, 2) + pow(ptImgCenter.y - cHolePoint[i].pCenter.y, 2);
            cHolePoint[i].iRadius = newblob->m_blobRect.size().width / 2;
            delete newblob;
        }

        sort(cHolePoint, cHolePoint + cnt);

        for (int i = 0; i < iArraySize; ++i)
        {
            if (i == 0) {
                circle(dispImg, cHolePoint[i].pCenter, cHolePoint[i].iRadius, CV_RGB(255, 0, 0), 2);
            }
            else {
                circle(dispImg, cHolePoint[i].pCenter, cHolePoint[i].iRadius, CV_RGB(0, 255, 255), 2);
            }
            cv::line(dispImg, Point(ptCircle[i].x - 10, ptCircle[i].y), Point(ptCircle[i].x + 10, ptCircle[i].y), CV_RGB(0, 255, 0), 2);
            cv::line(dispImg, Point(ptCircle[i].x, ptCircle[i].y - 10), Point(ptCircle[i].x, ptCircle[i].y + 10), CV_RGB(0, 255, 0), 2);
        }

        for (int j = 0; j < iArraySize; ++j)
        {
            ptCircle[j] = cHolePoint[j].pCenter;
        }
    }

    if (m_bIsDebugMode)
    {
        for (int i = iArraySize; i < cnt; ++i)
        {
            circle(dispImg, cHolePoint[i].pCenter, cHolePoint[i].iRadius, CV_RGB(0, 0, 255), 3);
            cv::line(dispImg, Point(cHolePoint[i].pCenter.x - 10, cHolePoint[i].pCenter.y), Point(cHolePoint[i].pCenter.x + 10, cHolePoint[i].pCenter.y), CV_RGB(255, 0, 0), 3);
            cv::line(dispImg, Point(cHolePoint[i].pCenter.x, cHolePoint[i].pCenter.y - 10), Point(cHolePoint[i].pCenter.x, cHolePoint[i].pCenter.y + 10), CV_RGB(255, 0, 0), 3);
        }
    }

    delete[] cHolePoint;
    cHolePoint = NULL;
    return;
}

Point2d CImageProcess::PowerModuleFindHalfCircle(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize, int blurVal, cv::Rect& roi, bool bIsFirstSearch)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;

    procImg = srcImg.clone();
    dispImg = procImg.clone();

    // blobÀž·Î half circle À§Ä¡ÇÑ °÷À» ÁŠ¿ÜÇÑ ºÎºÐÀ» ŸøŸÖ ¹öž°ŽÙ
    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    std::vector<cv::Point> vCont;
    int cnt = 0;
    int iCircleRadius = 0;
    double dDist = grayImg.size().width * 2;
    Mat imgMask = FindLargestContour(binImg, vCont, 100000, 10000000, cnt, true, CV_FILLED);

    Point2d circleCenter;
    Point2d returnCenter(-999, -999);
    if (cnt >= 1)
    {
        // do hough circle
        vector<Vec3f> circles;
        if (procImg.channels() == 3)
            cvtColor(procImg, grayImg, CV_BGR2GRAY);
        else
            grayImg = procImg.clone();

        GaussianBlur(grayImg, grayImg, Size(3, 3), 3, 3);
        HoughCircles(grayImg, circles, CV_HOUGH_GRADIENT, 2.0, 100, 100, 50, minRSize, maxRSize);

        int iMiddlePointX = grayImg.size().width / 2;
        int iMiddlePointY = grayImg.size().height / 2;

        if (circles.size() != 0)
        {
            int iMinDistX = grayImg.size().width;
            int iMinDistY = grayImg.size().height;
            for (int i = 0; i<int(circles.size()); i++)
            {
                circleCenter = Point2d(circles[i][0], circles[i][1]);
                uchar val = GetPixel(binImg, circleCenter);
                //if (bIsFirstSearch)
                //{
                //	if (circleCenter.x >= roi.x && circleCenter.x <= roi.x + roi.width &&
                //		circleCenter.y >= roi.y && circleCenter.y <= roi.y + roi.height)
                //	{
                //		if (val != 0)
                //		{
                //			returnCenter = circleCenter;
                //			iCircleRadius = circles[i][2];
                //		}
                //	}
                //}
                //else
                //{
                    if (val == 0)
                    {
                        double tempDist = calcDistance(circleCenter.x, circleCenter.y, static_cast<double>(iMiddlePointX), static_cast<double>(iMiddlePointY));
                        if (tempDist < dDist)
                        {
                            dDist = tempDist;
                            returnCenter = circleCenter;
                            iCircleRadius = (int)circles[i][2];
                        }
                    }
                //}

                if (m_bIsDebugMode)
                {
                    circle(dispImg, circleCenter, (int)circles[i][2], CV_RGB(0, 255, 255), 2);
                }
            }
        }
    }
    if (returnCenter.x != -999 && returnCenter.y != -999)
    {
        circle(dispImg, circleCenter, iCircleRadius, CV_RGB(255, 0, 0), 3);
        line(dispImg, Point((int)circleCenter.x - 10, (int)circleCenter.y), Point((int)circleCenter.x + 10, (int)circleCenter.y), CV_RGB(0, 255, 0), 3);
        line(dispImg, Point((int)circleCenter.x, (int)circleCenter.y - 10), Point((int)circleCenter.x, (int)circleCenter.y + 10), CV_RGB(0, 255, 0), 3);
        line(dispImg, Point(0, 0), Point((int)circleCenter.x, (int)circleCenter.y), CV_RGB(255, 255, 0), 2);
    }
    return returnCenter;
}


Point2d CImageProcess::FindHalfCircle(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, int minRSize, int maxRSize, int blurVal, int iModuleType)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;

    procImg = srcImg.clone();
    dispImg = procImg.clone();

    // blobÀž·Î half circle À§Ä¡ÇÑ °÷À» ÁŠ¿ÜÇÑ ºÎºÐÀ» ŸøŸÖ ¹öž°ŽÙ
    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);

    std::vector<cv::Point> vCont;
    int cnt = 0;
    Mat imgMask = FindLargestContour(binImg, vCont, 100000, 10000000, cnt, true, CV_FILLED);

    Point2d circleCenter;
    Point2d returnCenter(-999, -999);
    if (cnt >= 1)
    {
        // do hough circle
        vector<Vec3f> circles;
        if (procImg.channels() == 3)
            cvtColor(procImg, grayImg, CV_BGR2GRAY);
        else
            grayImg = procImg.clone();

        GaussianBlur(grayImg, grayImg, Size(3, 3), 3, 3);
        HoughCircles(grayImg, circles, CV_HOUGH_GRADIENT, 2.0, 100, 100, 50, minRSize, maxRSize);

        if (circles.size() != 0)
        {
            for (int i = 0; i<int(circles.size()); i++)
            {
                circleCenter = Point2d(circles[i][0], circles[i][1]);
                uchar val = GetPixel(binImg, circleCenter);
                if (val != 0)
                {
                    circle(dispImg, circleCenter, (int)circles[i][2], CV_RGB(255, 0, 0), 3);
                    if (m_bIsDebugMode)
                    {
                        line(dispImg, Point((int)circleCenter.x - 10, (int)circleCenter.y), Point((int)circleCenter.x + 10, (int)circleCenter.y), CV_RGB(0, 255, 0), 3);
                        line(dispImg, Point((int)circleCenter.x, (int)circleCenter.y - 10), Point((int)circleCenter.x, (int)circleCenter.y + 10), CV_RGB(0, 255, 0), 3);
                        line(dispImg, Point(0, 0), Point((int)circleCenter.x, (int)circleCenter.y), CV_RGB(255, 255, 0), 2);
                    }
                    returnCenter = circleCenter;
                }
            }
        }
    }
    return returnCenter;
}


Point2d CImageProcess::FindPinCenter(cv::Mat srcImg, cv::Mat& dispImg, int iThresholdValLow, int iThresholdValHigh, double dBlobArea, double dBlobDist)
{
    std::string strPath, strFileName;
    Mat procImg, grayImg, binImg;

    procImg = srcImg.clone();
    dispImg = procImg.clone();

    // blobÀž·Î half circle À§Ä¡ÇÑ °÷À» ÁŠ¿ÜÇÑ ºÎºÐÀ» ŸøŸÖ ¹öž°ŽÙ
    if (procImg.channels() != 1)
        cvtColor(procImg, grayImg, CV_BGR2GRAY);
    else
    {
        grayImg = procImg.clone();
        cvtColor(dispImg, dispImg, CV_GRAY2BGR);
    }

    Mat kernel = Mat::ones(Size(3, 3), CV_8UC1);

    //threshold(grayImg, binImg, iThresholdValLow, iThresholdValHigh, CVHRESH_BINARY);
    BinarizeImage(grayImg, binImg, iThresholdValLow, iThresholdValHigh);
    //morphologyEx(binImg, binImg, MORPH_OPEN, kernel, cv::Point(-1, -1), 11);
    //imwrite("threshold.jpg", binImg);

    std::vector<std::vector<cv::Point>> vAllContours;
    std::vector<cv::Vec4i> hierarchy;

    int l, r, b, t;
    l = t = 10000;
    r = b = 0;

    Mat imgMask = FindContours(binImg, vAllContours, dBlobArea - 50000, dBlobArea + 50000, CONT_AREA, true);
    {
        int iIdx = 0;
        std::vector<std::vector<cv::Point>>::iterator it_blob = vAllContours.begin();
        while (it_blob != vAllContours.end())
        {
            cv::Rect bndRect = cv::boundingRect(*it_blob);
            double dArea = cv::contourArea(*it_blob);
            double dDist = calcDistance(0, 0, bndRect.width, bndRect.height);

            if (dDist > dBlobDist - 200 && dDist < dBlobDist + 200)
            {
                rectangle(dispImg, bndRect, CV_RGB(255, 0, 0), 3);

                if (l > bndRect.x && bndRect.x != 0) l = bndRect.x;
                if (r < bndRect.x + bndRect.width && bndRect.x + bndRect.width != binImg.cols) r = bndRect.x + bndRect.width;
                if (t > bndRect.y && bndRect.y != 0) t = bndRect.y;
                if (b < bndRect.y + bndRect.height && bndRect.y + bndRect.height != binImg.rows) b = bndRect.y + bndRect.height;
            }
            it_blob++;
        }
    }
    if (l == 10000 || t == 10000 || b == 0 || r == 0)
        return Point(-999, -999);

    Point center((l + r) / 2, (t + b) / 2);
    line(dispImg, Point(center.x - 10, center.y), Point(center.x + 10, center.y), CV_RGB(0, 255, 0), 10);
    line(dispImg, Point(center.x, center.y - 10), Point(center.x, center.y + 10), CV_RGB(0, 255, 0), 10);

    return 	center;
}

template <class T>
double CImageProcess::calcDistance(T startX, T startY, T endX, T endY)
{
    return sqrt(static_cast<double>(pow(endX - startX, 2) + pow(endY - startY, 2)));
}

CNewBlob::CNewBlob()
{

}

