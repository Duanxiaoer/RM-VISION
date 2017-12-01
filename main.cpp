#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

float  min(float,float);

float max(float, float);
int main() {

    Mat tar = imread("deng.jpg");
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    VideoCapture capture("2.mov");
    VideoWriter writer("处理.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25.0, Size(300, 400));

    while (capture.read(tar)){
        resize(tar,tar,Size(600,400),0,0);
        Mat imgThreshold = tar(CvRect(300,0,300,400));//识别
        Mat imgThreshold_2 = tar(CvRect(300,0,300,400));
        Mat imageContours = Mat::zeros(imgThreshold.size(),CV_8UC3);//用于存储轮廓
        Mat Contours = Mat::zeros(imgThreshold.size(),CV_8UC3);//轮廓的点集
        Point2f lightLeft[4],lightRight[4],armoured[8];//存储旋转矩形的是个定点坐标

        blur(imgThreshold,imgThreshold,Size(3,3));
        cvtColor(imgThreshold, imgThreshold, COLOR_BGR2GRAY);
        threshold(imgThreshold, imgThreshold, 250, 255, CV_THRESH_BINARY);

        findContours(imgThreshold,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);//找轮廓
        drawContours(imageContours,contours,-1,Scalar(255,255,255),1,8,hierarchy);//画轮廓

        int a=0;//已标记的矩形数
        for(int i=0;i<contours.size();i++){//找左侧灯条
            RotatedRect minRect_Left = minAreaRect(Mat(contours[i]));//将轮廓拟合为旋转矩形
            if(max(minRect_Left.size.height,minRect_Left.size.width) > min(minRect_Left.size.height,minRect_Left.size.width)*2){//灯条的长是宽的两倍以上
                for(int j=0;j<contours.size();j++){//找右侧灯条
                    RotatedRect minRect_Right = minAreaRect(Mat(contours[j]));//将轮廓拟合为旋转矩形
                    minRect_Left.points(lightLeft);//提取左侧旋转矩形的四个点
                    minRect_Right.points(lightRight);//提取右侧旋转矩形的四个点
                    if( (max(minRect_Right.size.height,minRect_Right.size.width) > min(minRect_Right.size.height,minRect_Right.size.width)*2 )//灯条的长是宽的两倍以上
                        &&(lightLeft[0].x < lightRight[0].x)//找右侧灯条
                        && minRect_Left.angle == minRect_Right.angle //左右灯条平行
                        && abs(minRect_Left.center.y - minRect_Right.center.y) < minRect_Left.size.height//左右灯条的中心在y轴上相差不大于左侧灯条的长度
                        /**下列的条件根据装甲和机器人的距离来设置*/
                        && abs(minRect_Left.center.x - minRect_Right.center.x) < minRect_Left.size.height*2//左右灯条的中心在x轴上相差不大于左侧灯条长度的2倍
                            ) {

                        /*返回两个center的两线的中点*/

                        if (!a){//只显示一个矩形
                            a = 1;
                            //标记
                            for (int k = 0; k < 4; ++k) {
                                line(imageContours,lightRight[k],lightRight[(k+1)%4],Scalar(0,0,255));//四个角点连成线，最终形成旋转的矩形。
                                line(imgThreshold_2,lightLeft[k],lightRight[(k+1)%4],Scalar(0,255,0));//原图上画
                            }

                        }
                    } else{
                        continue;
                    }



                }
            }
        }




        writer << imgThreshold_2;
        imshow("test",tar);
        imshow("lunkuo",imageContours);

        waitKey(1);
    }
}


float  max(float height, float width){
    return (height>width)?height:width;
}

float  min(float height, float width){
    return (height<width)?height:width;
}


