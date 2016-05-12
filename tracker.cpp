#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "sound.h"
#include <unistd.h>
#include "tracker.h"

using namespace cv;
using namespace std;

int track(Mat *frame, KeyPoint *keypoints) {
  Mat image;
  int ballz;
  ballz = 2;

  //Mat img = imread("ball1.jpg", 1); //testing image

  // blur img
  blur(*frame, image, Size(20, 20));

  // convert image to HSV
  cvtColor(image, image, COLOR_BGR2HSV);
  Mat lower_red_hue_range, upper_red_hue_range;
  inRange(image, Scalar(0, 100, 100), Scalar(10, 255, 255), lower_red_hue_range);
  inRange(image, Scalar(160, 100, 100), Scalar(179, 255 ,255), upper_red_hue_range);
  addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, image);



  //   Mat im_with_keypoints;
  // //copy more params code later
  //
  SimpleBlobDetector detector;
  //
  vector<KeyPoint> keypointsVector;
  detector.detect(image, keypointsVector);
  //
  //
  drawKeypoints( image, keypointsVector, *frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );


  return 0;
}
