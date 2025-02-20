/**
 * SCAD Client version information holder.
 *
 * Copyright (C) Sapura Secured Technologies, 2019-2020. All Rights Reserved.
 *
 * @file
 * @version $Id: Version.h 1379 2020-05-19 06:10:21Z rosnin $
 * @author Zulzaidi Atan
 */
#ifndef VERSION_H
#define VERSION_H

#include <string>
#include <QString>

namespace Version
{
    extern const QString APP_TITLE;
    extern const QString APP_NAME;
    extern const QString NWK_NAME;
    extern const QString APP_VERSION;
    extern const QString APP_NAME_VERSION;
    extern const QString COPYRIGHT;
    extern const std::string LOG_HEADER;

    /**
     * Gets the full copyright statement, which is subject to translation.
     *
     * @return The copyright statement.
     */
    QString copyright();

    /**
     * Gets the log header containing application name, version, and
     * copyright statement which is never translated.
     *
     * @return The log header.
     */
    std::string logHeader();
}
#endif //VERSION_H
