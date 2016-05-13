#include "opencv2/opencv.hpp"

#define NUM_PTS 2
#define HI_THRESH 35
#define LO_THRESH 10
#define FRAME_THRESH 20
#define SEARCH_RADIUS 100

typedef struct {
  int x;
  int y;
  float vx;
  float vy;
  long frameNum;
  bool moving;
} TrackedPoint;

int track(cv::Mat *frame, TrackedPoint trackedPoints[], long frameNum);
