#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main()
{

	Mat frame;
	Mat gray;

	VideoCapture capture(0);
	int q;

	while (cvWaitKey(30) != 'q')
	{
		capture >> frame;
		if (true)
		{
			// Convert to grayscale
			cvtColor(frame, gray, CV_BGR2GRAY);

			imshow("src", frame);
			imshow("gray", gray);		

		}

	}
	return 0;
}