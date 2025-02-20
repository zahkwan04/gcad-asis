/**
 * UI Active Incidents module.
 *
 * Copyright (C) Sapura Secured Technologies, 2020-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ActiveIncidents.h 1838 2024-04-03 01:11:56Z rosnin $
 * @author Rosnin Mustaffa
 */
#ifndef ACTIVEINCIDENTS_H
#define ACTIVEINCIDENTS_H

#include <map>
#include <QList>
#include <QMenu>
#include <QStringList>
#include <QWidget>

#include "FlowLayout.h"
#include "IncidentButton.h"
#include "IncidentData.h"

namespace Ui {
class ActiveIncidents;
}

class ActiveIncidents : public QWidget
{
    Q_OBJECT

public:
    //position options inside parent, if applicable
    enum ePos
    {
        POS_LEFT,
        POS_TOP,
        POS_RIGHT,
        POS_BOTTOM
    };

    /**
     * Constructor.
     *
     * @param[in] showFull true to show the full title and the toggle display
     *                     button, and hide the position button.
     * @param[in] parent   Parent widget, if any.
     */
    explicit ActiveIncidents(bool showFull, QWidget *parent = 0);

    ~ActiveIncidents();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Sets the filter button menu, replacing any existing one.
     */
    void setFilterMenu();

    /**
     * Clears the active incidents.
     */
    void clear();

    /**
     * Refreshes the icon on all incident buttons.
     */
    void refreshIcons();

    /**
     * Gets incident data.
     *
     * @param[in] id The incident ID.
     * @return The data or 0 if not found.
     */
    IncidentData *getData(int id);

    /**
     * Either creates a button for a new incident, or updates data on an
     * existing button, or deletes a button for a closed incident.
     *
     * @param[in] data The Incident data.
     */
    void updateCase(IncidentData *data);

    /**
     * Deletes an incident button.
     *
     * @param[in] id The incident ID.
     */
    void closeCase(int id);

    /**
     * Sets or clears an incident lock holder.
     *
     * @param[in] id         The incident ID.
     * @param[in] lockHolder The lock holder ID. 0 to clear.
     */
    void setLockHolder(int id, int lockHolder);

    /**
     * Gets a list of all active incident data.
     *
     * @return The list.
     */
    QList<IncidentData *> getAllData();

signals:
    void setPosition(int pos);
    void showData(IncidentButton *btn);

private:
    Ui::ActiveIncidents *ui;
    FlowLayout          *mFlowLayout;
    std::map<int, IncidentButton *> mIncidentMap; //key: ID

    /**
     * Adds the position button menu, allowing positioning options within the
     * parent widget.
     */
    void addPositionMenu();

    /**
     * Gets the filter data of a field.
     *
     * @param[in] field The filter field - IncidentButton::eField.
     * @return The data - empty for invalid field.
     */
    QStringList getFilterData(int field) const;

    /**
     * Adds an incident filter menu.
     *
     * @param[in] topMenu The menu to be added to.
     * @param[in] txt     The display text.
     * @param[in] field   The filter field - IncidentButton::eField.
     */
    void addFilter(QMenu *topMenu, const QString &txt, int field);
};
#endif //ACTIVEINCIDENTS_H
