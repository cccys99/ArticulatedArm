#ifndef ROBOTDIALOG_H
#define ROBOTDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QButtonGroup>
#include "robot3dforddr6form.h"

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

namespace Ui {
    class RobotDialog;
}

class RobotDialog : public QWidget {
    Q_OBJECT // 必须包含这个宏来启用信号和槽机制

public:
    explicit RobotDialog(QWidget *parent = nullptr);

    ~RobotDialog();

private slots:
    void on_btn_openSerial_clicked();
    void recvSLOTS(void);

    void on_btn_closeSerial_clicked();

private:
    Ui::RobotDialog *ui;
    Robot3DForDDR6Form *mRobot3DForDDR6Form;

    QSerialPort *serial; // 串口对象

private:
    void initializeWindow();

};

#endif // ROBOTDIALOG_H
