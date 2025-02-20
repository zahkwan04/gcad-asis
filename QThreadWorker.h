/**
 * Class to run another class member function in a separate Qt thread.
 * Example usage: @code
 *     QThread *th = new QThread;
 *     QThreadWorker *w = new QThreadWorker(th, classObject, classFunction);
 *     w->moveToThread(th);
 *     QThread::connect(th, SIGNAL(started()), w, SLOT(run()));
 *     th->start();
 * @endcode
 * When terminating the thread, the owner should do something to make the
 * thread function return, and then delete this object.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2014. All Rights Reserved.
 *
 * @version $Id: QThreadWorker.h 167 2014-02-07 02:51:55Z hashim $
 * @author Mohd Rozaimi
 */
#ifndef QTHREADWORKER_H
#define QTHREADWORKER_H

#include <QThread>

typedef void *(*ThreadFn)(void *arg);

class QThreadWorker : public QObject
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] th  The new thread.
     * @param[in] obj The thread function owner.
     * @param[in] fn  The thread function.
     */
    QThreadWorker(QThread *th, void *obj, ThreadFn fn) :
        mQThread(th), mObj(obj), mThreadFn(fn) {}

    ~QThreadWorker()
    {
        mQThread->quit();
        mQThread->wait();
        delete mQThread;
    }

public slots:
    /**
     * Runs the thread function.
     */
    void run()
    {
        mThreadFn(mObj);
    }

private:
    QThread  *mQThread;
    void     *mObj;
    ThreadFn  mThreadFn;
};
#endif //QTHREADWORKER_H
