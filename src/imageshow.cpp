#include "imageshow.h"
#include "ui_imageshow.h"

ImageShow::ImageShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageShow)
{
    ui->setupUi(this);
}

ImageShow::ImageShow(QString window_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageShow)
{
    ui->setupUi(this);
    this->setWindowTitle(window_name);
}

ImageShow::~ImageShow()
{
    delete ui;
}

void ImageShow::showTx(common_utils::TranPkg new_tx)
{
    if(new_tx.detect_state == common_utils::DETECT_YES)
    {
        ui->state_tx->setText("detected");
    }
    else
    {
        ui->state_tx->setText("none");
    }
    if(new_tx.cmd == common_utils::SHOOT_AUTO)
    {
        ui->cmd_tx->setText("auto");
    }
    else if(new_tx.cmd == common_utils::SHOOT_SINGLE)
    {
        ui->cmd_tx->setText("single");
    }
    else if(new_tx.cmd == common_utils::SHOOT_DISABLED)
    {
        ui->cmd_tx->setText("disabled");
    }
    ui->pitch_tx->setText(QString::number(new_tx.pitch));
    ui->yaw_tx->setText(QString::number(new_tx.yaw));
}

void ImageShow::showRx(common_utils::RecvPkg new_rx)
{
    if(new_rx.recv_color == common_utils::SELF_BLUE)
    {
        ui->color_rx->setText("blue");
    }
    else if(new_rx.recv_color == common_utils::SELF_RED)
    {
        ui->color_rx->setText("red");
    }
    if(new_rx.recv_target == common_utils::TARGET_ARMOR)
    {
        ui->target_rx->setText("armor");
    }
    else if(new_rx.recv_target == common_utils::TARGET_ENERGY)
    {
        ui->target_rx->setText("energy");
    }
    if(new_rx.recv_energy == common_utils::ENERGY_SMALL)
    {
        ui->energy_rx->setText("small");
    }
    else if(new_rx.recv_energy == common_utils::ENERGY_BIG)
    {
        ui->energy_rx->setText("big");
    }
    if(new_rx.recv_video == common_utils::VIDEO_OFF)
    {
        ui->video_rx->setText("off");
    }
    else if(new_rx.recv_video == common_utils::VIDEO_ON)
    {
        ui->video_rx->setText("on");
    }
    ui->pitch_rx->setText(QString::number(new_rx.pitch));
    ui->speed_rx->setText(QString::number(new_rx.speed));
}

void ImageShow::draw(QImage qimg)
{
    sample_count++;
    if(sample_count == 1)
    {
        ui->image->setPixmap(QPixmap::fromImage(qimg.scaled(ui->image->size(),Qt::KeepAspectRatio)));
        sample_count = 0;
    }
}

void ImageShow::showFps(int fps)
{
    QString fpsText = QString("FPS: %1").arg(fps);
    ui->fpsLabel->setText(fpsText);
}


void ImageShow::on_closeButton_clicked()
{
    emit end_signal();
    this->close();
}
