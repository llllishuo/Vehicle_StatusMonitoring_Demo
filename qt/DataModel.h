#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QString>
#include <QList>
#include <QObject>

enum Id_Status{User, Root};
enum Option{Err, Ok};
enum Login_Status{Offline, Online};

template<typename  T>
struct Result{
    T data;
    QString msg;
    Option E;
};

struct User{
    QString id;
    QString username;
    Login_Status status;
    Id_Status isRoot;
};

struct UserResp{
    QString username;
    QString password;
    Option E;
};


struct TempHum{
    QString id;
    QString user_id;
    QString temp;
    QString hum;
    QString time;
};
struct Quaternion{
    QString id;
    QString user_id;
    QString pitch;
    QString roll;
    QString yaw;
    QString accelX;
    QString accelY;
    QString accelZ;
    QString gyroX;
    QString gyroY;
    QString gyroZ;
    QString time;
};

struct TempHum_Quaternion{
    struct TempHum temp_hum;
    struct Quaternion qua;
    unsigned int size;
};

struct Time{
    unsigned int year;
    unsigned int month;
    unsigned int day;
    unsigned int hour;
    unsigned int min;
    double s;
    unsigned int stamp;

};
Result<struct Time> toTime_from_string(QString str);


unsigned int is_leap_year(uint16_t year);

unsigned int time_to_stamp(struct Time date);




#endif // DATAMODEL_H
