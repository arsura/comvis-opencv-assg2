#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
	cv::Mat img = cv::imread("resource\\img\\opencv_logo.png");

	if (img.empty()) {
		std::cout << "failed to open opencv_logo.png" << std::endl;
	}
	else {
		std::cout << "opencv_logo.png loaded OK" << std::endl;
	}

	cv::namedWindow("opencv", cv::WINDOW_AUTOSIZE);
	cv::imshow("opencv", img);
	cv::waitKey(0);

	return 0;
}