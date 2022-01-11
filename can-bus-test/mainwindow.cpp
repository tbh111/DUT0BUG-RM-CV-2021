#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    if(QCanBus::instance()->plugins().contains(QStringLiteral("socketcan")))
//    {
//        qDebug() << "plugin available";
//    }
//    QString error_string;
//    QCanBusDevice *device = QCanBus::instance()
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::updateText()
{
    while(device->framesAvailable())
    {
        const QCanBusFrame frame = device->readFrame();
        QCanBusFrame::TimeStamp time = frame.timeStamp();
        qint64 time_strap = time.seconds();
        QString frame_str;
        if(frame.frameType() == QCanBusFrame::ErrorFrame)
        {
            frame_str = device->interpretErrorFrame(frame);
        }
        else if(frame.frameType() == QCanBusFrame::DataFrame)
        {
            frame_str = frame.toString();
        }
        qDebug() << time_strap;
        QString show_str = frame_str + "\n";
        ui->textBrowser->append(show_str);
        device->clear();
    }
}

void MainWindow::on_send_pushButton_clicked()
{
    QCanBusFrame frame;
    frame.setFrameId(1);
    QString data = ui->input_lineEdit->text();
    if(!data.isEmpty())
    {
        QByteArray payload = QByteArray::fromHex(data.toUtf8());
        frame.setPayload(payload);
        device->writeFrame(frame);
        qDebug() << "write";
    }
    else
    {
        qDebug() << "empty";
    }
}

void MainWindow::on_connect_pushButton_clicked()
{
    if(QCanBus::instance()->plugins().contains(QStringLiteral("socketcan")))
    {
        qDebug() << "plugin available";
    }
    QString error_string;
    const QList<QCanBusDeviceInfo> can_info = QCanBus::instance()->availableDevices(QStringLiteral("socketcan"), &error_string);
    if(can_info.empty())
    {
        qDebug() << "empty";
        qDebug() << error_string;
    }
    else
    {
        qDebug() << can_info.begin()->name();
    }

    device = QCanBus::instance()->createDevice(
                QStringLiteral("socketcan"), QStringLiteral("can0"), &error_string);
    if(!device)
    {
        qDebug() << "error on create device";
        qDebug() << error_string;
    }
    else
    {
        const int bitrate = 1000000;
        const int data_bitrate = 2000000;
        device->setConfigurationParameter(QCanBusDevice::BitRateKey, QVariant(bitrate));
        device->setConfigurationParameter(QCanBusDevice::DataBitRateKey, QVariant(data_bitrate));
        bool connect_status = device->connectDevice();
        if(connect_status)
        {
            qDebug() << "connect success";
            connect(device, SIGNAL(framesReceived()),
                    this, SLOT(updateText()));
//            device->disconnectDevice();
        }
        else
        {
            qDebug() << "connect failed";
        }
    }
}

void MainWindow::on_disconnect_pushButton_clicked()
{
    if(device->state() == QCanBusDevice::ConnectedState)
    {
        device->disconnectDevice();
        delete device;
        qDebug() << "disconnect";
    }
}
