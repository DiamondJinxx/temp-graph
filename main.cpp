#include "tempgraph.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TempGraph w;
    w.setFixedSize(1255,886);
    w.show();
    return a.exec();
}
