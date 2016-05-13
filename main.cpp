/* showvideo.cpp - display video in window
 *
 * CS 453 openCV demo
 */

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "sound.h"
#include <unistd.h>
#include "tracker.h"

using namespace cv;
using namespace std;

const char *win = "video";

int main()
{
	long frameNum = 0;
	int cam = 0; // default camera
	int key;
	VideoCapture cap(cam);
	if (!cap.isOpened()) {
		fprintf(stderr, "cannot open camera %d\n", cam);
		exit(1);
	}
	namedWindow(win, 0);

	Mat frame;
	TrackedPoint trackedPoints[NUM_PTS];
	for (int i = 0; i < 2; i++) {
		trackedPoints[i].x = 0;
		trackedPoints[i].y = 0;
		trackedPoints[i].vx = 0;
		trackedPoints[i].vy = 0;
		trackedPoints[i].frameNum = 0;
		trackedPoints[i].moving = false;
		trackedPoints[i].angle = 0;
	}

	int sound = 0, middle;
	cap >> frame;
	middle = frame.cols / 4;

	while (1) {
		frameNum += 1;
		cap >> frame;
		if (!frame.empty()) {
			//TODO: convert to grayscale
			sound = track(&frame, trackedPoints, frameNum);
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
				printf("tracked\n");
				//printf("\a");

				async_play("snare.mp3");
				//  printf("\n\n\n\n\n\nPLAY SOUND\n\n\n\n\n\n\n\n");
				//  printf("\a");
			} else if (sound == 2) {

        async_play("harm.mp3");
      }
			if (key == 106) {
				async_play("snare.mp3");
			} else if (key == 107) {
				async_play("harm.mp3");
			} else if (key >= 0) {
				printf("%d\n", key);
				break;
			}
		}

	}
	imwrite("frame.png", frame);

	//TODO: this segfaults. idk why
	//soundPlayer.clean();

	return 0;

 }

// int main() {
//   printf("playing\n");
//   async_play("snare.mp3");
//   usleep(500000);
//   async_play("snare2.mp3");
//   usleep(500000);
//   async_play("snare3.mp3");
//   usleep(500000);
//   async_play("snare4.mp3");
//
//   usleep(3000000);
//   printf("done sleeping\n");
//   return 0;
// }
