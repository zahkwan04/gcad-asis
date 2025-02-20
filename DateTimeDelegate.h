/**
 * Class for displaying date and time text of model data items.
 *
 * Copyright (C) Sapura Secured Technologies, 2016. All Rights Reserved.
 *
 * @file
 * @version $Id: DateTimeDelegate.h 681 2016-08-23 09:18:50Z zulzaidi $
 * @author Zulzaidi Atan
 */
#ifndef DATETIMEDELEGATE_H
#define DATETIMEDELEGATE_H

#include <QStyledItemDelegate>

class DateTimeDelegate: public QStyledItemDelegate
{
    Q_OBJECT

public:
    DateTimeDelegate(QObject *parent = 0): QStyledItemDelegate(parent)
    {}

    //override
    QString displayText(const QVariant &value, const QLocale &locale) const;

    /**
     * Returns a DateTime value as a string in QtUtils::timestampFormat.
     *
     * @param[in] value The DateTime value.
     * @return The formatted string.
     */
    static QString getDateTime(const QVariant &value);
};
#endif //DATETIMEDELEGATE_H
