#include "mychess.h"
#include "ui_mychess.h"

myChess::myChess(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::myChess)
{
    ui->setupUi(this);
}

myChess::~myChess()
{
    delete ui;
}
