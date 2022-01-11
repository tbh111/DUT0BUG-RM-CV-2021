#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QImage>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include "CameraApi.h"
#include "CameraDefine.h"
#include "CameraStatus.h"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread(QObject *parent = nullptr);
    int cameraOpen();
    void cameraClose();
    void getFrame();
//    int GetFps();
protected:
    void run();
signals:
    void camera_opened();
    void capture_done(cv::Mat img);
    void send_capture_fps(int send_fps);
public slots:
    void setCameraOff();
private:
    cv::Mat grabbed_frame;
    QMutex stopped_mutex, data_mutex;
    int frame_number = 0;
    double time_strap = 0.0;
    double fps = 0.0;
    volatile bool stop_flag = false;

    CameraHandle h_camera;
    tSdkCameraCapbility camera_capability;
    BYTE *pbyBuffer = NULL;
    BYTE *g_pRgbBuffer = NULL;
};

#endif // CAPTURETHREAD_H
