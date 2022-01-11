#ifndef ENERGYDETECTOR_H
#define ENERGYDETECTOR_H

#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>
#include <vector>
#include "common.h"

using namespace std;

using namespace cv;
using namespace cv::ml;

vector<cv::Point2f> ARMOR;
int16_t yawErr,pitchErr;

class EnergyBox
{
public:
    EnergyBox() = default;
    EnergyBox(const cv::Rect &t_rect):
        armor_rect(t_rect), isValid(true){}
    cv::Rect armor_rect;
    bool isValid = false;
    int armor_type = common_utils::ARMOR_ENERGY;

};

class EnergyDetector
{
public:
    EnergyDetector();
    bool run(const cv::Mat &src_img, cv::Mat &dst_img, EnergyBox &target, int &self_color);
private:
    double getDistance(Point A,Point B);
    std::vector<float> stander(Mat im);
    cv::Mat get(Mat input);

    cv::Ptr<cv::ml::SVM> svm;

};




#endif // ENERGYDETECTOR_H
