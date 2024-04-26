#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "comm.h"
#include "display.h"


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


    void on_dispaly_toolBut_clicked();

private:
    Ui::mainWidget *ui;
    comm *comm_ui;
    display *display_ui;
};

#endif // MAINWIDGET_H
