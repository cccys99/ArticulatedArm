#include "robotdialog.h"
#include "ui_robotdialog.h"

RobotDialog::RobotDialog(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::RobotDialog){
    ui->setupUi(this);

    // 初始化时，禁用 "关闭串口" 按钮，启用 "打开串口" 按钮
    ui->btn_closeSerial->setEnabled(false);  // 禁用关闭按钮
    ui->btn_openSerial->setEnabled(true);    // 启用打开按钮

    // 使用 QDoubleSpinBox 数组
    QDoubleSpinBox* spinBoxes[] = { ui->spinBox_joint1, ui->spinBox_joint2, ui->spinBox_joint3,
                                   ui->spinBox_joint4, ui->spinBox_joint5, ui->spinBox_joint6 };

    // 使用循环给每个 QDoubleSpinBox 设置属性
    for (int i = 0; i < 6; ++i) {
        spinBoxes[i]->setMaximum(36000); // 设置最大值为 36000
        spinBoxes[i]->setDecimals(2); // 设置小数点位数为2
        spinBoxes[i]->setSuffix("°"); // 设置单位符号为 "°"
    }

    //6个spinbox值变化的信号和槽函数连接
    QList<QDoubleSpinBox *> spinBoxList = ui->groupBox_Angle->findChildren<QDoubleSpinBox *>();
    for (QDoubleSpinBox *box : spinBoxList) {
        box->setMinimum(0.0);
        box->setMaximum(360.0);
        box->setSingleStep(0.1);
        box->setDecimals(2);
        box->setValue(180.0);  // 初始值
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &RobotDialog::slotUpdateJVarsValue,
                Qt::UniqueConnection);
    }

    // 初始化 previousValue 数组，存储6个关节的角度
    previousValue.resize(6);           // 调整大小为 6
    previousValue.fill(180.0);         // 用 180.0 填充数组中的每个元素

    // 设置9600为默认选择（假设它的文本是 "9600"）
    ui->comboBox_baudrate->setCurrentText("9600");
    // 设置8位数据位为默认选择（假设它的文本是 "8位"）
    ui->comboBox_databits->setCurrentText("8");
    // 设置1位停止位为默认选择（假设它的文本是 "1"）
    ui->comboBox_stopbits->setCurrentText("1");
    // 设置无校验位为默认选择（假设它的文本是 "None"）
    ui->comboBox_parity->setCurrentText("None");

    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    for(int i = 0; i<list.size(); i++){
        ui->comboBox_port->addItem(list.at(i).portName());
    }

    //创建一个串口类对象
    serial = new QSerialPort;

    //接收数据
    connect(serial, &QSerialPort::readyRead, this, &RobotDialog::recvSLOTS);

    //开始测量
    connect(ui->btn_startMeasurement, &QPushButton::clicked, this, &RobotDialog::on_btn_startMeasurement_clicked);

    initializeWindow();
}

RobotDialog::~RobotDialog() {
    delete ui;
}

void RobotDialog::initializeWindow() {
    mRobot3DForDDR6Form = new Robot3DForDDR6Form(this);//三维模型显示窗口
    ui->verticalLayout_4->addWidget(mRobot3DForDDR6Form);
}

//配置串口数据并打开
void RobotDialog::on_btn_openSerial_clicked(){
    // 如果串口已打开，先关闭它
    if (serial->isOpen()) {
        serial->close();
        qDebug() << "先关闭已有串口连接";
    }

    // 串口连接成功时，禁用 "打开串口" 按钮，启用 "关闭串口" 按钮
    ui->btn_openSerial->setEnabled(false);  // 禁用打开按钮
    ui->btn_closeSerial->setEnabled(true);  // 启用关闭按钮

    //1.选择要打开的串口
    serial->setPort(QSerialPortInfo(ui->comboBox_port->currentText()));

    //2.设置波特率
    //serial->setBaudRate(QSerialPort::Baud9600);

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

    //3.设置数据位
    if(ui->comboBox_databits->currentText() == "8"){
         serial->setDataBits(QSerialPort::Data8);  // 设置 8 位数据位
    }
    else if(ui->comboBox_databits->currentText() == "7"){
         serial->setDataBits(QSerialPort::Data7);  // 设置 7 位数据位
    }
    else if(ui->comboBox_databits->currentText() == "6"){
         serial->setDataBits(QSerialPort::Data6);  // 设置 6 位数据位
    }
    else if(ui->comboBox_databits->currentText() == "5"){
         serial->setDataBits(QSerialPort::Data5);  // 设置 5 位数据位
    }

    //4.设置校验位
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

    //5.设置停止位
    if(ui->comboBox_stopbits->currentText() == "1"){
        serial->setStopBits(QSerialPort::OneStop);
    }
    else if(ui->comboBox_stopbits->currentText() == "1.5"){
        serial->setStopBits(QSerialPort::OneAndHalfStop);
    }
    else if(ui->comboBox_stopbits->currentText() == "2"){
        serial->setStopBits(QSerialPort::TwoStop);
    }

    //6.打开串口
    if (serial->open(QIODevice::ReadWrite)){
        qDebug() << "串口打开成功";
    } else{
        qDebug() << "串口打开失败";
    }
}

