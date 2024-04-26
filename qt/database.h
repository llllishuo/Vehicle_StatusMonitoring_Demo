#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include "DataModel.h"
#include <QUuid>
#include <QTimer>
#include <QDateTime>


#define DB_NAME "menghanstudio"

extern struct User user;
static QSqlDatabase database;


Option db_Init();

Option db_username_is_have(QString username);

void db_set_user(struct User u);

unsigned int db_select_username_count(QString username);

Option db_select_user_isRoot(QString username);

Result<struct User> db_select_user_where_username_password(QString username, QString passworld, Id_Status login_status);

Result<struct User> db_insert_user(QString username, QString password);

Result<struct TempHum> db_insert_temp_and_hum(struct TempHum th);
Result<struct Quaternion> db_insert_quaternion(struct Quaternion qua);

Result<QVector<struct TempHum>> db_select_vec_temp_and_hum_where_userId(QString userId);



#endif // DATABASE_H
