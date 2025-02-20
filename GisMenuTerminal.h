/**
 * Terminal dialog module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisMenuTerminal.h 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#ifndef GISMENUTERMINAL_H
#define GISMENUTERMINAL_H

#include <QDialog>

#include "Props.h"

namespace Ui {
class GisMenuTerminal;
}

class GisMenuTerminal : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * Creates a dialog box to show terminal actions.
     *
     * @param[in] props  The resource properties.
     * @param[in] parent Parent widget, if any.
     */
    explicit GisMenuTerminal(const Props::ValueMapT &props, QWidget *parent = 0);

    ~GisMenuTerminal();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Gets a string showing a point's location validity age, i.e. time elapsed
     * since its last valid location update.
     * Format: "(<d> days <h> hours <m> minutes)",
     *         where each of days, hours and minutes is shown only if non-zero.
     * This is formed only if the age exceeds the threshold in GisShapePoint.
     *
     * @param[in] t Elapsed time in seconds.
     * @return The age string. Empty if threshold not exceeded.
     */
    static QString getValidityAge(time_t t);

private:
    Ui::GisMenuTerminal *ui;
};
#endif //GISMENUTERMINAL_H
