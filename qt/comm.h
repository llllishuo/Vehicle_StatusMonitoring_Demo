#ifndef COMM_H
#define COMM_H

#include <QWidget>
#include "DataModel.h"
#include "database.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include <QMessageBox>
#include <QTimer>
#include <QDateTime>

extern struct User user;

namespace Ui {
class comm;
}

class comm : public QWidget
{
    Q_OBJECT

public:
    explicit comm(QWidget *parent = nullptr);
    ~comm();
    Option serialPost_Init();
    Option serialPost_Search();
    Option serialPost_ReadData_from_port();

private slots:
    void on_port_comboBox_activated(int index);

    void on_serialPort_search_but_clicked();

    void on_baudRate_comboBox_activated(int index);

    void on_dataBits_comboBox_activated(int index);

    void on_stopBits_comboBox_activated(int index);

    void on_checkBits_comboBox_activated(int index);

    void on_serialPort_open_but_clicked();

    void onTimeout();


private:
    Ui::comm *ui;
    QSerialPort serialPort;
    QList<QSerialPortInfo> serialPortList;
    QTimer *timer;
    QString temp_value;
    QString hum_value;


    Option get_data_from_string(QString str);
    Option get_value_from_string(QString str);
};

#endif // COMM_H
