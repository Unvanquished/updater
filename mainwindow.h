#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QThread>

#include <memory>

namespace Ui {
class MainWindow;
}

class DownloadWorker;
class NewsFetcher;
class QLabel;
class QStandardItemModel;

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
    void toggleDownload(void);
    bool close(void);
    void onLoadNewsItems(QStandardItemModel* items);

private slots:
    void startUpdate(void);
    void openSettings(void);

private:
    QString sizeToString(int size);
    void stopAria(void);

    Ui::MainWindow *ui;
    DownloadWorker *worker;
    QSettings settings;
    std::unique_ptr<QLabel> textBrowser;
    std::unique_ptr<NewsFetcher> newsFetcher;
    int totalSize;
    bool paused;
};

#endif // MAINWINDOW_H
