/**
 * Drag and drop base class implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2015-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: Draggable.cpp 1695 2023-04-19 06:01:57Z zulzaidi $
 * @author Mohd Rozaimi
 * @author Zahari Hadzir
 */
#include <QApplication>
#include <QDrag>
#include <QMimeData>

#include "QtUtils.h"
#include "ResourceButton.h"
#include "Draggable.h"

const QString Draggable::MIMETYPE("application/x-dnd");

Draggable::Draggable(int type) : mDataStream(0)
{
    setType(type);
}

Draggable::~Draggable()
{
    delete mDataStream;
}

void Draggable::setType(int type)
{
    mType = type;
    mPixmap = QtUtils::getRscIcon(type).pixmap(ResourceButton::getIconSize());
}

void Draggable::appendData(const QString &text)
{
    *mDataStream << text;
}

void Draggable::appendData(int val)
{
    *mDataStream << val;
}

void Draggable::handleMousePress(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;
    mDragStartPos = event->pos();
    delete mDataStream;
    mDataStream = 0;
}

bool Draggable::handleMouseMove(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) == 0 || mDataStream != 0 ||
        (event->pos() - mDragStartPos).manhattanLength() <
            QApplication::startDragDistance())
    {
        return false;
    }
    mData.clear();
    mDataStream = new QDataStream(&mData, QIODevice::WriteOnly);
    *mDataStream << mType;
    return true;
}

void Draggable::startDrag(QObject *obj)
{
    //QMimeData and QDrag objects are destroyed by Qt
    auto *mimeData = new QMimeData();
    mimeData->setData(MIMETYPE, mData);
    auto *drag = new QDrag(obj);
    drag->setMimeData(mimeData);
    drag->setPixmap(mPixmap);
    drag->exec(Qt::CopyAction);
}
