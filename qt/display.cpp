#include "display.h"
#include "ui_display.h"
#include <QStandardItemModel>


display::display(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::display)
{
    ui->setupUi(this);
    ui->humidity_widget->setStyleSheet(QString::fromUtf8("#humidity_widget{border:1px solid black}"));
    ui->temperature_widget->setStyleSheet(QString::fromUtf8("#temperature_widget{border:1px solid black}"));
    ui->temp_tab->setStyleSheet(QString::fromUtf8("#temp_tab{border:1px solid black}"));
    ui->posture_tab->setStyleSheet(QString::fromUtf8("#posture_tab{border:1px solid black}"));
    ui->posture_show_widget->setStyleSheet(QString::fromUtf8("#posture_show_widget{border:1px solid black}"));
    ui->temp_hum_tableWidget->setStyleSheet(QString::fromUtf8("#temperature_tableWidget{border:1px solid black}"));
    tableView_Init();
}

display::~display()
{
    delete ui;
}



Option display::tableView_Init(){
    QTableWidget *temp_tableWidget = ui->temp_hum_tableWidget;
    QHeaderView* view = new QHeaderView(Qt::Horizontal);
    QStandardItemModel* model = new QStandardItemModel;
    QStringList labels;
    labels << QStringLiteral("温度") << QStringLiteral("湿度") << QStringLiteral("时间");
    model->setHorizontalHeaderLabels(labels);
    view->setModel(model);
    temp_tableWidget->setHorizontalHeader(view);
    temp_tableWidget->setColumnCount(3);
    temp_tableWidget->resize(60,30);
    temp_tableWidget->setColumnWidth(0, (temp_tableWidget->width() - 20)/4);
    temp_tableWidget->setColumnWidth(1, (temp_tableWidget->width() - 20)/4);
    temp_tableWidget->setColumnWidth(2, (temp_tableWidget->width() - 20)/4 * 2);

    Result<QVector<struct TempHum>> res = db_select_vec_temp_and_hum_where_userId(user.id);
    if(res.E == Option::Err){
        qDebug()<<"[Err] temp hum vec err: "<< res.msg<<"\n";
        return Option::Err;
    }
    add_vec_to_tableView(res.data);

    curve_init();

    curve_create(res.data);

    return Ok;
}




Option display::add_item_to_tableView(unsigned int x, unsigned int y, QString text){
    if(x <= 0 || y <= 0) return Err;
       unsigned int table_row_size = ui->temp_hum_tableWidget->rowCount();
       if(x > table_row_size){
           ui->temp_hum_tableWidget->setRowCount(x);
       }
       ui->temp_hum_tableWidget->setItem(--x,--y,new QTableWidgetItem(text));
       return Ok;
}




Option display::add_vec_to_tableView(QVector<struct TempHum> vec){
    qDebug()<<"User: username = "<< user.username<<", status = "<< user.status<< ", isRoot = "<< user.isRoot<< "\n";
    unsigned int i = 0;
    foreach (struct TempHum item, vec) {
        i++;
        add_item_to_tableView(i,1,item.temp);
        add_item_to_tableView(i,2,item.hum);
        add_item_to_tableView(i,3,item.time);
    }
    return Option::Ok;

}



Option display::curve_init(){
    temp_curve_init();
    hum_curve_init();
}

