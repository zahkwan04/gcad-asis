/**
 * Platform Abstraction Layer to provide common interface to
 * platform-specific semaphore implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2017. All Rights Reserved.
 *
 * @version $Id: PalSem.h 890 2017-02-27 04:15:24Z hashim $
 * @author Mohd Rozaimi
 */
#ifndef PAL_SEM_H
#define PAL_SEM_H

#ifdef QT_CORE_LIB  //QT ==================================================
#include <QSemaphore>

namespace PalSem
{
    typedef QSemaphore SemT;

    inline int init(SemT *)
    {
        return 0; //do nothing
    }

    inline int destroy(SemT *)
    {
        return 0; //do nothing
    }

    inline int post(SemT *sem)
    {
        sem->release();
        return 0;
    }

    inline bool wait(SemT *sem)
    {
        sem->acquire();
        return true;    //always successful when called with no timeout
    }

    inline void clear(SemT *sem)
    {
        int semAvailable = sem->available();
        if (semAvailable != 0)
            sem->tryAcquire(semAvailable);
    }
} //namespace PalSem

#else  //linux ============================================================
#include <semaphore.h>

namespace PalSem
{
    typedef sem_t SemT;

    inline int init(SemT *sem)
    {
        return sem_init(sem, 0, 0);
    }

    inline int destroy(SemT *sem)
    {
        return sem_destroy(sem);
    }

    inline int post(SemT *sem)
    {
        return sem_post(sem);
    }

    inline bool wait(SemT *sem)
    {
        return (sem_wait(sem) == 0);
    }

    inline void clear(SemT *sem)
    {
        //keep decrementing until failure, i.e. semaphore has value 0
        while (sem_trywait(sem) == 0); //empty loop
    }
} //namespace PalSem

#endif //QT_CORE_LIB
#endif //PAL_SEM_H
