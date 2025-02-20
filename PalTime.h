/**
 * Platform Abstraction Layer to provide common interface to
 * platform-specific time-related functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2018. All Rights Reserved.
 *
 * @version $Id: PalTime.h 1135 2018-01-26 04:00:11Z zulzaidi $
 * @author Mohd Rozaimi
 */
#ifndef PAL_TIME_H
#define PAL_TIME_H

#include <time.h>       //localtime_r(), localtime_s()
#if defined(_WIN32) || defined (WIN32)
#include <sys/timeb.h>  //struct timeb, ftime()
#else //linux
#include <sys/time.h>   //struct timeval, gettimeofday
#endif

#ifdef QT_CORE_LIB  //QT ==================================================
#include <QTime>

namespace PalTime
{
    inline unsigned int rand()
    {
        QTime qtime = QTime::currentTime();
        //no usec in QT
        return (qtime.second() * qtime.msec());
    }
} //namespace PalTime

#else  //linux ============================================================
namespace PalTime
{
    inline unsigned int rand()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return (tv.tv_sec * tv.tv_usec);
    }
} //namespace PalTime

#endif //QT_CORE_LIB

namespace PalTime
{
    inline time_t localtime(struct tm *stm, const time_t *timeVal = 0)
    {
        //ftime() is obsolete on Linux and should be replaced by
        //gettimeofday(), but the latter is not available on Windows
#if defined(_WIN32) || defined (WIN32)
        if (timeVal != 0)
        {
            localtime_s(stm, timeVal);
            return 0;
        }
        struct timeb tb;
        ftime(&tb);
        localtime_s(stm, &tb.time);
        return tb.millitm;
#else //linux
        if (timeVal != 0)
        {
            localtime_r(timeVal, stm);
            return 0;
        }
        struct timeval tv;
        gettimeofday(&tv, NULL);
        localtime_r(&tv.tv_sec, stm);
        return (tv.tv_usec/1000);
#endif
    }
} //namespace PalTime

#endif //PAL_TIME_H
