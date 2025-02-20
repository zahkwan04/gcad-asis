/**
 * Platform Abstraction Layer to provide common interface to
 * platform-specific mutex and lock implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013. All Rights Reserved.
 *
 * @version $Id: PalLock.h 59 2013-09-25 08:06:01Z mrozaimi $
 * @author Mohd Rozaimi
 */
#ifndef PAL_LOCK_H
#define PAL_LOCK_H

#ifdef QT_CORE_LIB  //QT ==================================================
#include <QMutex>
#include <QReadWriteLock>

namespace PalLock
{
    typedef QMutex         LockT;
    typedef QReadWriteLock RwLockT;

    //LockT ===========================================================
    inline int init(LockT *)
    {
        return 0; //do nothing
    }

    inline int destroy(LockT *)
    {
        return 0; //do nothing
    }

    inline int take(LockT *lock)
    {
        lock->lock();
        return 0;
    }

    inline int release(LockT *lock)
    {
        lock->unlock();
        return 0;
    }

    //RwLockT =========================================================
    inline int init(RwLockT *)
    {
        return 0; //do nothing
    }

    inline int destroy(RwLockT *)
    {
        return 0; //do nothing
    }

    inline int lockRd(RwLockT *lock)
    {
        lock->lockForRead();
        return 0;
    }

    inline int lockWr(RwLockT *lock)
    {
        lock->lockForWrite();
        return 0;
    }

    inline int release(RwLockT *lock)
    {
        lock->unlock();
        return 0;
    }
} //namespace PalLock

#else  //linux ============================================================
#include <pthread.h>

namespace PalLock
{
    typedef pthread_mutex_t  LockT;
    typedef pthread_rwlock_t RwLockT;

    //LockT ===========================================================
    inline int init(LockT *lock)
    {
        return pthread_mutex_init(lock, 0);
    }

    inline int destroy(LockT *lock)
    {
        return pthread_mutex_destroy(lock);
    }

    inline int take(LockT *lock)
    {
        return pthread_mutex_lock(lock);
    }

    inline int release(LockT *lock)
    {
        return pthread_mutex_unlock(lock);
    }

    //RwLockT =========================================================
    inline int init(RwLockT *lock)
    {
        return pthread_rwlock_init(lock, 0);
    }

    inline int destroy(RwLockT *lock)
    {
        return pthread_rwlock_destroy(lock);
    }

    inline int lockRd(RwLockT *lock)
    {
        return pthread_rwlock_rdlock(lock);
    }

    inline int lockWr(RwLockT *lock)
    {
        return pthread_rwlock_wrlock(lock);
    }

    inline int release(RwLockT *lock)
    {
        return pthread_rwlock_unlock(lock);
    }
} //namespace PalLock

#endif //QT_CORE_LIB
#endif //PAL_LOCK_H
