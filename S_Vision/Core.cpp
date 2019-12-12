#include "Core.h"
#include "Utils.h"
#include <stdio.h>
#include <string.h>

CCore::CCore()
{
	angle_offset = 0.0;
	binTh = 80;

	//if (folder == "../TEST_IMG_1/Data5_calX/")
	//{
	//	ref_pts.emplace_back(cv::Point2f{ 286.0f, 278.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 58.0f, 100.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 36.0f, 423.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 508.0f, 203.0f });

	//	angle_offset = 0.087264f;
	//}
	//else if (folder == "../TEST_IMG_1/Data3/" || folder == "../TEST_IMG_1/Data3_calX/" || folder == "../TEST_IMG_1/Data4/")
	//{
	//	//ref_pts.emplace_back(cv::Point2f{ 273.0f, 176.0f });
	//	//ref_pts.emplace_back(cv::Point2f{ 110.0f, 298.0f });
	//	//ref_pts.emplace_back(cv::Point2f{ 186.0f, 251.0f });
	//	//ref_pts.emplace_back(cv::Point2f{ 370.0f, 280.0f });

	//	//Data4
	//	ref_pts.emplace_back(cv::Point2f{ 119.0f, 254.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 169.0f, 115.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 175.0f, 359.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 36.0f, 308.0f });
	//}
	//else if (folder == "../TEST_IMG_1/Data2/" || folder == "../TEST_IMG_1/Data2_calX/")
	//{
	//	//cal X
	//	ref_pts.emplace_back(cv::Point2f{ 119.0f, 254.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 169.0f, 115.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 175.0f, 359.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 36.0f, 308.0f });

	//	//ref_pts.emplace_back(cv::Point2f{ 126.0f, 257.0f });
	//	//ref_pts.emplace_back(cv::Point2f{ 175.0f, 118.0f });
	//	//ref_pts.emplace_back(cv::Point2f{ 182.0f, 361.0f });
	//	//ref_pts.emplace_back(cv::Point2f{ 43.0f, 311.0f });
	//}
	//else if (folder == "../TEST_IMG_1/PBA1/")
	//{
	//	ref_pts.emplace_back(cv::Point2f{ 284.0f, 260.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 63.0f, 175.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 49.0f, 422.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 385.0f, 187.0f });
	//}
	//else if (folder == "../TEST_IMG_1/CAN/")
	//{
	//	ref_pts.emplace_back(cv::Point2f{ 170.0f, 83.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 247.0f, 239.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 42.0f, 433.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 39.0f, 110.0f });
	//}
	//else if (folder == "../TEST_IMG_2/CAN1/")
	//{
	//	ref_pts.emplace_back(cv::Point2f{ 57.0f, 182.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 46.0f, 433.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 248.0f, 238.0f });
	//	ref_pts.emplace_back(cv::Point2f{ 170.0f, 82.0f });
	//}


	xDiff.clear();
	yDiff.clear();
	thetaDiff.clear();

	//folder = CUtils::getFolder();
	//template_name = CUtils::getTemplateName()();
}


CCore::~CCore()
{
}

void CCore::setRefPtsToCore(vector<Point2f> _ref_pts)
{
	ref_pts.clear();
	for (int i = 0; i < _ref_pts.size(); i++)
		ref_pts.emplace_back(_ref_pts[i]);
}

void CCore::DoMatching(Mat& input_img, Mat& template_img, Mat& T, bool bIcp)
{
	cout << "DoMathcing folder : " << CUtils::getFolder() << endl;
	template_img = imread(CUtils::getFolder() + CUtils::getTemplateName(), 1);
	MakeBriskTemplate(template_img);
	MatchBrisk(template_img, input_img, T);
	
	if (bIcp)
	{
		MakeICPRoughTemplate();

		Mat icp_t;
		Mat tpl_contour = imread(CUtils::getFolder() + contour_def, 1);
		if (tpl_contour.empty())
		{
			printf("There is no contour_def.bmp!!!");
			return;
		}

		ExecuteICP(tpl_contour, input_img, T, icp_t);
		T = icp_t.clone();
	}
}

void CCore::MakeBriskTemplate(Mat& template_img)
{
	Mat template_resize_img = imread(CUtils::getFolder() + template_resize, 1);
	if (template_resize_img.empty())
	{
		resize(template_img, template_resize_img, Size(template_img.cols / 4, template_img.rows / 4));
		imwrite(CUtils::getFolder() + template_resize, template_resize_img);
	}

	template_img = template_resize_img.clone();
}

