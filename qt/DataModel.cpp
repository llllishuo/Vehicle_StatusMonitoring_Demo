#include "DataModel.h"

Result<struct Time> toTime_from_string(QString str){
    Result<struct Time> res;


    QList<QString> str_list = str.split("T");
    if(str_list.size() != 2){
        res.E = Option::Err;
        return res;
    }
    QString date = str_list[0];
    QString time = str_list[1];

    QList<QString> date_vec = date.split("-");
    if(date_vec.size() != 3){
        res.E = Option::Err;
        return res;
    }

    QList<QString> time_vec = time.split(":");
    if(time_vec.size() != 3){
        res.E = Option::Err;
        return res;
    }

    struct Time temp;
    temp.year = date_vec[0].toUInt();
    temp.month = date_vec[1].toUInt();
    temp.day = date_vec[2].toUInt();
    temp.hour = time_vec[0].toUInt();
    temp.min = time_vec[1].toUInt();
    temp.s = time_vec[2].toDouble();


    res.data = temp;
    res.E = Option::Ok;
    return res;
}

