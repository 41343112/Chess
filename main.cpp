#include <QApplication>
#include <QTranslator>
#include <QSettings>
#include <QLocale>
#include "mychess.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 根據使用者設定載入翻譯
    QSettings settings("ChessGame", "Settings");
    QString language = settings.value("language", "en").toString();
    
    QTranslator translator;
    if (language == "zh") {
        if (translator.load(":/translations/chess_zh_CN.qm")) {
            a.installTranslator(&translator);
        }
    }
    
    myChess w;
    w.show();
    return a.exec();
}
