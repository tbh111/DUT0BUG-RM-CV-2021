#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QImage>
#include <QMutex>
#include <vector>
#include <opencv2/opencv.hpp>
#include "armordetector.h"
//#include "energydetector.h"
#include "anglesolver.h"
#include "common.h"

class ProcessThread : public QThread
{
    Q_OBJECT
public:
    explicit ProcessThread(QObject *parent = nullptr);
protected:
    void run();
public slots:
    void getImage(cv::Mat image);
    void setProcessOn();
    void setProcessOff();
    void getRx(common_utils::RecvPkg new_rx);
//    void switch_mode(int change_mode);
signals:
    void update_show_image(QImage img);
    void send_process_fps(int fps);
    void send_tx_pkg(common_utils::TranPkg tx_pkg);
private:
    void preProcess();
    void process();

    cv::Mat src_img, bin_img, dst_img;
    QMutex stop_mutex;
    QMutex data_mutex;
    double time_strap = 0.0;
    double fps = 0.0;
    int process_mode = 0;

    // received data from stm32
    int color = common_utils::SELF_RED;
    int process_target = common_utils::TARGET_ARMOR;
    int energy_mode = common_utils::ENERGY_SMALL;
    int video_switch = common_utils::VIDEO_OFF;
    double pitch_rx = 0.0;
    double speed_rx = 0.0;

    //transmit data to stm32
    common_utils::TranPkg tx_pkg;
    void packTx();
    int detect_flag = common_utils::DETECT_NONE;
    double yaw_tx = 0.0;
    double pitch_tx = 0.0;
    int cmd_tx = common_utils::SHOOT_DISABLED;

    bool image_update_flag = false;
    volatile bool stop_flag = false;
    volatile bool start_flag = false;
};

#endif // PROCESSTHREAD_H
