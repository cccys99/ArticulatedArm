#include "SerialPortManager.h"
#include "robotdialog.h"

SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent) {
    serialPort = new QSerialPort(this);

    // 创建串口管理器
    SerialPortManager = new SerialPortManager(this);

    // 创建 Dialog 实例，用于访问 dialog.ui 中的控件
    RobotDialog = new RobotDialog(this);

    connect(RobotDialog->ui->comboBox_port)
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for(int i = 0; i<list.size(); i++)
    {


    }
}

SerialPortManager::~SerialPortManager() {
    if (serialPort->isOpen()) {
        serialPort->close();
    }
}

void SerialPortManager::initSerialPort(const QString &portName) {
    serialPort->setPortName(portName);
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "串口打开失败!";
        return;
    }
    qDebug() << "串口打开成功";

    // 连接 readyRead 信号
    connect(serialPort, &QSerialPort::readyRead, this, [=]() {
        QByteArray data = serialPort->readAll();
        emit dataReceived(data);  // 发出数据接收信号
    });
}

void SerialPortManager::closeSerialPort() {
    if (serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "串口已关闭";
    }
}

void SerialPortManager::sendData(const QByteArray &data) {
    if (serialPort->isOpen()) {
        serialPort->write(data);
    }
}

QByteArray SerialPortManager::receiveData() {
    QByteArray data;
    if (serialPort->isOpen() && serialPort->waitForReadyRead(500)) {
        data = serialPort->readAll();
    }
    return data;
}
