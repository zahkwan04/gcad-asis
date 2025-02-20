/**
 * The FlowLayout implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014. All Rights Reserved.
 *
 * @file
 * @version $Id: FlowLayout.cpp 796 2016-12-13 08:17:55Z hashim $
 * @author Mazdiana Makmor
 */
#include <assert.h>

#include "FlowLayout.h"

FlowLayout::FlowLayout(QWidget *parent,
                       int      margin,
                       int      hSpacing,
                       int      vSpacing) :
QLayout(parent), mHSpace(hSpacing), mVSpace(vSpacing)
{
    if (parent == 0)
    {
        assert("Bad param in FlowLayout::FlowLayout" == 0);
        return;
    }
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing) :
mHSpace(hSpacing), mVSpace(vSpacing)
{
    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    while (!isEmpty())
    {
        delete takeAt(0);
    }
}

int FlowLayout::horizontalSpacing() const
{
    if (mHSpace >= 0)
        return mHSpace;
    return defaultSpacing(QStyle::PM_LayoutHorizontalSpacing);
}

int FlowLayout::verticalSpacing() const
{
    if (mVSpace >= 0)
        return mVSpace;
    return defaultSpacing(QStyle::PM_LayoutVerticalSpacing);
}

QSize FlowLayout::minimumSize() const
{
    QSize        size;
    QLayoutItem *item;
    foreach (item, mLayoutItems)
    {
        size = size.expandedTo(item->minimumSize());
    }
    return size + QSize(2 * margin(), 2 * margin());
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    drawLayout(rect, true);
}

QLayoutItem *FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < mLayoutItems.size())
        return mLayoutItems.takeAt(index);
    return 0;
}

int FlowLayout::heightForWidth(int width) const
{
    return drawLayout(QRect(0, 0, width, 0), false);
}

int FlowLayout::drawLayout(const QRect &rect, bool doSetGeometry) const
{
    int left;
    int top;
    int right;
    int bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(left, top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    QLayoutItem *item;
    QWidget     *widget;
    int lineHeight = 0;

    foreach (item, mLayoutItems)
    {
        widget = item->widget();
        left = horizontalSpacing(); //reuse left variable
        if (left == -1)
            left = widget->style()->layoutSpacing(QSizePolicy::Frame,
                                                  QSizePolicy::Frame,
                                                  Qt::Horizontal);
        top = verticalSpacing(); //reuse top variable
        if (top == -1)
            top = widget->style()->layoutSpacing(QSizePolicy::Frame,
                                                 QSizePolicy::Frame,
                                                 Qt::Vertical);
        right = x + item->sizeHint().width() + left; //reuse right variable
        if (right - left > effectiveRect.right() && lineHeight > 0)
        {
             x = effectiveRect.x();
             y = y + lineHeight + top;
             right = x + item->sizeHint().width() + left;
             lineHeight = 0;
        }
        if (doSetGeometry)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));
        x = right;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::defaultSpacing(QStyle::PixelMetric pixMatrix) const
{
    QObject *parent = this->parent();
    if (parent == 0)
        return -1;
    if (parent->isWidgetType())
    {
        QWidget *widget = static_cast<QWidget *>(parent);
        return widget->style()->pixelMetric(pixMatrix, 0, widget);
    }
    return static_cast<QLayout *>(parent)->spacing();
}
