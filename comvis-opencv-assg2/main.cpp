#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;


static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1 * dy2) / sqrt((dx1*dx1 + dy1 * dy1)*(dx2*dx2 + dy2 * dy2) + 1e-10);
}


void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255, 255, 255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0, 0, 0), thickness, 8);
}


void colorSegment(Mat snapshot) {

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
			cout << color << endl;
		}	
	}


}


int main()
{

	Mat frame;
	Mat gray;
	Mat dst;
	Mat bw;
	

	std::vector<std::vector<cv::Point>> contours;
	vector<cv::Point> approx;

	namedWindow("bw", WINDOW_AUTOSIZE);
	namedWindow("dst", WINDOW_AUTOSIZE);
	namedWindow("video", WINDOW_AUTOSIZE);
	namedWindow("roi", WINDOW_AUTOSIZE);
	int q;

	VideoCapture capture("http://10.61.0.23:4747/video");

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
		imshow("video", frame);

		// Convert to grayscale
		cvtColor(frame, gray, CV_BGR2GRAY);

		// Use Canny instead of threshold to catch squares with gradient shading
		blur(gray, bw, Size(3, 3));
		Canny(gray, bw, 80, 100, 3);
			
		imshow("bw", bw);
		//imshow("src", frame);
		//imshow("gray", gray);


		// Find contours
		cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		frame.copyTo(dst);

		for (int i = 0; i < contours.size(); i++) {

			// Approximate contours
			cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*(0.02), true);
				
			if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
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
				std::vector<double> cos;

				for (int j = 2; j < vtc + 1; j++)
					cos.push_back(angle(cv::Point(approx[j%vtc]), cv::Point(approx[j - 2]), cv::Point(approx[j - 1])));

				// Sort ascending the cosine values
				std::sort(cos.begin(), cos.end());

				// Get the lowest and the highest cosine
				double mincos = cos.front();
				double maxcos = cos.back();

				// Use the degrees obtained above and the number of vertices
				// to determine the shape of the contour

				Rect bounding_rect;
				Mat snapshot, colorArray;	
				

				if (vtc == 4) {
						
					setLabel(dst, "RECT", contours[i]);

					//double area = contourArea(contours[i]);
					//bounding_rect = boundingRect(contours[i]);

					//Mat roi = Mat(frame, bounding_rect);
					//cv::imshow("Snapshot", snapshot);
					//snapshot = roi.clone();

					//imshow("roi", roi);
						
					//colorSegment(roi);
				
						
				}
	
			}
				else
				{
					// Detect and label circles
					Rect r;
				
					
					double area = contourArea(contours[i]);
					r = boundingRect(contours[i]);
					int radius = r.width / 2;

					if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
						std::abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.2) {
						
						setLabel(dst, "CIR", contours[i]);

						
						Mat roi = Mat(frame, r);
						imshow("roi", roi);


						colorSegment(roi);
						
					}
	

				}
				
		}
		//imshow("src", frame);
		imshow("dst", dst);


		if (waitKey(30) >= 0) break;
	}
	
	//waitKey(0);
	return 0;
}