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
    serial = new QSerialPort;

    //接收数据
    connect(serial, &QSerialPort::readyRead, this, &RobotDialog::recvSLOTS);

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
    serial->setPort(QSerialPortInfo(ui->comboBox_port->currentText()));

    //2.设置波特率
    if(ui->comboBox_baudrate->currentText() == "115200")
    {
        serial->setBaudRate(QSerialPort::Baud115200);
    }
    else if(ui->comboBox_baudrate->currentText() == "57600")
    {
        serial->setBaudRate(QSerialPort::Baud57600);
    }
    else if(ui->comboBox_baudrate->currentText() == "38400")
    {
        serial->setBaudRate(QSerialPort::Baud38400);
    }
    else if(ui->comboBox_baudrate->currentText() == "19200")
    {
        serial->setBaudRate(QSerialPort::Baud19200);
    }
    else if(ui->comboBox_baudrate->currentText() == "9600")
    {
        serial->setBaudRate(QSerialPort::Baud9600);
    }

    //3.设置校验位
    if(ui->comboBox_parity->currentText() == "NONE")
    {
        serial->setParity(QSerialPort::NoParity);
    }
    else if(ui->comboBox_parity->currentText() == "ODD")
    {
        serial->setParity(QSerialPort::OddParity);
    }
    else if(ui->comboBox_parity->currentText() == "EVEN")
    {
        serial->setParity(QSerialPort::EvenParity);
    }
    else if(ui->comboBox_parity->currentText() == "MARK")
    {
        serial->setParity(QSerialPort::MarkParity);
    }
    else if(ui->comboBox_parity->currentText() == "SPACE")
    {
        serial->setParity(QSerialPort::SpaceParity);
    }

    //4.设置停止位
    if(ui->comboBox_stopbits->currentText() == "1")
    {
        serial->setStopBits(QSerialPort::OneStop);
    }
    else if(ui->comboBox_stopbits->currentText() == "1.5")
    {
        serial->setStopBits(QSerialPort::OneAndHalfStop);
    }
    else if(ui->comboBox_stopbits->currentText() == "2")
    {
        serial->setStopBits(QSerialPort::TwoStop);
    }

    //5.打开串口
    bool info = serial->open(QIODevice::ReadWrite);
    if(info == true)
    {
        qDebug()<<"success";
    }
    else
    {
        qDebug()<<"fail";
    }

}

//接收数据的槽函数
void RobotDialog::recvSLOTS()
{
    //1.读取数据
    QByteArray data = serial->readAll();

    //2.直接在下面打印出来 先测试串口配置是否成功
    qDebug() << "接收到的数据：" << data;
}

