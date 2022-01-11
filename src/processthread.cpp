#include "processthread.h"

ProcessThread::ProcessThread(QObject *parent):
    QThread(parent)
{

}

void ProcessThread::run()
{
    qDebug() << "process thread id: " << currentThreadId();
    ArmorDetector armor_detector;
//    EnergyDetector energy_detector;
    AngleSolver pnp;
    int sample_count = 0;
//    int armor_type;
    while(true)
    {
        stop_mutex.lock();
        if(stop_flag)
        {
            stop_flag = false;
            stop_mutex.unlock();
            qDebug() << "process thread exit";
            break;
        }
        stop_mutex.unlock();

        data_mutex.lock();
        if(start_flag && image_update_flag)
        {
            time_strap = static_cast<double>(cv::getTickCount());
            if(video_switch == common_utils::VIDEO_ON)
            {
                if(sample_count++ == 10)
                {
                    sample_count = 0;
                    cv::imwrite("/home/dji/data/photo/" + std::to_string(cv::getTickCount()) + ".png", src_img);
                }
            }
            if(process_target == common_utils::TARGET_ARMOR)
            {
                ArmorBox target;
                armor_detector.run(src_img, dst_img, target, color);
                if(target.isValid)
                {
//                    std::vector<cv::Point2f> target_vec(target.armor_points, target.armor_points+4);

                    std::vector<double> result = pnp.p4pSolution(target.armor_rect, target.armor_type);
                    detect_flag = common_utils::DETECT_YES;
                    yaw_tx = result[0];
                    pitch_tx = result[1];
                    cmd_tx = common_utils::SHOOT_AUTO;
//                    packTx();
                    result.clear();
//                    std::cout << "color: " << color << " target: " << process_target << std::endl;
//                    std::cout << "energy_mode: " << energy_mode << "video_switch: " << video_switch << std::endl;
//                    std::cout << "yaw: " << yaw_rx << " pitch: " << pitch_rx << " speed: " << speed_rx << std::endl;
                }
                else
                {
                    detect_flag = common_utils::DETECT_NONE;
                    yaw_tx = 0.0;
                    pitch_tx = 0.0;
                    cmd_tx = common_utils::SHOOT_DISABLED;
//                    packTx();
                }
            }
            else if(process_target == common_utils::TARGET_ENERGY)
            {
//                EnergyBox target;
//                energy_detector.run(src_img, dst_img, target, color);
//                if(target.isValid)
//                {
//                    std::vector<double> result = pnp.p4pSolution(target.armor_rect, target.armor_type);
//                    detect_flag = common_utils::DETECT_YES;
//                    yaw_tx = result[0];
//                    pitch_tx = result[1];
//                    cmd_tx = common_utils::SHOOT_SINGLE;
//                    packTx();
//                    result.clear();
//                }
            }
            else
            {
                detect_flag = common_utils::DETECT_NONE;
                yaw_tx = 0.0;
                pitch_tx = 0.0;
                cmd_tx = common_utils::SHOOT_DISABLED;
//                packTx();
            }
//                std::cout << "energy" << std::endl;

//            preProcess();
//            process();
//            dst_img = bin_img;
//            cv::cvtColor(dst_img, dst_img, cv::COLOR_BGR2RGB);
            if(dst_img.channels() == 3)
            {
                QImage image(dst_img.data, dst_img.cols, dst_img.rows, static_cast<int>(dst_img.step), QImage::Format_RGB888);
                emit update_show_image(image);
            }
            else
            {
                QImage image(dst_img.data, dst_img.cols, dst_img.rows, static_cast<int>(dst_img.step), QImage::Format_Indexed8);
                emit update_show_image(image);
            }
            image_update_flag = false;
            time_strap = static_cast<double>(cv::getTickCount() - time_strap) / cv::getTickFrequency();
            fps = 1.0 / time_strap;
//            qDebug() << "fps: " << fps;
            emit send_process_fps(fps);
        }

        data_mutex.unlock();
    }
}

void ProcessThread::getRx(common_utils::RecvPkg new_rx)
{
//    data_mutex.lock();
    if(new_rx.func_code == common_utils::FUNC_TARGET)
    {
        process_target = new_rx.recv_target;
    }
    else if(new_rx.func_code == common_utils::FUNC_COLOR)
    {
        color = new_rx.recv_color;
    }
    else if(new_rx.func_code == common_utils::FUNC_VIDEO)
    {
        video_switch = new_rx.recv_video;
    }
    else if(new_rx.func_code == common_utils::FUNC_ENERGY)
    {
        energy_mode = new_rx.recv_energy;
    }
    else if(new_rx.func_code == common_utils::FUNC_ANGLE)
    {
        pitch_rx = new_rx.pitch / 100;
    }
    else if(new_rx.func_code == common_utils::FUNC_SPEED)
    {
        speed_rx = new_rx.speed / 100;
    }
//    data_mutex.unlock();
}

void ProcessThread::packTx()
{
//    data_mutex.lock();
    tx_pkg.check_mode = process_target;
    tx_pkg.check_color = color;
    tx_pkg.check_video = video_switch;
    tx_pkg.detect_state = detect_flag;
    tx_pkg.yaw = yaw_tx * 100;
    tx_pkg.pitch = pitch_tx * 100;
    tx_pkg.cmd = cmd_tx;
    emit send_tx_pkg(tx_pkg);
//    data_mutex.unlock();
}

void ProcessThread::setProcessOn()
{
    data_mutex.lock();
    qDebug() << "process start";
    start_flag = true;
    data_mutex.unlock();
}

void ProcessThread::setProcessOff()
{
    stop_mutex.lock();
    stop_flag = true;
    stop_mutex.unlock();
}

void ProcessThread::getImage(cv::Mat image)
{
    data_mutex.lock();
    if(!image.empty())
    {
        image.copyTo(src_img);
    }
    image_update_flag = true;
    data_mutex.unlock();
}