void CCore::MakeICPRoughTemplate()
{
	string img_path = CUtils::getFolder() + template_resize;
	Mat input_img = imread(img_path, 1);

	Mat src_gray;
	Mat detected_edges;
	src_gray.create(Size(input_img.cols, input_img.rows), CV_8UC1);
	cvtColor(input_img, src_gray, CV_BGR2GRAY);
	medianBlur(src_gray, detected_edges, 5);
	Canny(detected_edges, detected_edges, 10, 150, 3);
	
	imwrite(CUtils::getFolder() + contour_rough, detected_edges);
	
	Mat def_tpl = imread(CUtils::getFolder() + contour_def);
	if (def_tpl.empty())
	{
		imwrite(CUtils::getFolder() + contour_def, detected_edges);

		namedWindow("rough template", CV_WINDOW_NORMAL);
		imshow("rough template", detected_edges);
		setMouseCallback("rough template", onMouseEvent, (void*)&detected_edges);
		waitKey();
	}
}

bool drawing_box = false;
Rect box = Rect(-1, -1, 0, 0);

void CCore::onMouseEvent(int event, int x, int y, int flags, void* dstImage)
{
	Mat mouseImg = *(Mat*)dstImage;
	//imshow("onMouseEvent", mouseImg);

	
	int s_posX = -1;
	int s_posY = -1;

	
	
	switch (event)
	{
		case CV_EVENT_MOUSEMOVE:
		{
			if (flags & CV_EVENT_LBUTTONDOWN)
			{
				circle(mouseImg, Point(x, y), 13, Scalar::all(0), -1);
				namedWindow("rough template", CV_WINDOW_NORMAL);
				imshow("rough template", mouseImg);
			}
			else if (flags & CV_EVENT_RBUTTONDOWN)
			{
				if (drawing_box)
				{
					box.width = x - box.x;
					box.height = y - box.y;
				}
			}
			break;
		}	

		case CV_EVENT_LBUTTONDBLCLK:
		{
			cout << "image was saved!" << endl;
			string img_path = CUtils::getFolder() + contour_def;
			imwrite(img_path, mouseImg);
			destroyWindow("rough template");
			break;
		}

		case CV_EVENT_RBUTTONDOWN :
		{
			cout << "start drawing box!" << endl;
			drawing_box = true;
			box = Rect(x, y, 0, 0);
			break;
		}

		case CV_EVENT_RBUTTONUP:
		{
			drawing_box = false;
			if (box.width < 0)
			{
				box.x += box.width;
				box.width *= -1;
			}
			if (box.height < 0)
			{
				box.y += box.height;
				box.height *= -1;
			}

			rectangle(mouseImg, Point(box.x, box.y), Point(box.x + box.width, box.y + box.height), Scalar::all(0), -1);
			namedWindow("rough template", CV_WINDOW_NORMAL);
			imshow("rough template", mouseImg);

			break;
		}
		
	}
	
}


void CCore::MatchBrisk(Mat template_img, Mat input_img, Mat& T)
{
	CMatching matching;
	T = matching.briskMatching(template_img, input_img);

}

