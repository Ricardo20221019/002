#include "qgisdemo.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGisDemo w;
    w.show();

    return a.exec();
}
