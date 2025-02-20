/**
 * User Point of Interest dialog module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisMenuUserPoi.h 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#ifndef GISMENUUSERPOI_H
#define GISMENUUSERPOI_H

#include <QDialog>
#include <QStringListModel>

#include "Props.h"

namespace Ui {
class GisMenuUserPoi;
}

class GisMenuUserPoi : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * Creates a dialog box to insert user POI attributes.
     *
     * @param[in] props    The POI properties.
     * @param[in] userName The userName.
     * @param[in] parent   Parent widget, if any.
     */
    explicit GisMenuUserPoi(const Props::ValueMapT &props,
                            const QString          &userName,
                            QWidget                *parent = 0);

    ~GisMenuUserPoi();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Checks if POI is owned by current user.
     *
     * @return true if POI is owned by current user.
     */
    bool isOwner();

    /**
     * Sets the POI categories.
     */
    static void setCategories();

signals:
    void dismissed(Props::ValueMapT *pr);
    void poiDelete(int id);

public slots:
    /**
     * Updates the coordinates fields.
     *
     * @param[in] lat The latitude.
     * @param[in] lon The longitude.
     */
    void onPositionChanged(double lat, double lon);

private:
    Ui::GisMenuUserPoi *ui;
    std::string         mLat;
    std::string         mLon;
    std::string         mUserName;
    Props::ValueMapT    mProps;

    static QStringListModel sCatModel;
};
#endif //GISMENUUSERPOI_H
