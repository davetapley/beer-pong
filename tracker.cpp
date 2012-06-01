#include <opencv2/opencv.hpp>

//! Get a capture to track from
// \param file File to read from or NULL to use camera
CvCapture * GetCapture(const char * const file) {
  CvCapture * capture = 0;

  if(file) {
    capture = cvCaptureFromAVI(file);
    std::cout << "Capture from " << file << std::endl;

  } else {
    capture = cvCaptureFromCAM(0);
    std::cout << "Using camera" << std::endl;
  }

  if(!capture) {
    std::cout << "Couldn't read capture" << std::endl;
    return 0;
  }

  return capture;
}

int main(int argc, char * argv[]) {

  // Get a capture
  const char * const captureFile = (argc > 1) ? argv[1] : 0;
  CvCapture * capture = GetCapture(captureFile);

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

