#include "login.h"
#include "ui_login.h"
#include "mainWidget.h"

//struct User user;
//struct User root;

Id_Status login_id_status = Id_Status::User;
struct User user;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    ui->setupUi(this);

}

Widget::~Widget()
{
    delete ui;
    delete main_widget;
}


void Widget::on_login_but_clicked()
{

    ui->login_err_label->setText("");
    QString username = ui->username_lineEdit->text();
    QString password = ui->password_lineEdit->text();


    if(username == "" || password == ""){
        ui->login_err_label->setText("用户名或者密码不能为空!");
        return;
    }

    qDebug()<<"[Debug] input->username: \t"<<username<<"\n";
    qDebug()<<"[Debug] input->password: \t"<<password<<"\n";
    qDebug()<<"[Debug] id_status: \t"<<login_id_status<<"\n";

    static Result<struct User> result = db_select_user_where_username_password(username, password, login_id_status);
    if(result.E == Option::Err){
        ui->login_err_label->setText(result.msg);
        return;
    }
    user = result.data;
    qDebug("[Debug] user login Ok ...\n");

    //qDebug()<<"User: username = "<< user.username<<", status = "<< user.status<< ", isRoot = "<< user.isRoot<< "\n";
    main_widget = new mainWidget;
    this->close();
    main_widget->show();

}


void Widget::on_switch_but_clicked()
{
    if(login_id_status == Id_Status::User){
        login_id_status = Id_Status::Root;
        ui->switch_but->setText("管理员登录");
    }else{
        login_id_status = Id_Status::User;
        ui->switch_but->setText("普通用户登录");
    }
}




void Widget::on_logon_but_clicked(){
    logon_widget = new logonWidget();
    logon_widget->show();
}

