/**
 * Platform Abstraction Layer to provide common interface to
 * platform-specific files-related functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2013. All Rights Reserved.
 *
 * @version $Id: PalFiles.h 59 2013-09-25 08:06:01Z mrozaimi $
 * @author Mohd Rozaimi
 */
#ifndef PAL_FILES_H
#define PAL_FILES_H

#include <set>
#include <sstream>
#include <string>
#ifdef QT_CORE_LIB
#include <QDir>
#else
#include <dirent.h>
#endif //QT_CORE_LIB

namespace PalFiles
{
    inline int findFiles(const std::string     &path,
                         const std::string     &prefix,
                         const std::string     &ext,
                         std::set<std::string> &files)
    {
        const size_t tsLen     = sizeof("YYYYMMdd_hhmmss") - 1;
        const size_t tsSepPos  = 8;  //date and time separator '_' position
        const size_t prefixLen = prefix.size();
        int          i;
        int          j;
        std::string  fname;

#ifdef QT_CORE_LIB
        std::string timestamp("????????_??????");
        QStringList qslist = QDir(QString::fromStdString(path),
                                  QString::fromStdString(std::string(
                                                 prefix + timestamp + ext))).
                             entryList();
        QStringList::const_iterator qit = qslist.begin();
        for (; qit!=qslist.constEnd(); ++qit)
        {
            //prefix and ext already matched, now check timestamp format
            fname = qit->toStdString();
            std::istringstream is(fname.substr(prefixLen, tsLen)
                                       .replace(tsSepPos, 1, 1, ' '));
            if (is >> i >> j)
                files.insert(fname);
        }

#else //linux
        DIR *dp = opendir((path.empty())? ".": path.c_str());
        if (dp != NULL)
        {
            size_t      extLen = ext.size();
            std::string ts;

            //check every directory content
            struct dirent *dEnt = readdir(dp);
            while (dEnt != NULL)
            {
                fname.assign(dEnt->d_name);
                //check whether this is an old log file
                //first, check the length and prefix
                if (fname.size() == prefixLen + tsLen + extLen &&
                    fname.compare(0, prefixLen, prefix) == 0)
                {
                    //check extension
                    if (extLen == 0 ||
                        fname.compare(prefixLen + tsLen, extLen, ext) == 0)
                    {
                        //check timestamp format
                        ts = fname.substr(prefixLen, tsLen);
                        if (ts[tsSepPos] == '_')
                        {
                            ts.replace(tsSepPos, 1, 1, ' ');
                            std::istringstream is(ts);
                            if (is >> i >> j)
                                files.insert(fname);
                        }
                    }
                }
                dEnt = readdir(dp);
            } //while (dEnt != NULL)
            closedir(dp);
        } //if (dp != NULL)
#endif //QT_CORE_LIB

        return files.size();
    }
} //namespace PalFiles

#endif //PAL_FILES_H
