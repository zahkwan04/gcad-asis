/**
 * The module to display map layer list.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisLayerList.h 1894 2024-12-20 07:15:28Z rosnin $
 * @author Muhd Hashim Wahab
 */
#ifndef GISLAYERLIST_H
#define GISLAYERLIST_H

#include <QDialog>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QWidget>

#include "GisCanvas.h"

namespace Ui {
class GisLayerList;
}

class GisLayerList : public QWidget
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
    explicit GisLayerList(Logger    *logger,
                          GisCanvas *canvas,
                          QWidget   *parent = 0);

    ~GisLayerList();

    /**
     * Populates the map layer list.
     */
    void populateLayerList();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Enables/disables seamap layers (SEADEPTH and SEAMARKS).
     *
     * @param[in] on true to enable.
     */
    void enableSeaMap(bool on);

signals:
    void dock(QDialog *dlg);

private:
    Ui::GisLayerList   *ui;
    GisCanvas          *mCanvas;
    Logger             *mLogger;
    QSpacerItem        *mSpacer;
    QStandardItemModel *mModel;
};
#endif //GISLAYERLIST_H
