#include "DataModel.h"
const unsigned int month_days_table[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

Result<struct Time> toTime_from_string(QString str){
    Result<struct Time> res;

    str.replace("T"," ");

    QList<QString> str_list = str.split(" ");
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
    temp.stamp = time_to_stamp(temp);

    res.data = temp;
    res.E = Option::Ok;
    return res;
}
unsigned int is_leap_year(uint16_t year)
{
    return (((year % 4 == 0)&&(year % 100 != 0)) || (year % 400 == 0));
}


unsigned int time_to_stamp(struct Time date){
    static  uint32_t dax = 0;
    static  uint32_t day_count = 0;
    uint16_t leap_year_count = 0;
    uint16_t i;

    // 计算闰年数
    for (i = 1970; i < date.year; i++)
    {
        if (is_leap_year(i))
        {
            leap_year_count++;
        }
    }

    // 计算年的总天数
    day_count = leap_year_count * 366 + (date.year - 1970 - leap_year_count) * 365;

    // 累加计算当年所有月的天数
    for (i = 1; i < date.month; i++)
    {
        if ((2 == i) && (is_leap_year(date.year)))
        {
            day_count += 29;
        }
        else
        {
            day_count += month_days_table[i];
        }
    }

    // 累加计算当月的天数
    day_count += (date.day - 1);

    dax = (uint32_t)(day_count * 86400) + (uint32_t)((uint32_t)date.hour * 3600) + (uint32_t)((uint32_t)date.min * 60) + (uint32_t)date.s;

    /* 北京时间补偿 */
    dax = dax - 8*60*60;

    return dax;
}

