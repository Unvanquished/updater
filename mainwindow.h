#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread thread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void operate();

public slots:
    void setDownloadSpeed(int speed);
    void setUploadSpeed(int speed);
    void setTotalSize(int size);
    void setCompletedSize(int size);
    void onDownloadEvent(int event);
    void startUpdate(void);

private:
    QString sizeToString(int size);

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
