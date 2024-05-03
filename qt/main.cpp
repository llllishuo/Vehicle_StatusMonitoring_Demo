#include "login.h"
#include <QApplication>
#include "database.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    login w;
    db_Init();
    w.show();
    return a.exec();
}
