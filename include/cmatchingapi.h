#ifndef CMATCHINGAPI_H
#define CMATCHINGAPI_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "matchingDEF.h"

using namespace std;
using namespace cv;

class CMatchingAPI
{

public:
    CMatchingAPI();
    //if your resize_ratio parameter is impossible to set, return false and compute proper ratio.
    /***********************************************************************************************/
    bool initialize(String folder, String template_name, int resize_ratio = 4);
    /***********************************************************************************************/

    void finalize();
    MATCHING_RET doCore(Mat& input_img, bool bIcp);
    void setRefPts(vector<Point2f> ref_pts);
    void estimateErrorRate();
    void showResult();
    void getResult(Mat& result_img, Point2f& center_pt, double& angle);

    //must be called before initialize() !!
    /****************************************************************************/
    /*decide blur size when edge template is created*/
    void setBlurSize(int size = 3);

    /*decide canny threshold when edge template is created*/
    void setCannyThreshold(int threshold1, int threshold2);
    /****************************************************************************/

    //must be called before doCore() !!
    /****************************************************************************/
    int getNumOfObjects(
            Mat input_img,
            int match_method = 1, /*0 SQDIFF
                                                                1 SQDIFF NORMED
                                                                2 TM CCORR
                                                                3 TM CCORR NORMED
                                                                4 TM COEFF
                                                                5 TM COEFF NORMED*/
            float threshold = -1  /*default minVal = 0.25
                                                                default maxVal = 0.75*/);

    void setMatchingRatio(float ratio = 0.75f);
    bool MakeTemplateImg(const int iResizeRatio);
    string GetConTourImgName() const;
    /****************************************************************************/
};

#endif // CMATCHINGAPI_H
