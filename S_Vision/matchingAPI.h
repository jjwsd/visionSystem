#ifndef CMATCHINGAPI_H
#define CMATCHINGAPI_H

#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class CMatchingAPI
{
public:
    bool initialize(cv::String folder, cv::String template_name);
    void finalize();

    bool doCore(Mat& input_img, bool bIcp);
    void showResult();
    void getResult(Mat& output_img);
    void setRefPts(vector<Point2f> ref_pts);
    void estimateErrorRate();

    cv::Point2f m_centerPt;
    float       m_angle;
	//__declspec(dllexport) void setTemplatePath(String folder, String template_name);
};

#endif //CMATCHINGAPI_H
