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

  cvLine( &image, tl, tr, cvScalar( 0, 255, 0), 5 );
  cvLine( &image, tr, br, cvScalar( 0, 255, 0), 5 );
  cvLine( &image, br, bl, cvScalar( 0, 255, 0), 5 );
  cvLine( &image, bl, tl, cvScalar( 0, 255, 0), 5 );
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
  IplImage * tableBounds = cvCreateImage( captureSize, 8, 3 );

  // Track the ball
  while( true ) {
    IplImage * const frame = cvQueryFrame( capture );
    if( !frame ) break;

    table.DrawBounds( *tableBounds );
    cvAdd( frame, tableBounds, frame );
    cvSet(tableBounds, cvScalar(0,0,0));

    cvShowImage( "capture", frame );

    const int c = cvWaitKey( 10 );
    if( c != -1 ) break;
  }

  cvReleaseCapture( &capture );
  return 0;
}

