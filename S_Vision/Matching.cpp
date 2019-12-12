#include "Matching.h"
#include "Utils.h"
#include <opencv2/opencv.hpp>

CMatching::CMatching()
{
}


CMatching::~CMatching()
{
}

/*******************************************************************/
Mat CMatching::briskMatching(Mat img_object, Mat img_scene)
/*******************************************************************/
{
	medianBlur(img_object, img_object, 3);
	//medianBlur(img_scene, img_scene, 3);

	vector<KeyPoint> keypoints_object, keypoints_scene;
	Mat descriptor_object, descriptor_scene;

	Ptr<BRISK> brisk = BRISK::create();
	//BFMatcher matcher(NORM_HAMMING, true);
	Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create(DescriptorMatcher::FLANNBASED);
	brisk->detectAndCompute(img_object, Mat(), keypoints_object, descriptor_object);
	brisk->detectAndCompute(img_scene, Mat(), keypoints_scene, descriptor_scene);

	Mat outImg_1;
	drawKeypoints(img_object, keypoints_object, outImg_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
#if IMSHOW
	imshow("BRISK", outImg_1);
	waitKey(0);
#endif
	vector<DMatch> matches;
	vector<vector<DMatch>> knn_matches;
	descriptor_object.convertTo(descriptor_object, CV_32F);
	descriptor_scene.convertTo(descriptor_scene, CV_32F);
	matcher->knnMatch(descriptor_object, descriptor_scene, knn_matches, 2);
	//matcher.match(descriptor_object, descriptor_scene, matches, Mat());

	cout << "knnMatches size = " << knn_matches.size() << endl;
	const float ratio_threshold = 0.75f;
	std::vector<DMatch> good_matches;
	for (size_t i = 0; i < knn_matches.size(); ++i)
		if (knn_matches[i][0].distance < ratio_threshold * knn_matches[i][1].distance)
			good_matches.emplace_back(knn_matches[i][0]);

	/*Mat index;
	int nbMatch = int(matches.size());
	Mat tab(nbMatch, 1, CV_32F);
	for (int i = 0; i < nbMatch; i++)
	{
		tab.at<float>(i, 0) = matches[i].distance;
	}

	cout << "matches : " << matches.size() << endl;


	sortIdx(tab, index, SORT_EVERY_COLUMN + SORT_ASCENDING);
	vector<DMatch> good_matches;
	for (int i = 0; i < matches.size() * 0.1; i++)
	{
		good_matches.push_back(matches[index.at<int>(i, 0)]);
	}*/

	Mat img_matches;
	drawMatches(img_object, keypoints_object, img_scene, keypoints_scene, good_matches, img_matches, Scalar::all(-1),
		Scalar::all(-1), std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	
	std::vector<Point2f> obj;
	std::vector<Point2f> scene;
	obj.clear();
	scene.clear();
	for (int i = 0; i < good_matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		obj.emplace_back(keypoints_object[good_matches[i].queryIdx].pt);
		scene.emplace_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	
	//Mat H = findHomography(obj, scene, RANSAC);
	//Mat R = estimateRigidTransform(obj, scene, false);
	Mat R = estimateAffinePartial2D(obj, scene); //x scale    y sckew    0
												 //x skew     y scale    0
												 //x position y position 1 

	Mat H = Mat(3, 3, R.type());
	H.at<double>(0, 0) = R.at<double>(0, 0);
	H.at<double>(0, 1) = R.at<double>(0, 1);
	H.at<double>(0, 2) = R.at<double>(0, 2);

	H.at<double>(1, 0) = R.at<double>(1, 0);
	H.at<double>(1, 1) = R.at<double>(1, 1);
	H.at<double>(1, 2) = R.at<double>(1, 2);

	H.at<double>(2, 0) = 0.0;
	H.at<double>(2, 1) = 0.0;
	H.at<double>(2, 2) = 1.0;

	//-- Get the corners from the image_1 ( the object to be "detected" )
	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows);
	obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	perspectiveTransform(obj_corners, scene_corners, H);

#if LOG
	cout << "H = " << H << endl;
#endif 
	double a = H.at<double>(0, 0);
	double b = H.at<double>(0, 1);
	
	float angle = atan2(b, a);


	Point2f point_0 = scene_corners[0];
	Point2f point_1 = scene_corners[1];
	Point2f point_2 = scene_corners[2];
	Point2f point_3 = scene_corners[3];


#if IMSHOW
	//-- Draw lines between the corners (the mapped object in the scene - image_2 )
	line(img_matches, point_0 + Point2f(img_object.cols, 0), point_1 + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, point_1 + Point2f(img_object.cols, 0), point_2 + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, point_2 + Point2f(img_object.cols, 0), point_3 + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);
	line(img_matches, point_3 + Point2f(img_object.cols, 0), point_0 + Point2f(img_object.cols, 0), Scalar(0, 255, 0), 4);

	namedWindow("img_matches", CV_WINDOW_NORMAL);
	imshow("img_matches", img_matches);
	waitKey(0);
#endif
	
	return H;
}


