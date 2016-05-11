#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "sound.h"
#include <unistd.h>
#include "tracker.h"

using namespace cv;
using namespace std;

int track(Mat frame, KeyPoint *keypoints) {
  // SimpleBlobDetector::Params params;
  // params.minThreshold = 10;
  // params.maxThreshold = 200;
  //
  // params.filterByArea = true;
  // params.minArea = 1500;
  //
  // params.filterByCircularity = true;
  // params.minCircularity = 0.1;
  //
  // SimpleBlobDetector detector(params);

    Mat im_with_keypoints;


  //copy more params code later

  SimpleBlobDetector detector;



  vector<KeyPoint> keypointsVector;
  detector.detect(frame, keypointsVector);


  drawKeypoints( frame, keypointsVector, frame, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );


  return 0;
}
