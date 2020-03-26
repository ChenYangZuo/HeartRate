#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace cv;
using namespace std;

extern double fps = 0;
extern int framesNum = 0;

bool extractInfo(vector<int>&array);
bool drawHistogram(vector<int>&array);
bool writeData(vector<int>&array);
bool filter(vector<int>&array);
vector<int> findPeak(vector<int>&array);
bool drawHistogramWithBeats(vector<int>&array,vector<int>&beats);

int main(){
    vector<int> beats;
    vector<int> arrays;
    extractInfo(arrays);
    filter(arrays);
    writeData(arrays);
    drawHistogram(arrays);
    beats = findPeak(arrays);
    drawHistogramWithBeats(arrays,beats);
    cout << "HeartRate:" << 60/(framesNum/fps)*beats.size() << endl;
    waitKey(0);
    return 0;
}

bool extractInfo(vector<int>&array){
    VideoCapture cap;
    cap.open("../src/test.mp4");
    if(!cap.isOpened()){
        cout << "ERROR 01!";
        return false;
    }
    fps = cap.get(CAP_PROP_FPS);
    cout << "FPS:" << fps << endl;
    framesNum = cap.get(CAP_PROP_FRAME_COUNT);
    cout << "FRAME:" << framesNum <<endl;

    while(1){
        Mat frame;
        bool rSucess = cap.read(frame);
        if(!rSucess){
            cout << "ERROR 02!" << endl;
            break;
        }
        else{
            vector<Mat> channels;
            cvtColor(frame,frame,COLOR_BGR2HSV);
            split(frame,channels);
            Mat R = channels.at(2);
            int height = frame.rows;
            int width = frame.cols;
            int all = 0;

            for(int i=0;i<height;i++)
                for(int j=0;j<width;j++)
                    all += frame.at<Vec3b>(i,j)[0];
            
            array.push_back(all);
        }
    }

    return true;
}

bool writeData(vector<int>&array){
    ofstream outFile;
    outFile.open("../data/data_hsv_filter.txt");
    for(vector<int>::size_type i=0;i<array.size();i++){
        outFile << array[i] << endl;
    }
    outFile.close();
    return true;
}

bool filter(vector<int>&array){
    for(vector<int>::size_type i=1;i!=array.size();i++){
        array[i] = array[i-1]*0.3 + array[i]*0.7;
    }
    return true;
}

vector<int> findPeak(vector<int>&array){
    int num = array.size() - 1;
    int diff_array[num-1];
    vector<int> beats;

    for(int i=0;i<num-1;i++){
        if(array[i + 1] - array[i]>0)
            diff_array[i] = 1;
        else if(array[i + 1] - array[i] < 0)
            diff_array[i] = -1;
        else
            diff_array[i] = 0;
    }

    for(int j=0;j<num-1;j++){
        if(diff_array[j]==0 && diff_array[j+1]>=0)
            diff_array[j]=1;
        if(diff_array[j]==0 && diff_array[j+1]<0)
            diff_array[j]=-1;
    }

    for(int k=0;k<num-2;k++){
        if(diff_array[k+1]-diff_array[k] == -2){
            beats.push_back(k+1);
        }            
    }

    return beats;
}

bool drawHistogramWithBeats(vector<int>&array,vector<int>&beats){
    int num = array.size()-1;
    Mat img = Mat(400,num+12,CV_8UC3,Scalar(0,0,0));
    Point pt1,pt2;
    float max=0;
    vector<int>::iterator iter;

    for(int k=0;k<num;k++){
        if(array[k]>max)
            max=array[k];
    }

    for(int i=0;i<num;i++){
        pt1.x = 6 + i;
        pt1.y = 394;
        pt2.x = 6 + i;
        pt2.y = 394 - array[i]/max*390;

        iter=find(beats.begin(),beats.end(),i);
        if(iter==beats.end()){
            line(img,pt1,pt2,CV_RGB(255,255,255),1,8,0);
        }
        else{
            line(img,pt1,pt2,CV_RGB(255,0,0),1,8,0);
        }
    }
   
    imwrite("../data/histogram_beats.bmp",img);
    imshow("Histogram_beats",img);
    return true;
}

bool drawHistogram(vector<int>&array){
    int num = array.size();
    Mat img = Mat(400,num+12,CV_8UC3,Scalar(0,0,0));
    Point pt1,pt2;
    float max=0;
    for(int k=0;k<num;k++){
        if(array[k]>max)
            max=array[k];
    }

    for(int i=0;i<num;i++){
        pt1.x = 6 + i;
        pt1.y = 394;
        pt2.x = 6 + i;
        pt2.y = 394 - array[i]/max*390;
        line(img,pt1,pt2,CV_RGB(255,255,255),1,8,0);
    }

    imwrite("../data/histogram_hsv_filter.bmp",img);
    imshow("Histogram_hsv_filter",img);
    waitKey(0);
    return true;
}