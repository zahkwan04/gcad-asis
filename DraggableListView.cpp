/**
 * Draggable QListView implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2015-2020. All Rights Reserved.
 *
 * @file
 * @version $Id: DraggableListView.cpp 1355 2020-04-13 04:25:55Z rosnin $
 * @author Mohd Rozaimi
 * @author Mohd Rashid
 */
#include "ResourceData.h"
#include "DraggableListView.h"

DraggableListView::DraggableListView(int type, QWidget *parent) :
QListView(parent), Draggable(type)
{
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
}

void DraggableListView::mousePressEvent(QMouseEvent *event)
{
    handleMousePress(event);
    QListView::mousePressEvent(event);
}

void DraggableListView::mouseMoveEvent(QMouseEvent *event)
{
    if (!handleMouseMove(event))
        return;
    QModelIndexList l(selectedIndexes());
    if (l.isEmpty())
        return;
    auto *mdl = dynamic_cast<ResourceData::ListModel *>(model());
    if (mdl != 0)
    {
        for (const auto &idx : l)
        {
            appendData(ResourceData::getItemId(mdl->item(idx.row())));
        }
    }
    else
    {
        for (const auto &idx : l)
        {
            appendData(idx.data().toString());
        }
    }
    Draggable::startDrag(this);
}
