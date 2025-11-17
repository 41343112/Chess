#include "mychess.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    myChess w; 
    w.show();
    return a.exec();
}