Option display::temp_curve_init(){
    //实现一个数据曲线显示
    QSplineSeries *series = new QSplineSeries();
    series->setColor(QColor(0,100,255));
    series->setVisible(true);//可见
    //series->setPointLabelsFormat("(@xPoint, @yPoint)");
    //series->setPointLabelsVisible(true);

    QChart *chart = new QChart();
    //设置曲线样式
    //设置主题
    chart->setTheme(QChart::ChartThemeLight);
    //设置背景阴影
    chart->setDropShadowEnabled(true);
    //设置背景自动填充
    chart->setAutoFillBackground(true);
    //将数据曲线放入图表管理类中
    chart->addSeries(series);

    //创建X和Y轴对象(本质就是2D的坐标系)
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 150);//默认坐标为动态计算大小的
    axisX->setLabelFormat("%ds");
    axisX->setTickType(QValueAxis::TickType::TicksDynamic);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 50);//默认坐标为动态计算大小的
    axisY->setLabelFormat("%.2fC");

    //设置x和y轴的数据关联
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    //chart->createDefaultAxes();//默认轴

    chart->legend()->setVisible(false);

    //将图形管理对象与视图控件联系到一起
    QChartView *view = new QChartView(chart);
    //设置平滑性
    view->setRenderHint(QPainter::Antialiasing);
    QVBoxLayout *p = new QVBoxLayout(ui->temperature_widget);
    p->addWidget(view);
    view->setContentsMargins(0,0,0,0);
    p->setContentsMargins(0,0,0,0);
    ui->temperature_widget->setLayout(p);
    ui->temperature_widget->setContentsMargins(0,0,0,0);

    temp_chart_view = view;
    temp_chart = chart;
    temp_series = series;
    temp_x = axisX;
    temp_y = axisY;
    return Ok;
}

Option display::hum_curve_init(){
    //实现一个数据曲线显示
    QSplineSeries *series = new QSplineSeries();
    series->setColor(QColor(0,100,255));
    series->setVisible(true);//可见

    QChart *chart = new QChart();
    //设置曲线样式
    //设置主题
    chart->setTheme(QChart::ChartThemeLight);
    //设置背景阴影
    chart->setDropShadowEnabled(true);
    //设置背景自动填充
    chart->setAutoFillBackground(true);
    //将数据曲线放入图表管理类中
    chart->addSeries(series);

    //创建X和Y轴对象(本质就是2D的坐标系)
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 150);//默认坐标为动态计算大小的
    axisX->setLabelFormat("%ds");
    axisX->setTickType(QValueAxis::TickType::TicksDynamic);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 50);//默认坐标为动态计算大小的
    axisY->setLabelFormat("%.2f%");

    //设置x和y轴的数据关联
    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);
    //chart->createDefaultAxes();//默认轴

    chart->legend()->setVisible(false);

    //将图形管理对象与视图控件联系到一起
    QChartView *view = new QChartView(chart);
    //设置平滑性
    view->setRenderHint(QPainter::Antialiasing);
    QVBoxLayout *p = new QVBoxLayout(ui->temperature_widget);
    p->addWidget(view);
    view->setContentsMargins(0,0,0,0);
    p->setContentsMargins(0,0,0,0);
    ui->humidity_widget->setLayout(p);
    ui->humidity_widget->setContentsMargins(0,0,0,0);

    hum_chart_view = view;
    hum_chart = chart;
    hum_series = series;
    hum_x = axisX;
    hum_y = axisY;
    return Ok;
}


Option display::curve_create(QVector<struct TempHum> data){

    temp_x->setRange(0.00, 60.00);
    temp_x->setLabelFormat("%.1fs");
    temp_series->setPointLabelsClipping(false);
    hum_x->setRange(0.00, 60.00);
    temp_x->setLabelFormat("%.1fs");
    hum_series->setPointLabelsClipping(false);

    foreach (struct TempHum item, data) {
        double temp = item.temp.toDouble();
        double hum = item.hum.toDouble();
        Result<struct Time> res = toTime_from_string(item.time);
        if(res.E == Option::Err){
            continue;
        }

        qDebug()<<"[Debug] time: s = "<< res.data.s <<"\n";
        qDebug()<<"[Debug] temp curve: "<< temp <<"\n";
        temp_series->append(QPointF(res.data.s, temp));
        qDebug()<<"[Debug] hum curve: "<< temp <<"\n";
        hum_series->append(QPointF(res.data.s, hum));
    }

    return Ok;


}







void display::on_comboBox_activated(int index)
{

}

