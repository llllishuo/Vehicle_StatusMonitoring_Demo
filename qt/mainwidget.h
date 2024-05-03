#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "comm.h"
#include "temp_hum_display.h"
#include "qua_dispaly.h"


namespace Ui {
class mainWidget;
}

class mainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit mainWidget(QWidget *parent = nullptr);
    ~mainWidget();

private slots:
    void on_comm_toolBut_clicked();
    void on_qua_display_toolButton_clicked();
    void on_temp_hum_dispaly_toolBut_clicked();

    void on_user_switch_toolBut_clicked();

    void on_exit_toolBut_clicked();

private:
    Ui::mainWidget *ui;
    comm *comm_ui;
    temp_hum_display *temp_hum_dispaly_ui;
    qua_dispaly *qua_display_ui;
};

#endif // MAINWIDGET_H
