#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


void main() {

	string path = "Resources/perspective poster.jpg";
	Mat img = imread(path);
	Mat matrix, imgWarp;

	float scale = 0.3;

	resize(img, img, Size(), scale, scale);

	float w = 700, h = 500;

	Point2f src[4] = { {scale * 480,scale * 1500},{scale * 2450,scale * 350},{scale * 860,scale * 2560},{scale * 3190,scale * 2541} };
	Point2f dst[4] = { {0.0f,0.0f},{w,0.0f},{0.0f,h},{w,h} };

	matrix = getPerspectiveTransform(src, dst);
	warpPerspective(img, imgWarp, matrix, Point(w, h));

	for (int i = 0; i < 4; i++)
	{
		circle(img, src[i], 10, Scalar(0, 0, 255), FILLED);
	}

	imshow("Image", img);
	imshow("Image Warp", imgWarp);
	waitKey(0);

}