#include "robotdialog.h"
#include "ui_robotdialog.h"

RobotDialog::RobotDialog(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::RobotDialog){
    ui->setupUi(this);

    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for(int i = 0; i<list.size(); i++)
    {
        ui->comboBox_port->addItem(list.at(i).portName());
    }

    //创建一个串口类对象
    serialPort = new QSerialPort;

    initializeWindow();
}

RobotDialog::~RobotDialog() {
    delete ui;
}

void RobotDialog::initializeWindow() {
    mRobot3DForDDR6Form = new Robot3DForDDR6Form(this);                     //三维模型显示窗口
    ui->verticalLayout_4->addWidget(mRobot3DForDDR6Form);
}

//配置串口数据并打开
void RobotDialog::on_btn_openSerial_clicked()
{
    //1.选择要打开的串口

    //2.设置波特率

    //3.设置校验位

    //4.设置停止位

    //5.打开串口

}

