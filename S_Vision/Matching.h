#ifndef MATCHING_H
#define MATCHING_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/photo.hpp>
#include <iostream>

#define IMSHOW 0
#define LOG 0


using namespace cv;
using namespace std;

class CMatching
{
public:

	CMatching();
	~CMatching();

	Mat briskMatching(Mat img_object, Mat img_scene);
};

#endif
