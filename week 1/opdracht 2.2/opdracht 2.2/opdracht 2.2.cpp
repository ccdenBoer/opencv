#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void main() {

	string path = "C:/Users/coend/Pictures/perspective poster.jpg";
	Mat img = imread(path);
	Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

	int cannyThresh1 = 0;
	int cannyThresh2 = 0;

	resize(img, img, Size(), 0.3, 0.3);

	namedWindow("Trackbars", (640, 200));
	createTrackbar("Threshold 1", "Trackbars", &cannyThresh1, 500);
	createTrackbar("Threshold 2", "Trackbars", &cannyThresh2, 500);

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(7, 7), 5, 0);

	while (true) {
		Canny(imgBlur, imgCanny, cannyThresh1, cannyThresh2);

		Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
		dilate(imgCanny, imgDil, kernel);
		erode(imgDil, imgErode, kernel);

		imshow("Image", img);
		imshow("Image Gray", imgGray);
		imshow("Image Blur", imgBlur);
		imshow("Image Canny", imgCanny);
		imshow("Image Dilation", imgDil);
		imshow("Image Erode", imgErode);
		waitKey(1);
	}
}
