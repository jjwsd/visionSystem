#ifndef MYCORE_H
#define MYCORE_H

#pragma once
#include "Matching.h"
#include "Preprocess.h"
#include "ICP.h"

#define RESULT_IMSHOW	0
#define LOG				0


class CCore
{
public:
	CCore();
	~CCore();

	void DoMatching(Mat& input_img, Mat& template_img, Mat& T, bool bIcp = false);
	void MakeBriskTemplate(Mat& template_img);
	void MakeICPRoughTemplate();
	void MatchBrisk(Mat template_img, Mat input_img, Mat& T);
	void ExecuteICP(Mat template_img, Mat input_img, Mat& brisk_t, Mat& icp_t);
	void setRefPtsToCore(vector<Point2f> _ref_pts);
    void ShowBriskResult(Mat& input_img, Mat H, bool bMarkPoint);
    void ShowICPResult(Mat& input_img, Mat template_img, Mat H, bool bMarkPoint, Point2f& centerPt, float& fAngle);

	void EstimateErrorRate(Mat& input_img, Mat template_img, Mat H);
	void FindHoleCenter(Mat& input_img, vector<Point2f> start_pos, vector<Point2f>& contour_centrs);
	Mat GetROIimage(Mat img, vector<Point2f> &v_roi_corners);
	double GetAngleFromHough(Mat& input_img, vector<Point2f> t_pts, float angle);
	double FindBestAngleFromHough(vector<float> v_theta, float angle);
	void CalculateRMSE(bool bIcp);
	static void onMouseEvent(int event, int x, int y, int flags, void* dstImages);
	
private:

	vector<Point2f> ref_pts;
	vector<Point2f> tpl_points;
	vector<Point2f> scene_points;

	vector<double> xDiff;
	vector<double> yDiff;
	vector<double> thetaDiff;

	float angle_offset;
	float binTh;

	String folder;
	String template_name;
};

#endif
