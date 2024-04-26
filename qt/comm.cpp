#include "comm.h"
#include "ui_comm.h"
Result<struct TempHum_Quaternion> res;

extern struct User user;

comm::comm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::comm)
{
    ui->setupUi(this);
    serialPost_Init();

    timer = new QTimer();
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
    timer->start();

}

comm::~comm()
{
    if(serialPort.isOpen()){
        serialPort.clear();
        serialPort.close();
    }
    delete ui;
}
Option comm::serialPost_Search(){
    serialPortList = *new QList<QSerialPortInfo>;
    for(int i = 0; i <= ui->port_comboBox->count(); i++)
        ui->port_comboBox->removeItem(i);
    const auto infos = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &info : infos)
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.isOpen()){
            serial.clear();
            serial.close();
        }
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->port_comboBox->addItem(info.portName());
            serialPortList.append(info);
            serial.close();
        }
    }
}
Option comm::serialPost_Init(){
    serialPost_Search();
    QStringList baudList;   //波特率
    QStringList parityList; //校验位
    QStringList dataBitsList;   //数据位
    QStringList stopBitsList;   //停止位
    // 波特率    //波特率默认选择下拉第三项：9600
    baudList<<"1200"<<"2400"<<"4800"<<"9600"
           <<"14400"<<"19200"<<"38400"<<"56000"
          <<"57600"<<"115200";
    ui->baudRate_comboBox->addItems(baudList);
    ui->baudRate_comboBox->setCurrentIndex(3);
    // 校验      //校验默认选择无
    parityList<<"NONE"<<"ODD"<<"EVEN";
    ui->checkBits_comboBox->addItems(parityList);
    ui->checkBits_comboBox->setCurrentIndex(0);
    // 数据位      //数据位默认选择8位
    dataBitsList<<"5"<<"6"<<"7"<<"8";
    ui->dataBits_comboBox->addItems(dataBitsList);
    ui->dataBits_comboBox->setCurrentIndex(3);
    // 停止位      //停止位默认选择1位
    stopBitsList<<"1"<<"2";
    ui->stopBits_comboBox->addItems(stopBitsList);
    ui->stopBits_comboBox->setCurrentIndex(0);


    serialPort.setPort(serialPortList[0]);
    serialPort.setBaudRate(9600);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setParity(QSerialPort::NoParity);
}
Option comm::serialPost_ReadData_from_port(){
    res.data.size = 0;

    if(!serialPort.isOpen()){
        return Err;
    }

    QByteArray buffer;

    buffer = serialPort.readAll();

    if(buffer.isEmpty()){
        return Err;
    }
    QString str = QString::fromLocal8Bit(buffer); //支持中文显示
    ui->debug_textEdit->append(str);


    get_data_from_string(str);

}


void comm::on_serialPort_search_but_clicked(){

   serialPost_Search();

}

void comm::on_port_comboBox_activated(int index){
    QComboBox *butBox = ui->port_comboBox;
    QString text = butBox->currentText();
    QSerialPortInfo info;
    for(QSerialPortInfo &i: serialPortList){
        if(i.portName() == text){
           info = i;
           qDebug()<<"[Debug] 串口已匹配: "<< i.portName()<<"\n";
           break;
        }
    }
    serialPort.setPort(info);
}


void comm::on_baudRate_comboBox_activated(int index){

    QComboBox *butBox = ui->baudRate_comboBox;
    QString text = butBox->currentText();

    qDebug()<<"[Debug] 波特率: "<< text<< "\n";
    serialPort.setBaudRate(text.toInt());
}


void comm::on_dataBits_comboBox_activated(int index){
    QComboBox *butBox = ui->dataBits_comboBox;
    QString text = butBox->currentText();

    switch (text.toInt()) {
    case 8: serialPort.setDataBits(QSerialPort::Data8); break;
    case 7: serialPort.setDataBits(QSerialPort::Data7); break;
    case 6: serialPort.setDataBits(QSerialPort::Data6); break;
    case 5: serialPort.setDataBits(QSerialPort::Data5); break;
    default:
        break;
    }

    qDebug()<<"[Debug] 数据位: "<< serialPort.dataBits()<<"\n";


}


void comm::on_stopBits_comboBox_activated(int index){
    QComboBox *butBox = ui->stopBits_comboBox;
    QString text = butBox->currentText();

    switch (text.toInt()) {
    case 1: serialPort.setStopBits(QSerialPort::OneStop); break;
    case 2: serialPort.setStopBits(QSerialPort::TwoStop); break;
    default:
        break;
    }
    qDebug()<<"[Debug] 停止位: "<< serialPort.stopBits()<<"\n";



}


