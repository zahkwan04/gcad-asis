/**
 * Base class for drag and drop support. Provides functions to start a drag
 * process.
 *
 * Copyright (C) Sapura Secured Technologies, 2015-2020. All Rights Reserved.
 *
 * @file
 * @version $Id: Draggable.h 1355 2020-04-13 04:25:55Z rosnin $
 * @author Mohd Rozaimi
 * @author Zahari Hadzir
 */
#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <QByteArray>
#include <QDataStream>
#include <QMouseEvent>
#include <QPixmap>
#include <QPoint>

class Draggable
{
public:
    static const QString MIMETYPE;

    /**
     * Constructor.
     *
     * @param[in] type The resource type. See CmnTypes::eIdType.
     */
    Draggable(int type);

    ~Draggable();

    /**
     * Sets the type and drag pixmap.
     *
     * @param[in] type The type.
     */
    void setType(int type);

    int getType() const { return mType; }

protected:
    int mType;

    /**
     * Appends text to the drag data.
     *
     * @param[in] text The text.
     */
    void appendData(const QString &text);

    /**
     * Appends a value to the drag data.
     *
     * @param[in] val The value.
     */
    void appendData(int val);

    /**
     * Initializes the drag process if the left mouse button is pressed.
     * To be called by the component MousePress event handler.
     *
     * @param[in] event The event.
     */
    void handleMousePress(QMouseEvent *event);

    /**
     * Initializes the drag data if the given event can start a drag process,
     * which means that the process has not yet been started and the mouse
     * has moved a minimum distance with the left button pressed.
     * To be called by the component MouseMove event handler.
     *
     * @param[in] event The event.
     * @return true if the event can start a drag process and the caller can
     *         call startDrag().
     */
    bool handleMouseMove(QMouseEvent *event);

    /**
     * Starts a drag process.
     *
     * @param[in] obj The caller component.
     */
    void startDrag(QObject *obj);

private:
    QByteArray   mData;
    QPixmap      mPixmap;
    QPoint       mDragStartPos;
    QDataStream *mDataStream;
};
#endif //DRAGGABLE_H
