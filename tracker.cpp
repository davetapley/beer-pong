#include <opencv2/opencv.hpp>

int main(int argc, char * argv[]) {

  // Get a capture
  const char * const captureFile = (argc > 1) ? argv[1] : 0;

  CvCapture * capture = 0;

  if(captureFile) {
    capture = cvCaptureFromAVI(captureFile);
    std::cout << "Capture from " << captureFile << std::endl;

  } else {
    capture = cvCaptureFromCAM(0);
    std::cout << "Using camera" << std::endl;
  }

  if(!capture) {
    std::cout << "Couldn't read capture" << std::endl;
    return 1;
  }

  cvNamedWindow("capture");

  while(true) {
    IplImage * const frame = cvQueryFrame(capture);

    if(!frame) break;

    cvShowImage("capture", frame);

    const int c = cvWaitKey(10);
    if(c != -1) break;
  }

  cvReleaseCapture(&capture);
  return 0;
}

