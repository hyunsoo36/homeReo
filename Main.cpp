#include <iostream>
#include <stdio.h>
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ctime>

#include "CoolingFanControl.hpp"
#include "wiringPi.h"
#include "wiringSerial.h"

using namespace std;
using namespace cv;

#define FRAME_SIZE_WIDTH    640    // max : 2592
#define FRAME_SIZE_HEIGHT   360    // max : 1944
#define BLOCK_COUNT_WIDTH   ((int)(FRAME_SIZE_WIDTH/20.0))
#define BLOCK_COUNT_HEIGHT  ((int)(FRAME_SIZE_HEIGHT/20.0))


#define FPS_DEFAULT 5
#define SHADOW_FRAME_COUNT (FPS_DEFAULT * 5) 
#define HS_DEBUG

int isRecording = 0;

void getFileNameFromDate(char *buf, int len);

void *imageProcessingThread(void *arg) 
{
    raspicam::RaspiCam_Cv Camera;
 
    Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
    Camera.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_SIZE_WIDTH);
    Camera.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_SIZE_HEIGHT);
    
 
    if (!Camera.open()) 
    {
        printf("Error opening the camera\n");
        return NULL;
    }

    cv::VideoWriter outputVideo;
    cv::Size frameSize(FRAME_SIZE_WIDTH, FRAME_SIZE_HEIGHT);
    double fps = FPS_DEFAULT;
    
    Mat frame_prev;
    Mat frame, frame_gray;
	Mat mask;


    int w = FRAME_SIZE_WIDTH / BLOCK_COUNT_WIDTH;
	int h = FRAME_SIZE_HEIGHT / BLOCK_COUNT_HEIGHT;
    printf("h = %d, w = %d\n", h , w);
    vector<Point2f> featurePrev, featureCurr;

    TermCriteria termcrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 1, 1.0);
	
    vector<uchar> isFound;
	vector<float> err;
	Size winSize(7, 7);

	float flowMap[BLOCK_COUNT_WIDTH][BLOCK_COUNT_HEIGHT] = {0, };
    float flowMap_hori[BLOCK_COUNT_WIDTH] = {0, };
    float flowMap_vert[BLOCK_COUNT_HEIGHT] = {0, };

    bool isFirst = true;

    clock_t timeStart = millis(), timeEnd = millis(), timeElapsed = 1;
    int delay = 0;
    char printBuf[100];

    int movingFlag = 0;

    while(1) 
    {
        timeStart = millis();

        Camera.grab();
        Camera.retrieve (frame);
        //resize(frame, frame, cv::Size( FRAME_SIZE_WIDTH, FRAME_SIZE_HEIGHT ));
        

        flip(frame, frame, -1);
        cvtColor(frame, frame_gray, CV_RGB2GRAY);

        if(isFirst == false) {
            

            calcOpticalFlowPyrLK(frame_prev, frame_gray, featurePrev, featureCurr, isFound, err, winSize, 3, termcrit, 0, 0.05);
            
            //goodFeaturesToTrack(frame_gray, featurePrev, 100, 0.05, 10);

            float flowSum = 0.0f, flowAverage = 0.0f, flowMin = 9999.0f, flowMax = 0.0f;
            int cntMovingBlock = 0;
            for (int i=0; i<BLOCK_COUNT_WIDTH; i++) {
                    for (int j=0; j<BLOCK_COUNT_HEIGHT; j++) {
                        int index1D = i * BLOCK_COUNT_HEIGHT + j;

#ifdef HS_DEBUG         
                        line(frame, featurePrev[index1D], featureCurr[index1D], Scalar(0, 0, 255), 1, 1, 0);
#endif
                        //circle(frame, featurePrev[index1D], 5, Scalar(0, 0, 255), 1);
                        float flow_x = featureCurr[index1D].x - featurePrev[index1D].x;
                        float flow_y = featureCurr[index1D].y - featurePrev[index1D].y;
                        flowMap[i][j] = sqrt(flow_x * flow_x + flow_y * flow_y);
                        flowSum += flowMap[i][j];
                        if(flowMap[i][j] < flowMin) flowMin = flowMap[i][j];
                        if(flowMap[i][j] > flowMax) flowMax = flowMap[i][j];

                        if(flowMap[i][j] > 5) {
                            cntMovingBlock++;
                        }
                        // camshift 적용 // fps가 낮아서 트래킹에 적합하지 않은 듯
                        // https://webnautes.tistory.com/945
                    }
            }

            if(flowMin == 0.0f && flowMax > 5.0f && cntMovingBlock > 4) {
                movingFlag = SHADOW_FRAME_COUNT;
            }
            
            flowAverage = flowSum / (BLOCK_COUNT_WIDTH * BLOCK_COUNT_HEIGHT);
            float flowBias = flowAverage * (BLOCK_COUNT_WIDTH * BLOCK_COUNT_HEIGHT);
            snprintf(printBuf, 100, "sum : %4.4f, average : %3.4f, min : %3.4f, max : %3.4f, \n", flowSum, flowAverage, flowMin, flowMax);
            


            
        }
        else 
        {
            isFirst = false;
            featurePrev.clear();
            for (int i=0; i<BLOCK_COUNT_WIDTH; i++) {
                    for (int j=0; j<BLOCK_COUNT_HEIGHT; j++) {
                        featurePrev.push_back(Point2f( i * w + (w/2), j * h + (h/2) ));
                    }
            }
            
            
        }

        frame_prev = frame_gray.clone();


        if(movingFlag > 0) {
            char fpsStr[16];
            snprintf(fpsStr, 16, "%2.2f fps", 1000.0 / (float)(timeElapsed + delay));
            putText(frame, fpsStr, Point(0, 12), 2, 0.5, Scalar(0, 255, 0));
            putText(frame, printBuf, Point(0, 24), 2, 0.5, Scalar(0, 255, 0));
            //printf("%s", fpsStr);
        
            movingFlag --;
            circle(frame, Point(FRAME_SIZE_WIDTH-20, 20), 6, Scalar(0, 0, 255), -1);
            if(outputVideo.isOpened() == false) {
                char fileName[64];
                getFileNameFromDate(fileName, 64);
                outputVideo.open(fileName, cv::VideoWriter::fourcc('F','M','P','4'), fps, frameSize, true);
                if(outputVideo.isOpened())  printf("Vedio file is opened. (%s)\n", fileName);
            }
            outputVideo.write(frame);
            if(movingFlag == 0) {
                outputVideo.release();
            }
        }
#ifdef HS_DEBUG
        imshow("frame viewer", frame);
#endif


        timeEnd = millis();
        timeElapsed = timeEnd - timeStart;
        delay = ((int)(1000.0 / fps) - (int)timeElapsed);
        if(delay <= 0) delay = 1;
        //printf("timeElapsed : %d, delay : %d\n", timeElapsed, delay);
        if ( waitKey(delay) == 27 ) break; 
 
    }

 RETURN:

    Camera.release();
}

void getFileNameFromDate(char *buf, int len) {
    time_t curr_time;
    struct tm *curr_tm;
    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);
    snprintf(buf, len, "%02d%02d%02d_%02d%02d%02d_homeReo.avi", curr_tm->tm_year-100, curr_tm->tm_mon + 1, curr_tm->tm_mday
                            , curr_tm->tm_hour, curr_tm->tm_min, curr_tm->tm_sec);
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