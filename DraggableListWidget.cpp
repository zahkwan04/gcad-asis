/**
 * Draggable QListWidget implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2015-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: DraggableListWidget.cpp 1695 2023-04-19 06:01:57Z zulzaidi $
 * @author Mohd Rashid
 */
#include "DraggableListWidget.h"

DraggableListWidget::DraggableListWidget(int type, QWidget *parent) :
QListWidget(parent), Draggable(type)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
}

void DraggableListWidget::mousePressEvent(QMouseEvent *event)
{
    handleMousePress(event);
    QListWidget::mousePressEvent(event);
}

void DraggableListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!handleMouseMove(event))
        return;
    auto l(selectedItems());
    if (!l.isEmpty())
    {
        for (auto &it : l)
        {
            appendData(it->text());
        }
        Draggable::startDrag(this);
    }
}
