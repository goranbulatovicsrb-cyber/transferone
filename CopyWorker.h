#ifndef COPYWORKER_H
#define COPYWORKER_H

#include <QObject>
#include <QStringList>

class CopyWorker : public QObject
{
    Q_OBJECT
public:
    CopyWorker(QStringList sources, QString destination);

signals:
    void progress(int);
    void speedUpdate(double);
    void etaUpdate(QString);
    void finished();

public slots:
    void process();
    void pause();
    void resume();

private:
    QStringList m_sources;
    QString m_destination;
    bool m_paused = false;
};

#endif
