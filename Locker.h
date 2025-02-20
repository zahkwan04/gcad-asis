/**
 * RAII (Resource Acquisition Is Initialization) wrapper class for a mutex
 * and read-write lock.
 * Code that needs to apply a lock just needs to create a locally-scoped
 * (stack) instance of this class with the required mutex/lock. It will be
 * unlocked automatically when the class object is destroyed upon going out
 * of scope.
 * Example:@code
 * void f1()
 * {
 *     Locker lock(&myMutex); //mutex locked here
 *     ...do the work...
 * }                          //mutex automatically unlocked here
 *
 * void f2()
 * {
 *     Locker lock(&myReadWriteLock, true);
 *     ...do the work...
 * }
 * @endcode
 *
 * Copyright (C) Sapura Secured Technologies, 2011-2013. All Rights Reserved.
 *
 * @version $Id: Locker.h 59 2013-09-25 08:06:01Z mrozaimi $
 * @author Mohd Rozaimi
 */
#ifndef LOCKER_H
#define LOCKER_H

#include "PalLock.h"

class Locker
{
public:
    /**
     * Constructor - applies a lock.
     *
     * @param[in] lock The mutex.
     */
    Locker(PalLock::LockT *lock) : mLock(lock), mRwLock(0)
    {
        PalLock::take(lock);
    }

    /**
     * Constructor - applies a read or write lock.
     *
     * @param[in] rwLock    The read-write lock.
     * @param[in] isWriting true for write lock, false for read lock.
     */
    Locker(PalLock::RwLockT *rwLock, bool isWriting) :
    mLock(0), mRwLock(rwLock)
    {
        if (isWriting)
            PalLock::lockWr(rwLock);
        else
            PalLock::lockRd(rwLock);
    }

    ~Locker()
    {
        unlock();
    }

    /**
     * Releases the lock.
     */
    void unlock()
    {
        if (mLock != 0)
        {
            PalLock::release(mLock);
            mLock = 0;
        }
        else if (mRwLock != 0)
        {
            PalLock::release(mRwLock);
            mRwLock = 0;
        }
    }

private:
    PalLock::LockT   *mLock;
    PalLock::RwLockT *mRwLock;

    /**
     * Prevents copy construction.
     */
    Locker(const Locker &);

    /**
     * Prevents assignment operation.
     */
    Locker &operator=(const Locker &);
};
#endif //LOCKER_H
