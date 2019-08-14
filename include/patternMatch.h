#ifndef PATTERNMATCH_H
#define PATTERNMATCH_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "matchingDEF.h"

using namespace std;
using namespace cv;

class PatternMatch
{

public:
    PatternMatch();

    bool initialize(String folder, String template_name);
    void finalize();

    MATCHING_RET doCore(Mat& input_img, bool bIcp, int resize_ratio = 4);
    void setRefPts(vector<Point2f> ref_pts);
    void estimateErrorRate();
    void showResult();
    void getResult(Mat& result_img, Point2f& center_pt, double& angle);
    void MakeTemplateImg(const int iResizeRatio);
    std::string GetConTourImgName() const;
};

#endif // PATTERNMATCH_H
