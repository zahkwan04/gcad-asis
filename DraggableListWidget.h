/**
 * QListWidget that supports drag and drop.
 *
 * Copyright (C) Sapura Secured Technologies, 2015. All Rights Reserved.
 *
 * @file
 * @version $Id: DraggableListWidget.h 455 2015-12-03 00:29:24Z mohdrashid $
 * @author Mohd Rashid
 */
#ifndef DRAGGABLELISTWIDGET_H
#define DRAGGABLELISTWIDGET_H

#include <QListWidget>
#include <QMouseEvent>

#include "Draggable.h"

class DraggableListWidget : public QListWidget, public Draggable
{
    Q_OBJECT

public:
    DraggableListWidget(int type, QWidget *parent = 0);

protected:
    //QListWidget overrides
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};
#endif //DRAGGABLELISTWIDGET_H
