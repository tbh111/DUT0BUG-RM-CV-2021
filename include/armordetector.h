#ifndef ARMORDETECTOR_H
#define ARMORDETECTOR_H
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <QString>
#include "common.h"

class ArmorBox
{
public:
    ArmorBox() = default;
    ArmorBox(const cv::Rect &t_rect, const cv::RotatedRect &t_rotate_rect, const cv::Mat &t_img, int t_armor_type):
        armor_rect(t_rect), armor_rotated_rect(t_rotate_rect), armor_img(t_img), armor_type(t_armor_type), isValid(true)
    {
//        armor_rect.points(armor_points);
    }
    static bool areaComparator(const ArmorBox &a, const ArmorBox &b);
    static bool ratioComparator(const ArmorBox &a, const ArmorBox &b);
//    cv::Point2f armor_points[4];
    cv::Rect armor_rect;
    cv::RotatedRect armor_rotated_rect;
    cv::Mat armor_img;
    bool isValid = false;
    int armor_type = common_utils::ARMOR_SMALL;
};

class ArmorDetector
{
public:
    ArmorDetector();
    bool run(const cv::Mat &src_img, cv::Mat &dst_img, ArmorBox &armor_target, int &self_color);
private:
    void preProcess(const cv::Mat &src_img, cv::Mat &bin_img, int &self_color);
    void findLightbars(const cv::Mat &bin_img, cv::Mat &dst_img);
    void matchArmorBoxes(const cv::Mat &src_img, cv::Mat &dst_img);
    bool getTarget();
    cv::Mat warpTransform(const cv::Mat &src_img, cv::RotatedRect rect, double height, double width);
    cv::Point2i getCenterPoint(cv::Rect rect);
    cv::Mat bin_img;
    std::vector<cv::RotatedRect> lightbars;
    std::vector<ArmorBox> armor_boxes;
    ArmorBox armor_target;

    int frame_count = 0;
};

#endif // ARMORDETECTOR_H