void CCore::ExecuteICP(Mat template_img, Mat input_img, Mat& brisk_t, Mat& icp_t)
{
	CPreprocess pre;
	Mat crop_mask = pre.getDilatedMask();
	Mat img = pre.getCroppedInput(input_img, crop_mask, brisk_t);

	tpl_points.clear();
	scene_points.clear();

	vector<vector<Point> > tpl_contours;
	vector<vector<Point> > scene_contours;
	vector<Vec4i> hierarchy;

	Mat template_gray;
	cvtColor(template_img, template_img, CV_BGR2GRAY);

	/*namedWindow("template_img", CV_WINDOW_NORMAL);
	imshow("template_img", template_img);
	waitKey(0);*/

	for (int j = 0; j < template_img.rows; j++)
	{
		for (int i = 0; i < template_img.cols; i++)
		{
			if (template_img.at<uchar>(j, i) == 255)
				tpl_points.push_back(Point(i, j));
		}
	}	

	for (int j = 0; j < img.rows; j++)
	{
		for (int i = 0; i < img.cols; i++)
		{
			if (img.at<uchar>(j, i) == 255)
				scene_points.push_back(Point(i, j));
		}
	}

	CEst* m_pCEst = new CEst;

	dmat Init_T = m_pCEst->newdmat(0, 2, 0, 2, &errno);;
	int x, y;
	for (x = 0; x < 3; x++)
	{
		for (y = 0; y < 3; y++)
		{
			Init_T.el[x][y] = brisk_t.at<double>(x, y);
		}
	}

	int m_nOfFirstPartModelPnts = tpl_points.size();
	int m_nOfSceneSamplePnts = scene_points.size();
	std::vector<std::array<float, 2>> m_pFirstPartModelPnts;
	std::vector<std::array<float, 2>> m_pSceneSamplePnts;
	m_pFirstPartModelPnts.clear();
	m_pSceneSamplePnts.clear();

	m_pFirstPartModelPnts = std::vector<std::array<float, 2>>(m_nOfFirstPartModelPnts);
	for (int n = 0; n < m_nOfFirstPartModelPnts; n++)
	{
		m_pFirstPartModelPnts[n][0] = tpl_points[n].x;
		m_pFirstPartModelPnts[n][1] = tpl_points[n].y;
	}

	m_pSceneSamplePnts = std::vector<std::array<float, 2>>(m_nOfSceneSamplePnts);
	for (int n = 0; n < m_nOfSceneSamplePnts; n++)
	{
		m_pSceneSamplePnts[n][0] = scene_points[n].x;
		m_pSceneSamplePnts[n][1] = scene_points[n].y;
	}

	dmat T = m_pCEst->newdmat(0, 2, 0, 2, &errno);
	// clock_gettime(CLOCK_MONOTONIC, &begin);

	m_pCEst->Pose_Estimation_with_ICP(m_pFirstPartModelPnts, m_nOfFirstPartModelPnts,
		m_pSceneSamplePnts, m_nOfSceneSamplePnts,
		Init_T, T);
#if LOG
	for (int n = 0; n < 3; n++)
	{
		for (int m = 0; m < 3; m++)
			cout << Init_T.el[n][m] << " ";
		cout << endl;
	}
	cout << endl;
#endif


	icp_t = Mat(3, 3, CV_64FC1);
	for (int n = 0; n < 3; n++)
	{
		for (int m = 0; m < 3; m++)
			icp_t.at<double>(m, n) = T.el[m][n];
	}

	freemat(Init_T);
	freemat(T);

}

void CCore::ShowBriskResult(Mat& input_img, Mat H, bool bMarkPoint)
{
	tpl_points.clear();
	Mat tpl_contour = imread(CUtils::getFolder() + contour_def, 1);
	cvtColor(tpl_contour, tpl_contour, CV_BGR2GRAY);
	if (tpl_contour.empty())
	{
		printf("There is no contour_def.bmp!!!");
		return;
	}

	for (int j = 0; j < tpl_contour.rows; j++)
	{
		for (int i = 0; i < tpl_contour.cols; i++)
		{
			if (tpl_contour.at<uchar>(j, i) == 255)
				tpl_points.push_back(Point(i, j));
		}
	}
	

	float pose[3];
	pose[0] = H.at<double>(0, 2); // x [pixel]
	pose[1] = H.at<double>(1, 2); // y [pixel]
	pose[2] = atan2(H.at<double>(1, 0), H.at<double>(0, 0));

	float R[2][2];

	R[0][0] = cos(pose[2]);
	R[0][1] = -sin(pose[2]);

	R[1][0] = sin(pose[2]);
	R[1][1] = cos(pose[2]);

	if (bMarkPoint)
	{
		vector<Point2f> t_pts;
		perspectiveTransform(ref_pts, t_pts, H);

		for (size_t i = 0; i < t_pts.size(); ++i)
			circle(input_img, t_pts[i], 3, Scalar(255, 0, 255), -1, 8);
	}
	
	int size = tpl_points.size();
	vector<Point2f> output_tpl_points(size);
	output_tpl_points.clear();
	for (int i = 0; i < size; i++)
	{
		output_tpl_points[i].x = R[0][0] * tpl_points[i].x + R[0][1] * tpl_points[i].y + pose[0];
		output_tpl_points[i].y = R[1][0] * tpl_points[i].x + R[1][1] * tpl_points[i].y + pose[1];
	}

	for (int i = 0; i < size; i++)
	{
		circle(input_img, output_tpl_points[i], 2, Scalar(0, 0, 255), 1);
	}
#if RESULT_IMSHOW
	namedWindow("brisk_result", CV_WINDOW_NORMAL);
	imshow("brisk_result", input_img);
	waitKey(0);
#endif
}

