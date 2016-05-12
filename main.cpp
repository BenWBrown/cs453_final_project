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
      SoundPlayer soundPlayer;
     int cam = 0; // default camera
     int key;
     VideoCapture cap(cam);
     if (!cap.isOpened()) {
 	fprintf(stderr, "cannot open camera %d\n", cam);
 	exit(1);
     }
     namedWindow(win, 0);

     Mat frame;
     KeyPoint keypoints[4];

     int sound;
     while (1) {
         cap >> frame;
         //TODO: convert to grayscale
         //sound = track(&frame, keypoints);
         imshow(win, frame);
         key = waitKey(30);
         if (key == 106) {
           soundPlayer.async_play("snare2.mp3");
         } else if (key == 107) {
           soundPlayer.async_play("snare.mp3");
         } else if (key >= 0) {
           printf("%d\n", key);
           break;
         }

     }
     imwrite("frame.png", frame);

     //TODO: this segfaults. idk why
     soundPlayer.clean();

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
