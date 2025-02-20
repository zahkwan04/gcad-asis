/**
 * A custom layout class that arranges its child widgets horizontally and
 * vertically.
 *
 * Copyright (C) Sapura Secured Technologies, 2014. All Rights Reserved.
 *
 * @file
 * @version $Id: FlowLayout.h 796 2016-12-13 08:17:55Z hashim $
 * @author Mazdiana Makmor
 */
#ifndef FLOWLAYOUT_H
#define FLOWLAYOUT_H

#include <QLayout>
#include <QStyle>
#include <QWidget>

class FlowLayout : public QLayout
{
    Q_OBJECT

public:
    /**
     * Constructor for a top-level layout.
     *
     * @param[in] parent   Parent widget - cannot be 0.
     * @param[in] margin   Margin size in pixels. By default, QLayout uses
     *                     the values provided by the style in
     *                     QStyle::PixelMetric. See defaultSpacing().
     * @param[in] hSpacing Horizontal spacing in pixels.
     * @param[in] vSpacing Vertical spacing in pixels.
     */
    explicit FlowLayout(QWidget *parent,
                        int      margin   = -1,
                        int      hSpacing = -1,
                        int      vSpacing = -1);

    /**
     * Constructor for a child layout, which has to be inserted into another
     * layout.
     *
     * @param[in] margin   Margin size in pixels.
     * @param[in] hSpacing Horizontal spacing in pixels.
     * @param[in] vSpacing Vertical spacing in pixels.
     */
    explicit FlowLayout(int margin   = -1,
                        int hSpacing = -1,
                        int vSpacing = -1);

    ~FlowLayout();

    /**
     * Gets the horizontal spacing between the widgets.
     *
     * @return The spacing in pixels.
     */
    int horizontalSpacing() const;

    /**
     * Gets the vertical spacing between the widgets.
     *
     * @return The spacing in pixels.
     */
    int verticalSpacing() const;

    /**
     * Returns the minimum size of the layout.
     *
     * @return The minimum size in pixels.
     */
    QSize minimumSize() const;

    /**
     * Sets the geometry to a rectangle.
     *
     * @param[in] The rectangle.
     */
    void setGeometry(const QRect &rect);

    /**
     * Removes a layout item.
     *
     * @param[in] index The item index.
     * @return The removed item. Caller takes ownership.
     */
    QLayoutItem *takeAt(int index);

    /**
     * Sets the height of the layout to the width of the widget.
     *
     * @param[in] width The width of the widget.
     * @return The height of the layout.
     */
    int heightForWidth(int width) const;

    void addItem(QLayoutItem *item) { mLayoutItems.append(item); }

    Qt::Orientations expandingDirections() const { return 0; }

    bool hasHeightForWidth() const { return true; }

    int count() const { return mLayoutItems.size(); }

    QLayoutItem *itemAt(int index) const { return mLayoutItems.value(index); }

    QSize sizeHint() const { return minimumSize(); }

private:
    int                  mHSpace;
    int                  mVSpace;
    QList<QLayoutItem *> mLayoutItems;

    /**
     * Draws the layout area.
     *
     * @param[in] rect          The geometry rectangle.
     * @param[in] doSetGeometry true to set the item geometry.
     * @return The layout size in pixels.
     */
    int drawLayout(const QRect &rect, bool doSetGeometry) const;

    /**
     * Gets default spacing of the layout.
     *
     * @param[in] pixMatrix The style dependent size represented by a single
     *                      pixel value.
     * @return The default spacing of the layout.
     */
    int defaultSpacing(QStyle::PixelMetric pixMatrix) const;
};
#endif //FLOWLAYOUT_H
