#include "anglesolver.h"


//AngleSolverParam::AngleSolverParam()
//{

//}

//void AngleSolverParam::read_XMLFile(void)
//{
//    FileStorage fsRead("../config/camera.xml", FileStorage::READ);

//    if(!fsRead.isOpened())
//    {
//        cout << "failed to open xml" << endl;
//        return;
//    }

//    fsRead["Y_DISTANCE_BETWEEN_GUN_AND_CAM"] >> Y_DISTANCE_BETWEEN_GUN_AND_CAM;
//    fsRead["Z_DISTANCE_BETWEEN_MUZZLE_AND_CAM"] >> Z_DISTANCE_BETWEEN_MUZZLE_AND_CAM;
//    fsRead["Camera_Matrix"] >> CameraIntrinsicMatrix;
//    fsRead["Distortion_Coefficients"] >> DistortionCoefficient;
//    return;
//}

AngleSolver::AngleSolver()
{
    init();
}

bool AngleSolver::init()
{


    //装甲板世界坐标,把装甲板的中心对准定义的图像中心
    POINT_3D_OF_ARMOR_BIG.push_back(cv::Point3f(-117.5f, -63.5f, 0.0f)); // top left
    POINT_3D_OF_ARMOR_BIG.push_back(cv::Point3f( 117.5f, -63.5f, 0.0f)); // top right
    POINT_3D_OF_ARMOR_BIG.push_back(cv::Point3f( 117.5f,  63.5f, 0.0f)); // bottom right
    POINT_3D_OF_ARMOR_BIG.push_back(cv::Point3f(-117.5f,  63.5f, 0.0f)); // bottom left

    POINT_3D_OF_ARMOR_SMALL.push_back(cv::Point3f(-70.0f, -28.0f, 0.0f));
    POINT_3D_OF_ARMOR_SMALL.push_back(cv::Point3f( 70.0f, -28.0f, 0.0f));
    POINT_3D_OF_ARMOR_SMALL.push_back(cv::Point3f( 70.0f,  28.0f, 0.0f));
    POINT_3D_OF_ARMOR_SMALL.push_back(cv::Point3f(-70.0f,  28.0f, 0.0f));

    cv::FileStorage fsRead("../config/camera.xml", cv::FileStorage::READ);

    if(!fsRead.isOpened())
    {
        std::cout << "failed to open xml" << std::endl;
        return false;
    }

    fsRead["Y_DISTANCE_BETWEEN_GUN_AND_CAM"] >> Y_DISTANCE_BETWEEN_GUN_AND_CAM;
    fsRead["Z_DISTANCE_BETWEEN_MUZZLE_AND_CAM"] >> Z_DISTANCE_BETWEEN_MUZZLE_AND_CAM;
    fsRead["Camera_Matrix"] >> CameraIntrinsicMatrix;
    fsRead["Distortion_Coefficients"] >> DistortionCoefficient;
//    std::cout << CameraIntrinsicMatrix << std::endl;
    return true;

}

void AngleSolver::onePointSolution(const std::vector<cv::Point2f> centerPoint)
{
    double fx = CameraIntrinsicMatrix.at<double>(0,0);
    double fy = CameraIntrinsicMatrix.at<double>(1,1);
    double cx = CameraIntrinsicMatrix.at<double>(0,2);
    double cy = CameraIntrinsicMatrix.at<double>(1,2);

    std::vector<cv::Point2f> dstPoint;
    //单点矫正
    cv::undistortPoints(centerPoint, dstPoint, CameraIntrinsicMatrix,
                    DistortionCoefficient, cv::noArray(), CameraIntrinsicMatrix);
    cv::Point2f pnt = dstPoint.front();//返回dstPoint中的第一个元素
    //去畸变后的比值，根据像素坐标系与世界坐标系的关系得出,pnt的坐标就是在整幅图像中的坐标
    double rxNew=(pnt.x-cx)/fx;
    double ryNew=(pnt.y-cy)/fy;

    yawErr = atan(rxNew)/CV_PI*180;
    pitchErr = atan(ryNew)/CV_PI*180;
}

