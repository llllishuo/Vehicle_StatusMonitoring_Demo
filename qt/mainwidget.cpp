#include "mainwidget.h"
#include "ui_mainwidget.h"

mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainWidget)
{

    ui->setupUi(this);
    ui->show_widget->setStyleSheet(QString::fromUtf8("#show_widget{border:1px solid black}"));


    comm_ui = new comm(ui->show_widget);
    comm_ui->resize(ui->show_widget->size());
    display_ui = new display(ui->show_widget);
    display_ui->resize(ui->show_widget->size());

    display_ui->hide();
    comm_ui->show();
}

mainWidget::~mainWidget()
{
    delete ui;
    delete comm_ui;
    delete display_ui;
}

void mainWidget::on_comm_toolBut_clicked()
{
    display_ui->hide();
    comm_ui->show();
}


void mainWidget::on_dispaly_toolBut_clicked()
{
    comm_ui->hide();
    display_ui->show();

}



