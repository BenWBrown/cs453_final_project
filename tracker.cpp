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
	KeyPoint temp1, temp2;
	ballz = 2;

	temp1 = keypoints[0];
	temp2 = keypoints[1];
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
	params.minArea = 800;
	 
	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.45;
	 
	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.85;
	 
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
	
	printf("original keypoints (%f, %f,) and (%f, %f)\n", temp1.pt.x, temp1.pt.y, temp2.pt.x, temp2.pt.y);
//	printf("new keypoints (%f, %f,) and (%f, %f) and (%f, %f)\n", keypointsVector[0].pt.x, keypointsVector[0].pt.y, keypointsVector[1].pt.x, keypointsVector[1].pt.y);
	//
	//
	drawKeypoints(image, keypointsVector, *frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

	Point2f diff1, diff2;
	float size, angle;
	for (int i = 0; i < keypointsVector.size(); i++) {
		diff1.x = keypoints[0].pt.x - keypointsVector[i].pt.x;
		diff1.y = keypoints[0].pt.y - keypointsVector[i].pt.y;
		diff2.x = keypoints[1].pt.x - keypointsVector[i].pt.x;
		diff2.y = keypoints[1].pt.y - keypointsVector[i].pt.y;
		size = sqrt(diff1.x*diff1.x + diff1.y*diff1.y);
		angle = atan(diff1.y/diff1.x);
		//printf("new kp (%f, %f) - diffsize %f / angle %f\n", keypointsVector[i].pt.x, keypointsVector[i].pt.y, size, angle);
		if (((diff1.x > 1.0) || (diff1.x < -1.0)) && size < 100) {
			keypoints[0] = KeyPoint(keypointsVector[i].pt, 1, -1);
			printf("* new keypoint added (%f, %f) @ size %f, %f\n", keypoints[0].pt.x, keypoints[0].pt.y, size, angle);
		}
		size = sqrt(diff2.x*diff2.x + diff2.y*diff2.y);
		angle = atan(diff2.y/diff2.x);
		//printf("new kp (%f, %f) - diffsize %f / angle %f\n", keypointsVector[i].pt.x, keypointsVector[i].pt.y, size, angle);
		if (((diff2.x > 1.0) || (diff2.x < -1.0)) && size < 100) {
			keypoints[1] = KeyPoint(keypointsVector[i].pt, 1, -1);
			printf("* new keypoint added (%f, %f) @ size %f, %f\n", keypoints[1].pt.x, keypoints[1].pt.y, size, angle);
		size = 0;
		angle = 0;
		}
	}
	if (keypoints[0].pt.x == 0.0) {
//		printf("should update keypoints 0\n");
		keypoints[0] = keypointsVector[0];
			printf("new keypoint added (%f, %f)\n", keypoints[0].pt.x, keypoints[0].pt.y);
	}
	if (keypoints[1].pt.x == 0.0) {
//		printf("should update keypoints 1\n");
		keypoints[1] = keypointsVector[1];
			printf("new keypoint added (%f, %f)\n", keypoints[1].pt.x, keypoints[1].pt.y);
	}
	return 0;
}
/*
int main(int argc, char** argv) {
	if( argc != 6)
		{
			cout <<" Usage: in_img1 in_img2 in_img2" << endl;
			return -1;
		}
	Mat frame;
	KeyPoint keypoints[4];
	keypoints[0].pt.x = 0.0;
	keypoints[1].pt.x = 0.0;
	frame = imread(argv[1], CV_LOAD_IMAGE_COLOR);
	track(&frame, keypoints);
	imshow("Display window", frame );                   // Show our image inside it.
	waitKey(0);
	printf("-----------\n");
	frame = imread(argv[2], CV_LOAD_IMAGE_COLOR);
	track(&frame, keypoints);
	imshow("Display window", frame );                   // Show our image inside it.
	waitKey(0);
	printf("-----------\n");
	frame = imread(argv[3], CV_LOAD_IMAGE_COLOR);
	track(&frame, keypoints);
	imshow("Display window", frame );                   // Show our image inside it.
	waitKey(0);
	printf("-----------\n");
	frame = imread(argv[4], CV_LOAD_IMAGE_COLOR);
	track(&frame, keypoints);
	imshow("Display window", frame );                   // Show our image inside it.
	waitKey(0);
	printf("-----------\n");
	frame = imread(argv[5], CV_LOAD_IMAGE_COLOR);
	track(&frame, keypoints);
	imshow("Display window", frame );                   // Show our image inside it.
	waitKey(0);
	
}
*/
