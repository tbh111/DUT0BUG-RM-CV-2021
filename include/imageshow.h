#ifndef IMAGESHOW_H
#define IMAGESHOW_H

#include <QWidget>
#include <QImage>
#include <QString>
#include <QDebug>
#include "common.h"

namespace Ui {
class ImageShow;
}

class ImageShow : public QWidget
{
    Q_OBJECT

public:
    explicit ImageShow(QWidget *parent = nullptr);
    explicit ImageShow(QString window_name, QWidget *parent = nullptr);
    ~ImageShow();
public slots:
    void draw(QImage qimg);
    void showFps(int fps);
    void showTx(common_utils::TranPkg new_tx);
    void showRx(common_utils::RecvPkg new_rx);
private slots:
    void on_closeButton_clicked();
signals:
    void end_signal();

private:
    Ui::ImageShow *ui;
    int sample_count = 0;
};

#endif // IMAGESHOW_H
