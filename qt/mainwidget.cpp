#include "mainwidget.h"
#include "ui_mainwidget.h"

mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{

    ui->setupUi(this);
}

mainWidget::~mainWidget()
{
    delete ui;
    delete comm_ui;
    delete temp_hum_dispaly_ui;
}

void mainWidget::on_comm_toolBut_clicked()
{
    comm_ui = new comm();
    comm_ui->show();
}






void mainWidget::on_qua_display_toolButton_clicked()
{

    qua_display_ui = new qua_dispaly();
    qua_display_ui->show();
}


void mainWidget::on_temp_hum_dispaly_toolBut_clicked()
{

    temp_hum_dispaly_ui = new temp_hum_display();
    temp_hum_dispaly_ui->show();
}


void mainWidget::on_user_switch_toolBut_clicked()
{

}


void mainWidget::on_exit_toolBut_clicked()
{
   qua_display_ui->close();
   temp_hum_dispaly_ui->close();
   comm_ui->close();
   this->close();
}

