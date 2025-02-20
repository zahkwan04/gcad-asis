/**
 * The module to display map overview.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisMapOverview.h 1857 2024-05-29 06:56:01Z rosnin $
 * @author Muhd Hashim Wahab
 */
#ifndef GISMAPOVERVIEW_H
#define GISMAPOVERVIEW_H

#include <QSpacerItem>

#include "GisCanvas.h"
#include "Logger.h"

namespace Ui {
class GisMapOverview;
}

class GisMapOverview : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] logger App logger.
     * @param[in] parent Parent widget, if any.
     */
    explicit GisMapOverview(Logger *logger, QWidget *parent = 0);

    ~GisMapOverview();

    /**
     * Applies color theme to UI components.
     */
    void onSetTheme();

    /**
     * Triggers map reload.
     */
    void reload();

    GisCanvas *getCanvas() { return mCanvas; }

private:
    GisCanvas          *mCanvas;
    Logger             *mLogger;
    QSpacerItem        *mSpacer;
    Ui::GisMapOverview *ui;
};
#endif //GISMAPOVERVIEW_H
