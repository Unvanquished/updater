#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

#ifdef USE_STATIC
#include <QtPlugin>
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
#endif

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
    a.setOrganizationName("Unvanquished Development");
    a.setOrganizationDomain("unvanquished.net");
    w.setWindowTitle("Unvanquished Updater");
    w.show();

    return a.exec();
}
