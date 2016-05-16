/* tracker.cpp - track drumstick locations and movement from previous frame
 *
 * CS 453 Final Project - airdrums through video
 *
 * Ben Brown, Dylan Quenneville, Max Shashoua
 *
 * Based on code by Satya Mallick
 * http://www.learningopencv.com/blob-detection-using-opencv-python-c/
 *
 */

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

// track motion of drumsticks in new frame from tracked points in previous frame
int track(Mat *frame, TrackedPoint trackedPoints[], long frameNum) {
	Mat image, big;
	int drum = 0;

	// Blur image
	blur(*frame, big, Size(75, 75));

	// Resize image for faster tracking
	resize(big, image, Size(round(0.5*big.cols), round(0.5*big.rows)), 0.5, 0.5);
	resize(*frame, *frame, Size(round(0.5*big.cols), round(0.5*big.rows)), 0.5, 0.5);

	// Convert image to HSV
	cvtColor(image, image, COLOR_BGR2HSV);
	Mat lower_red_hue_range, upper_red_hue_range;
	inRange(image, Scalar(0, 100, 100), Scalar(10, 255, 255), lower_red_hue_range);
	inRange(image, Scalar(160, 100, 100), Scalar(179, 255 ,255), upper_red_hue_range);
	// Create black-and-white image isolating orange regions
	addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, image);

	// Invert image for use by SimpleBlobDetector
	image.convertTo(image, -1, -1, 255);

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

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

	// Set up vector to store blob detector results
	vector<KeyPoint> keypointsVector;
	detector.detect(image, keypointsVector);

	// Draw detected points on frame for user viewing
	drawKeypoints(*frame, keypointsVector, *frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );

	// Debug output
	// if (keypointsVector.size() > 0)
	// 	printf("\nframeNum: %lu  keyPoints:", frameNum);
	// for (int i=0;i<keypointsVector.size();i++) {
	// 	printf("kp: ((%f, %f))\n",keypointsVector[i].pt.x, keypointsVector[i].pt.y);
	// }

	// Check if previously seen blob hasn't been seen in over FRAME_THRESH frames
	// and reset it if so (clean up room for a significantly displace drumstick or
	// new object)
	for (int i = 0; i < NUM_PTS; i++) {
		if ((trackedPoints[i].frameNum != 0) && (frameNum - trackedPoints[i].frameNum > FRAME_THRESH)) {
			// printf("frameNum: %lu\n", frameNum);
			// printf("trackedPoints[i].frameNum : %lu\n", trackedPoints[i].frameNum );
			// printf("about to reset\n x: %d\n", trackedPoints[i].x);
			resetPoint(&trackedPoints[i]);
			//printf("x: %d\n", trackedPoints[i].x);
		}
	}

	vector<int> freeTrackingPoints;
	set<int> usedKeypoints;
	vector<int> freeKeyPoints;

	for (int i = 0; i < NUM_PTS; i++) {
		if (trackedPoints[i].x == 0 && trackedPoints[i].y == 0) {
			freeTrackingPoints.push_back(i);
		}
	}

	if ((trackedPoints[0].x == trackedPoints[1].x) && (trackedPoints[0].y == trackedPoints[1].y)) {
		resetPoint(&trackedPoints[1]);
	}

	// Match keypoints to tracked points
	for (int i = 0; i < NUM_PTS; i++) {
		for (int j = 0; j < (long long) keypointsVector.size(); j++) {
			if (matching(keypointsVector[j], trackedPoints[i])) {
				usedKeypoints.insert(j);
				// calculate velocity based on previous frame's tracked keypoints
				float vx, vy, velocitySquared;
				vx = ((float) (trackedPoints[i].x - keypointsVector[j].pt.x)) / (frameNum - trackedPoints[i].frameNum);
				vy = ((float) (trackedPoints[i].y - keypointsVector[j].pt.y)) / (frameNum - trackedPoints[i].frameNum);
				velocitySquared = vx*vx + vy*vy;

				float angle = atan2(vy, vx);

				// Check if need to play drum sound
				// (Either went from going fast to going slow or significant change in angle)
				if (trackedPoints[i].moving && (velocitySquared < LO_THRESH*LO_THRESH) ) {
					if (trackedPoints[i].x < frame->cols/2) {
						//printf("drum = 1");
						drum = 1;
					} else {
						//printf("drum = 2");
						drum = 2;
					}
				}

				//printf("i: %d velocity: %f\n",i,  sqrt(velocitySquared));


				// Update trackedPoints based on new locations
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
				trackedPoints[i].angle = angle;
			}
		}
	}

	// Create freeKeyPoints vector
	// Used to find new drumstick ends when old ends are no longer found
	for (int j = 0; j < (long long) keypointsVector.size(); j++) {
		if (usedKeypoints.find(j) == usedKeypoints.end())
			freeKeyPoints.push_back(j);
	}

	// printf("size of freeTrackingPoints: %d\n", freeTrackingPoints.size());
	// printf("size of freeKeyPoints: %d\n", freeKeyPoints.size());

	long long trackingPointsSize = (long long) freeTrackingPoints.size();
	long long keyPointsSize = (long long) freeKeyPoints.size();

	for (int i = 0; (i < trackingPointsSize) && (i < keyPointsSize); i++) {
		trackedPoints[freeTrackingPoints[i]].x = keypointsVector[freeKeyPoints[i]].pt.x;
		trackedPoints[freeTrackingPoints[i]].y = keypointsVector[freeKeyPoints[i]].pt.y;
		trackedPoints[freeTrackingPoints[i]].frameNum = 0;
	}
	return drum;		// Return drum ID (0: no play, 1: play sound #1, 2: play sound #2)
}

// Reset tracked point value
void resetPoint(TrackedPoint *pt) {
	pt->x = 0;
	pt->y = 0;
	pt->vx = 0;
	pt->vy = 0;
	pt->frameNum = 0;
	pt->moving = false;
	pt->angle = 0;
}

// Determine if new keypoint is within range to be matched with previously tracked point
bool matching(KeyPoint keypoint, TrackedPoint trackedpoint) {
	long dist_sq = (keypoint.pt.x - trackedpoint.x)*(keypoint.pt.x - trackedpoint.x)
							 + (keypoint.pt.y - trackedpoint.y)*(keypoint.pt.y - trackedpoint.y);
	return dist_sq < SEARCH_RADIUS*SEARCH_RADIUS;
}
