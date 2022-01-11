#include "commprotocol.h"

CommProtocol::CommProtocol(int t_protocol, QString t_port):
    protocol(t_protocol), port(t_port)
{
    if(t_protocol == common_utils::CAN)
    {
//        system("echo dji | sudo -S ip link set down can0");
//        system("echo dji | sudo ip link set can0 type can bitrate 1000000 dbitrate 2000000 berr-reporting on fd on restart-ms 100");
//        system("echo dji | sudo ip link set up can0");

        qDebug() << "use socketcan interface";
        if(QCanBus::instance()->plugins().contains(QStringLiteral("socketcan")))
        {
            qDebug() << "socketcan plugin available";
        }
        QString err_string;
        const QList<QCanBusDeviceInfo> can_info = QCanBus::instance()->
                availableDevices(QStringLiteral("socketcan"), &err_string);
        if(can_info.isEmpty())
        {
            qDebug() << "can not detect can-bus device";
        }
        else
        {
            if(can_info.begin()->name() == port)
            {
                qDebug() << "successfully detect device can0";
            }
        }
    }
}

bool CommProtocol::initConnect()
{
    QString err_string;
    device = QCanBus::instance()->createDevice(QStringLiteral("socketcan"), port, &err_string);
    device->setConfigurationParameter(QCanBusDevice::BitRateKey, QVariant(bitrate));
    device->setConfigurationParameter(QCanBusDevice::DataBitRateKey, QVariant(dbitrate));

    QCanBusDevice::Filter filter;
    QList<QCanBusDevice::Filter> filterlist;
    filter.frameId = 0x400u;
    filter.frameIdMask = 0x7FFu;
    filter.format = QCanBusDevice::Filter::MatchBaseFormat;
    filter.type = QCanBusFrame::DataFrame;
    filterlist.append(filter);
    device->setConfigurationParameter(QCanBusDevice::RawFilterKey, QVariant::fromValue(filterlist));

    bool connect_status = device->connectDevice();
    if(connect_status)
    {
        qDebug() << "connect succeess";
        return true;
    }
    else
    {
        qDebug() << "connect failed";
        return false;
    }
}

void CommProtocol::disConnect()
{
    if(device->state() == QCanBusDevice::ConnectedState)
    {
        device->disconnectDevice();
        delete device;
        qDebug() << "disconnect";
    }
}

bool CommProtocol::writeData(common_utils::TranPkg tx_pkg)
{
//    qDebug() << "write state: " << device->state() << " status: " << device->busStatus();

    QCanBusFrame tx_frame;
    tx_frame.setFrameId(0x401);
    QByteArray payload;
    payload.resize(8);

    // byte 0: ckeck state 0000_0xxx
    payload[0] = 0x07 & ((tx_pkg.check_video & common_utils::VIDEO_ON) << 2 ^
                 (tx_pkg.check_color & common_utils::SELF_BLUE) << 1 ^
                 (tx_pkg.check_mode & common_utils::TARGET_ARMOR));
//    qDebug() << payload[0];
//    qDebug() << " 1: " << tx_pkg.check_video << " 2: " << tx_pkg.check_color << " 3: " << tx_pkg.check_mode;

    // byte 1: whether detect valid
    payload[1] = tx_pkg.detect_state;

    // byte 2-3: yaw offset
    payload[2] = (0xff00 & tx_pkg.yaw) >> 8;
    payload[3] = 0x00ff & tx_pkg.yaw;

    // byte 4-5: pitch offset
    payload[4] = (0xff00 & tx_pkg.pitch) >> 8;
    payload[5] = 0x00ff & tx_pkg.pitch;

    // byte 6: shoot command
    payload[6] = tx_pkg.cmd;

    // byte 7: undefined
    payload[7] = 0x00;

    tx_frame.setPayload(payload);
    bool write_status = device->writeFrame(tx_frame);
    if(write_status)
    {
//        qDebug() << "write";
        return true;
    }
    else
    {
        qDebug() << "write failed";
        return false;
    }
}

int CommProtocol::getFrameNumber()
{
    return device->framesAvailable();
}

common_utils::RecvPkg CommProtocol::readData()
{
//    qDebug() << "read state: " << device->state() << " status: " << device->busStatus();

    common_utils::RecvPkg rx_pkg;
    while(device->framesAvailable())
    {
        const QCanBusFrame frame = device->readFrame();
        QString frame_str;
        if(frame.frameType() == QCanBusFrame::DataFrame)
        {
            frame_str = frame.toString();
//            qDebug() << frame_str;

            // frame:  7FF   [8]  00 01 02 03 04 05 06 07
            QStringList list = frame_str.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            // ensure frame length
            if(list.at(1) == "[8]")
            {
                bool ok;
                rx_pkg.func_code = list.at(2).toInt(&ok, 16);
                if(rx_pkg.func_code == common_utils::FUNC_COLOR)
                {
                    rx_pkg.recv_color = list.at(3).toInt(&ok, 16);
                }
                else if(rx_pkg.func_code == common_utils::FUNC_TARGET)
                {
                    rx_pkg.recv_target = list.at(3).toInt(&ok, 16);
                }
                else if(rx_pkg.func_code == common_utils::FUNC_ENERGY)
                {
                    rx_pkg.recv_energy = list.at(3).toInt(&ok, 16);
                }
                else if(rx_pkg.func_code == common_utils::FUNC_VIDEO)
                {
                    rx_pkg.recv_video = list.at(3).toInt(&ok, 16);
                }
                else if(rx_pkg.func_code == common_utils::FUNC_ANGLE)
                {
                    QString pitch_str;
                    pitch_str = list.at(3) + list.at(4);
                    rx_pkg.pitch = pitch_str.toInt(&ok, 16);

                }
                else if(rx_pkg.func_code == common_utils::FUNC_SPEED)
                {
                    QString speed_str = list.at(3) + list.at(4);
                    rx_pkg.speed = speed_str.toInt(&ok, 16);
                }
                else
                {
                    qDebug() << rx_pkg.func_code;
                    qDebug() << "package error";
                }
            }
        }
        else
        {
            qDebug() << device->interpretErrorFrame(frame);
        }
    }
    return rx_pkg;
}
