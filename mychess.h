#ifndef MYCHESS_H
#define MYCHESS_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class myChess;
}
QT_END_NAMESPACE

class myChess : public QMainWindow
{
    Q_OBJECT

public:
    myChess(QWidget *parent = nullptr);
    ~myChess();

private:
    Ui::myChess *ui;
};
#endif // MYCHESS_H
