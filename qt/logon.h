#ifndef LOGON_H
#define LOGON_H

#include <QWidget>
#include "DataModel.h"
#include <QRegularExpression>
#include "database.h"
namespace Ui {
class logonWidget;
}

class logonWidget : public QWidget
{
    Q_OBJECT

public:
    explicit logonWidget(QWidget *parent = nullptr);
    ~logonWidget();
    Option password_inspect(QString password);
    Option username_inspect(QString password);
    Option string_is_size(QString str, unsigned int min, unsigned int max);

private slots:
    void on_break_but_clicked();

    void on_logon_but_clicked();

    void on_username_edit_editingFinished();


    void on_confirm_password_edit_editingFinished();

    void on_password_edit_editingFinished();

private:
    Ui::logonWidget *ui;
};

#endif // LOGON_H
