/**
 * The Incident Button UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: IncidentButton.h 1823 2024-03-11 09:03:42Z rosnin $
 * @author Mazdiana Makmor
 */
#ifndef INCIDENTBUTTON_H
#define INCIDENTBUTTON_H

#include <map>
#include <set>
#include <QMouseEvent>
#include <QPixmap>
#include <QWidget>

#include "IncidentData.h"

namespace Ui {
class IncidentButton;
}

class IncidentButton : public QWidget
{
    Q_OBJECT

public:
    enum eField
    {
        FIELD_ADDR2,
        FIELD_ADDRSTATE,
        FIELD_CATEGORY,
        FIELD_PRIORITY,
        FIELD_STATE
    };

    static const char FILTER_SEP = ',';

    /**
     * Constructor.
     *
     * @param[in] data   The incident data. Takes ownership.
     * @param[in] parent Parent widget, if any.
     */
    explicit IncidentButton(IncidentData *data, QWidget *parent = 0);

    ~IncidentButton();

    /**
     * Triggers the mouse release action.
     */
    void release();

    /**
     * Updates the state and background color.
     *
     * @param[in] data The incident data.
     */
    void updateData(IncidentData *data);

    IncidentData *getData() { return mData; }

    /**
     * Refreshes the icon.
     */
    void refreshIcon();

    /**
     * Sets the lock holder.
     *
     * @param[in] holder The user ID, or 0 if unlocked.
     */
    void setLockHolder(int holder);

    /**
     * Sets the visibility based on filter criteria.
     */
    void setFilteredVisible();

    /**
     * Loads custom icons to replace built-in ones and to add for base
     * categories without built-in icons. Clears current icons upon finding the
     * first valid icon.
     * Icon files must be named "icon_inc_<cat>.png", where <cat> is in the
     * given base categories.
     *
     * @param[in]  cats  The base categories.
     * @param[in]  dir   The source directory.
     * @param[out] files Only if successful, the directory and loaded base
     *                   categories, on separate lines.
     * @return true if successful - at least one icon loaded.
     */
    static bool setCustomIcons(const QStringList &cats,
                               const QString     &dir,
                               QString           &files);

    /**
     * Clears current icons to reload default.
     */
    static void setDefaultIcons();

    /**
     * Sets the button style, and optionally the status label text, based on the
     * status.
     *
     * @param[in] doSetLabel true to set the status label text.
     */
    void setStyle(bool doSetLabel = false);

    /**
     * Checks whether any filter is set for a field.
     *
     * @param[in] field The field - eField.
     * @return true if has filter.
     */
    static bool hasFilter(int field);

    /**
     * Checks whether an integer value is in a filter.
     *
     * @param[in] field The filter field - eField.
     * @param[in] value The value.
     * @return true if value exists.
     */
    static bool isInFilter(int field, int value);

    /**
     * Checks whether a string value is in a filter.
     *
     * @param[in] field The filter field - eField.
     * @param[in] value The value.
     * @return true if value exists.
     */
    static bool isInFilter(int field, const QString &value);

    /**
     * Adds or removes a value to/from a filter.
     *
     * @param[in] field The filter field - eField.
     * @param[in] value The value.
     * @param[in] doAdd true to add.
     */
    static void updateFilter(int field, const QString &value, bool doAdd = true);

    /**
     * Adds a list of values to a filter.
     *
     * @param[in] field  The filter field - eField.
     * @param[in] values The values.
     */
    static void addFilter(int field, const QStringList &values);

    /**
     * Gets a list of values from a filter.
     *
     * @param[in] field The filter field - eField.
     * @return The comma separated list. Empty string if filter is configured
     *         with all possible values.
     */
    static std::string getFilter(int field);

    /**
     * Sets the maximum size of a filter, which is based on the total number of
     * possible values for a field.
     *
     * @param[in] field The filter field - eField.
     * @param[in] size  The size.
     */
    static void setFilterMaxSize(int field, int size);

    /**
     * Clears all filters.
     */
    static void resetFilter();

    /**
     * Gets the icon pixmap for an Incident base category.
     * Creates and stores the pixmap if not yet created for the category.
     *
     * @param[in,out] cat The category. May be changed to default if not
     *                    recognized.
     * @return The pixmap.
     */
    static const QPixmap &getPixmap(std::string &cat);

signals:
    void showData();

protected:
    /**
     * Changes the button appearance for mouse hover. Parameter not used.
     */
    void enterEvent(QEvent *);

    /**
     * Restores the button appearance. Parameter not used.
     */
    void leaveEvent(QEvent *);

    /**
     * Changes the button appearance to indicate a pressed state, and displays
     * the incident data.
     *
     * @param[in] event The mouse press event.
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * Restores the button appearance.
     *
     * @param[in] event The mouse press event.
     */
    void mouseReleaseEvent(QMouseEvent *event);

private:
    typedef std::set<int>             IntValuesT;
    typedef std::set<QString>         StrValuesT;
    //key is eField
    typedef std::map<int, IntValuesT> IntFilterT;
    typedef std::map<int, StrValuesT> StrFilterT;
    typedef std::map<int, int>        FilterMaxSizesT;

    Ui::IncidentButton *ui;
    IncidentData       *mData;

    static IntFilterT      sIntFilter;
    static StrFilterT      sStrFilter;
    static FilterMaxSizesT sFilterMax;
    //key=base category, value=icon - populated dynamically at runtime
    static std::map<std::string, QPixmap> sIconMap;

    /**
     * Sets the tooltip text based on the lock holder ID.
     *
     * @param[in] lockHolder The ID, or 0 to clear text.
     */
    void setLockToolTip(int lockHolder);
};
#endif //INCIDENTBUTTON_H
