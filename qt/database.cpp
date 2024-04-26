#include "database.h"



Option db_Init(){
    database = QSqlDatabase::addDatabase("QODBC");
    database.setDatabaseName(DB_NAME);
    bool ok = database.open();
    if(!ok) {
        qDebug()<<"[Err] error open database because: "<<database.lastError().text()<<"\n";
        return Err;
    }

    return Ok;
}

unsigned int db_select_username_count(QString username){
    QSqlQuery qry(database);
    QString sql = "select count(*) from vs_user where username = :u";

    qry.prepare(sql);
    qry.bindValue(":u", username);
    qry.exec();

    if(!qry.next()){
        return 0;
    }

    unsigned int count = qry.value(0).toInt();

    qDebug()<<"[Debug] "<< username<< " count: "<< count <<"\n";

    return count;

}
Option db_username_is_have(QString username){
    if(db_select_username_count(username) == 0){
        return Err;
    }

    return Ok;

}

void db_set_user(struct User u){
    user = u;

}
Result<struct User> db_select_user_where_username_password(QString username, QString password, Id_Status login_status){
    Result<struct User> result;
    if(db_select_user_isRoot(username) == Option::Err && login_status == Id_Status::Root){
        result.E = Option::Err;
        result.msg = "当前用户没有管理员权限!";
        return result;
    }

    if(db_username_is_have(username) == Option::Err){
        result.E = Option::Err;
        result.msg = "用户不存在!请注册!";
        return result;
    }


    struct User user;
    QSqlQuery qry(database);
    QString sql = "select id, username, status, is_root from vs_user where username = :u and password = :p and is_root = :r";

    qry.prepare(sql);
    qry.bindValue(":u", username);
    qry.bindValue(":p", password);
    qry.bindValue(":r", login_status);
    qry.exec();

    if(!qry.next()){
        result.E = Option::Err;
        result.msg = "用户名或者密码错误!";
        return result;
    }


    user.id = qry.value("id").toString();
    user.username = qry.value("username").toString();
    switch (qry.value("status").toInt()) {
    case 0:
        user.status = Login_Status::Offline;
        break;
    case 1:
        user.status = Login_Status::Online;
    default:
        break;
    }

    switch (qry.value("is_root").toInt()) {
    case 0:
        user.isRoot = Id_Status::User;
        break;
    case 1:
        user.isRoot = Id_Status::Root;
    default:
        break;
    }


    result.data = user;
    result.E = Option::Ok;
    return result;

}


Option db_select_user_isRoot(QString username){
    QSqlQuery qry(database);
    QString sql = "select is_root from vs_user where username = :u";

    qry.prepare(sql);
    qry.bindValue(":u", username);
    qry.exec();

    if(!qry.next()){
        return Err;
    }

    if(qry.value("is_root").toInt() == 0){
        return Err;
    }

    return Ok;
}




Result<struct User> db_insert_user(QString username, QString password){

    Result<struct User> res;

    if(db_username_is_have(username) == Option::Ok){
        res.msg = "用户名已存在!";
        res.E = Option::Err;
        return res;
    }

    QString uuid = QUuid::createUuid().toString();
    uuid.remove("{").remove("}").remove("-");


    QSqlQuery qry(database);
    QString sql = "insert into vs_user values( :i, :u, :p, 0, 0)";

    qry.prepare(sql);
    qry.bindValue(":i", uuid);
    qry.bindValue(":u", username);
    qry.bindValue(":p", password);
    bool re = qry.exec();

    if(!re){
        res.E = Option::Err;
        return res;
    }

    res.E = Option::Ok;
    return res;
}


Result<struct TempHum> db_insert_temp_and_hum(struct TempHum th){
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString time = dateTime .toString("yyyy-MM-dd hh:mm:ss");//格式化时间
    Result<struct TempHum> res;

    QString uuid = QUuid::createUuid().toString();
    uuid.remove("{").remove("}").remove("-");


    QSqlQuery qry(database);
    QString sql = "insert into vs_tempHum values( :i, :u, :t, :h, :time)";

    qry.prepare(sql);
    qry.bindValue(":i", uuid);
    qry.bindValue(":u", user.id);
    qry.bindValue(":t", th.temp);
    qry.bindValue(":h", th.hum);
    qry.bindValue(":time", time);
    qDebug()<<"[Debug] insert temp and hum to user: "<<user.id<<"\n";
    bool re = qry.exec();

    if(!re){
        res.msg = "温湿度添加失败!";
        res.E = Option::Err;
        return res;
    }

    res.data = th;
    res.data.id = uuid;
    res.data.user_id = user.id;
    res.data.time = time;
    res.E = Option::Ok;
    return res;
}
Result<struct Quaternion> db_insert_quaternion(struct Quaternion qua){
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString time = dateTime .toString("yyyy-MM-dd hh:mm:ss");//格式化时间
    Result<struct Quaternion> res;
    struct Quaternion temp;

    QString uuid = QUuid::createUuid().toString();
    uuid.remove("{").remove("}").remove("-");


    QSqlQuery qry(database);
    QString sql = "insert into vs_quaternion values( :i, :u, :p, :r, :y, :ax, :ay, :az, :gx, :gy, :gz, :t)";

    qry.prepare(sql);
    qry.bindValue(":i", uuid);
    qry.bindValue(":u", user.id);
    qry.bindValue(":p", qua.pitch);
    qry.bindValue(":r", qua.roll);
    qry.bindValue(":y", qua.yaw);
    qry.bindValue(":ax", qua.accelX);
    qry.bindValue(":ay", qua.accelY);
    qry.bindValue(":az", qua.accelZ);
    qry.bindValue(":gx", qua.gyroX);
    qry.bindValue(":gy", qua.gyroY);
    qry.bindValue(":gz", qua.gyroZ);
    qry.bindValue(":t", time);
    bool re = qry.exec();

    if(!re){
        res.msg = "三元数据添加失败!";
        res.E = Option::Err;
        return res;
    }

    res.data = qua;
    res.data.id = uuid;
    res.data.user_id = user.id;
    res.data.time = time;

    res.E = Option::Ok;
    return res;

}

Result<QVector<struct TempHum>> db_select_vec_temp_and_hum_where_userId(QString userId){

    Result<QVector<struct TempHum>> res;
    QVector<struct TempHum> temp_hum_vec;
    QSqlQuery qry(database);
    QString sql = "select id, temp, hum, time from vs_tempHum where user_id = :i";
    //QString sql = "select id, temp, hum, time from vs_tempHum";

    qry.prepare(sql);
    qry.bindValue(":i", userId);
    bool re = qry.exec();
    if(!re){
        res.msg = "温湿度查询失败!";
        res.E = Option::Err;
        return res;
    }
    unsigned int i = 0;
    for(i = 0; qry.next(); i++){
        struct TempHum temp_hum;
        temp_hum.id = qry.value("id").toString();
        temp_hum.temp = qry.value("temp").toString();
        temp_hum.hum = qry.value("hum").toString();
        temp_hum.time = qry.value("time").toString();
        temp_hum_vec.append(temp_hum);
    }
    res.data = temp_hum_vec;
    res.E = Option::Ok;
    return res;

}











