#include "Preprocess.h"
#include "Utils.h"
//#include <opencv2/xfeatures2d.hpp>


CPreprocess::CPreprocess() {
}

Mat CPreprocess::getDilatedMask()
{
	Mat src, mask;
	src = imread(CUtils::getFolder() + contour_def, 1);
	dilate(src, src, getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)), Point(-1, -1), 3);
	mask = Mat(src.size(), CV_8UC3, cv::Scalar(0, 0, 0));
	src.copyTo(mask(Rect(0, 0, src.cols, src.rows)));

	return src;
}

Mat CPreprocess::getResizedMask(Mat input_img, Mat mask, Mat H)
{
	Mat resize_mask = Mat::zeros(input_img.size(), input_img.type());
	warpPerspective(mask, resize_mask, H, resize_mask.size());

	return resize_mask;
}


Mat CPreprocess::getCroppedInput(Mat input_img, Mat mask, Mat H)
{
	Mat src_gray, detected_edges, cropped;
	
	mask = getResizedMask(input_img, mask, H);
	src_gray.create(Size(input_img.cols, input_img.rows), CV_8UC1);
	cvtColor(input_img, src_gray, CV_BGR2GRAY);
	GaussianBlur(src_gray, detected_edges, Size(3, 3), 0, 0);
	Canny(detected_edges, detected_edges, 255 / 3, 255, 3);
	cvtColor(mask, mask, CV_BGR2GRAY);

	bitwise_and(detected_edges, mask, cropped);

	return cropped;
}

