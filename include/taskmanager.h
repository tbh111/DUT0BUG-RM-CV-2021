#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QApplication>
#include <QObject>
#include <QDebug>
#include <QImage>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include "capturethread.h"
#include "processthread.h"
#include "commprotocol.h"
#include "imageshow.h"
#include "common.h"

class TaskManager:public QObject
{
    Q_OBJECT
public:
    explicit TaskManager(int mode, QObject *parent = nullptr);
signals:
    void new_rx(common_utils::RecvPkg rx);
public slots:
    void end_show();
    void updateTx(common_utils::TranPkg tx);
    void updateRx();
private:
    CaptureThread* capture_thread;
    ProcessThread* process_thread;
    ImageShow* image_show;
    CommProtocol *comm;
    common_utils::RecvPkg current_rx;
    QMutex mutex;
    int task_mode;
};

#endif // TASKMANAGER_H
