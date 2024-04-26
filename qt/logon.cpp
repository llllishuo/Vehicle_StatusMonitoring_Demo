#include "logon.h"
#include "ui_logon.h"

logonWidget::logonWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::logonWidget)
{
    ui->setupUi(this);

}

logonWidget::~logonWidget()
{
    delete ui;
}

void logonWidget::on_break_but_clicked()
{

    this->close();
}


void logonWidget::on_logon_but_clicked(){

    ui->username_err_label->setText("");
    ui->password_err_label->setText("");
    ui->confirm_password_err_label->setText("");

    QString username = ui->username_edit->text();
    QString password = ui->password_edit->text();
    QString confirm_password = ui->confirm_password_edit->text();

    Option username_option = username_inspect(username);
    Option password_option = password_inspect(password);
    Option confirm_password_option = Option::Ok;
    if(password != confirm_password){
        ui->confirm_password_err_label->setText("两次密码不一致!");
        confirm_password_option = Option::Err;
    }


    if(username_option == Option::Ok
            && password_option == Option::Ok
            && confirm_password_option == Option::Ok
            ){
        Result<struct User> res =  db_insert_user(username, password);
        if(res.E == Option::Err){
            ui->username_err_label->setText(res.msg);
            return;
        }
        qDebug()<<"[Debug] create ok: username-"<<username<<", password-"<<password<<"\n";
        this->close();
        return;
    }



}


Option logonWidget::password_inspect(QString password){
    ui->password_err_label->setText("");

    if(password.size() == 0){
        ui->password_err_label->setText("密码不能为空!");
        return Err;
    }

    QRegularExpression reg("[0-9]+");

    int temp = password.indexOf(reg);

    if(temp < 0){
        ui->password_err_label->setText("密码需要包含字母与数字!");
        return Err;
    }

    reg.setPattern("[a-z,A-Z]+");
    temp = password.indexOf(reg);
    if(temp < 0){
        ui->password_err_label->setText("密码需要包含字母与数字!");
        return Err;
    }

    return Ok;
}

Option logonWidget::username_inspect(QString username){
    ui->username_err_label->setText("");
    if(username.size() == 0){
        ui->username_err_label->setText("用户名不能为空!");
        return Err;
    }

    if(string_is_size(username, 6, 13) == Option::Err){
        ui->username_err_label->setText("用户名长度请在6-13之间!");
        return Err;
    }

    return Ok;

}



Option logonWidget::string_is_size(QString str, unsigned int min, unsigned int max){
    if(str.size() <= max && str.size() >= min){
        return Ok;
    }

    return Err;
}

void logonWidget::on_username_edit_editingFinished(){

    QString username = ui->username_edit->text();
    username_inspect(username);
}




void logonWidget::on_confirm_password_edit_editingFinished()
{
    ui->confirm_password_err_label->setText("");
    QString password = ui->password_edit->text();
    QString confirm_password = ui->confirm_password_edit->text();

    if(password != confirm_password){
        ui->confirm_password_err_label->setText("两次密码不一致!");
    }

}


void logonWidget::on_password_edit_editingFinished()
{
    ui->password_err_label->setText("");
    QString password = ui->password_edit->text();
    password_inspect(password);

}

