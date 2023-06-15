#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//point data struct
struct drawingPoint {
	vector<int> points;
	vector<double> color;
	enum shape pointShape;
	int width;
	int heigth;
};

enum shape {
	Circle,
	Square,
	Triangle,
	None
};

Mat img;
VideoCapture cap(0);
vector<struct drawingPoint> newPoints;  // to store all points
enum shape detectedShape = None;
vector<double> detectedColor;
int detectedWidth = 0;
int detectedHeight = 0;
int penColors[] = {91, 161, 62, 113, 255, 255};


void findColor(Mat image) {
	// Convert the image to floating-point type
	cv::Mat imageFloat;
	image.convertTo(imageFloat, CV_32FC3);

	// Get the image dimensions
	int width = imageFloat.cols;
	int height = imageFloat.rows;

	// Initialize the RGB values
	float sumR = 0, sumG = 0, sumB = 0;

	//take middle pixel
	Vec3f pixel = imageFloat.at<cv::Vec3f>(height/2, width/2);
	detectedColor.clear();
	detectedColor.push_back(pixel[0]);
	detectedColor.push_back(pixel[1]);
	detectedColor.push_back(pixel[2]);

	cout << "found color: " << pixel[0] << " " << pixel[1] << " " << pixel[2] << endl;
}

//Find where a pen is and save the shape data to its location as a new point, uses a binary masked image
Point findPen(Mat image) {
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	//get the shapes
	findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);

	//check each shape that needs to be a certain size
	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		//cout << area << endl;

		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << " waht " << detectedShape << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			//save the middle of the pen
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y + boundRect[i].height / 2;

			//make sure that a shape has been found
			if (detectedShape != None) {
				struct drawingPoint newPoint =
				{
					{ myPoint.x,myPoint.y },
					{detectedColor[0], detectedColor[1], detectedColor[2]},
					detectedShape,
					detectedWidth,
					detectedHeight
				};
				newPoints.push_back(newPoint);
				cout << "added point " << detectedShape << endl;
			}
		}
	}
	//show what the pen detection sees
	imshow("pen image", image);
	return myPoint;
}

void detectShape(Mat img) {
	Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

	// Preprocessing
	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgDil, kernel);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255), 2);
	//show what the detection process sees
	imshow("before detection image", imgDil);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	Mat imgCrop;
	 
	//go through each contour
	for (int i = 0; i < contours.size(); i++)
	{

		int area = contourArea(contours[i]);
		//cout << area << endl;
		string objectType;


		//only use shapes of a certain size
		if (area > 1000 && area < 6000) 
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			boundRect[i] = boundingRect(conPoly[i]);
			
		
			int objCor = (int)conPoly[i].size();

			imgCrop = img(boundRect[i]);

			if (objCor == 3) { 
				//make sure its an actual good triangle
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				if (aspRatio < 0.8 || aspRatio > 1.2) {
					continue;
				}
				objectType = "Tri";
				detectedShape = Triangle;
			}
			else if (objCor == 4)
			{ 
				//make sure its no crazy rectangle or square
				//square or rectangle doesnt matter
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				//cout << aspRatio << endl;
				if (aspRatio> 0.7 && aspRatio< 1.3){ objectType = "Square"; }
				else { continue; }
				detectedShape = Square;
			}
			else if (objCor > 4) { 
				//make sure its an actual circle and no random polygon
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				if (aspRatio < 0.8 || aspRatio > 1.2) {
					continue;
				}
				if (!isContourConvex(conPoly[i])) {
					continue; // Not a circle
				}

				objectType = "Circle"; 
				detectedShape = Circle;
			}
			else {
				//dont save other shapes
				continue;
			}

			//save the shapes widht and height
			detectedHeight = boundRect[i].height;
			detectedWidth = boundRect[i].width;

			cout << "Detected shape " << objectType << " " << detectedHeight << " " << detectedWidth << " " << area << endl;

			//save the shapes color
			findColor(imgCrop);

			//show what shape was detected
			cvtColor(imgDil, imgDil, COLOR_GRAY2BGR);
			drawContours(imgDil, conPoly, i, Scalar(255, 0, 255), 2);
			rectangle(imgDil, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);
			putText(imgDil, objectType, { boundRect[i].x,boundRect[i].y - 5 }, FONT_HERSHEY_PLAIN,1, Scalar(0, 69, 255), 2);
			//drawContours(imgDil, contours, i, Scalar(255, 0, 0), 4);
			imshow("detection image", imgDil);
		}
	}
}

