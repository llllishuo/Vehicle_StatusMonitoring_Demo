#include "temp_hum_display.h"
#include "ui_temp_hum_display.h"
extern struct Quaternion _qua;
int temp_hum_screen_key = 0;
unsigned int temp_time_x_value[60 * 2] = {0};
unsigned int hum_time_x_value[60 * 2] = {0};
void time_x_value_init(){
    int i = 0;
    for(i = 0; i < 60 * 2; i++){
        temp_time_x_value[i] = 0;
        hum_time_x_value[i] = 0;
    }
}
void printf_time_x_value(){
    int i = 0;
    for(i = 0; i < 24; i++){
        qDebug()<<"[Debug] temp = "<<temp_time_x_value[i]<<", i = "<< i<<"num("<< temp_time_x_value[24 + i]<< ")\n";
        qDebug()<<"[Debug] hum = "<<hum_time_x_value[i]<<", i = "<< i<<"num("<< hum_time_x_value[24 + i]<< ")\n";
    }
}
void time_x_value_average(){
    unsigned int size = 0;
    if(temp_hum_screen_key == 0){
        size = 60;
    }else if(temp_hum_screen_key == 1){
        size = 60;
    }else if(temp_hum_screen_key == 2){
        size = 24;
    }
    int i = 0;
    for(i = 0; i < size; i++){
        if(temp_time_x_value[i] == 0) continue;
        if(hum_time_x_value[i] == 0) continue;
        temp_time_x_value[i] = temp_time_x_value[i] / temp_time_x_value[size + i];
        hum_time_x_value[i] = hum_time_x_value[i] / hum_time_x_value[size + i];
    }
}

temp_hum_display::temp_hum_display(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::temp_hum_display)
{
    ui->setupUi(this);
    tableView_Init();

    timer = new QTimer();
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
    timer->start();
}

temp_hum_display::~temp_hum_display()
{
    delete ui;
}




void temp_hum_display::onTimeout(){

}


Option temp_hum_display::tableView_Init(){
    QTableWidget *temp_tableWidget = ui->temp_hum_tableWidget;
    QHeaderView* view = new QHeaderView(Qt::Horizontal);
    QStandardItemModel* model = new QStandardItemModel;
    QStringList labels;
    labels << QStringLiteral("温度") << QStringLiteral("湿度") << QStringLiteral("时间");
    model->setHorizontalHeaderLabels(labels);
    view->setModel(model);
    view->setSectionResizeMode(QHeaderView::ResizeToContents);
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

    time_x_value_init();

    curve_init();

    curve_create(res.data);

    return Ok;
}




Option temp_hum_display::add_item_to_tableView(unsigned int x, unsigned int y, QString text){
    if(x <= 0 || y <= 0) return Err;
       unsigned int table_row_size = ui->temp_hum_tableWidget->rowCount();
       if(x > table_row_size){
           ui->temp_hum_tableWidget->setRowCount(x);
       }
       ui->temp_hum_tableWidget->setItem(--x,--y,new QTableWidgetItem(text));
       return Ok;
}




Option temp_hum_display::add_vec_to_tableView(QVector<struct TempHum> vec){
    ui->temp_hum_tableWidget->clear();
    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString time = dateTime.toString("yyyy-MM-dd hh:mm:ss");//格式化时间

    Result<struct Time> res = toTime_from_string(time);
    qDebug()<<"User: username = "<< user.username<<", status = "<< user.status<< ", isRoot = "<< user.isRoot<< "\n";
    unsigned int i = 0;
    foreach (struct TempHum item, vec) {
        Result<struct Time> item_time = toTime_from_string(item.time);
        if(item_time.E == Option::Err){
            continue;
        }
        unsigned int time_diff = res.data.stamp - item_time.data.stamp;
        if(temp_hum_screen_key == 0 && time_diff > 60){
            continue;
        }else if(temp_hum_screen_key == 1 && time_diff > 60 * 60){
            continue;
        }else if(temp_hum_screen_key == 2 && time_diff > 60 * 60 * 24){
            continue;
        }
        i++;
        add_item_to_tableView(i,1,item.temp);
        add_item_to_tableView(i,2,item.hum);
        add_item_to_tableView(i,3,item.time);
    }
    return Option::Ok;

}



Option temp_hum_display::curve_init(){
    temp_curve_init();
    hum_curve_init();
}

Option temp_hum_display::temp_curve_init(){
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
    axisX->setLabelFormat("%ds");
    axisX->setTickType(QValueAxis::TickType::TicksDynamic);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 50);//默认坐标为动态计算大小的
    axisY->setLabelFormat("%.2fC");

    axisX->setTitleText("时间");
    axisX->setTitleText("温度");

    //设置x和y轴的数据关联
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

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

