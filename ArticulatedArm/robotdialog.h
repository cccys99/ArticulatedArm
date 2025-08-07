#ifndef ROBOTDIALOG_H
#define ROBOTDIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QButtonGroup>
#include "robot3dforddr6form.h"
#include "calibrationdialog.h"

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QtDebug>

namespace Ui {
    class RobotDialog;
}

// 在 robotdialog.h 中
class Robot3DForDDR6Form;  // 前向声明 Robot3DForDDR6Form 类

class RobotDialog : public QWidget {
    Q_OBJECT // 必须包含这个宏来启用信号和槽机制

public:
    explicit RobotDialog(QWidget *parent = nullptr);

    ~RobotDialog();

private slots:
    void on_btn_openSerial_clicked();
    void recvSLOTS(void);

    void on_btn_closeSerial_clicked();
    void on_btn_startMeasurement_clicked();
    void openCalibrationDialog();

public slots:
    void slotUpdateJVarsValue(double value);

signals:
    // 关节Z方向旋转
    void sigJoinValueChanged(int index, double value); // 信号声明

private:
    Ui::RobotDialog *ui;
    Robot3DForDDR6Form *mRobot3DForDDR6Form;  // 使用前向声明的类
    QVector<double> previousValue;  // 用来保存之前的角度值

    QSerialPort *serial; // 串口对象

private:
    void initializeWindow();

};

#endif // ROBOTDIALOG_H