void comm::on_checkBits_comboBox_activated(int index){
    QComboBox *butBox = ui->checkBits_comboBox;
    QString text = butBox->currentText();

    if(text == "NONE"){
        serialPort.setParity(QSerialPort::NoParity);
    }else if(text == "ODD"){
        serialPort.setParity(QSerialPort::OddParity);
    }else if(text == "EVEN"){
        serialPort.setParity(QSerialPort::EvenParity);
    }else{
        serialPort.setParity(QSerialPort::NoParity);
    }


    qDebug()<<"[Debug] 检验位: "<< serialPort.parity()<<"\n";

}


void comm::on_serialPort_open_but_clicked(){
    if(serialPort.isOpen()){
        serialPort.clear();
        serialPort.close();

        qDebug()<<"[Debug] 串口已关闭: "<< serialPort.isOpen()<<"\n";
        ui->debug_textEdit->append("串口已关闭...\n");
        ui->serialPort_open_but->setText("打开串口");
        return;
    }

    if(!serialPort.open(QIODeviceBase::ReadWrite)){
        QMessageBox::warning(this, "设备打开提示","串口设备打开失败！");
        return;
    }
    qDebug()<<"[Debug] 串口已打开: "<< serialPort.isOpen()<<"\n";
    ui->debug_textEdit->append("串口已打开...\n");
    ui->serialPort_open_but->setText("关闭串口");
}


void comm::onTimeout(){
    //qDebug()<<"[Debug] timer...\n";
    serialPost_ReadData_from_port();
}
Option comm::get_value_from_string(QString str){
    QStringList str_list = str.split(":");
    str_list[0].replace("\n","");
    str_list[0].replace("\r","");
    str_list[1].replace("\n","");
    str_list[1].replace("\r","");
    QString str_lower = str_list[0].toLower();
    if(str_lower == "pitch"){
        res.data.size++;
        res.data.qua.pitch = str_list[1];
    }else if(str_lower == "roll"){
        res.data.size++;
        res.data.qua.roll = str_list[1];
    }else if(str_lower == "yaw"){
        res.data.size++;
        res.data.qua.yaw = str_list[1];
    }else if(str_lower == "accelx"){
        res.data.size++;
        res.data.qua.accelX = str_list[1];
    }else if(str_lower == "accely"){
        res.data.size++;
        res.data.qua.accelY = str_list[1];
    }else if(str_lower == "accelz"){
        res.data.size++;
        res.data.qua.accelZ = str_list[1];
    }else if(str_lower == "gyrox"){
        res.data.size++;
        res.data.qua.gyroX = str_list[1];
    }else if(str_lower == "gyroy"){
        res.data.size++;
        res.data.qua.gyroY = str_list[1];
    }else if(str_lower == "gyroz"){
        res.data.size++;
        res.data.qua.gyroZ = str_list[1];
    }else if(str_lower == "temp"){
        res.data.size++;
        res.data.temp_hum.temp = str_list[1];
    }else if(str_lower == "hum"){
        res.data.size++;
        res.data.temp_hum.hum = str_list[1];
    }else{

    }

    if(res.data.size == 11){
        res.E = Option::Ok;
    }
    return Ok;
}


Option comm::get_data_from_string(QString str){
    QStringList str_list = str.split("\n");

    foreach (QString item, str_list) {
        if(item == "start" || item == ""){
            continue;
        }
        if(item == "over"){
            break;
        }
        get_value_from_string(item);
    }

    if(res.E == Ok){

        ui->pitch_value_label->setText(res.data.qua.pitch);
        ui->roll_value_label->setText(res.data.qua.roll);
        ui->yaw_value_label->setText(res.data.qua.yaw);
        ui->accel_x_value_label->setText(res.data.qua.accelX);
        ui->accel_y_value_label->setText(res.data.qua.accelY);
        ui->accel_z_value_label->setText(res.data.qua.accelZ);
        ui->gyro_x_value_label->setText(res.data.qua.gyroX);
        ui->gyro_y_value_label->setText(res.data.qua.gyroY);
        ui->gyro_z_value_label->setText(res.data.qua.gyroZ);
        ui->temp_value_label->setText(res.data.temp_hum.temp);
        ui->hum_value_label->setText(res.data.temp_hum.hum);
        res.E = Err;
        res.data.size = 0;

        db_insert_temp_and_hum(res.data.temp_hum);
        db_insert_quaternion(res.data.qua);
    }

    return Option::Ok;
}