//接收数据的槽函数
void RobotDialog::recvSLOTS(){
    //1.读取数据
    QByteArray Data = serial->readAll();

    // 将QByteArray转换为uint8_t数组
    uint8_t* data = reinterpret_cast<uint8_t*>(Data.data());

    //2.直接在下面打印出来 先测试串口配置是否成功
    qDebug() << "接收到的数据：";
    for (int i = 0; i < Data.size(); ++i) {
        qDebug() << "Byte" << i << ": 0x" << QString::number(data[i], 16).toUpper();  // 打印每个字节的十六进制值
    }
    // 校验帧头
     if (data[0]== 0x80 || data[0]== 0x81){
         qDebug() << "帧头校验通过";

        // 校验和计算
        uint8_t checksum = 0;
        for (unsigned int i = 0; i < static_cast<unsigned int>(Data.size()) - 1; ++i){
            checksum += data[i];  // 求和
        }
        checksum &= 0xFF;  // 取低8位
        qDebug() << "计算出的Checksum:0x" << QString::number(checksum, 16).toUpper(); // 打印计算出的校验和

        // 确保串口打开且可写 向下位机发送我计算的校验和
        if (serial->isOpen() && serial->isWritable()) {
            // 将 uint8_t checksum 转换为 QByteArray
            QByteArray checksumData;
            checksumData.append(static_cast<char>(checksum)); // 转换为字节流
            qDebug() << "发送校验和：" << checksumData.toHex();
            serial->write(checksumData);  // 发送校验和
        } else {
            qDebug() << "串口未打开或不可写！";
        }

        //开始解析数据
        if (checksum == data[ static_cast<unsigned int>(Data.size()) - 1]){
            qDebug() << "校验通过";

            // 跳过10字节的测头数据
            int offset = 11; // 帧头 + 10字节测头数据

            // 解析每个设备的数据
            for (int i = 0; i < 6; i++){
                uint8_t id = data[offset + i * 3];    // id (0x01 到 0x06)
                uint8_t high = data[offset + i * 3 + 1];  // 编码器数据高8位
                uint8_t low = data[offset + i * 3 + 2];       // 编码器数据低8位
                uint16_t encoderData = (high << 8) | low; // 拼接数据

                qDebug() << "ID:" << id << "编码器数据：" << encoderData;

                // 将编码器数据转换为角度值（单位：度）
                // 假设最大编码器值为 36000，最大角度为 360.00 度
                float angle = (encoderData / 100.0f);  // 右移两位转换为角度

                // 更新对应的控件（比如 spinBox_joint1, spinBox_joint2 等）
                switch (id){
                case 0x01:
                    ui->spinBox_joint1->setValue(angle);  // 设置对应的 SpinBox
                    break;
                case 0x02:
                    ui->spinBox_joint2->setValue(angle);
                    break;
                case 0x03:
                    ui->spinBox_joint3->setValue(angle);
                    break;
                case 0x04:
                    ui->spinBox_joint4->setValue(angle);
                    break;
                case 0x05:
                    ui->spinBox_joint5->setValue(angle);
                    break;
                case 0x06:
                    ui->spinBox_joint6->setValue(angle);
                    break;
                default:
                    break;
                }
            }
        } else{
            qDebug() << "校验失败";
        }
    } else{
        qDebug() << "帧头不匹配";
    }
}

