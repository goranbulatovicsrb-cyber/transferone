#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QListWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include "CopyWorker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void addFolder();
    void chooseDestination();
    void startCopy();
    void pauseResume();
    void updateProgress(int);
    void updateSpeed(double);
    void updateETA(QString);
    void copyFinished();

private:
    QStringList selectedFolders;
    QString destinationFolder;

    QListWidget *listWidget;
    QProgressBar *progressBar;
    QLabel *speedLabel;
    QLabel *etaLabel;
    QPushButton *pauseBtn;

    QThread *thread = nullptr;
    CopyWorker *worker = nullptr;
    bool paused = false;
};

#endif
