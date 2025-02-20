/**
 * The routing UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisRouting.h 1833 2024-03-25 07:55:22Z hazim.rujhan $
 * @author Muhd Hashim Wahab
 */
#ifndef GISROUTING_H
#define GISROUTING_H

#include <QDialog>
#include <QSpacerItem>
#include <QWidget>

#include "GisCanvas.h"

namespace Ui {
class GisRouting;
}

class GisRouting : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] canvas The map canvas.
     * @param[in] parent Parent widget, if any.
     */
    explicit GisRouting(GisCanvas *canvas, QWidget *parent = 0);

    ~GisRouting();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Removes all points and clear text boxes.
     */
    void removePoints();

signals:
    void clearResult();
    void dock(QDialog *dlg);
    void getRouting(double srcLat, double srcLon, double tarLat, double tarlon);

public slots:
    void onCoordReceived(int typeId, double x, double y);

private:
    GisCanvas      *mMapCanvas;
    Ui::GisRouting *ui;
    QSpacerItem    *mSpacer;
};
#endif //GISROUTING_H
