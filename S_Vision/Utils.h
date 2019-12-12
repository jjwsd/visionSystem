#ifndef MYUTILES_H
#define MYUTILES_H

#include <iostream>
#include <opencv2/core.hpp>

using namespace cv;
using namespace std;

const float pi = 3.14159265f;

static string template_resize = "template_resize.bmp";
static string contour_rough = "contour_rough.bmp";
static string contour_def = "contour_def_canny_totally.bmp";

class CUtils
{
public:
	CUtils();
	~CUtils();

	string type2str(int type);
	double wrapMax(double x, double max);
	double wrapPI(double angle);

	void setCorners(vector<Point2f> point);
	vector<Point2f> getCorners();

	static void setFolder(string folder);
	static void setTemplateName(string template_name);
	static string getTemplateName();
	static string getFolder();

public:
	vector<Point2f> v_corners;
	static string strFolder;
	static string strTemplateName;	
};

#endif
