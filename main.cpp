/* showvideo.cpp - display video in window
 *
 * CS 453 openCV demo
 */

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "sound.h"

using namespace cv;
using namespace std;

const char *win = "video";

int main()
{
    int cam = 0; // default camera
    int key;
    VideoCapture cap(cam);
    if (!cap.isOpened()) {
	fprintf(stderr, "cannot open camera %d\n", cam);
	exit(1);
    }

     namedWindow(win, 0);
    Mat frame;
    while (1) {
        cap >> frame;
        //DO STUFF WITH frame here
        imshow(win, frame);
        key = waitKey(30);
        if (key == 107) {
          play("snare.mp3");
        } else if (key >= 0) {
          break;
        }
    }
    imwrite("frame.png", frame);

    return 0;

  int key = waitKey(0);
    printf("%d\n", key);

}
