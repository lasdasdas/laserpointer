
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include<fstream>
#include<cstdlib>
#include <unistd.h>


//-------------------------------GLOBAL VARIABLES
CvPoint laserpointval = CvPoint(0,0);
CvPoint* laserpoint = &laserpointval;
using namespace cv;
const char* serial = "/dev/ttyUSB1";

//------------------------------ PROGRAM FUNCIONS
void cvSearchLaserDot(IplImage* image, CvPoint* dot) {
  double minVal, maxVal , maxValthresh;
  maxValthresh=250;//laser intensity threshhold

  // Select's the image's first channel. Assuming this is an RGB image, the
  // Red channel will be selected.
  cvSetImageCOI(image, 1);

  // Looks for the biggest-valued pixel on the above selected channel.
  cvMinMaxLoc(image, &minVal, &maxVal, NULL, dot, NULL);
  //If the points are below the thresh just point at 50 50 px
  if ( maxVal<maxValthresh){
    dot->x =200;
    dot->y=200;
  }
  // Resets the channel selection performed in the previous call to

  // cvSetImageROI().
  cvSetImageCOI(image, 0);
}

Mat redFilter(const Mat& src)
{
  assert(src.type() == CV_8UC3);

  Mat redOnly;
  inRange(src, Scalar(0, 0, 0), Scalar(0, 0, 255), redOnly);

  return redOnly;
}

//--------------------------------MAIN

int main()
{

  //Data Structure to store cam.
  CvCapture* cap=cvCreateCameraCapture(0);
  //Image variable to store frame
  IplImage* frame;
  //Window to show livefeed
  cvNamedWindow("LiveFeed",CV_WINDOW_AUTOSIZE);
  //Arduino link

  // FILE *file;
  // file = fopen(serial,"w");  //Opening device file
  // usleep(2000000);

  while(1){
    //Load the next frame
    frame=cvQueryFrame(cap);
    //If frame is not loaded break from the loop
    if(!frame)
    printf("\n no");;

//--------------------------------LASER TRACKER
    cvSearchLaserDot(frame , laserpoint) ;

//--------------------------------PING PONG DETECTION

    //Max min HSV threshes
    CvScalar hsv_min = cvScalar(4, 117, 184, 0);
    CvScalar hsv_max = cvScalar(47, 228, 260, 0);
    //Some declarations
    IplImage * hsv_frame = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 3);
    IplImage* thresholded = cvCreateImage(cvSize(640,480), IPL_DEPTH_8U, 1);
    //Converting to hsv and filtering
    cvCvtColor(frame, hsv_frame, CV_BGR2HSV);
    cvInRangeS(hsv_frame, hsv_min, hsv_max, thresholded);
    CvMemStorage* storage = cvCreateMemStorage(0);
    //Smooth and detect w/ hougCircles
    cvSmooth( thresholded, thresholded, CV_GAUSSIAN, 9, 9 );
    CvSeq* circles = cvHoughCircles(thresholded, storage, CV_HOUGH_GRADIENT, 2,
      thresholded->height/16, 50, 25, 5, 200);

//-----------------------------CICLE DRAWING----------
      //Circle on the ping pong
      if (circles->total ==1)
      {
        float* p = (float*)cvGetSeqElem( circles, 1 );
        cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])),
        3, CV_RGB(0,255,0), -1, 8, 0 );
        cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])),
        cvRound(p[2]), CV_RGB(255,0,0), 3, 8, 0 );
      }
      //Laser cricle drawing
      cvCircle( frame, cvPoint(laserpoint->x ,laserpoint->y) ,  //plot circle
      5, CV_RGB(0,255,0), 2, 8, 0 );

//--------------------------LASER CONTROL TODO

      if (circles->total ==1){
        float* p = (float*)cvGetSeqElem( circles, 1 );
        std::cout <<"Ball Pointer" << p[0] << '\t' <<p[1]<< '\t'<<p[2]<< '\n';
      }

//-------------------------PRINTING THE FEED
      //Show the feed
      cvShowImage( "LiveFeed", frame );
      //Escape Sequence
      char c=cvWaitKey(33);
//-------------------------CLEANING UP
cvReleaseImage(&hsv_frame);
cvReleaseImage(&thresholded);

      if(c==27)  // Esc(ASCII is 27)
      break;
    }
    //CleanUp
    cvReleaseCapture(&cap);
    cvDestroyAllWindows();
  }
