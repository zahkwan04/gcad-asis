/**
 * GIS trailing selector module to allow selection of terminals to enable
 * trailing on the map.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisTrailingSelector.h 1819 2024-02-29 01:50:24Z rosnin $
 * @author Mohd Rozaimi
 * @author Muhd Hashim Wahab
 */
#ifndef GISTRAILINGSELECTOR_H
#define GISTRAILINGSELECTOR_H

#include <QDialog>
#include <QSortFilterProxyModel>

#include "DraggableListView.h"
#include "GisCanvas.h"

namespace Ui {
class GisTrailingSelector;
}

class GisTrailingSelector : public QDialog
{
    Q_OBJECT

public:
    ~GisTrailingSelector();

    /**
     * Shows the trailing selector dialog.
     *
     * @param[in] canvas The map canvas.
     * @param[in] parent The parent widget.
     */
    static void display(GisCanvas *canvas, QWidget *parent = 0);

protected:
    //override
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::GisTrailingSelector *ui;
    DraggableListView       *mRscList;
    DraggableListView       *mTrlList;
    QSortFilterProxyModel   *mSortModel;
    GisCanvas               *mCanvas;    //not owned

    /**
     * Constructor.
     *
     * @param[in] canvas The map canvas.
     * @param[in] parent Parent widget.
     */
    explicit GisTrailingSelector(GisCanvas *canvas, QWidget *parent);
};
#endif //GISTRAILINGSELECTOR_H
