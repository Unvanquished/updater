#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

class DownloadWorker;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread thread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setDownloadSpeed(int speed);
    void setUploadSpeed(int speed);
    void setTotalSize(int size);
    void setCompletedSize(int size);
    void onDownloadEvent(int event);
    void startUpdate(void);
    void toggleDownload(void);

private:
    QString sizeToString(int size);

    Ui::MainWindow *ui;
    DownloadWorker *worker;
    int totalSize;
    bool paused;
};

#endif // MAINWINDOW_H
