#ifndef MYCPREPROCESS_H
#define MYCPREPROCESS_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream> // For debugging
#include <string>
#include <stdio.h>

using namespace std;
using namespace cv;

class CPreprocess {
public:

	CPreprocess();

	Mat getDilatedMask();
	Mat getCroppedInput(Mat input_img, Mat mask, Mat H);
	Mat getResizedMask(Mat input_img, Mat mask, Mat H);

};

#endif //MYCPREPROCESS_H
