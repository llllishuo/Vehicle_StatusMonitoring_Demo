#ifndef TEMP_HUM_DISPLAY_H
#define TEMP_HUM_DISPLAY_H

#include <QWidget>
#include "DataModel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QtCharts>
#include "model_3d.h"
#include "database.h"
#include <stdio.h>

namespace Ui {
class temp_hum_display;
}

class temp_hum_display : public QWidget
{
    Q_OBJECT

public:
    explicit temp_hum_display(QWidget *parent = nullptr);
    ~temp_hum_display();
    Option add_item_to_tableView(unsigned int x, unsigned int y, QString text);
    Option tableView_Init();
    Option add_vec_to_tableView(QVector<struct TempHum>);
    Option curve_init();
    Option temp_curve_init();
    Option hum_curve_init();
    Option curve_create(QVector<struct TempHum>);
    Option create_mpu6050_gui();

private slots:
    void onTimeout();
    void on_refresh_toolBut_clicked();
    void on_temp_hum_screen_comboBox_activated(int index);

private:
    Ui::temp_hum_display *ui;
    QTimer *timer;
    QChartView *temp_chart_view;
    QChart *temp_chart;
    QSplineSeries  * temp_series;
    QValueAxis * temp_x;
    QValueAxis * temp_y;
    QChartView *hum_chart_view;
    QChart *hum_chart;
    QSplineSeries  * hum_series;
    QValueAxis * hum_x;
    QValueAxis * hum_y;
};

#endif // TEMP_HUM_DISPLAY_H
