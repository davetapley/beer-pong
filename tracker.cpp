#include <opencv2/opencv.hpp>
#include "tracker.h"

void PongTable::MouseCallback( int event, int x, int y, int flags, void * param )
{
  PongTable * const table = (PongTable *) param;

  CvRect & box = table->m_boundingBox;

  switch( event ) {
    case CV_EVENT_MOUSEMOVE:

    if( table->m_draggingBox ) {
      box.width = x-box.x;
      box.height = y-box.y;
    }
    break;

    case CV_EVENT_LBUTTONDOWN:

    table->m_hasBox = false;
    table->m_draggingBox = true;

    box = cvRect( x, y, 0, 0 );
    break;

    case CV_EVENT_LBUTTONUP:

    table->m_draggingBox = false;
    table->m_hasBox = true;

    if( box.width < 0 ){
      box.x += box.width;
      box.width *= -1;
    }
    if( box.height < 0 ){
      box.y += box.height;
      box.height *= -1;
    }
    break;
  }
}

void PongTable::GetThresholdedImage( const IplImage & image, IplImage & out )
{
  IplImage * hsv = cvCreateImage( cvGetSize( &image ), 8, 3 );
  cvCvtColor(&image, hsv, CV_BGR2HSV);

  cvInRangeS(hsv, cvScalar(0, 0, 230), cvScalar(255, 255, 255), &out);

  cvReleaseImage(&hsv);
}


PongTable::PongTable() : m_draggingBox(false), m_hasBox(false) {}

void PongTable::DrawBounds( IplImage & image ) {

  if (!m_draggingBox && !m_hasBox) return;

  CvPoint tl;
  tl.x = m_boundingBox.x;
  tl.y = m_boundingBox.y;

  CvPoint tr;
  tr.x = m_boundingBox.x + m_boundingBox.width;
  tr.y = m_boundingBox.y;

  CvPoint bl;
  bl.x = tl.x;
  bl.y = tl.y + m_boundingBox.height;

  CvPoint br;
  br.x = tl.x + m_boundingBox.width;
  br.y = tl.y + m_boundingBox.height;

  cvLine( &image, tl, tr, CV_RGB( 0, 255, 0), 5 );
  cvLine( &image, tr, br, CV_RGB( 0, 255, 0), 5 );
  cvLine( &image, br, bl, CV_RGB( 0, 255, 0), 5 );
  cvLine( &image, bl, tl, CV_RGB( 0, 255, 0), 5 );
}

CvPoint PongTable::GetBallPosition( const IplImage & image ) {

  IplImage * threshed = cvCreateImage( cvGetSize( &image ), 8, 1 );
  GetThresholdedImage( image, *threshed );

  //
  // Calculate the moments to estimate the position of the ball
  CvMoments moments;
  cvMoments( threshed, &moments, 1 );

  // The actual moment values
  double moment10 = cvGetSpatialMoment(&moments, 1, 0);
  double moment01 = cvGetSpatialMoment(&moments, 0, 1);
  double area = cvGetCentralMoment(&moments, 0, 0);

  // Holding the last and current ball positions
  CvPoint out;
  out.x = moment10 / area;
  out.y = moment01 / area;

  return out;
}


//! Get a capture to track from
// \param file File to read from or NULL to use camera
CvCapture * GetCapture( const char * const file ) {
  CvCapture * capture = 0;

  if(file) {
    capture = cvCaptureFromAVI( file );
    std::cout << "Capture from " << file << std::endl;

  } else {
    capture = cvCaptureFromCAM( 0 );
    std::cout << "Using camera" << std::endl;
  }

  if( !capture ) {
    std::cout << "Couldn't read capture" << std::endl;
    return 0;
  }

  return capture;
}

int main( int argc, char * argv[] ) {

  // Get a capture
  const char * const captureFile = (argc > 1) ? argv[1] : 0;
  CvCapture * capture = GetCapture( captureFile );

  cvNamedWindow( "capture" );

  // Make a table
  PongTable table;

  cvSetMouseCallback ( "capture", PongTable::MouseCallback, &table );

  CvSize captureSize;
  captureSize.width = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH );
  captureSize.height = cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT );
  IplImage * overlay = cvCreateImage( captureSize, 8, 3 );

  // Track the ball
  while( true ) {
    IplImage * const frame = cvQueryFrame( capture );
    if( !frame ) break;

    // Draw the table
    table.DrawBounds( *overlay );

    // Highlight the ball
    CvPoint ball = table.GetBallPosition( *frame );
    cvCircle( overlay, ball, 20, CV_RGB( 255, 0, 0 ) );

    // Add the overlay
    cvAdd( frame, overlay, frame );
    cvSet( overlay, cvScalar( 0, 0, 0) );

    cvShowImage( "capture", frame );

    const int c = cvWaitKey( 10 );
    if( c != -1 ) break;
  }

  cvReleaseCapture( &capture );
  return 0;
}

