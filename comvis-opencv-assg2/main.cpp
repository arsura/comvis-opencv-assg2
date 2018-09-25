#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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


int main()
{

	Mat frame;
	Mat gray;
	Mat dst;
	Mat bw;
	std::vector<std::vector<cv::Point>> contours;
	vector<cv::Point> approx;

	VideoCapture capture(0);
	int q;

	while (cvWaitKey(30) != 'q')
	{
		capture >> frame;
		if (true)
		{
			// Convert to grayscale
			cvtColor(frame, gray, CV_BGR2GRAY);

			// Use Canny instead of threshold to catch squares with gradient shading
			blur(gray, bw, Size(3, 3));
			cv::Canny(gray, bw, 80, 240, 3);
			
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
					if (vtc == 4)
						setLabel(dst, "RECT", contours[i]);
					else if (vtc == 5)
						setLabel(dst, "PENTA", contours[i]);
					else if (vtc == 6)
						setLabel(dst, "HEXA", contours[i]);

				}
				else
				{
					// Detect and label circles
					double area = cv::contourArea(contours[i]);
					cv::Rect r = cv::boundingRect(contours[i]);
					int radius = r.width / 2;

					if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
						std::abs(1 - (area / (CV_PI * (radius*radius)))) <= 0.2)
						setLabel(dst, "CIR", contours[i]);
				}
			}
			//imshow("src", frame);
			imshow("dst", dst);

		}
	}
		
	return 0;
}