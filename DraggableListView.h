/**
 * QListView that supports drag and drop.
 *
 * Copyright (C) Sapura Secured Technologies, 2015. All Rights Reserved.
 *
 * @file
 * @version $Id: DraggableListView.h 455 2015-12-03 00:29:24Z mohdrashid $
 * @author Mohd Rozaimi
 * @author Mohd Rashid
 */
#ifndef DRAGGABLELISTVIEW_H
#define DRAGGABLELISTVIEW_H

#include <QListView>
#include <QMouseEvent>

#include "Draggable.h"

class DraggableListView : public QListView, public Draggable
{
    Q_OBJECT

public:
    DraggableListView(int type, QWidget *parent = 0);

protected:
    //QListView overrides
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
};
#endif //DRAGGABLELISTVIEW_H