void RobotDialog::on_btn_closeSerial_clicked(){
    if (serial->isOpen()){
        serial->close();
        qDebug() << "串口已关闭";

        // 串口关闭时，禁用关闭按钮，启用打开按钮
        ui->btn_openSerial->setEnabled(true);  // 启用打开按钮
        ui->btn_closeSerial->setEnabled(false);  // 禁用关闭按钮
    }
}

void RobotDialog::slotUpdateJVarsValue(double value) {
    QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox *>(sender());
    if (!box) return;

    QString name = box->objectName();  // 如 "spinBox_joint4"
    QString indexStr = name.right(1);  // 获取最后一个数字
    int index = indexStr.toInt();

    qDebug() << "Joint" << index << "value changed to" << value;
    emit sigJoinValueChanged(index, value);  // 发射信号
}

void RobotDialog::on_btn_startMeasurement_clicked() {
    qDebug() << "开始测量按钮被点击";

    // 发送开始测量的命令
    QByteArray command;
    command.append(0xAA);  // 起始命令字节
    command.append(0x44);  // 后续的命令字节

    if (serial->isOpen() && serial->isWritable()) {
        serial->write(command);  // 通过串口发送命令
        qDebug() << "发送开始测量命令: " << command.toHex();
    } else {
        qDebug() << "串口未打开或不可写！";
        return;
    }

    // 等待下位机返回数据并解析
    if (serial->waitForReadyRead(5000)) {  // 等待 5 秒
        QByteArray receivedData = serial->readAll();  // 读取数据
        uint8_t* data = reinterpret_cast<uint8_t*>(receivedData.data());

        // 打印接收到的每个字节
        qDebug() << "接收到的数据：";
        for (int i = 0; i < receivedData.size(); ++i) {
            qDebug() << "Byte" << i << ": 0x" << QString::number(data[i], 16).toUpper();
        }

        // 校验帧头
        if (data[0] == 0x80) {
            qDebug() << "帧头校验通过";

            // 校验和计算
            uint8_t checksum = 0;
            for (unsigned int i = 0; i < static_cast<unsigned int>(receivedData.size()) - 1; ++i) {
                checksum += data[i];  // 求和
            }
            checksum &= 0xFF;  // 取低8位
            qDebug() << "计算出的Checksum:0x" << QString::number(checksum, 16).toUpper(); //打印计算出的校验和

            // 校验和比对
            if (checksum == data[static_cast<unsigned int>(receivedData.size()) - 1]) {
                qDebug() << "校验通过";

                // 跳过10字节的测头数据，从第12字节开始解析
                int offset = 11;  // 测头数据的结束字节是 Byte 10，数据从 Byte 11 开始

                // 获取六个关节的角度
                float jointAngles[6];
                for (int i = 0; i < 6; i++) {
                    uint8_t id = data[offset + i * 3];       // 获取ID（每3个字节为一组）
                    uint8_t high = data[offset + i * 3 + 1]; // 获取编码器数据高位
                    uint8_t low = data[offset + i * 3 + 2];  // 获取编码器数据低位
                    uint16_t encoderData = (high << 8) | low; // 拼接编码器数据

                    qDebug() << "ID:" << id << "编码器数据：" << encoderData;

                    // 将编码器数据转换为角度值（单位：度）
                    jointAngles[i] = (encoderData / 100.0f);  // 假设最大编码器值为 36000，转换为角度
                }

                // 假设测头数据是10个字节，我们可以提取相关数据
                float probeData[10];
                for (int i = 0; i < 10; ++i) {
                    probeData[i] = data[offset + 6 * 3 + i]; // 10字节测头数据的提取
                }

                // 调用正运动学计算函数
               // EndEffectorPose pose = calculateForwardKinematics(jointAngles, probeData);

                // 打印计算得到的末端坐标
               // qDebug() << "末端坐标: X=" << pose.x << ", Y=" << pose.y << ", Z=" << pose.z;

//                // 将末端坐标显示到 label_endPose 中
//                QString poseText = QString("X: %.2f, Y: %.2f, Z: %.2f").arg(pose.x).arg(pose.y).arg(pose.z);
//                ui->label_endPose->setText(poseText);  // 设置 QLabel 的文本
            } else {
                qDebug() << "校验失败";
            }
        } else {
            qDebug() << "帧头不匹配";
        }
    } else {
        qDebug() << "等待数据超时，未接收到有效数据";
    }
}


