#ifndef ANGLESOLVER_H
#define ANGLESOLVER_H

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "common.h"

class AngleSolver
{
public:
    AngleSolver();

    //初始化角度解算的参数
    bool init();

    //利用小孔成像原理进行单点解算，只能得到相对于摄像头中心的转角,没有深度信息，仅需一个点
    void onePointSolution(const std::vector<cv::Point2f> centerPoint);

    //利用solvePNP进行位置解算，包含深度信息,可以自定义中心,需要四个点
    std::vector<double> p4pSolution(const cv::Rect &rect, int objectType);

    //重力补偿
/*
    //air friction is considered
    float BulletModel(float x, float v, float angle);

    //x:distance , y: height
    float GetPitch(float x, float y, float v);

    void Transform(cv::Point3f &postion, float &pitch, float &yaw);
*/
private:

    cv::Mat CameraIntrinsicMatrix; //相机内参矩阵
    cv::Mat DistortionCoefficient; //相机畸变系数
    //单位为mm
    std::vector<cv::Point3f> POINT_3D_OF_ARMOR_BIG;
    std::vector<cv::Point3f> POINT_3D_OF_ARMOR_SMALL;
    float Y_DISTANCE_BETWEEN_GUN_AND_CAM;//如果摄像头在枪管的上面，这个变量为正
    float Z_DISTANCE_BETWEEN_MUZZLE_AND_CAM;//如果摄像头在枪口的后面，这个变量为正


    float _euclideanDistance;//x坐标下的差值，y坐标下的差值，欧氏距离
    float yawErr = 0;
    float pitchErr = 0; //yaw轴的误差，pitch轴的误差

    const float GRAVITY = 9.78;

};

#endif // ANGLESOLVER_H
