#include "matchingAPI.h"
#include "Core.h"
#include "Utils.h"
#include <time.h>

CCore core;
Mat m_resize_img, m_template_img, T;
vector<Point2f> m_ref_pts;
bool m_bIcp, m_bRefPts;

bool CMatchingAPI::initialize(cv::String folder, cv::String template_name)
{
	m_ref_pts.clear();
	m_bIcp = true;
	m_bRefPts = false;

	CUtils::setFolder(folder);
	CUtils::setTemplateName(template_name);

    std::cout << folder << ", " << template_name << endl;

	return true;
}

void CMatchingAPI::finalize()
{
}

//void CMatchingAPI::setTemplatePath(String folder, String template_name)
//{
//	CUtils::setFolder(folder);
//	CUtils::setTemplateName(template_name);
//	
//
//	cout << folder << ", " << template_name << endl;
//}

bool CMatchingAPI::doCore(Mat& input_img, bool bIcp)
{
	cout << "doCore start!" << endl;
	clock_t start, end;
	
	m_bIcp = bIcp;
	
	start = clock();

	Mat resize_img;
	resize(input_img, resize_img, Size(input_img.size().width / 4, input_img.size().height / 4));
	m_resize_img = resize_img.clone();
	core.DoMatching(resize_img, m_template_img, T, m_bIcp);

	end = clock();
	double img_preprocess_t = (double)(end - start) / CLOCKS_PER_SEC;
	printf("TIME : %5.2fs \n", img_preprocess_t);
    std::cout << "doCore finish!" << endl;

	return true;
}

void CMatchingAPI::setRefPts(vector<Point2f> ref_pts)
{
	m_bRefPts = true;
	core.setRefPtsToCore(ref_pts);
}

void CMatchingAPI::showResult()
{
	Mat result_img = m_resize_img.clone();

	bool bMarkPoint = m_bRefPts;
	if (!m_bIcp)
		core.ShowBriskResult(result_img, T, bMarkPoint);
	else
        core.ShowICPResult(result_img, m_template_img, T, bMarkPoint, m_centerPt, m_angle);
}

void CMatchingAPI::getResult(Mat &output_img)
{
    output_img = m_resize_img.clone();

    bool bMarkPoint = m_bRefPts;
    if (!m_bIcp)
        core.ShowBriskResult(output_img, T, bMarkPoint);
    else
        core.ShowICPResult(output_img, m_template_img, T, bMarkPoint, m_centerPt, m_angle);
}


void CMatchingAPI::estimateErrorRate()
{
	core.EstimateErrorRate(m_resize_img, m_template_img, T);
	core.CalculateRMSE(m_bIcp);
}
