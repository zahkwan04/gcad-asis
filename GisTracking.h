/**
 * The module to display BFT tracking.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisTracking.h 1834 2024-03-26 03:11:20Z hazim.rujhan $
 * @author Muhd Hashim Wahab
 */
#ifndef GISTRACKING_H
#define GISTRACKING_H

#include <set>
#include <QDialog>
#include <QWidget>

#include "GisCanvas.h"
#include "GisTrackingReplay.h"

namespace Ui {
class GisTracking;
}

class GisTracking : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] canvas The map canvas.
     * @param[in] parent Parent widget, if any.
     */
    explicit GisTracking(GisCanvas *canvas, QWidget *parent = 0);

    ~GisTracking();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Retrieves tracking data for selected ISSIs and starts playback.
     *
     * @param[in] issis The ISSIs.
     */
    void replay(const std::set<int> &issis);

signals:
    void dock(QDialog *dlg);

private:
    Ui::GisTracking   *ui;
    GisTrackingReplay *mReplay;
    GisCanvas         *mCanvas; //owned by another module

    /**
     * Gets ISSIs from user input and validates the time range.
     *
     * @param[out] issis The ISSIs.
     * @return true if input and time range are valid.
     */
    bool getIssis(std::set<int> &issis);
};
#endif //GISTRACKING_H
