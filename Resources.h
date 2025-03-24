/**
 * UI Resources module.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Resources.h 1908 2025-03-05 00:54:00Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#ifndef RESOURCES_H
#define RESOURCES_H

#include <map>
#include <set>
#include <QList>
#include <QModelIndexList>
#include <QPoint>
#include <QStandardItem>
#include <QString>
#include <QTableWidget>

#include "DraggableListView.h"
#include "Logger.h"
#include "ResourceButton.h"
#include "ResourceData.h"

namespace Ui {
class Resources;
}

class Resources : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] logger App logger.
     * @param[in] parent Parent widget, if any.
     */
    explicit Resources(Logger *logger, QWidget *parent = 0);

    ~Resources();

    /**
     * Completes the UI initialization.
     *
     * @param[in] full true for full mode, i.e. showing all resource types.
     */
    void init(bool full);

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Loads the subscriber data.
     *
     * @param[in] username The current username.
     */
    void loadData(const QString &username);

    /**
     * Reloads the subscriber or group data.
     *
     * @param[in] type The data type - ResourceData::eType.
     * @param[in] id   The specific subscriber or group ID.
     */
    void reloadData(int type, int id);

    /**
     * Adds or removes an ID from a list model.
     *
     * @param[in] doAdd true to add, false to remove.
     * @param[in] id    The ID.
     * @param[in] type  The ID type - ResourceData::eType.
     */
    void addRemoveId(bool doAdd, int id, int type);

    /**
     * Clears all tabs and cleans up data.
     *
     * @param[in] onLogoutNoExit true if called on logout without exit.
     */
    void deactivate(bool onLogoutNoExit);

    /**
     * Emits the selected DGNA-Ind group data.
     *
     * @param[in] gssi The GSSI.
     */
    void selectDgnaInd(int gssi);

    /**
     * Emits the selected DGNA group data.
     *
     * @param[in] type The resource type - ResourceData::eType.
     * @param[in] gssi The GSSI.
     */
    void selectedDgna(int type, int gssi);

    /**
     * Sets or clears the active indicator on a group button (including in
     * phonebook) or list item.
     *
     * @param[in] gssi The GSSI.
     */
    void setGrpActive(int gssi);

    /**
     * Displays attached members of a group.
     *
     * @param[in] gssi   The GSSI.
     * @param[in] parent The parent widget, if any.
     */
    void showGrpAttachedMembers(int gssi, QWidget *parent = 0);

    /**
     * Adds to group attachments history.
     *
     * @param[in] detach true for detachment of all GSSIs, false if need to
     *                   determine attachment/detachment of each GSSI from
     *                   SubsData.
     * @param[in] issi   The ISSI.
     * @param[in] gssis  The affected GSSIs.
     */
    void addGrpAttachData(bool detach, int issi, const std::set<int> &gssis);

signals:
    void dgnaSelected(int gssi, int type, ResourceData::ListModel *rscModel);
    void locateResource(int issi);
    void sendData(int type);
    void startAction(int actType, int idType, const ResourceData::IdsT &r);

protected:
    /**
     * Sets focus to the search text box.
     */
    void showEvent(QShowEvent *);

