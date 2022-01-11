#include "capturethread.h"

CaptureThread::CaptureThread(QObject *parent):
    QThread(parent)
{

}

void CaptureThread::run()
{
    qDebug() << "capture thread id: " << currentThreadId();
    cameraOpen();
    while(true)
    {
        stopped_mutex.lock();
        // exit thread if stop flag is true
        if(stop_flag)
        {
            stop_flag = false;
            data_mutex.unlock();
            stopped_mutex.unlock();
            cameraClose();
            qDebug() << "capture thread exit";
            break;
        }
        stopped_mutex.unlock();

//        data_mutex.lock();
        // capture a frame from image buffer
        getFrame();
//        data_mutex.unlock();
    }
}

int CaptureThread::cameraOpen()
{
    CameraSdkStatus camera_status;
    tSdkCameraDevInfo camera_enum_list;
    int camera_count = 1;
    qDebug() << "camera SDK initializing";
    CameraSdkInit(0);
    qDebug() << "camera SDK init done";
    CameraEnumerateDevice(&camera_enum_list, &camera_count);
    if(camera_count == 0)
    {
        qDebug() << "ERROR: no camera connected";
        return -1;
    }
    else
    {
        qDebug() << "connected camera numbers:" << camera_count;
    }
    camera_status = CameraInit(&camera_enum_list, -1, -1, &h_camera);
    if(camera_status != CAMERA_STATUS_SUCCESS)
    {
        qDebug() << "camera initialize failed";
        return -1;
    }
    else
    {
        qDebug() << "camera initialize success";
    }
    CameraGetCapability(h_camera, &camera_capability);
    g_pRgbBuffer = (unsigned char*)malloc(camera_capability.sResolutionRange.iHeightMax*camera_capability.sResolutionRange.iWidthMax*3);

    // set param
    CameraSetIspOutFormat(h_camera, CAMERA_MEDIA_TYPE_BGR8);
    CameraSetAeState(h_camera, false);
    CameraSetExposureTime(h_camera, 1000);
    // run
    CameraPlay(h_camera);
    qDebug() << "camera playing";
    emit camera_opened();
    return 1;
}

void CaptureThread::getFrame()
{
//    time_strap = static_cast<double>(cv::getTickCount());
    tSdkFrameHead frame_info;
    if(CameraGetImageBuffer(h_camera, &frame_info, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS)
    {
        CameraImageProcess(h_camera, pbyBuffer, g_pRgbBuffer, &frame_info);
        cv::Mat frame(cv::Size(frame_info.iWidth, frame_info.iHeight), CV_8UC3, g_pRgbBuffer);
        grabbed_frame = frame;
//        qDebug() << "get frame: " << frame_number++;
//        cv::cvtColor(grabbed_frame, grabbed_frame, cv::COLOR_BGR2RGB);
        emit capture_done(grabbed_frame);
        CameraReleaseImageBuffer(h_camera, pbyBuffer);
    }
    else
    {
        qDebug() << "timeout";
        usleep(1000);
    }
//    time_strap = static_cast<double>(cv::getTickCount() - time_strap) / cv::getTickFrequency();
//    fps = 1.0 / time_strap;
//    qDebug() << "fps: " << fps;
//    emit send_capture_fps(fps);
}

void CaptureThread::setCameraOff()
{
    stopped_mutex.lock();
    stop_flag = true;
    stopped_mutex.unlock();
}

void CaptureThread::cameraClose()
{
    CameraSdkStatus camera_status;
    qDebug() << "try close camera";
    camera_status = CameraUnInit(h_camera);
    if(camera_status != CAMERA_STATUS_SUCCESS)
    {
        qDebug() << "can not close camera";
    }
    else
    {
        if(g_pRgbBuffer != NULL)
        {
            free(g_pRgbBuffer);
        }
        qDebug() << "close camera success";
    }
}
