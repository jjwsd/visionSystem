#include "Utils.h"

string CUtils::strFolder = "";
string CUtils::strTemplateName = "";

CUtils::CUtils()
{
}

CUtils::~CUtils()
{
}

void CUtils::setCorners(vector<Point2f> point)
{
	cout << point << endl;

	v_corners.clear();
	for (int i = 0; i < 4; i++)
		v_corners.emplace_back(point[i]);	
}
vector<Point2f> CUtils::getCorners(void)
{
	cout << v_corners << endl;
	return v_corners;
}

string CUtils::type2str(int type) 
{
	
	string r;
	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}

double CUtils::wrapMax(double x, double max)
{
	/* integer math: `(max + x % max) % max` */
	return fmod(max + fmod(x, max), max);
}
/* wrap x -> [min,max) */
double CUtils::wrapPI(double angle)
{
	double a = fmod(angle + pi, 2 * pi);
	return a >= 0 ? (a - pi) : (a + pi);
}

//void CUtils::setDataDir(string dir)
//{
//	folder = dir;
//}


void CUtils::setFolder(string folder)
{
	strFolder = folder;
}
void CUtils::setTemplateName(string template_name)
{
	strTemplateName = template_name;
}
string CUtils::getTemplateName()
{
	return strTemplateName;
}
string CUtils::getFolder()
{
	return strFolder;
}