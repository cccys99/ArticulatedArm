#include "robotdialog.h"
#include "ui_robotdialog.h"

RobotDialog::RobotDialog(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::RobotDialog){
    ui->setupUi(this);


    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for(int i = 0; i<list.size(); i++){
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
void RobotDialog::on_btn_openSerial_clicked(){
    // 如果串口已打开，先关闭它
    if (serial->isOpen()) {
        serial->close();
        qDebug() << "先关闭已有串口连接";
    }

    //1.选择要打开的串口
    serial->setPort(QSerialPortInfo(ui->comboBox_port->currentText()));

    //2.设置波特率
    if(ui->comboBox_baudrate->currentText() == "115200"){
        serial->setBaudRate(QSerialPort::Baud115200);
    }
    else if(ui->comboBox_baudrate->currentText() == "57600"){
        serial->setBaudRate(QSerialPort::Baud57600);
    }
    else if(ui->comboBox_baudrate->currentText() == "38400"){
        serial->setBaudRate(QSerialPort::Baud38400);
    }
    else if(ui->comboBox_baudrate->currentText() == "19200"){
        serial->setBaudRate(QSerialPort::Baud19200);
    }
    else if(ui->comboBox_baudrate->currentText() == "9600"){
        serial->setBaudRate(QSerialPort::Baud9600);
    }

    //3.设置校验位
    if(ui->comboBox_parity->currentText() == "NONE"){
        serial->setParity(QSerialPort::NoParity);
    }
    else if(ui->comboBox_parity->currentText() == "ODD"){
        serial->setParity(QSerialPort::OddParity);
    }
    else if(ui->comboBox_parity->currentText() == "EVEN"){
        serial->setParity(QSerialPort::EvenParity);
    }
    else if(ui->comboBox_parity->currentText() == "MARK"){
        serial->setParity(QSerialPort::MarkParity);
    }
    else if(ui->comboBox_parity->currentText() == "SPACE"){
        serial->setParity(QSerialPort::SpaceParity);
    }

    //4.设置停止位
    if(ui->comboBox_stopbits->currentText() == "1"){
        serial->setStopBits(QSerialPort::OneStop);
    }
    else if(ui->comboBox_stopbits->currentText() == "1.5"){
        serial->setStopBits(QSerialPort::OneAndHalfStop);
    }
    else if(ui->comboBox_stopbits->currentText() == "2"){
        serial->setStopBits(QSerialPort::TwoStop);
    }

    //5.打开串口
    if (serial->open(QIODevice::ReadWrite)){
        qDebug() << "串口打开成功";
    } else{
        qDebug() << "串口打开失败";
    }
}

//接收数据的槽函数
void RobotDialog::recvSLOTS(){
    //1.读取数据
    QByteArray data = serial->readAll();

    //2.直接在下面打印出来 先测试串口配置是否成功
    qDebug() << "接收到的数据：" << data;

    // 校验帧头和帧尾
   if (static_cast<unsigned char>(data.at(0)) == 0xAA && static_cast<unsigned char>(data.at(data.size() - 2)) == 0xFF){
        qDebug() << "帧头帧尾校验通过";

        // 校验和计算（数据从第1字节开始，到倒数第二个字节结束）
        uint8_t checksum = 0;
        for (int i = 1; i < data.size() - 2; ++i){
            checksum += data[i];  // 求和
        }
        checksum = checksum & 0xFF;  // 取低8位
        if (checksum == data[data.size() - 1]){
            qDebug() << "校验通过";
            // 解析每个设备的数据
            for (int i = 1; i <= 6; i++){
                uint8_t id = data[i * 3 - 2];    // id (0x01 到 0x06)
                uint8_t high = data[i * 3 - 1];  // 编码器数据高8位
                uint8_t low = data[i * 3];       // 编码器数据低8位
                uint16_t encoderData = (high << 8) | low; // 拼接数据

                qDebug() << "ID:" << id << "编码器数据：" << encoderData;

                // 更新对应的控件（比如 spinBox_joint1, spinBox_joint2 等）
                switch (id){
                case 0x01:
                    ui->spinBox_joint1->setValue(encoderData);  // 设置对应的 SpinBox
                    break;
                case 0x02:
                    ui->spinBox_joint2->setValue(encoderData);
                    break;
                case 0x03:
                    ui->spinBox_joint3->setValue(encoderData);
                    break;
                case 0x04:
                    ui->spinBox_joint4->setValue(encoderData);
                    break;
                case 0x05:
                    ui->spinBox_joint5->setValue(encoderData);
                    break;
                case 0x06:
                    ui->spinBox_joint6->setValue(encoderData);
                    break;
                default:
                    break;
                }
            }
        } else{
            qDebug() << "校验失败";
        }
    } else{
        qDebug() << "帧头帧尾不匹配";
    }
}


void RobotDialog::on_btn_closeSerial_clicked(){
    if (serial->isOpen()){
        serial->close();
        qDebug() << "串口已关闭";
    } else{
    qDebug() << "串口未打开，无法关闭";
    }
}

