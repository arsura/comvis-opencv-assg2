#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/// Global Variables
Mat img;
Mat templ;
Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";

int match_method;
int i = 0;
int max_Trackbar = 5;

/// Function Headers
void MatchingMethod(int, void*);
void delay();

/** @function main */
int main(int argc, char** argv)
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
	{
		return -1;
	}
	// Load image and template
	templ = imread("D:\\Com_Vision\\pic\\match.jpg", 1);

	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		if (waitKey(30) >= 0) break;
		while (frame.empty())
		{
			std::cout << "Frame empty" << std::endl;
		}
		// do any processing
		/// Load image and template
		img = frame.clone();
		//frame.clone();

		/// Create windows
		namedWindow(image_window, CV_WINDOW_AUTOSIZE);
		namedWindow(result_window, CV_WINDOW_AUTOSIZE);

		/// Create Trackbar
		const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
		createTrackbar( trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod );


		MatchingMethod(0, 0);

		waitKey(30);

	}
	return 0;
}

/**
 * @function MatchingMethod
 * @brief Trackbar callback
 */
void MatchingMethod(int, void*)
{
	/// Source image to display
	Mat img_display;
	img.copyTo(img_display);

	/// Create the result matrix
	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	/// Do the Matching and Normalize
	matchTemplate(img, templ, result, match_method);
	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	double minVal;
	double maxVal;
	Point minLoc;
	Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
	}
	else
	{
		matchLoc = maxLoc;
	}

	/// Show me what you got
	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

	imshow(image_window, img_display);
	imshow(result_window, result);

	return;
}