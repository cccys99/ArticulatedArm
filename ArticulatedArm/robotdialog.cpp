#include "robotdialog.h"
#include "ui_robotdialog.h"

RobotDialog::RobotDialog(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::RobotDialog){
    ui->setupUi(this);

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
     if (data[0]== 0xAA){
            qDebug() << "帧头校验通过";

        // 校验和计算
        uint8_t checksum = 0;
        for (unsigned int i = 0; i < static_cast<unsigned int>(Data.size()) - 1; ++i){
            checksum += data[i];  // 求和
        }
        checksum = checksum & 0xFF;  // 取低8位
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
            // 解析每个设备的数据
            for (int i = 1; i <= 6; i++){
                uint8_t id = data[i * 3 - 2];    // id (0x01 到 0x06)
                uint8_t high = data[i * 3 - 1];  // 编码器数据高8位
                uint8_t low = data[i * 3];       // 编码器数据低8位
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
    } else{
    qDebug() << "串口未打开，无法关闭";
    }
}

void RobotDialog::slotUpdateJVarsValue(double value) {
    QDoubleSpinBox *box = qobject_cast<QDoubleSpinBox *>(sender());
    if (!box) return;

    QString name = box->objectName();  // 如 "spinBox_joint4"
    QString indexStr = name.right(1);  // 获取最后一个数字
    int index = indexStr.toInt();

    qDebug() << "Joint" << index << "value changed to" << value;
//    // 如果当前值与之前的值不相等，才发射信号
//    if (value != previousValue[index - 1]) {  // 索引从0开始，所以需要减去1
//        previousValue[index - 1] = value;  // 更新之前的值
//        emit sigJoinValueChanged(index, value);  // 发射信号
//    }
    emit sigJoinValueChanged(index, value);  // 发射信号
}

void RobotDialog::slotDebugRobotConfig(double value) {
//    QDoubleSpinBox *dsb = (QDoubleSpinBox *) sender();
//    QString objectName = dsb->objectName();
//    QString index = objectName.at(objectName.size() - 1);

//    if (objectName.contains("doubleSpinBox_d")) {
//        emit sigDValueChanged(index.toInt(), value);
//    } else if (objectName.contains("doubleSpinBox_JVars")) {
//        emit sigJoinValueChanged(index.toInt(), value);
//    } else if (objectName.contains("doubleSpinBox_alpha")) {
//        emit sigAlphaValueChanged(index.toInt(), value);
//    } else if (objectName.contains("doubleSpinBox_a")) {
//        emit sigAValueChanged(index.toInt(), value);
//    }
    qDebug() << "slotDebugRobotConfig  value:"<< value;
}


