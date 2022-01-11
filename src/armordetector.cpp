#include "armordetector.h"

bool ArmorBox::areaComparator(const ArmorBox &a, const ArmorBox &b)
{
    return a.armor_rotated_rect.size.area() > b.armor_rotated_rect.size.area();
}

//bool ArmorBox::ratioComparator(const ArmorBox &a, const ArmorBox &b)
//{
//    return fabs(a.armor_rect.size.aspectRatio()-1) < fabs(b.armor_rect.size.aspectRatio()-1);
//}

ArmorDetector::ArmorDetector()
{

}

bool ArmorDetector::run(const cv::Mat &src_img, cv::Mat &dst_img, ArmorBox &target, int &self_color)
{
    preProcess(src_img, bin_img, self_color);
    findLightbars(bin_img, dst_img);
    matchArmorBoxes(src_img, dst_img);
//    dst_img = bin_img.clone();
    if(getTarget())
    {
        target = armor_target;

        lightbars.clear();
        armor_boxes.clear();
        cv::circle(dst_img, target.armor_rotated_rect.center, 7, cv::Scalar(255,0,255), -1);

        return true;
    }
    else
    {
        lightbars.clear();
        armor_boxes.clear();
        return false;
    }
}

void ArmorDetector::preProcess(const cv::Mat &src_img, cv::Mat &bin_img, int &self_color)
{
    cv::Mat img = src_img.clone();
    cv::Mat color_channel, color_channel_binary;
    std::vector<cv::Mat> channels;
    cv::split(img, channels);
    if(self_color == common_utils::SELF_RED)
    {
        color_channel = channels[0] - channels[2]; // enemy=blue
    }
    else
    {
        color_channel = channels[2] - channels[0]; // enemy=red
    }
//    cv::threshold(color_channel, color_channel_binary, 150, 255, cv::THRESH_BINARY);

    cv::threshold(color_channel, color_channel_binary, 150, 255, cv::THRESH_BINARY);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
//    cv::morphologyEx(color_channel_binary, color_channel_binary, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(color_channel_binary, bin_img, cv::MORPH_CLOSE, kernel);
}

