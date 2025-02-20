/**
 * Collection of Qt-specific utility functions.
 *
 * Copyright (C) Sapura Secured Technologies, 2015-2021. All Rights Reserved.
 *
 * @file
 * @version $Id: QtUtils.h 1567 2021-11-02 09:38:25Z zulzaidi $
 * @author Mohd Rozaimi
 * @author Rashid Yatim
 */
#ifndef QTUTILS_H
#define QTUTILS_H

#include <map>
#include <QComboBox>
#include <QDateTime>
#include <QIcon>
#include <QList>
#include <QListView>
#include <QMenu>
#include <QString>

namespace QtUtils
{
    extern bool                 gisOpen;
    extern std::map<int, QIcon> actionIconMap; //key is CmnTypes::eActionType
    extern std::map<int, QIcon> commsIconMap;  //key is CmnTypes::eCommsType
    extern std::map<int, QIcon> rscIconMap;    //key is CmnTypes::eIdType
    extern QString              timestampFormat;
    extern QString              timestampFormatWithDayName;

    /**
     * Converts a hexadecimal string of UTF8 Unicode values into QString.
     * Each value must be represented by an even number of hexadecimal
     * characters in host byte order (big-endian).
     * This assumes the input string contains only hexadecimal characters
     * without validating.
     * E.g. "414263313233" is "ABc123",
     *      "48656C6C6F" is "Hello",
     *
     * @param[in] hexStr The hexadecimal string.
     * @return The converted string.
     */
    QString fromHexUnicode(const QString &hexStr);

    /**
     * This is an overloaded function.
     *
     * @param[in] hexStr The hexadecimal string.
     * @return The converted string.
     */
    QString fromHexUnicode(const std::string &hexStr);

    /**
     * Sets the timestamp formats.
     *
     * @param[in] format            The regular date and time format,
     *                              e.g. "dd/MM/yyyy HH:mm:ss".
     * @param[in] formatWithDayName The longer format including the day name,
     *                              e.g. "ddd dd/MM/yyyy HH:mm:ss".
     * @return true if both formats are valid.
     */
    bool setTimestampFormats(const QString &format,
                             const QString &formatWithDayName);

    /**
     * Converts a datetime object into a timestamp string in one of the
     * pre-set formats: timestampFormat or timestampFormatWithDayName.
     * Uses the default locale for the latter.
     *
     * @param[in] dt      The datetime object.
     * @param[in] showDay true to use timestampFormatWithDayName.
     * @return The timestamp.
     */
    QString getTimestamp(const QDateTime &dt, bool showDay = false);

    /**
     * Gets the current time in one of the pre-set formats.
     * See getTimestamp(const QDateTime &, bool).
     *
     * @param[in] showDay true to use timestampFormatWithDayName.
     * @return The timestamp.
     */
    QString getTimestamp(bool showDay = false);

    /**
     * Converts a timestamp string into a datetime object.
     *
     * @param[in] ts     The timestamp.
     * @param[in] hasDay true if the timestamp uses the format string
     *                   timestampFormatWithDayName.
     * @return The datetime object.
     */
    QDateTime getDateTime(const QString &ts, bool hasDay = false);

    /**
     * Extracts space-separated substrings, which may be words or phrases,
     * from a string. A phrase is a multiword substring within double-quotes.
     *
     * @param[in] str The string.
     * @return The extracted substrings.
     */
    QStringList tokenize(const QString &str);

    /**
     * Checks whether the host of a URL is available.
     *
     * @param[in] url The URL.
     * @return true if available.
     */
    bool isHostAvailable(const QUrl &url);

    /**
     * Adds a string at the top of a combobox popup list. If the string
     * already exists, moves it to the top.
     *
     * @param[in]     txt The string.
     * @param[in,out] cb  The ComboBox.
     */
    void addToComboBox(const QString &txt, QComboBox *cb);

    /**
     * Gets the icon resource string for a call/message direction or message
     * type.
     *
     * @param[in] type CmnTypes::eCommsType.
     * @return The resource string.
     */
    QString getCommsIconSrc(int type);

    /**
     * Gets the resource text for a call/message direction or message type.
     *
     * @param[in] type CmnTypes::eCommsType.
     * @return The resource text.
     */
    QString getCommsText(int type);

    /**
     * Gets the icon for a call/message direction or message type.
     * If the icon for the type is not yet stored in the lookup table, creates
     * and adds it to the table.
     *
     * @param[in] type CmnTypes::eCommsType.
     * @return The icon.
     */
    const QIcon &getCommsIcon(int type);

    /**
     * Gets the icon resource string for a menu action type.
     *
     * @param[in] type The action type - CmnTypes::eActionType.
     * @return The resource string.
     */
    const QString getActionIconSrc(int type);

    /**
     * Gets the icon for a menu action type.
     * If the icon for the type is not yet stored in the lookup table, creates
     * and adds it to the table.
     *
     * @param[in] type The action type - CmnTypes::eActionType.
     * @return The icon.
     */
    const QIcon &getActionIcon(int type);

    /**
     * Gets the icon resource string for a resource type.
     *
     * @param[in] type The resource type - CmnTypes::eIdType.
     * @return The resource string.
     */
    const QString getRscIconSrc(int type);

    /**
     * Gets the icon for a resource type.
     *
     * @param[in] type The resource type - CmnTypes::eIdType.
     * @return The icon.
     */
    const QIcon &getRscIcon(int type);

    /**
     * Adds a menu action with predefined text for the given type.
     * Stores the action type into the action internal data.
     *
     * @param[in] menu The menu.
     * @param[in] type The action type - CmnTypes::eActionTypes.
     * @param[in] txt  The menu text, if any, that overrides the predefined text
     *                 for the type.
     * @return The action.
     */
    QAction *addMenuAction(QMenu &menu, int type, const QString &txt = "");

    /**
     * Adds menu actions using addMenuAction().
     *
     * @param[in] menu  The menu.
     * @param[in] types The action types - CmnTypes::eActionTypes.
     */
    void addMenuActions(QMenu &menu, const QList<int> &types);

    /**
     * Copies text into the clipboard.
     *
     * @param[in] txt The text.
     */
    void copyToClipboard(const QString &txt);

    /**
     * Copies text from a list selection into the clipboard - one per line.
     *
     * @param[in] list The list.
     */
    void copyToClipboard(const QListView *list);

    /**
     * Sets the GIS Window status.
     *
     * @param[in] open true if the window is open.
     */
    void setGisOpen(bool open);

    /**
     * Gets the GIS Window status.
     *
     * @return true if the window is open.
     */
    bool isGisOpen();
}
#endif //QTUTILS_H
