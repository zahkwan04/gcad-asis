/**
 * SCAD Client version definitions.
 *
 * Copyright (C) Sapura Secured Technologies, 2019-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Version.cpp 1905 2025-02-21 02:55:53Z rosnin $
 * @author Zulzaidi Atan
 */
#include <QObject>

#include "Version.h"

const QString Version::APP_TITLE("SCAD");
const QString Version::APP_NAME("GIRN Dispatcher");
const QString Version::NWK_NAME(
                       "Government Integrated Radio Network (GIRN) Dispatcher");
const QString Version::APP_VERSION("02.05.02");
const QString Version::APP_NAME_VERSION(APP_NAME + " v" + APP_VERSION);
const QString Version::COPYRIGHT("Sapura Secured Technologies, 2013-2025");

std::string Version::logHeader()
{
    return std::string(
        "\n*****************************************************************"
        "**********\n" +
        APP_NAME_VERSION.toStdString() +
        "\nCopyright (C) " + COPYRIGHT.toStdString() +
        ". All Rights Reserved.\n"
        "*******************************************************************"
        "********");
}

QString Version::copyright()
{
    return QObject::tr("Copyright %1 %2. All Rights Reserved.")
               .arg(QChar(0xA9)) //copyright symbol
               .arg(COPYRIGHT);
}
