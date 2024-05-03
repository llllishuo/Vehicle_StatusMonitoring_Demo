#ifndef LOGIN_H
#define LOGIN_H

#include "mainwidget.h"
#include "database.h"
#include "logon.h"

namespace Ui { class login; }

class login : public QWidget
{
    Q_OBJECT

public:
    login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_login_but_clicked();
    void on_logon_but_clicked();

    void on_switch_but_clicked();

private:
    Ui::login *ui;
    mainWidget *main_widget;
    logonWidget *logon_widget;
};
#endif // LOGIN_H
