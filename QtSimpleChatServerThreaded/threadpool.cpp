#include "threadpool.h"

ThreadPool::ThreadPool(int numberOfThreads, QObject * parent)
    : QObject(parent), m_threads(numberOfThreads), m_threadsLoad(numberOfThreads)
{
    for (qint32 i = 0; i < numberOfThreads; i++)  {
        // Create and initialize a worker thread for the pool
        QThread * thread = new QThread(this);

        // Whenever a thread quits release one to the semaphore, so when all of them are finished we can shutdown safely
        QObject::connect(thread, &QThread::finished, [this] () -> void {
            m_shutdownBarrier.release();
        });

        // Start the thread
        thread->start();

        // Add it to the pool
        m_threads[i] = thread;
    }
}

ThreadPool::~ThreadPool()
{
    emit aboutToQuit();

    // Tell each of the threads it needs to quit
    for (QVector<QThread *>::ConstIterator i = m_threads.constBegin(), end = m_threads.constEnd(); i != end; ++i)
        (*i)->quit();

    // Wait for all the threads to actually quit before returning
    m_shutdownBarrier.acquire(m_threads.size());
}

void ThreadPool::moveObject(QObject * worker)
{
    // Find the index of the least loaded thread - the one with fewest worker objects
    int threadIndex = indexOfLeastLoadedThread();
    // Add one to its load counter for our object
    ++m_threadsLoad[threadIndex];
    // Connect the destruction of the object to decrement the load counter
    QObject::connect(worker, &QObject::destroyed, [this, threadIndex] () -> void  {
        --m_threadsLoad[threadIndex];
    });
    // Connect the worker's deleteLater to the thread's finished signal so we don't leak memory
    QThread * thread = m_threads[threadIndex];
    QObject::connect(thread, &QThread::finished, worker, &QObject::deleteLater);

    // Move the object to the worker thread
    worker->moveToThread(thread);
}

int ThreadPool::indexOfLeastLoadedThread() const
{
    int minIndex = 0;
    for (int i = 1, size = m_threads.size(); i < size; i++)  {
        if (m_threadsLoad.at(i) < m_threadsLoad.at(minIndex))
            minIndex = i;
    }

    return minIndex;
}
