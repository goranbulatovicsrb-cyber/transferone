#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("TransferOne Ultimate");
    resize(900, 560);
    setAcceptDrops(true);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QLabel *title = new QLabel("TransferOne Ultimate");
    title->setStyleSheet("font-size:26px; font-weight:bold;");
    mainLayout->addWidget(title);

    listWidget = new QListWidget();
    listWidget->setMinimumHeight(220);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("Add Folder");
    QPushButton *destBtn = new QPushButton("Choose Destination");
    QPushButton *startBtn = new QPushButton("Start");
    pauseBtn = new QPushButton("Pause");

    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(destBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(startBtn);
    btnLayout->addWidget(pauseBtn);

    progressBar = new QProgressBar();
    progressBar->setMinimumHeight(28);

    speedLabel = new QLabel("Speed: 0 MB/s");
    etaLabel = new QLabel("ETA: --");

    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->addWidget(speedLabel);
    infoLayout->addStretch();
    infoLayout->addWidget(etaLabel);

    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(btnLayout);
    mainLayout->addWidget(progressBar);
    mainLayout->addLayout(infoLayout);

    setStyleSheet(
        "QMainWindow { background-color: #101010; color: white; }"
        "QListWidget { background-color: #1e1e1e; border-radius:10px; padding:8px; }"
        "QPushButton { background-color:#2962FF; color:white; padding:8px 18px; border-radius:10px; }"
        "QPushButton:hover { background-color:#0039CB; }"
        "QProgressBar { border-radius:14px; background:#2a2a2a; }"
        "QProgressBar::chunk { border-radius:14px; background:#00E676; }"
    );

    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addFolder);
    connect(destBtn, &QPushButton::clicked, this, &MainWindow::chooseDestination);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::startCopy);
    connect(pauseBtn, &QPushButton::clicked, this, &MainWindow::pauseResume);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    for (const QUrl &url : event->mimeData()->urls()) {
        QString path = url.toLocalFile();
        if (!path.isEmpty()) {
            selectedFolders.append(path);
            listWidget->addItem(path);
        }
    }
}

void MainWindow::addFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Select Folder");
    if (!folder.isEmpty()) {
        selectedFolders.append(folder);
        listWidget->addItem(folder);
    }
}

void MainWindow::chooseDestination()
{
    destinationFolder = QFileDialog::getExistingDirectory(this, "Select Destination");
}

void MainWindow::startCopy()
{
    thread = new QThread;
    worker = new CopyWorker(selectedFolders, destinationFolder);
    worker->moveToThread(thread);

    connect(thread, &QThread::started, worker, &CopyWorker::process);
    connect(worker, &CopyWorker::progress, this, &MainWindow::updateProgress);
    connect(worker, &CopyWorker::speedUpdate, this, &MainWindow::updateSpeed);
    connect(worker, &CopyWorker::etaUpdate, this, &MainWindow::updateETA);
    connect(worker, &CopyWorker::finished, this, &MainWindow::copyFinished);

    connect(worker, &CopyWorker::finished, thread, &QThread::quit);
    connect(worker, &CopyWorker::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void MainWindow::pauseResume()
{
    if (!worker) return;

    if (!paused) {
        worker->pause();
        pauseBtn->setText("Resume");
    } else {
        worker->resume();
        pauseBtn->setText("Pause");
    }
    paused = !paused;
}

void MainWindow::updateProgress(int v) { progressBar->setValue(v); }
void MainWindow::updateSpeed(double s) { speedLabel->setText("Speed: " + QString::number(s,'f',2) + " MB/s"); }
void MainWindow::updateETA(QString e) { etaLabel->setText("ETA: " + e); }
void MainWindow::copyFinished() { etaLabel->setText("Transfer Complete"); }
