/**
 * Displays the date and time of data items from a model according to the
 * predefined format.
 *
 * Copyright (C) Sapura Secured Technologies, 2016. All Rights Reserved.
 *
 * @file
 * @version $Id: DateTimeDelegate.cpp 681 2016-08-23 09:18:50Z zulzaidi $
 * @author Zulzaidi Atan
 */
#include <QDateTime>

#include "QtUtils.h"
#include "DateTimeDelegate.h"

QString DateTimeDelegate::displayText(const QVariant &value,
                                      const QLocale  &locale) const
{
    if (value.type() == QVariant::DateTime)
        return getDateTime(value);
    return QStyledItemDelegate::displayText(value, locale);
}

QString DateTimeDelegate::getDateTime(const QVariant &value)
{
    return value.toDateTime().toString(QtUtils::timestampFormat);
}