private:
    Ui::Resources *ui;
    Logger        *mLogger;

    //search result for each ResourceData::TYPE_*
    std::map<int, ResourceData::ListModel *> mSearchResultMap;
    //grp attachments as in mGrpAttTbl, GSSI=>ISSIs
    std::map<int, std::set<int>>             mGrpAttMap;

    QString                  mUsername;
    QTableWidget            *mGrpAttTbl;        //grp attachments history
    ResourceData::ListModel *mDgnaMembersModel; //potential DGNA members
    QList<ResourceButton *>  mPhonebookButtons;

    /**
     * Refreshes the group attachments history from SubsData. Only adds changes,
     * i.e. current attachments that are not in the history, and attachments in
     * history that are no longer present.
     */
    void refreshGrpAtt();

    /**
     * Checks whether the given resource type has an Active state.
     * Currently this is true for a group type only.
     *
     * @param[in] type The resource type - ResourceData::eType.
     * @return true for type with Active state.
     */
    bool hasActiveState(int type) const;

    /**
     * Finds resource items matching a search text.
     *
     * @param[in] type The resource type - ResourceData::eType.
     * @param[in] re   The search text as a regular expression.
     */
    void search(int type, const QString &re);

    /**
     * Displays search results in the resource type tab.
     *
     * @param[in] type The resource type - ResourceData::eType.
     */
    void showSearchResults(int type);

    /**
     * Displays resources in the resource type tab in list view.
     *
     * @param[in] type     The resource type - ResourceData::eType.
     * @param[in] filtered true to show the filtered resources.
     * @return The list size.
     */
    int showList(int type, bool filtered);

    /**
     * Displays resources in the resource type tab in button view.
     *
     * @param[in] type     The resource type - ResourceData::eType.
     * @param[in] filtered true to show the filtered resources.
     * @return The number of buttons.
     */
    int showButtons(int type, bool filtered);

    /**
     * Creates a resource button.
     *
     * @param[in] type        The resource type - ResourceData::eType.
     * @param[in] id          The resource ID (ISSI, GSSI or dispatcher ID).
     * @param[in] enableNotes true to enable notes on the button,
     *                        i.e. for a Phonebook entry.
     * @param[in] parent      The parent widget containing a layout where to add
     *                        the button, if any.
     * @param[in] notes       The notes text.
     * @return The created button.
     */
    ResourceButton *createButton(int      type,
                                 int      id,
                                 bool     enableNotes,
                                 QWidget *parent = 0,
                                 QString *notes = 0);

    /**
     * Finds a phonebook item with the given type and ID.
     *
     * @param[in] type The resource type - ResourceData::eType.
     * @param[in] id   The resource ID.
     * @return The resource button, or 0 if not found.
     */
    ResourceButton *phonebookGetItem(int type, int id);

    /**
     * Adds a resource item to the phonebook if it does not exist.
     *
     * @param[in] type  The resource type - ResourceData::eType.
     * @param[in] id    The resource ID.
     * @param[in] notes The notes text.
     * @return true if resource item is successfully added.
     */
    bool phonebookAddItem(int type, int id, QString *notes = 0);

    /**
     * Adds resource items to the phonebook.
     *
     * @param[in] list    The originating resource list.
     * @param[in] idxList The resource list indexes.
     */
    void phonebookAddItems(const DraggableListView *list,
                           const QModelIndexList   &idxList);

    /**
     * Removes a resource button from the phonebook.
     *
     * @param[in] btn The button.
     */
    void phonebookRemoveItem(ResourceButton *btn);

    /**
     * Removes resource buttons that are no longer valid.
     * This is to refresh the phonebook if it is not cleared after data update.
     *
     * @param[in] ids The invalid IDs.
     */
    void phonebookRemoveItems(const std::set<int> &ids);

    /**
     * Clears the phonebook entries.
     *
     * @param[in] doCreateLayout true to create a new empty layout.
     */
    void phonebookClear(bool doCreateLayout);

    /**
     * Updates status of phonebook item(s) as follows:
     *   -All items if type is ResourceData::TYPE_UNKNOWN.
     *   -Else an item type if ID is 0.
     *   -Else a specific item.
     *
     * @param[in] type   The item type - ResourceData::eType.
     * @param[in] id     The resource ID.
     * @param[in] online true for online status.
     *                   Used only if updating a specific named item.
     */
    void phonebookUpdate(int  type   = ResourceData::TYPE_UNKNOWN,
                         int  id     = 0,
                         bool online = true);

    /**
     * Sorts phonebook items by resource button text in alphabetical order and
     * grouped by type.
     */
    void phonebookSort();

    /**
     * Deletes all items from a tab.
     *
     * @param[in] type The resource type.
     *                 ResourceData::TYPE_UNKNOWN to clear all types.
     */
    void clearTab(int type);

    /**
     * Displays the context menu for a resource from a list or button, and emits
     * the required signal on menu selection.
     *
     * @param[in] list The resource list. 0 if using btn.
     * @param[in] btn  The resource button. 0 if using list.
     * @param[in] pos  The mouse position.
     */
    void showContextMenu(DraggableListView *list,
                         ResourceButton    *btn,
                         const QPoint      &pos);

    /**
     * Sets a group resource list item appearance according to the group member
     * assignment/attachment status. Affects the background color and tooltip.
     *
     * @param[in] item The list item.
     * @param[in] gssi The GSSI when updating the list. Omit when creating the
     *                 list.
     */
    void setGrpStatus(QStandardItem *item, int gssi = 0);

    /**
     * Sets a mobile resource list item appearance based on its online status.
     *
     * @param[in] item The list item.
     * @param[in] stat Positive if online, 0 if offline, negative if unknown and
     *                 to be checked with ResourceData.
     */
    void setMobOnlineStatus(QStandardItem *item, int stat = -1);

    /**
     * Adds a search result button.
     *
     * @param[in] type The resource type - ResourceData::eType.
     * @param[in] tab  The corresponding tab widget.
     */
    void addSearchResBtn(int type, QWidget *tab);
};
//use custom type as signal parameter
Q_DECLARE_METATYPE(ResourceData::ListModel *)
#endif //RESOURCES_H