void ArmorDetector::findLightbars(const cv::Mat &bin_img, cv::Mat &dst_img)
{

    std::vector<std::vector<cv::Point>> lightbar_contours;
    std::vector<cv::Vec4i> hierarchy_lightbar;

    cv::RotatedRect rect_lightbar;
    cv::findContours(bin_img, lightbar_contours, hierarchy_lightbar, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    for(uint i=0; i<lightbar_contours.size(); i++)
    {
        rect_lightbar = cv::minAreaRect(lightbar_contours[i]);
//        if(rect_lightbar.size.area() > 0)
//        {
//            std::cout << "area: " << rect_lightbar.size.area() << std::endl;
//        }
        if(rect_lightbar.size.area() > 5.0)
        {
            if(rect_lightbar.size.width > rect_lightbar.size.height)
            {
                rect_lightbar = cv::RotatedRect(rect_lightbar.center,
                    cv::Size2f(rect_lightbar.size.height, rect_lightbar.size.width),rect_lightbar.angle-90);
            }
            lightbars.emplace_back(rect_lightbar);
        }
    }
    dst_img = cv::Mat::zeros(480, 640, CV_8UC3);
    cv::drawContours(dst_img, lightbar_contours, -1, cv::Scalar(0,255,0), 1);
}

void ArmorDetector::matchArmorBoxes(const cv::Mat &src_img, cv::Mat &dst_img)
{
    // armor's height, width, ratio between length of two lightbars, ratio between armor's height and width
    double armor_height, armor_width, length_ratio, aspect_ratio;

    // angle/height distance between two lightbars, angle of armor, angle distance between armor and lightbars
    double angle_distance, height_distance, armor_angle, armor_light_angle;

    std::vector<cv::Point2f> armor_points_vec;
    cv::Point2f left_points[4], right_points[4], armor_points[4];
    cv::RotatedRect armor_rotated_rect;
    cv::Rect armor_rect;
    cv::Mat armor_img;
    cv::Point2f target;

    armor_boxes.clear();

    if(lightbars.size() < 2)
    {
        lightbars.clear();
        return;
    }

    for(uint i=0; i<lightbars.size()-1; i++)
    {
        for(uint j=i+1; j<lightbars.size(); j++)
        {
            // lightbar group param
            cv::Point2f centers = lightbars.at(i).center - lightbars.at(j).center;
            armor_height = (lightbars.at(i).size.height + lightbars.at(j).size.height) / 2;
            armor_width = sqrt(centers.ddot(centers));
            aspect_ratio = armor_width / armor_height;
            length_ratio = lightbars.at(i).size.height / lightbars.at(j).size.height;

            angle_distance = fabs(lightbars.at(i).angle - lightbars.at(j).angle);
            height_distance = fabs(centers.y);
            armor_angle = atan((lightbars.at(i).center.y - lightbars.at(j).center.y)/
                               (lightbars.at(i).center.x - lightbars.at(j).center.x)) * 180 / CV_PI;
            armor_light_angle = fabs((lightbars.at(i).angle + lightbars.at(j).angle)/2.0 - armor_angle);
//            std::cout << "number: " << frame_count << std::endl;
//            std::cout << "height: " << armor_height << " width: " << armor_width << std::endl;
//            std::cout << "aspect_ratio: " << aspect_ratio << " length_ratio: " << length_ratio << std::endl;
//            std::cout << "angle_distance: " << angle_distance << " height_distance: " << height_distance << std::endl;
//            std::cout << "armor_angle: " << armor_angle << " armor_light_angle: " << armor_light_angle << std::endl;
//            std::cout << "----------------------------------" << std::endl;
            if((length_ratio>0.5) && (length_ratio<1.5) &&
                (height_distance<30) && ((angle_distance)<10 || (angle_distance>170)) &&
                (aspect_ratio>2.0) && (aspect_ratio<7.0))
            {
                lightbars.at(i).points(left_points);
                lightbars.at(j).points(right_points);
                for(int k=0; k<4; k++)
                {
                    armor_points_vec.emplace_back(left_points[k]);
                    armor_points_vec.emplace_back(right_points[k]);
                }
                armor_rotated_rect = cv::minAreaRect(armor_points_vec);
                armor_rect = cv::boundingRect(armor_points_vec);
                target = getCenterPoint(armor_rect);
                armor_rotated_rect.points(armor_points);

                cv::Point rect_top_left, rect_bottom_right;
                rect_top_left = armor_rect.tl();
                rect_bottom_right = armor_rect.br();
//                std::cout << int(rect_top_left.x) << " " << int(fmax(rect_top_left.y-armor_height*0.5, 0)) << " ";
//                std::cout << int(armor_width) << " " << int(armor_height*2) << std::endl;
                if(rect_top_left.x+armor_width*0.12 <= 0 ||
                        (int(rect_top_left.x+armor_width*0.12) + int(armor_width) > 640) ||
                        (int(fmax(rect_top_left.y-armor_height*0.5, 0)) + int(armor_height*2) > 480))
                {
                    continue;
                }
                cv::Rect armor_img_rect(int(rect_top_left.x+armor_width*0.12),
                                        int(fmax(rect_top_left.y-armor_height*0.5, 0)),
                                        int(armor_width*0.8),
                                        int(armor_height*2));
                cv::Mat roi = src_img(armor_img_rect);
                cv::Mat roi_gray;
                cv::Mat roi_gamma;

                cv::cvtColor(roi, roi_gray, cv::COLOR_BGR2GRAY);
                roi_gray.convertTo(roi_gray, CV_64F, 1.0/255, 0);
                double gamma = 0.46;

                cv::pow(roi_gray, gamma, roi_gamma);
                roi_gamma.convertTo(roi_gamma, CV_8U, 255, 0);
                cv::threshold(roi_gamma, roi_gamma, 45, 255, cv::THRESH_BINARY);

//                armor_img = warpTransform(src_img, armor_rect, armor_height, armor_width);
                QString filename = "../dataset/" + QString::number(frame_count++) + ".png";
                cv::imwrite(filename.toStdString(), roi_gamma);
                for(int m=0; m<4; m++)
                {
                    cv::line(dst_img, armor_points[m], armor_points[(m+1)%4], cv::Scalar(0,255,255));
                }
                if(aspect_ratio < 4.4)
                {
                    armor_boxes.emplace_back(ArmorBox(armor_rect, armor_rotated_rect,
                                                      roi_gamma, common_utils::ARMOR_SMALL));
                }
                else
                {
                    armor_boxes.emplace_back(ArmorBox(armor_rect, armor_rotated_rect,
                                                      roi_gamma, common_utils::ARMOR_BIG));
                }
                armor_points_vec.clear();
            }
        }
    }
}

bool ArmorDetector::getTarget()
{
    if(armor_boxes.empty())
    {
        return false;
    }
    else if(armor_boxes.size() == 1)
    {
//        std::sort(armor_boxes.begin(), armor_boxes.end(), ArmorBox::ratioComparator);
        armor_target = armor_boxes[0];
        return true;
    }
    else
    {
        std::cout << "size: " << armor_boxes.size() << std::endl;
//        for(int k=0; j<armor_boxes.size(); k++)
//        {

//        }
        std::vector<ArmorBox> armor_match_list;
        for(int i=0; i<armor_boxes.size(); i++)
        {
            cv::Mat_<uchar>::iterator it = armor_boxes[i].armor_img.begin<uchar>();
            cv::Mat_<uchar>::iterator it_end = armor_boxes[i].armor_img.end<uchar>();
            int count = 0;
            for(; it!=it_end; ++it)
            {
                if((*it) > 0)
                {
                    count++;
                }
            }
            std::cout << "ratio: " << float(count) / float(armor_boxes[i].armor_img.cols * armor_boxes[i].armor_img.rows) << std::endl;
            if(float(count) / float(armor_boxes[i].armor_img.cols * armor_boxes[i].armor_img.rows) > 0.3)
            {
                armor_match_list.emplace_back(armor_boxes[i]);
            }
        }
        if(armor_match_list.size() == 0)
        {
            return false;
        }
        else
        {
            std::sort(armor_match_list.begin(), armor_match_list.end(), ArmorBox::areaComparator);
            armor_target = armor_match_list[0];
            armor_match_list.clear();
            return true;
        }
    }
}
cv::Mat ArmorDetector::warpTransform(const cv::Mat &src_img, cv::RotatedRect rect, double height, double width)
{
    cv::Point2f rect_point[4], rect_point_src[4], rect_point_dst[4];
    cv::Mat transform_mat, perspect_mat;
    rect.points(rect_point_src);

//    if(width < height)
//    {
//        double temp = 0;
//        rect_point_src[0] = rect_point[1];
//        rect_point_src[1] = rect_point[2];
//        rect_point_src[2] = rect_point[3];
//        rect_point_src[3] = rect_point[0];
//    }
//    else
//    {
//    rect_point_src[0] = rect_point[0];
//    rect_point_src[1] = rect_point[1];
//    rect_point_src[2] = rect_point[2];
//    rect_point_src[3] = rect_point[3];
//    }

    rect_point_src[0].y = fmax(rect_point_src[0].y - height/2, 0);
    rect_point_src[1].y = fmax(rect_point_src[1].y - height/2, 0);
    rect_point_src[2].y = fmin(rect_point_src[2].y + height/2, 480);
    rect_point_src[3].y = fmin(rect_point_src[3].y + height/2, 480);
    rect_point_dst[0] = cv::Point2f(0, 0);
    rect_point_dst[1] = cv::Point2f(width, 0);
    rect_point_dst[2] = cv::Point2f(width, 2*height);
    rect_point_dst[3] = cv::Point2f(0, 2*height);
    std::vector<cv::Point2f> rect_point_src_vec(rect_point_src, rect_point_src+4);
    std::vector<cv::Point2f> rect_point_dst_vec(rect_point_dst, rect_point_dst+4);
//    transform_mat = cv::getPerspectiveTransform(rect_point_src, rect_point_dst);
    transform_mat = cv::findHomography(rect_point_src_vec, rect_point_dst_vec);
    cv::warpPerspective(src_img, perspect_mat, transform_mat, cv::Size(width, 2*height));
    return perspect_mat;
}

cv::Point2i ArmorDetector::getCenterPoint(cv::Rect rect)
{
    cv::Point center;
    center.x = rect.x + cvRound(rect.width/2.0);
    center.y = rect.y + cvRound(rect.height/2.0);
    return center;
}