void CCore::ShowICPResult(Mat& input_img, Mat template_img, Mat H, bool bMarkPoint, Point2f& centerPt, float& fAngle)
{
	cout << "ShowICPResult!!\n";
	float pose[3];
	pose[0] = H.at<double>(0, 2); // x [pixel]
	pose[1] = H.at<double>(1, 2); // y [pixel]
	pose[2] = atan2(H.at<double>(1, 0), H.at<double>(0, 0));

	float R[2][2];

	R[0][0] = cos(pose[2]);
	R[0][1] = -sin(pose[2]);

	R[1][0] = sin(pose[2]);
	R[1][1] = cos(pose[2]);

	if (bMarkPoint)
	{
		vector<Point2f> output_points(4);
		output_points.clear();

		for (int i = 0; i < 4; i++)
		{
			output_points[i].x = R[0][0] * ref_pts[i].x + R[0][1] * ref_pts[i].y + pose[0];
			output_points[i].y = R[1][0] * ref_pts[i].x + R[1][1] * ref_pts[i].y + pose[1];
		}

		for (int i = 0; i < 4; i++)
		{
			drawMarker(input_img, output_points[i], cv::Scalar(255, 0, 0), MARKER_CROSS, 10, 1);
		}
	}

	int size = tpl_points.size();
	vector<Point2f> output_tpl_points(size);
	output_tpl_points.clear();
	for (int i = 0; i < size; i++)
	{
		output_tpl_points[i].x = R[0][0] * tpl_points[i].x + R[0][1] * tpl_points[i].y + pose[0];
		output_tpl_points[i].y = R[1][0] * tpl_points[i].x + R[1][1] * tpl_points[i].y + pose[1];
	}

	for (int i = 0; i < size; i++)
	{
		circle(input_img, output_tpl_points[i], 2, Scalar(0, 0, 255), 1);
	}


	double a = H.at<double>(0, 0);
	double b = H.at<double>(0, 1);

	float angle = atan2(b, a);

	std::vector<Point2f> obj_corners(1);
	float _col = template_img.cols / 2;
	float _row = template_img.rows / 2;
	obj_corners[0] = cvPoint(_col, _row);
	std::vector<Point2f> scene_corners(1);

	perspectiveTransform(obj_corners, scene_corners, H);
	drawMarker(input_img, scene_corners[0], cv::Scalar(255, 0, 255), MARKER_CROSS, 10, 2);

	cout << "===========================================================================" << endl;
	cout << "center point = " << scene_corners[0] * 4 << endl;
	cout << "angle = " << angle << endl;
	cout << "===========================================================================" << endl;
    centerPt = scene_corners[0] * 4;
    fAngle = angle;
#if RESULT_IMSHOW
	stringstream win_name;
	win_name << "icp_result" << ".bmp";
	namedWindow(win_name.str(), CV_WINDOW_NORMAL);
	imshow(win_name.str(), input_img);
	waitKey(0);
#endif
}

