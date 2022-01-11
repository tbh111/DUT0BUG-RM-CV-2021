#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include <QCanBus>
#include <QCanBusDevice>
#include <QCanBusDeviceInfo>
#include <QCanBusFrame>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateText();

    void on_send_pushButton_clicked();

    void on_connect_pushButton_clicked();

    void on_disconnect_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QCanBusDevice *device;
};
#endif // MAINWINDOW_H
