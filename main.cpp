#include "tempgraph.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TempGraph w;
    w.show();
    return a.exec();
}
