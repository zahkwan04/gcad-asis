/**
 * The module to display DGNA list.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisDgnaList.h 1833 2024-03-25 07:55:22Z hazim.rujhan $
 * @author Muhd Hashim Wahab
 */
#ifndef GISDGNALIST_H
#define GISDGNALIST_H

#include <QDialog>
#include <QSpacerItem>
#include <QWidget>

#include "Dgna.h"
#include "GisCanvas.h"
#include "ResourceData.h"
#include "Resources.h"

namespace Ui {
class GisDgnaList;
}

class GisDgnaList : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] resources The Resources object.
     * @param[in] dgna      The Dgna object.
     * @param[in] canvas    The GisCanvas object.
     * @param[in] parent    Parent widget, if any.
     */
    explicit GisDgnaList(Resources *resources,
                         Dgna      *dgna,
                         GisCanvas *canvas,
                         QWidget   *parent = 0);

    ~GisDgnaList();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

signals:
    void dock(QDialog *dlg);

public slots:
    /**
     * Shows the DGNA Assignment dialog.
     *
     * @param[in] mdl The potential members. Takes ownership.
     */
    void onDgnaAssign(ResourceData::ListModel *mdl);

private:
    Ui::GisDgnaList         *ui;
    GisCanvas               *mCanvas;
    QSpacerItem             *mSpacer;
    ResourceData::ListModel *mModel;
    //the following objects are owned by another module
    Dgna                    *mDgna;
    Resources               *mResources;

    /**
      * Handles a QPushButton click.
      *
      * @param[in] actType Action type for the respective buttons -
      *                    CmnTypes::ACTIONTYPE_*.
      */
    void onBtnClick(int actType);
};
#endif //GISDGNALIST_H
