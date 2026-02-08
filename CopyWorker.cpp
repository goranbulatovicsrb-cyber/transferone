#include "CopyWorker.h"
#include <QFile>
#include <QDirIterator>
#include <QElapsedTimer>
#include <QThread>

CopyWorker::CopyWorker(QStringList sources, QString destination)
    : m_sources(sources), m_destination(destination) {}

void CopyWorker::process()
{
    qint64 totalBytes = 0;

    for (const QString &folder : m_sources) {
        QDirIterator it(folder, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
            totalBytes += QFileInfo(it.next()).size();
    }

    qint64 copiedBytes = 0;
    QElapsedTimer timer;
    timer.start();

    for (const QString &folder : m_sources) {
        QDirIterator it(folder, QDir::Files, QDirIterator::Subdirectories);

        while (it.hasNext()) {

            while (m_paused)
                QThread::msleep(100);

            QString filePath = it.next();
            QString relative = QDir(folder).relativeFilePath(filePath);
            QString destPath = m_destination + "/" +
                               QFileInfo(folder).fileName() + "/" +
                               relative;

            QDir().mkpath(QFileInfo(destPath).path());
            QFile::copy(filePath, destPath);

            copiedBytes += QFileInfo(filePath).size();

            int percent = (copiedBytes * 100) / totalBytes;
            emit progress(percent);

            double seconds = timer.elapsed() / 1000.0;
            double mbps = (copiedBytes / 1024.0 / 1024.0) / seconds;
            emit speedUpdate(mbps);

            double remaining = (totalBytes - copiedBytes) / (copiedBytes / seconds);
            emit etaUpdate(QString::number((int)(remaining/60)) + " min");
        }
    }

    emit finished();
}

void CopyWorker::pause() { m_paused = true; }
void CopyWorker::resume() { m_paused = false; }
