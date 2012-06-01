//! Keep the bounding box of a table
class PongTable
{
  public:
    static void MouseCallback( int event, int x, int y, int flags, void * table );
    static void GetThresholdedImage( const IplImage & image, IplImage & out );

    PongTable();

    void DrawBounds( IplImage & image );
    CvPoint GetBallPosition( const IplImage & image );

  private:
    CvRect m_boundingBox;

    bool m_draggingBox;
    bool m_hasBox;
};

