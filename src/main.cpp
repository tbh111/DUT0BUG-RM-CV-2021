#include <iostream>
#include <time.h>
#include <QApplication>
#include <QDebug>
#include <QMutex>
#include <QFile>
#include <QMessageLogContext>
#include <QDateTime>

#include "taskmanager.h"
#include "common.h"
#include "config.h"

void logger(QtMsgType type,const QMessageLogContext &context,const QString &msg);
unsigned long GetTickCount();

int main(int argc, char *argv[])
{
//    qInstallMessageHandler(logger);
    QApplication a(argc, argv);
    qDebug() << "STARTING MAIN CLIENT...";
    TaskManager manager(common_utils::SHOW_ON);

    //Config config;

    //config.InitJsonFile();
    //QString test = config.ReadJsonFile("serial", "baud_rate");
    //qDebug() << test;
//    QString test1 = config.ReadJsonFile("serial", "port");
//    qDebug() << test1;
//    QString test2 = config.ReadJsonFile("energy", "energy_armor_area_max_threshold");
//    qDebug() << test2;
     return a.exec();

}

unsigned long GetTickCount()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

void logger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal");
        break;
    default:
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("[%1][%2] File:%3 Line:%4  %5")
            .arg(strDateTime).arg(strMsg).arg(context.file).arg(context.line).arg(localMsg.constData());
    // 输出信息至文件中（读写、追加形式）
    QFile file("../log/log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}
