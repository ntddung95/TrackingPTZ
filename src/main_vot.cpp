#include <stdlib.h>

#include "kcf.h"
#include "vot.hpp"
#include "PTZController.h"
#include <thread>
#include <pthread.h>
#include <unistd.h>

using namespace cv;
using namespace std;
int x_mouse,y_mouse,width,heigh;
cv::Mat image;
bool start = false;
int deltaX,deltaY;
bool startVid = false;
float scale = 0.2;
int centerX = 1920/4, centerY = 1080/4;

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        centerX = 1920/4;centerY = 1080/4;
        x_mouse = x;
        y_mouse = y;
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;

    }

    if (event == EVENT_RBUTTONDOWN){
        x_mouse = x;
        centerX = x;
        centerY = y;
        y_mouse = y;
        std::cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << std::endl;
    }
    
}

bool stop = false;

void getFrame(char *ip){
    char buff[40];
    sprintf(buff,"rtsp://%s:554/1/h264major",ip);
    VideoCapture vid(buff);
    startVid = true;
    Mat tmp;
    while (!stop){
        
        vid >> tmp;
        resize(tmp,image,Size(),0.5,0.5);
    }
}



int main(int argc, char **argv){

    KCF_Tracker tracker;
    tracker.m_use_linearkernel = false; // true: Linear, false: Gaussian
    x_mouse = -1;y_mouse = -1;
    namedWindow("Tracking",WINDOW_NORMAL);

    //img = firts frame, initPos = initial position in the first frame
    setMouseCallback("Tracking",CallBackFunc, NULL); 
    BBox_c bb;
    
    int centerBX,centerBY;
    char buff[20];
    sprintf(buff,"%s:8999",argv[1]);
    PTZController pz(buff);
    thread getFr(getFrame,argv[1]);
    usleep(5000000);
    //thread control(Control,pz);
    while(true){

        if ((x_mouse > 0) && (y_mouse > 0)){
            Rect rect(x_mouse-15,y_mouse-15,30,30);
            tracker.init(image, rect); // if clicking mouse, tracker is initialized then reset mouse coordinate
            x_mouse = -1;
            y_mouse = -1;
            start = true;
        }

        if (start){
            tracker.track(image);
            bb = tracker.getBBox();
            
            centerBX = bb.cx;
            centerBY = bb.cy;
            deltaX = centerBX - centerX;
            deltaY = centerY - centerBY;
            scale = sqrt(deltaX*deltaX + deltaY*deltaY) * 0.9/ 450;
             scale = 0.5;
            if (abs(sqrt(deltaX*deltaX + deltaY*deltaY)) < 50)  
                scale = 0.2;
            //scale = 0.001;
            if ((abs(deltaY) < 20) && (abs(deltaX) < 20))
                pz.stop();           
            else if (abs(deltaY) > abs(deltaX)){
                cout << ((float)deltaX*scale/abs(deltaY)) << ":::::::" << ((deltaY > 0)?scale:(-scale)) << endl;
                pz.move((float)deltaX*scale/abs(deltaY),(deltaY > 0)?scale:(-scale));
               
            }
            else{
                cout << ((deltaX > 0)?scale:(-scale)) <<":::::::"<< ((float)deltaY*scale/abs(deltaX)) << endl;
                pz.move((deltaX > 0)?scale:(-scale),(float)deltaY*scale/abs(deltaX));
            }
            
            label:
            rectangle(image,cv::Rect(bb.cx - bb.w/2., bb.cy - bb.h/2., bb.w, bb.h),Scalar(0,0,255),1);

        }

        cv::rectangle(image,Point(centerX-2,centerY-2),Point(centerX +2,centerY+2),Scalar(255,0,0),1);

        imshow("Tracking",image);
        if (waitKey(20) == 27){
            imwrite("xxx.bmp",image);
            stop = true;
            getFr.join();
            pz.stop();
            break;
        }
    }

    return EXIT_SUCCESS;
}