#include "qua_dispaly.h"
#include "ui_qua_dispaly.h"

extern struct Quaternion _qua;

qua_dispaly::qua_dispaly(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::qua_dispaly)
{
    ui->setupUi(this);
    timer = new QTimer();

    timer->setInterval(10);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
    timer->start();

    create_mpu6050_gui();
}

qua_dispaly::~qua_dispaly()
{
    delete ui;
}

void qua_dispaly::onTimeout(){

    //Result<struct Quaternion> res = db_select_top_one_from_qua_where_user_id(user.id);
    Result<struct Quaternion> res;
    res.E = Ok;
    res.data = _qua;
    if(res.E == Ok){
        cube->changeRotateByRPY(res.data.roll.toFloat(), res.data.pitch.toFloat(), res.data.yaw.toFloat());

        ui->pitch_value_label->setText(res.data.pitch);
        ui->roll_value_label->setText(res.data.roll);
        ui->yaw_value_label->setText(res.data.yaw);
        ui->accl_x_value_label->setText(res.data.accelX);
        ui->accl_y_value_label->setText(res.data.accelY);
        ui->accl_z_value_label->setText(res.data.accelZ);
        ui->gyro_x_value_label->setText(res.data.gyroX);
        ui->gyro_y_value_label->setText(res.data.gyroY);
        ui->gyro_z_value_label->setText(res.data.gyroZ);
    }

}
Option qua_dispaly::create_mpu6050_gui(){
    ui->posture_show_widget->setAttribute(Qt::WA_StyledBackground);

    QVBoxLayout *layout = new QVBoxLayout;
    cube = new model_3d(ui->posture_show_widget);
    layout->addWidget(cube);
    ui->posture_show_widget->setLayout(layout);
    return Ok;
}
