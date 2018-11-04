#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <unistd.h>

#include "CoolingFanControl.hpp"
#include "wiringPi.h"
 
using namespace std;
using namespace cv;

#define BLOCK_COUNT 10


void *imageProcessingThread(void *arg) 
{
    raspicam::RaspiCam_Cv Camera;
 
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3);
    Camera.set( CV_CAP_PROP_FRAME_WIDTH, 640 );
    Camera.set( CV_CAP_PROP_FRAME_HEIGHT, 480 );
    
 
    if (!Camera.open()) 
    {
        printf("Error opening the camera\n");
        return NULL;
    }

    cv::VideoWriter outputVideo;
    cv::Size frameSize(640, 480);
    double fps = 2.0;

    outputVideo.open("./output.avi", cv::VideoWriter::fourcc('M','J','P','G'), fps, frameSize, true);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << "output.avi" << endl;
        return NULL;
    }
    
    Mat frame_prev;
    Mat frame, frame_gray;
	Mat mask;


    int h = frame.size().height / BLOCK_COUNT;
	int w = frame.size().width / BLOCK_COUNT;
    vector<Point2f> featurePrev, featureCurr;

    TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1, 1.0);
	
    vector<uchar> isFound;
	vector<float> err;
	Size winSize(21, 21);

	

    bool isFirst = true;

    while(1) 
    {
        
        Camera.grab();
        Camera.retrieve (frame);
        flip(frame, frame, 0);
        cvtColor(frame, frame_gray, CV_RGB2GRAY);

        if(isFirst == false) {
            

            calcOpticalFlowPyrLK(frame_prev, frame_gray, featurePrev, featureCurr, isFound, err, winSize, 3, termcrit, 0, 0.1);
            
            //goodFeaturesToTrack(frame_prev, featurePrev, 10, 0.1, 10, mask, 3, false, 0.04);
            
            //calcOpticalFlowPyrLK(frame_prev, frame_gray, featurePrev, featureNext, isFound, err, winSize, 3, termcrit, 0, 0.01);

            
            for (int i=0; i<featurePrev.size(); i++) {
                //float flow_x = featureCurr[i].x - featurePrev[i].x;
                //float flow_y = featureCurr[i].y - featurePrev[i].y;
                line(frame, featurePrev[i], featureCurr[i], Scalar(0, 0, 255), 1, 1, 0);
                //sum_x_slow += flow_x;
                //sum_y_slow += flow_y;
                //flow_cnt_slow ++;
            }

            featurePrev = featureCurr;
        }
        else 
        {
            isFirst = false;
            featurePrev.clear();
            for (int i=0; i<BLOCK_COUNT; i++) {
                    for (int j=0; j<BLOCK_COUNT; j++) {
                        featurePrev.push_back(Point2f( i * w + (w/2), j * h + (h/2) ));
                    }
            }
            featureCurr = featurePrev;
            
        }

        frame_prev = frame_gray.clone();

        outputVideo.write(frame);
        imshow("frame viewer", frame);
        if ( waitKey((int)(1000.0 / fps)) == 27 ) break; 
 
    }

 RETURN:

    Camera.release();
}

int main ( int argc,char **argv ) 
{
 
    pthread_t imageProcessing;

    if(pthread_create(&imageProcessing, NULL, imageProcessingThread, NULL)) 
    {
		printf("Cannot creating thread - imageProcessing\n");
		return -1;
	}

    CoolingFanControl fanController;
    printf("CPU temperature : %lf\n", fanController.getCPUtemperature());

    while(1) {
        sleep(1);
    }
    
}