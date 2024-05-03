#ifndef QUA_DISPALY_H
#define QUA_DISPALY_H

#include <QWidget>
#include "model_3d.h"
#include "DataModel.h"
#include "database.h"

namespace Ui {
class qua_dispaly;
}

class qua_dispaly : public QWidget
{
    Q_OBJECT

public:
    explicit qua_dispaly(QWidget *parent = nullptr);
    ~qua_dispaly();
    Option create_mpu6050_gui();
private slots:
    void onTimeout();

private:
    QTimer *timer;
    Ui::qua_dispaly *ui;
    model_3d *cube;
};

#endif // QUA_DISPALY_H