void CCore::EstimateErrorRate(Mat& input_img, Mat template_img, Mat H)
{
	vector<Point2f> t_pts;
	perspectiveTransform(ref_pts, t_pts, H);

	vector<Point2f> contour_centers;
	FindHoleCenter(input_img, t_pts, contour_centers);
	
	for (size_t i = 0; i < t_pts.size(); ++i)
	{
		xDiff.emplace_back(abs(t_pts[i].x - contour_centers[i].x));
		yDiff.emplace_back(abs(t_pts[i].y - contour_centers[i].y));
	}

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(template_img.cols, 0);
	obj_corners[2] = cvPoint(template_img.cols, template_img.rows);
	obj_corners[3] = cvPoint(0, template_img.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

	double a = H.at<double>(0, 0);
	double b = H.at<double>(0, 1);

	float angle = atan2(b, a);

	vector<Point2f> v_roi_corners;
	for (int i = 0; i < 4; i++)
		v_roi_corners.emplace_back(scene_corners[i]);
	
	Mat roi = GetROIimage(input_img, v_roi_corners);

	cout << "\nangle: " << angle << endl << endl;
	double houghAngle = GetAngleFromHough(roi, t_pts, angle);

	thetaDiff.emplace_back(abs(houghAngle - angle));

	/*std::ofstream outfile;
	outfile.open("angle.txt", std::ios_base::app);
	outfile << angle << "," <<houghAngle<<"\n";
	outfile.close();*/

	cout << "hough angle = " << houghAngle << endl;
}

void CCore::CalculateRMSE(bool bIcp)
{
	double point_size = xDiff.size();
	double angle_size = thetaDiff.size();
	double errorSum_x = 0.0f;
	double errorSum_y = 0.0f;
	double errorSum_theta = 0.0f;
	for (size_t i = 0; i < point_size; ++i)
	{
#if 0
		errorSum_x += pow(xDiff[i], 2.0);
		errorSum_y += pow(yDiff[i], 2.0);
#else
		errorSum_x += xDiff[i];
		errorSum_y += yDiff[i];
#endif
	}

	for (size_t i = 0; i < angle_size; ++i)
	{
#if 0
		errorSum_theta += pow(thetaDiff[i], 2.0);
#else
		errorSum_theta += thetaDiff[i];
#endif

	}

#if 0
	double x_rmse = sqrt(errorSum_x / point_size);
	double y_rmse = sqrt(errorSum_y / point_size);
	double theta_rmse = sqrt(errorSum_theta / angle_size);

	cout << "\n\nxError : " << x_rmse << endl;
	cout << "yError : " << y_rmse << endl;
	cout << "thetaError : " << theta_rmse << endl << endl;

	std::ofstream outfile;
	outfile.open("result2.txt", std::ios_base::app);
	outfile << x_rmse << "\n";
	outfile << y_rmse << "\n";
	outfile << theta_rmse << "\n";
	outfile.close();
#else
	double x_e = (errorSum_x / point_size);
	double y_e = (errorSum_y / point_size);
	double theta_e = (errorSum_theta / angle_size);

	cout << "\n\nxError : " << x_e << endl;
	cout << "yError : " << y_e << endl;
	cout << "thetaError : " << theta_e << endl << endl;

	std::ofstream outfile;
	if(!bIcp)
		outfile.open("brisk_result.txt", std::ios_base::app);
	else
		outfile.open("brisk+icp_result.txt", std::ios_base::app);
	outfile << x_e << "\n";
	outfile << y_e << "\n";
	outfile << theta_e * 180 / pi << "\n";
	outfile.close();
	
#endif

}

double CCore::GetAngleFromHough(Mat& gray, vector<Point2f> t_pts, float angle)
{
	Mat contours;
	Canny(gray, contours, 255/3, 255);
	
	std::vector<cv::Vec2f> lines;
	cv::HoughLines(contours, lines,
		1, pi / 180, // 단계별 크기
		80);  // 투표(vote) 최대 개수

	 // 선 그리기
	cv::Mat result(contours.rows, contours.cols, CV_8U, cv::Scalar(255));
	//std::cout << "Lines detected: " << lines.size() << std::endl;

	// 선 벡터를 반복해 선 그리기
	vector<float> v_theta;
	std::vector<cv::Vec2f>::const_iterator it = lines.begin();
	while (it != lines.end()) {
		float rho = (*it)[0];   // 첫 번째 요소는 rho 거리
		float theta = (*it)[1]; // 두 번째 요소는 델타 각도
		v_theta.emplace_back(theta);
		
		if (theta < pi / 4. || theta > 3.*pi / 4.) { // 수직 행
			cv::Point pt1(rho / cos(theta), 0); // 첫 행에서 해당 선의 교차점   
			cv::Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
			// 마지막 행에서 해당 선의 교차점
			cv::line(gray, pt1, pt2, cv::Scalar(255), 1); // 하얀 선으로 그리기

		}
		else { // 수평 행
			cv::Point pt1(0, rho / sin(theta)); // 첫 번째 열에서 해당 선의 교차점  
			cv::Point pt2(result.cols, (rho - result.cols*cos(theta)) / sin(theta));
			// 마지막 열에서 해당 선의 교차점
			cv::line(gray, pt1, pt2, cv::Scalar(255), 1); // 하얀 선으로 그리기
		}
		//std::cout << "line: (" << rho << "," << theta << ")\n";

		++it;

	}
#if IMSHOW
	namedWindow("gradient", CV_WINDOW_NORMAL);
	imshow("gradient", gray);
	waitKey(0);
#endif

	return FindBestAngleFromHough(v_theta, angle);
}

double CCore::FindBestAngleFromHough(vector<float> v_theta, float angle)
{
	double t1, t2;
	double minDiffAngle = 12345.0, houghAngle = 0.0;
	for (int i = 0; i < v_theta.size(); i++)
	{
		t1 = (pi / 2.0) - v_theta[i] + angle_offset;
			//t1 += data_5_offset;

		for (int k = 0; k < 4; k++)
		{
			double offset_angle = (pi / 2.0 * k);
			t2 = t1 + offset_angle;
			double diff = abs(angle - t2);
			if (diff < minDiffAngle)
			{
				houghAngle = t2;
				minDiffAngle = diff;
			}

			t2 = t1 - offset_angle;
			diff = abs(angle - t2);
			if (diff < minDiffAngle)
			{
				houghAngle = t2;
				minDiffAngle = diff;
			}
		}

	}

	return houghAngle;
}

void CCore::FindHoleCenter(Mat& input_img, vector<Point2f> t_pts, vector<Point2f>& contour_centers)
{
	Mat img = input_img.clone();
	Mat gray;
	cvtColor(input_img, gray, CV_BGR2GRAY);
	threshold(gray, gray, binTh, 255, THRESH_BINARY);

	contour_centers.clear();
	Point2f start_pos;
	for (size_t i = 0; i < t_pts.size(); ++i)
	{
		start_pos.x = MAX(0, t_pts[i].x - 10);
		start_pos.y = MAX(0, t_pts[i].y - 10);

		int offset = 20;
		int roiW = offset, roiH = offset;
		if (start_pos.x + offset > input_img.cols)
			roiW = input_img.cols - start_pos.x;
		if (start_pos.y + offset > input_img.rows)
			roiH = input_img.rows - start_pos.y;

		Rect roi = Rect(start_pos.x, start_pos.y, roiW, roiH);
		Mat test_roi = img(roi);
		Mat hole_roi = gray(roi);

		vector<vector<Point> > contours; // Vector for storing contour
		vector<Vec4i> hierarchy;
		findContours(hole_roi, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

		RNG rng(12345);
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		
		vector<RotatedRect> minRect(contours.size());
		double max = 0;
		int maxIdx = 0;
		for (size_t i = 0; i < contours.size(); i++)
		{
			double area = contourArea(Mat(contours[i]));
			minRect[i] = minAreaRect(Mat(contours[i]));
			if (i != 0 && area > max)
			{
				max = area;
				maxIdx = i;
			}

		}

		circle(test_roi, minRect[maxIdx].center, 2, Scalar(0, 0, 255), 1);
		contour_centers.emplace_back(minRect[maxIdx].center + start_pos);
		
		//drawContours(test_roi, contours, maxIdx, color, 2, 8, vector<Vec4i>(), 0, Point());
		Point2f rect_points[4]; minRect[maxIdx].points(rect_points);
		for (int j = 0; j < 4; j++)
			line(test_roi, rect_points[j], rect_points[(j + 1) % 4], Scalar(0, 255, 0), 1, 8);

#if IMSHOW
		namedWindow("img", CV_WINDOW_NORMAL);
		imshow("img", test_roi);
		waitKey(0);
#endif
	}
	
	
}


Mat CCore::GetROIimage(Mat img, vector<Point2f> &v_roi_corners)
{
	Mat detect_img = img.clone();
	cvtColor(detect_img, detect_img, CV_BGR2GRAY);
	cout << v_roi_corners << endl;

	Point corners[1][4] = { Point(0,0), };
	corners[0][0] = v_roi_corners[0];
	corners[0][1] = v_roi_corners[1];
	corners[0][2] = v_roi_corners[2];
	corners[0][3] = v_roi_corners[3];


	double minX = 12345, maxX = 0, minY = 12345, maxY = 0;
	for (int i = 0; i < 4; i++)
	{
		if (corners[0][i].x > maxX)
			maxX = corners[0][i].x;
		if (corners[0][i].x < minX)
			minX = corners[0][i].x;
		if (corners[0][i].y > maxY)
			maxY = corners[0][i].y;
		if (corners[0][i].y < minY)
			minY = corners[0][i].y;
	}
	minX = MAX(0, minX);
	minY = MAX(0, minY);
	maxX = MIN(img.cols, maxX);
	maxY = MIN(img.rows, maxY);

	Rect roi = Rect(minX, minY, MAX(0,maxX-minX), MAX(0,maxY-minY));
	Mat roi_img = detect_img(roi);

	return roi_img;
}
