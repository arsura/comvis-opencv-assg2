#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;


static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}


void setLabel(Mat& im, const string label, vector<Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	Size text = getTextSize(label, fontface, scale, thickness, &baseline);
	Rect r = boundingRect(contour);

	Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	rectangle(im, pt + Point(0, baseline), pt + Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}


void colorSegment(Mat& snapshot, Mat& src) {

	// find color
	
	int countGreen = 0; // 20
	int countBlue = 0; // 50
	int countRed = 0; // 100
	int countPurple = 0; // 500
	int countGray = 0; // 1000


	for (int y = 0; y < snapshot.rows; y++)
	{
		for (int x = 0; x < snapshot.cols; x++)
		{
			// get pixel
			Vec3b color = snapshot.at<Vec3b>(Point(x, y));
			//cout << color << endl;

			// 20 Banknotes
			if ((81 <= color[0] && color[0] <= 90) && 
				(81 <= color[1] && color[1] <= 90) &&
				(31 <= color[2] && color[2] <= 40)) 
			{
				countGreen++;
				//cout << "countGreen = " << countGreen << endl;
			
				if (countGreen == 30) {
					// do something
					//cout << "20 Found" << endl;
					y = snapshot.rows-1;
				}
				putText(src, "20 Thai Baht Banknote", Point(150, 100), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2);

			}

			// 50 Banknotes

			// 100 Banknotes
			else if 
				((81 <= color[0] && color[0] <= 90) &&
				(81 <= color[1] && color[1] <= 90) &&
				(131 <= color[2] && color[2] <= 140))
			{
				countRed++;
				//cout << "countRed = " << countRed << endl;

				if (countRed == 30) {
					// do something
				//	cout << " 100 Found" << endl;
					y = snapshot.rows - 1;
				}
				putText(src, "100 Thai Baht Banknote", Point(150, 100), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2);
			}

			// 500 Banknotes
			else if
				((81 <= color[0] && color[0] <= 90) &&
				(61 <= color[1] && color[1] <= 70) &&
				(51 <= color[2] && color[2] <= 60))
			{
				countPurple++;
				//cout << "countPurple = " << countPurple << endl;

				if (countPurple == 30) {
					// do something
				//	cout << " 500 Found" << endl;
					y = snapshot.rows - 1;
				}
				putText(src, "500 Thai Baht Banknote", Point(150, 100), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2);
			}

			// 1000 Banknotes
			else if
				((55 <= color[0] && color[0] <= 65) &&
				(65 <= color[1] && color[1] <= 85) &&
				(91 <= color[2] && color[2] <= 100))
			{
				countGray++;
			//	cout << "countGray = " << countGray << endl;

				if (countGray == 30) {
					// do something
			//		cout << " 1000 Found" << endl;
					y = snapshot.rows - 1;
				}
				putText(src, "1000 Thai Baht Banknote", Point(150, 100), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 2);
			}


		}	
	}


}


int main()
{

	Mat frame;
	Mat gray;
	Mat dst;
	Mat bw;
	
	

	vector<vector<Point>> contours;
	vector<Point> approx;

	namedWindow("bw", WINDOW_AUTOSIZE);
	//namedWindow("dst", WINDOW_AUTOSIZE);
	namedWindow("src", WINDOW_AUTOSIZE);
	//namedWindow("roi", WINDOW_AUTOSIZE);
	int q;

	VideoCapture capture(0);

	if (!capture.isOpened())
	{
		cout << "Camera not found" << endl;
		getchar();
		return -1;
	}

	while (capture.isOpened())
	{
		
		capture >> frame;
		
		if (frame.empty()) break;

		//frame = imread("resource/img/test_pict.jpg");
		//imshow("video", frame);

		// Convert to grayscale
		cvtColor(frame, gray, CV_BGR2GRAY);

		// Use Canny instead of threshold to catch squares with gradient shading
		blur(gray, bw, Size(3, 3));
		Canny(gray, bw, 80, 100, 3);
			
		imshow("bw", bw);
		//imshow("src", frame);
		//imshow("gray", gray);


		// Find contours
		findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		frame.copyTo(dst);

		for (int i = 0; i < contours.size(); i++) {

			// Approximate contours
			approxPolyDP(cv::Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*(0.02), true);
				
			if (fabs(contourArea(contours[i])) < 100 || !isContourConvex(approx))
				continue;
			
			if (approx.size() == 3)
			{
				setLabel(dst, "TRI", contours[i]);    // Triangles
			}
			else if (approx.size() >= 4 && approx.size() <= 6)
			{
				// Number of vertices of polygonal curve
				int vtc = approx.size();

				// Get the cosines of all corners
				vector<double> cos;

				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(Point(approx[j%vtc]), Point(approx[j - 2]), Point(approx[j - 1])));

				// Sort ascending the cosine values
				sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour

				Rect bounding_rect;
				Mat snapshot, colorArray;	
				
				// Rectangle detect
				if (vtc == 4) {
						
					setLabel(dst, "RECT", contours[i]);
				}
	
			}
				else
				{
					// Detect and label circles
					Rect r;
				
					
					double area = contourArea(contours[i]);
					r = boundingRect(contours[i]);
					int radius = r.width / 2;

					if (abs(1 - ((double)r.width / r.height)) <= 0.2 &&
						abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.2) {
						
						setLabel(dst, "CIR", contours[i]);

						
						Mat roi = Mat(frame, r);
						//imshow("roi", roi);


						colorSegment(roi, frame);
						
					}
	

				}
				
		}
		imshow("src", frame);
		//imshow("dst", dst);


		if (waitKey(30) >= 0) break;
	}
	
	//waitKey(0);
	return 0;
}