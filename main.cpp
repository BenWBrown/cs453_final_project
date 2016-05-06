/* showvideo.cpp - display video in window
 *
 * CS 453 openCV demo
 */

#include <stdio.h>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

const char *win = "video";

int main()
{
    int cam = 0; // default camera
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
        if (waitKey(30) >= 0) // wait up to 30 msec
	    break;
    }
    imwrite("frame.png", frame);

    return 0;
}