Option temp_hum_display::hum_curve_init(){
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
    axisX->setLabelFormat("%ds");
    axisX->setTickType(QValueAxis::TickType::TicksDynamic);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(20, 50);//默认坐标为动态计算大小的
    axisY->setLabelFormat("%.2f%");


    axisX->setTitleText("时间");
    axisX->setTitleText("湿度");

    //设置x和y轴的数据关联
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

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


Option temp_hum_display::curve_create(QVector<struct TempHum> data){
    time_x_value_init();

    QDateTime dateTime= QDateTime::currentDateTime();//获取系统当前的时间
    QString time = dateTime.toString("yyyy-MM-dd hh:mm:ss");//格式化时间

    Result<struct Time> res = toTime_from_string(time);

    QString time_unit = "";

    switch (temp_hum_screen_key) {
    case 0:
        time_unit = "s";
        temp_x->setRange(0.00, 60.00);
        hum_x->setRange(0.00, 60.00);
        temp_x->setLabelFormat("%0.2fs");
        temp_x->setLabelFormat("%0.2fs");
        break;
    case 1:
        time_unit = "m";
        temp_x->setRange(0.00, 60.00);
        hum_x->setRange(0.00, 60.00);
        temp_x->setLabelFormat("%0.2fm");
        temp_x->setLabelFormat("%0.2fm");
        break;
    case 2:
        time_unit = "d";
        temp_x->setRange(0.00, 24.00);
        hum_x->setRange(0.00, 24.00);
        temp_x->setLabelFormat("%0.2fh");
        temp_x->setLabelFormat("%0.2fh");
        break;
    default:
        break;
    }

    temp_series->setPointLabelsClipping(false);
    hum_series->setPointLabelsClipping(false);


    QVector<QPointF> temp_points;
    QVector<QPointF> hum_points;

    foreach (struct TempHum item, data) {
        Result<struct Time> item_time = toTime_from_string(item.time);
        if(item_time.E == Option::Err){
            continue;
        }
        double temp = item.temp.toDouble();
        double hum = item.hum.toDouble();
        unsigned int time_diff = res.data.stamp - item_time.data.stamp;
        if(temp_hum_screen_key == 0 && time_diff <= 60){
            double x = item_time.data.s;
            temp_time_x_value[(int)item_time.data.s] += temp;
            temp_time_x_value[60 + (int)item_time.data.s] += 1;
            hum_time_x_value[(int)item_time.data.s] += hum;
            hum_time_x_value[60 + (int)item_time.data.s] += 1;
        }else if(temp_hum_screen_key == 1 && time_diff <= 60 * 60){
            double x = item_time.data.min * 60 + item_time.data.s;
            temp_time_x_value[(int)item_time.data.min] += temp;
            temp_time_x_value[60 + (int)item_time.data.min] += 1;
            hum_time_x_value[(int)item_time.data.min] += hum;
            hum_time_x_value[60 + (int)item_time.data.min] += 1;
        }else if(temp_hum_screen_key == 2 && time_diff <= 60 * 60 * 24){
            double x = item_time.data.hour * 60 * 60 + item_time.data.min * 60 + item_time.data.s;
            temp_time_x_value[(int)item_time.data.hour] += temp;
            temp_time_x_value[24 + (int)item_time.data.hour] += 1;
            hum_time_x_value[(int)item_time.data.hour] += hum;
            hum_time_x_value[24 + (int)item_time.data.hour] += 1;
        }

    }
    time_x_value_average();

    unsigned int i_start = 0;
    unsigned int i_end = 0;
    unsigned int i_oper = 0;
    switch (temp_hum_screen_key) {
    case 0:
        i_start = res.data.s;
        i_oper = 60;
        i_end = res.data.s;
        break;
    case 1:
        i_start = res.data.min;
        i_oper = 60;
        i_end = res.data.min;
        break;
    case 2:
        i_start = res.data.hour;
        i_oper = 24;
        i_end = res.data.hour;
        break;
    default:
        break;
    }
    temp_points.append(QPointF(i_start, temp_time_x_value[i_start]));
    hum_points.append(QPointF(i_start, hum_time_x_value[i_start]));
    i_start++;
    i_start %= i_oper;
    while(i_start != i_end){
        temp_points.append(QPointF(i_start, temp_time_x_value[i_start]));
        hum_points.append(QPointF(i_start, hum_time_x_value[i_start]));
        i_start++;
        i_start %= i_oper;
    }
    //printf_time_x_value();
    std::sort(temp_points.begin(), temp_points.end(), [](const QPointF &p1, const QPointF &p2) {
                return p1.x() < p2.x();
    });
    std::sort(hum_points.begin(), hum_points.end(), [](const QPointF &p1, const QPointF &p2) {
                return p1.x() < p2.x();
    });
    foreach (QPointF item, temp_points) {
        temp_series->append(item);
    }
    foreach (QPointF item, hum_points) {
        hum_series->append(item);
    }


    temp_chart->setAnimationOptions(QChart::SeriesAnimations);
    hum_chart->setAnimationOptions(QChart::SeriesAnimations);



    return Ok;
}




void temp_hum_display::on_refresh_toolBut_clicked()
{
    temp_series->clear();
    hum_series->clear();
    Result<QVector<struct TempHum>> res = db_select_vec_temp_and_hum_where_userId(user.id);
    if(res.E == Option::Err){
        qDebug()<<"[Err] temp hum vec err: "<< res.msg<<"\n";
        return;
    }
    curve_create(res.data);

}

void temp_hum_display::on_temp_hum_screen_comboBox_activated(int index)
{
    temp_hum_screen_key = index;
    temp_series->clear();
    hum_series->clear();
    Result<QVector<struct TempHum>> res = db_select_vec_temp_and_hum_where_userId(user.id);
    if(res.E == Option::Err){
        qDebug()<<"[Err] temp hum vec err: "<< res.msg<<"\n";
        return;
    }
    curve_create(res.data);
    add_vec_to_tableView(res.data);

}