//draw each point and its shape on an image
void drawOnCanvas(vector<struct drawingPoint> newPoints)
{

	for (int i = 0; i < newPoints.size(); i++)
	{
		vector<Point> trianglePoints;
		vector<vector<Point>> triangles;
		switch (newPoints[i].pointShape)
		{
		case Circle:
			circle(img, Point(newPoints[i].points[0], newPoints[i].points[1]), newPoints[i].width/2, Scalar(newPoints[i].color[0], newPoints[i].color[1], newPoints[i].color[2]), FILLED);
			break;
		case Square:
			rectangle(img, Point(newPoints[i].points[0] - newPoints[i].width / 2, newPoints[i].points[1] - newPoints[i].heigth / 2), Point(newPoints[i].points[0] + newPoints[i].width/2, newPoints[i].points[1] + newPoints[i].heigth/2), Scalar(newPoints[i].color[0], newPoints[i].color[1], newPoints[i].color[2]), FILLED);
			break;
		case Triangle:
			trianglePoints.push_back(Point(newPoints[i].points[0], newPoints[i].points[1] - newPoints[i].heigth/2));
			trianglePoints.push_back(Point(newPoints[i].points[0] + newPoints[i].width/2, newPoints[i].points[1] + newPoints[i].heigth/2));
			trianglePoints.push_back(Point(newPoints[i].points[0] - newPoints[i].width/2, newPoints[i].points[1] + newPoints[i].heigth/2));
			
			triangles.push_back(trianglePoints);

			fillPoly(img, triangles, Scalar(newPoints[i].color[0], newPoints[i].color[1], newPoints[i].color[2]));
			trianglePoints.clear();
			triangles.clear();
			break;
		default:
			break;
		}
		
	}
}

void findPoints(Mat img)
{
	//HSV format for color detection
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	//filter the pen color (in this case a blue switch joycon)
	Scalar lower(penColors[0], penColors[1], penColors[2]);
	Scalar upper(penColors[3], penColors[4], penColors[5]);
	Mat mask;
	
	inRange(imgHSV, lower, upper, mask);

	//use the color mask to detect a large enough one to add as a point
	findPen(mask);

	
}


void main() {

	while (true) {

		cap.read(img);

		//find a shape and save its stats
		detectShape(img);

		//find a pen and save its current point and shape settings
		findPoints(img);

		//draw all points on the image
		drawOnCanvas(newPoints);
		
		imshow("Image", img);
		waitKey(1);
	}
}

//
// To find pen color
// 
//void main() {
//
//	string path = "C:/Users/coend/Downloads/Images/Images/Images1/kerstballen.bmp";
//	cap.read(img);
//	Mat imgHSV, mask;
//	int hmin = 0, smin = 110, vmin = 153;
//	int hmax = 19, smax = 240, vmax = 255;
//
//	cvtColor(img, imgHSV, COLOR_BGR2HSV);
//
//	namedWindow("Trackbars", (640, 200));
//	createTrackbar("Hue Min", "Trackbars", &hmin, 179);
//	createTrackbar("Hue Max", "Trackbars", &hmax, 179);
//	createTrackbar("Sat Min", "Trackbars", &smin, 255);
//	createTrackbar("Sat Max", "Trackbars", &smax, 255);
//	createTrackbar("Val Min", "Trackbars", &vmin, 255);
//	createTrackbar("Val Max", "Trackbars", &vmax, 255);
//
//	while (true) {
//		cap.read(img);
//		cvtColor(img, imgHSV, COLOR_BGR2HSV);
//
//		Scalar lower(hmin, smin, vmin);
//		Scalar upper(hmax, smax, vmax);
//		inRange(imgHSV, lower, upper, mask);
//
//		imshow("Image", img);
//		imshow("Image HSV", imgHSV);
//		imshow("Image Mask", mask);
//		waitKey(1);
//	}
//}

