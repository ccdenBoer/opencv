#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


void main() {

	string path = "Resources/test.png";
	Mat img = imread(path);
	Mat imgGray, imgBlur1, imgBlur2, imgBlur3;

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur1, Size(7, 7), 5, 0);
	GaussianBlur(imgGray, imgBlur2, Size(7, 7), 5, 0);
	GaussianBlur(imgGray, imgBlur3, Size(7, 7), 14, 0);

	imshow("Image", img);
	imshow("Image Blur 1", imgBlur1);
	imshow("Image Blur 2", imgBlur2);
	imshow("Image Blur 3", imgBlur3);
	waitKey(0);
}