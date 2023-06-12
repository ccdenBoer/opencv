#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


void main() {

	string path = "Resources/balls.png";
	Mat img = imread(path);
	Mat imgGray, imgDil, imgErode;

	cvtColor(img, imgGray, COLOR_BGR2GRAY);


	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(imgGray, imgDil, kernel);
	erode(imgDil, imgErode, kernel);
	imshow("Image", img);
	imshow("Image Erosion", imgErode);
	waitKey(0);
}