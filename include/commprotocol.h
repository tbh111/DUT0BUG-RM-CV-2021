#ifndef COMMPROTOCOL_H
#define COMMPROTOCOL_H

#include <QString>
#include <QStringList>
#include <QStringLiteral>
#include <QDebug>
#include <QList>
#include <QTime>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusDeviceInfo>
#include <QCanBusFrame>
#include "common.h"

class CommProtocol
{
public:
    CommProtocol(int t_protocol, QString t_port);
    bool initConnect();
    void disConnect();
    bool writeData(common_utils::TranPkg tx_pkg);
    int getFrameNumber();
    common_utils::RecvPkg readData();
    QCanBusDevice* device;
private:
    int protocol;
    QString port;
    const int bitrate = 1000000;
    const int dbitrate = 2000000;
};

#endif // COMMPROTOCOL_H
