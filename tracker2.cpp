#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d/features2d.hpp"
//#include "sound.h"
#include <unistd.h>
#include "tracker.h"
#include <stdlib.h>

using namespace cv;
using namespace std;

void resetPoint(TrackedPoint *pt);
bool matching(KeyPoint keypoint, TrackedPoint trackedpoint);

int track(Mat *frame, TrackedPoint trackedPoints[], long frameNum) {
	Mat image, big;
	int drum = 0;

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


	vector<KeyPoint> keypointsVector;
	detector.detect(image, keypointsVector);

	drawKeypoints(image, keypointsVector, *frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

	//debug output
	// if (keypointsVector.size() > 0)
	// 	printf("\nframeNum: %lu  keyPoints:", frameNum);
	// for (int i=0;i<keypointsVector.size();i++) {
	// 	printf("kp: ((%f, %f))\n",keypointsVector[i].pt.x, keypointsVector[i].pt.y);
	// }

	//check if previously seen blob hasn't been seen in over FRAME_THRESH frames
	//and reset it if so
	for (int i = 0; i < NUM_PTS; i++) {
		if ((trackedPoints[i].frameNum != 0) && (trackedPoints[i].frameNum - frameNum > FRAME_THRESH))
			resetPoint(&trackedPoints[i]);
	}

	bool forceMatch = false;

	//match keypoints to tracked points
	for (int i = 0; i < NUM_PTS; i++) {
		//forcematch if trackedpoint is at initial state
		if (trackedPoints[i].x == 0 && trackedPoints[i].y == 0)
			forceMatch = true;
		for (int j = 0; j < keypointsVector.size(); j++) {
			if (matching(keypointsVector[j], trackedPoints[i]) || forceMatch) {
				//do stuff with matching points
				float vx, vy, velocitySquared;
				vx = ((float) (trackedPoints[i].x - keypointsVector[j].pt.x)) / (frameNum - trackedPoints[i].frameNum);
				vy = ((float) (trackedPoints[i].y - keypointsVector[j].pt.y)) / (frameNum - trackedPoints[i].frameNum);
				velocitySquared = vx*vx + vy*vy;

				//check if need to play drum sound
				//went from going fast to going slow
				if (trackedPoints[i].moving && (velocitySquared < LO_THRESH*LO_THRESH)) {
					printf("drum = 1");
					drum = 1;
				}

				printf("velocity: %f\n", sqrt(velocitySquared));


				//update trackedPoints
				if (velocitySquared > HI_THRESH*HI_THRESH) {
					trackedPoints[i].moving = true;
				} else if (velocitySquared < LO_THRESH*LO_THRESH) {
					trackedPoints[i].moving = false;
				}
				trackedPoints[i].x = keypointsVector[j].pt.x;
				trackedPoints[i].y = keypointsVector[j].pt.y;
				trackedPoints[i].vx = vx;
				trackedPoints[i].vy = vy;
				trackedPoints[i].frameNum = frameNum;

				if (forceMatch) forceMatch = false;
			}
		}
	}

	//printf("drew keypoints\n");
	return drum;
}

void resetPoint(TrackedPoint *pt) {
	pt->x = 0;
	pt->y = 0;
	pt->vx = 0;
	pt->vy = 0;
	pt->frameNum = 0;
	pt->moving = false;
}

bool matching(KeyPoint keypoint, TrackedPoint trackedpoint) {
	long dist_sq = (keypoint.pt.x - trackedpoint.x)*(keypoint.pt.x - trackedpoint.x)
							 + (keypoint.pt.y - trackedpoint.y)*(keypoint.pt.y - trackedpoint.y);
	return dist_sq < SEARCH_RADIUS*SEARCH_RADIUS;
}