std::vector<double> AngleSolver::p4pSolution(const cv::Rect &rect, int objectType)
{
    cv::Point2f rect_points[4];
//    cv::Point2f left_up, left_down, right_up, right_down;
    std::vector<cv::Point2f> rect_points_vec;
//    rect.points(rect_points);

//    std::cout << "rect_point0: " << rect_points[0] << std::endl;
//    std::cout << "rect_point1: " << rect_points[1] << std::endl;
//    std::cout << "rect_point2: " << rect_points[2] << std::endl;
//    std::cout << "rect_point3: " << rect_points[3] << std::endl;



    rect_points_vec.clear();
    rect_points_vec.emplace_back(cv::Point(std::max(rect.x, 0), std::max(rect.y, 0)));
    rect_points_vec.emplace_back(cv::Point(std::min(rect.x+rect.width, 640), std::max(rect.y, 0)));
    rect_points_vec.emplace_back(cv::Point(std::min(rect.x+rect.width, 640), std::min(rect.y+rect.height, 480)));
    rect_points_vec.emplace_back(cv::Point(std::max(rect.x, 0), std::min(rect.y+rect.height, 480)));
//    std::cout << rect_points_vec << std::endl;
    cv::Mat raux, taux;

    float yawErr_current;
    float pitchErr_current;
    if(objectType == common_utils::ARMOR_BIG)
    {
        std::cout << "big" << std::endl;
        cv::solvePnP(POINT_3D_OF_ARMOR_BIG, rect_points_vec, CameraIntrinsicMatrix,
                 DistortionCoefficient, raux, taux, false, cv::SOLVEPNP_ITERATIVE);

    }
    else
    {
        std::cout << "small" << std::endl;
        cv::solvePnP(POINT_3D_OF_ARMOR_SMALL, rect_points_vec, CameraIntrinsicMatrix,
                 DistortionCoefficient, raux, taux, false, cv::SOLVEPNP_ITERATIVE);
    }

//    tVec.at<float>(1, 0) -= _params.Y_DISTANCE_BETWEEN_GUN_AND_CAM;
//    tVec.at<float>(2, 0) -= _params.Z_DISTANCE_BETWEEN_MUZZLE_AND_CAM;

    yawErr_current = atan(taux.at<double>(0, 0)/taux.at<double>(2, 0))/CV_PI*180;
    pitchErr_current = atan(taux.at<double>(1, 0)/taux.at<double>(2, 0))/CV_PI*180;
    //计算三维空间下的欧氏距离
    _euclideanDistance = sqrt(taux.at<double>(0, 0) * taux.at<double>(0, 0) + taux.at<double>(1, 0) *
                              taux.at<double>(1, 0) + taux.at<double>(2, 0) * taux.at<double>(2, 0));
    std::vector<double> result;

    result.resize(3); //指定容器的大小为3
    result[0] = yawErr_current;
    result[1] = pitchErr_current;
    result[2] =_euclideanDistance;
    std::cout << "yaw:" << result[0] << " pitch: " << result[1] << " dis: " << result[2] << std::endl;
    return result;
}



/*
//air friction is considered
float BulletModel(float x, float v, float angle) { //x:m,v:m/s,angle:rad
    float t, y;
    float init_k_;
    t = (float)((exp(init_k_ * x) - 1) / (init_k_ * v * cos(angle)));
    y = (float)(v * sin(angle) * t - GRAVITY * t * t / 2);
    return y;
}

//x:distance , y: height
float GetPitch(float x, float y, float v) {
    float y_temp, y_actual, dy;
    float a;
    y_temp = y;
    // by iteration
    for (int i = 0; i < 20; i++) {
        a = (float) atan2(y_temp, x);
        y_actual = BulletModel(x, v, a);
        dy = y - y_actual;
        y_temp = y_temp + dy;
        if (fabsf(dy) < 0.001) {
        break;
        }
        //printf("iteration num %d: angle %f,temp target y:%f,err of y:%f\n",i+1,a*180/3.1415926535,yTemp,dy);
    }
    return a;

}

void Transform(vector<double> result, float &pitch, float v, float angle) {
    pitch =
        -GetPitch(result[2] / 100, -BulletModel(result[2], v, angle) / 100, v) + result[1];

}

*/
