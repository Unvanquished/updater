#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    int fontId = QFontDatabase::addApplicationFont(":fonts/Roboto-Regular.ttf");
    if (fontId != -1) {
        QFont font("Roboto-Regular");
        font.setPointSize(10);
        a.setFont(font);
    }

    w.setWindowTitle("Unvanquished Updater");
    w.show();

    return a.exec();
}
