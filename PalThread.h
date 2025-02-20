/**
 * Platform Abstraction Layer to provide common interface to
 * platform-specific thread implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2023. All Rights Reserved.
 *
 * @version $Id: PalThread.h 1671 2023-02-27 04:29:13Z rosnin $
 * @author Mohd Rozaimi
 */
#ifndef PAL_THREAD_H
#define PAL_THREAD_H

#ifdef QT_CORE_LIB  //Qt ==================================================
#include <QThread>

#include "QThreadWorker.h"

namespace PalThread
{
    //ThreadFn typedef is in QThreadWorker

    typedef QThreadWorker *ThreadT;

    inline void sleep(unsigned int secs)
    {
        QThread::sleep(secs);
    }

    inline void msleep(unsigned int msecs)
    {
        QThread::msleep(msecs);
    }

    inline int start(ThreadT *thread, ThreadFn fn, void *obj)
    {
        QThread *th = new QThread;
        QThreadWorker *w = new QThreadWorker(th, obj, fn);
        w->moveToThread(th);
        QThread::connect(th, SIGNAL(started()), w, SLOT(run()));
        th->start();
        *thread = w;
        return 0;
    }

    inline int stop(ThreadT thread)
    {
        delete thread;
        return 0;
    }
} //namespace PalThread

#else  //linux ============================================================
#include <pthread.h>
#include <unistd.h>     //usleep()

namespace PalThread
{
    typedef void *(*ThreadFn)(void *arg);

    typedef pthread_t ThreadT;

    inline void sleep(unsigned int secs)
    {
        ::sleep(secs);
    }

    inline void msleep(unsigned int msecs)
    {
        usleep(msecs * 1000);
    }

    inline int start(ThreadT *thread, ThreadFn fn, void *obj)
    {
        return pthread_create(thread, NULL, fn, obj);
    }

    inline int stop(ThreadT thread)
    {
#ifdef MOBILE
        return 0; //do nothing - pthread_cancel() not supported by Android
#else
        pthread_cancel(thread);
        return pthread_join(thread, NULL);
#endif
    }
} //namespace PalThread

#endif //QT_CORE_LIB
#endif //PAL_THREAD_H
