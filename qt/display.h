#ifndef DISPLAY_H
#define DISPLAY_H

#include <QWidget>
#include "DataModel.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include "database.h"
#include <QtCharts>



namespace Ui {
class display;
}

class display : public QWidget
{
    Q_OBJECT

public:
    explicit display(QWidget *parent = nullptr);
    Option add_item_to_tableView(unsigned int x, unsigned int y, QString text);
    Option tableView_Init();
    Option add_vec_to_tableView(QVector<struct TempHum>);
    Option curve_init();
    Option temp_curve_init();
    Option hum_curve_init();
    Option curve_create(QVector<struct TempHum>);
    ~display();

private slots:
    void on_comboBox_activated(int index);

private:
    Ui::display *ui;
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
#endif // DISPLAY_H
