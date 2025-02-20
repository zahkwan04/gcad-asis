/**
 * UI ResourceSelector module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ResourceSelector.h 1813 2024-02-15 06:01:16Z rosnin $
 * @author Mazdiana Makmor
 */
#ifndef RESOURCESELECTOR_H
#define RESOURCESELECTOR_H

#include <map>
#include <QCompleter>
#include <QList>
#include <QStringListModel>
#include <QWidget>

#include "ResourceData.h"

namespace Ui {
class ResourceSelector;
}

class ResourceSelector : public QWidget
{
    Q_OBJECT

public:
    struct Rsc
    {
        Rsc() {}
        Rsc(int t, const QString &n) : type(t), name(n) {}

        int     type = 0;
        QString name;
    };
    typedef std::map<int, Rsc> SelectionsT; //index is resource ID

    /**
     * Constructor.
     *
     * @param[in] parent Parent widget, if any.
     */
    explicit ResourceSelector(QWidget *parent = 0);

    ~ResourceSelector();

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
     * Sets the mobile type for the resource list.
     *
     * @param[in] online true for online type.
     */
    void setMobileType(bool online);

    /**
     * Enables (shows) or disables (hides) the multiple recipients UI.
     *
     * @param[in] enable true to enable.
     */
    void enableMultiSelect(bool enable);

    /**
     * Sets the selected IDs.
     *
     * @param[in] type      Resource type - ResourceData::eType.
     * @param[in] ids       The IDs.
     * @param[in] doAdd     true to add to the recipient list.
     * @param[in] doReplace true to replace the recipient list instead of
     *                      appending to it.
     * @return true if at least one selection is accepted.
     */
    bool setSelectedId(int                       type,
                       const ResourceData::IdsT &ids,
                       bool                      doAdd,
                       bool                      doReplace = false);

    /**
     * Sets the selected ID. If type is unknown, tries to determine it first.
     *
     * @param[in] type Resource type - ResourceData::eType.
     * @param[in] id   The ID.
     * @return true for valid input.
     */
    bool setSelectedId(int type, int id);

    /**
     * Gets the single-selection recipient.
     *
     * @param[out] type Resource type - ResourceData::eType.
     * @param[out] ssi  The SSI.
     * @param[out] name The name, if required.
     * @return true if the selected ID is valid.
     */
    bool getSelectedId(int &type, int &ssi, QString *name = 0);

    /**
     * Checks whether there is one or more valid recipients selected.
     *
     * @param[in] includeSingle true to include the single-selected recipient.
     * @return true if there is.
     */
    bool hasSelection(bool includeSingle);

    /**
     * Creates a resource list containing the multi-selection recipients, if
     * any, or otherwise the single-selection.
     *
     * @param[out] sel The resource list.
     * @return true if all recipient IDs are valid, i.e. output has valid
     *         content.
     */
    bool getSelection(SelectionsT &sel);

signals:
    void selectionChanged(bool isValidSingle, bool isValidMulti);

public slots:
    /**
     * Sets the recipient data.
     *
     * @param[in] type Resource type - ResourceData::eType.
     *                 -1 means all types.
     */
    void setData(int type);

private:
    Ui::ResourceSelector *ui;
    int                   mSsi;
    QCompleter           *mCompleter;
    SelectionsT           mSelections;

    /**
     * Sets the data model.
     *
     * @param[in] type        Resource type - ResourceData::eType.
     * @param[in] showFirstId true to show the first ID.
     */
    void setModel(int type, bool showFirstId = true);

    /**
     * Appends resources to the recipient list while preventing duplicates.
     *
     * @param[in] type Resource type - ResourceData::eType.
     * @param[in] rscs The comma-separated resources.
     */
    void appendSelection(int type, const QString &rscs);

    /**
     * Appends IDs to the recipient list.
     *
     * @param[in] type Resource type - ResourceData::eType.
     * @param[in] ids  The IDs.
     */
    bool appendSelection(int type, const ResourceData::IdsT &ids);

    /**
     * Sets the recipients in ui->recEdit from mSelections.
     */
    void setRecipients();
};
#endif //RESOURCESELECTOR_H
