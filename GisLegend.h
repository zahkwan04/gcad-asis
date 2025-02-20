/**
 * The Map Legend module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisLegend.h 1833 2024-03-25 07:55:22Z hazim.rujhan $
 * @author Mohd Fashan Abdul Munir
 */
#ifndef GISLEGEND_H
#define GISLEGEND_H

#include <QDialog>
#include <QSpacerItem>
#include <QWidget>

#include "GisCanvas.h"
#include "Logger.h"

namespace Ui {
class GisLegend;
}

class GisLegend : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] logger App logger.
     * @param[in] canvas The map canvas.
     * @param[in] parent Parent widget, if any.
     */
    explicit GisLegend(Logger *logger, GisCanvas *canvas, QWidget *parent = 0);

    ~GisLegend();

    /**
     * Displays the map legend.
     */
    void showLegend();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

signals:
    void dock(QDialog *dlg);

private:
    Ui::GisLegend *ui;
    GisCanvas     *mCanvas;
    Logger        *mLogger;
    QSpacerItem   *mSpacer;
};
#endif //GISLEGEND_H
