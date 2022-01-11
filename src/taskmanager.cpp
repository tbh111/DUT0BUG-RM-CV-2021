#include "taskmanager.h"

// structure: TaskManager --- CaptureThread
//                        --- ProcessThread --- Armor/Energy
//                        --- CanBus
//
//                                            pitch, roll, yaw
// work flow: Camera ---------------> process -----------------> canbus
//           (thread1)               (thread2)
//                                       |               image
//                              armor/energy/video mode -------> show image
//                                                              (TaskManager(Main) thread)
//

TaskManager::TaskManager(int mode, QObject *parent) :
    task_mode(mode), QObject(parent)
{
    capture_thread = new CaptureThread;
    process_thread = new ProcessThread;
    comm = new CommProtocol(common_utils::CAN, "can0");
    comm->initConnect();
    qRegisterMetaType<common_utils::TranPkg>("common_utils::TranPkg");
    qRegisterMetaType<common_utils::RecvPkg>("common_utils::RecvPkg");
    qRegisterMetaType<cv::Mat>("cv::Mat");

    // set process thread on after camera opened
    connect(capture_thread, SIGNAL(camera_opened()), process_thread, SLOT(setProcessOn()));
    // send frame to process thread after capture get a frame(update slot in capture thread)
    connect(capture_thread, SIGNAL(capture_done(cv::Mat)), process_thread, SLOT(getImage(cv::Mat)), Qt::DirectConnection);
    // transfer the data frame to can device, and send to stm32
    connect(process_thread, SIGNAL(send_tx_pkg(common_utils::TranPkg)), this, SLOT(updateTx(common_utils::TranPkg)));
    // if can device detected new frame, read frame data into current_rx
    connect(comm->device, SIGNAL(framesReceived()), this, SLOT(updateRx()));
    // send current_rx to process thread
    connect(this, SIGNAL(new_rx(common_utils::RecvPkg)), process_thread, SLOT(getRx(common_utils::RecvPkg)));

    if(mode == common_utils::SHOW_ON)
    {
        image_show = new ImageShow;
        // send frame to image show window after process is done
        connect(process_thread, SIGNAL(update_show_image(QImage)), image_show, SLOT(draw(QImage)), Qt::BlockingQueuedConnection);
        connect(image_show, SIGNAL(end_signal()), this, SLOT(end_show()));
        connect(process_thread, SIGNAL(send_process_fps(int)), image_show, SLOT(showFps(int)));
        connect(this, SIGNAL(new_rx(common_utils::RecvPkg)), image_show, SLOT(showRx(common_utils::RecvPkg)));
        connect(process_thread, SIGNAL(send_tx_pkg(common_utils::TranPkg)), image_show, SLOT(showTx(common_utils::TranPkg)));
        image_show->show();
    }
    capture_thread->start();
    process_thread->start();
}

void TaskManager::updateRx()
{
    current_rx = comm->readData();
    emit new_rx(current_rx);
}

void TaskManager::updateTx(common_utils::TranPkg tx)
{
    comm->writeData(tx);
}

void TaskManager::end_show()
{
    image_show->close();
    capture_thread->setCameraOff();
    process_thread->setProcessOff();
    comm->disConnect();
//    capture_thread->wait();
//    process_thread->wait();
    qDebug() << "end";
}
