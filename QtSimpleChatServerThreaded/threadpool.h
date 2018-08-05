#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <QObject>
#include <QVector>
#include <QThread>

class ThreadPool : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ThreadPool)

public:
    explicit ThreadPool(int numberOfThreads = QThread::idealThreadCount(), QObject * parent = nullptr);
    ~ThreadPool() override;

signals:
    void aboutToQuit();

public slots:
    void moveObject(QObject *);

private:
    int indexOfLeastLoadedThread() const;

    QVector<QThread *> m_threads;
    QVector<QAtomicInt> m_threadsLoad;
};

#endif // THREADPOOL_H
