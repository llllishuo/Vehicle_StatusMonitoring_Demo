#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "mainwidget.h"
#include "DataModel.h"
#include "database.h"
#include "logon.h"

namespace Ui { class Widget; }

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_login_but_clicked();
    void on_logon_but_clicked();

    void on_switch_but_clicked();

private:
    Ui::Widget *ui;
    mainWidget *main_widget;
    logonWidget *logon_widget;
};
#endif // LOGIN_H
