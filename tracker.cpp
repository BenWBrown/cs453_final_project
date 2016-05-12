#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d/features2d.hpp"
//#include "sound.h"
#include <unistd.h>
#include "tracker.h"

using namespace cv;
using namespace std;

int track(Mat *frame, KeyPoint *keypoints) {
	Mat image, big;
	int ballz;
	ballz = 2;

	//Mat img = imread("ball1.jpg", 1); //testing image

	// blur img
	blur(*frame, big, Size(75, 75));

	resize(big, image, Size(round(0.5*big.cols), round(0.5*big.rows)), 0.5, 0.5);

	// convert image to HSV
	cvtColor(image, image, COLOR_BGR2HSV);
	Mat lower_red_hue_range, upper_red_hue_range;
	inRange(image, Scalar(0, 100, 100), Scalar(10, 255, 255), lower_red_hue_range);
	inRange(image, Scalar(160, 100, 100), Scalar(179, 255 ,255), upper_red_hue_range);
	addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, image);


	image.convertTo(image, -1, -1, 255); // rtype=-1 (same result type), scale=-1, offset=255

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// Change thresholds
	//params.minThreshold = 10;
	//params.maxThreshold = 200;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = 1000;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.47;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01;


	// Set up detector with params
	SimpleBlobDetector detector(params);

	// You can use the detector this way
	// detector.detect( im, keypoints);
	//   Mat im_with_keypoints;
	// //copy more params code later
	//

	//SimpleBlobDetector detector;

	//
	vector<KeyPoint> keypointsVector;
	detector.detect(image, keypointsVector);
	//
	//
	drawKeypoints( image, keypointsVector, *frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );


  return 0;
}

// int main(int argc, char** argv) {
// 	if( argc != 2)
// 		{
// 			cout <<" Usage: in_img" << endl;
// 			return -1;
// 		}
// 	Mat frame;
// 	KeyPoint keypoints[4];
// 	frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);
// 	printf("so far so good\n");
// 	track(&frame, keypoints);
// 	printf("so far so good\n");
// 	imshow("Display window", frame );                   // Show our image inside it.
//
// 	waitKey(0);
//
// }
