/* main.cpp - get video, track drumsticks, play sound
 *
 * CS 453 Final Project - airdrums through video
 *
 * Ben Brown, Dylan Quenneville, Max Shashoua
 */

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "sound.h"
#include <unistd.h>
#include "tracker.h"

using namespace cv;
using namespace std;

const char *win = "video";
const char *sound1 = "snare.mp3";
const char *sound2 = "harm.mp3";

int main()
{
	long frameNum = 0;			// counter for frames passed in main video loop
	int cam = 0; 				// default camera
	int key;   				// keyboard input ID
	VideoCapture cap(cam);			// OpenCV VideoCapture for getting webcam video
	if (!cap.isOpened()) {			// check that video is working
		fprintf(stderr, "cannot open camera %d\n", cam);
		exit(1);
	}
	Mat frame;				// video capture frame
	namedWindow(win, 0);			// video display window
	int sound = 0;				// 0: no sound, 1: play sound 1, 2: play sound 2
	int middle;				// x-coordinate to draw screen dividing line
	cap >> frame;
	middle = frame.cols / 4;

	TrackedPoint trackedPoints[NUM_PTS];	// array of tracked drumstick ends
	for (int i = 0; i < 2; i++) {
		trackedPoints[i].x = 0;
		trackedPoints[i].y = 0;
		trackedPoints[i].vx = 0;
		trackedPoints[i].vy = 0;
		trackedPoints[i].frameNum = 0;
		trackedPoints[i].moving = false;
		trackedPoints[i].angle = 0;
	}

	//main run loop
	while (1) {
		frameNum += 1;			// increase frame count
		cap >> frame;			// capture next video frame
		if (!frame.empty()) {
			sound = track(&frame, trackedPoints, frameNum); // determine is sound should be played
			// draw line in center of frame to divide between 2 drum sounds
			for (int i = 0; i < frame.rows; i++) {
				for (int j = middle-1; j < middle+2; j++) {
					frame.at<Vec3b>(i, j)[0] = 150;
					frame.at<Vec3b>(i, j)[1] = 0;
					frame.at<Vec3b>(i, j)[2] = 0;
				}
			}
			imshow(win, frame);
			key = waitKey(5);
			if (sound == 1) {
				//printf("tracked\n");
				async_play(sound1);
			} else if (sound == 2) {
				async_play(sound2);
			}

			//debug functionality: play sounds on key press
			if (key == 106) { // J key
				async_play(sound1);
			} else if (key == 107) { // K key
				async_play(sound2);
			}
			//quit loop on other key press
			else if (key >= 0) {
				printf("%d\n", key);
				break;
			}
		}
	}
	imwrite("frame.png", frame);

	return 0;
}
