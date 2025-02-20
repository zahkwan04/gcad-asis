/**
 * A custom input QDialog module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: InputDialog.h 1899 2025-01-22 05:42:07Z rosnin $
 * @author Mohd Rozaimi
 * @author Zulzaidi Atan
 */
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QSet>
#include <QStandardItemModel>
#include <QStringListModel>

#include "Props.h"
#include "ResourceData.h"
#include "SubsData.h"

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    enum eType
    {
        TYPE_BRANCH_SELECT,
        TYPE_CHANGE_PASSWORD,
        TYPE_DGNA_ASSIGN,
        TYPE_SEARCH_RESULTS,
        TYPE_TRACKING_INC_RESOURCES
    };

    enum
    {
        //columns for search results - must be in display order
        COL_NAME,
        COL_DISTANCE,
        COL_LAYER,
        COL_CATEGORY,
        COL_ID,
        COL_LAT,
        COL_LON,
        COL_ITEM,
        COL_NUM,     //just to indicate the number of columns

        //columns for terminal search results
        COL_TERM_ID = 0,
        COL_TERM_DISTANCE,
        COL_TERM_DATETIME,
        COL_TERM_ITEM,
        COL_TERM_NUM
    };

    /**
     * Constructor.
     *
     * @param[in] type   The dialog type - eType.
     * @param[in] parent The parent widget, if any.
     */
    InputDialog(int type, QWidget *parent = 0);

    /**
     * Constructor for TYPE_DGNA_ASSIGN.
     *
     * @param[in] listMdl    DGNA-Ind groups.
     * @param[in] membersMdl The potential members.
     * @param[in] parent     The parent widget, if any.
     */
    InputDialog(ResourceData::ListModel *listMdl,
                ResourceData::ListModel *membersMdl,
                QWidget                 *parent = 0);

    /**
     * Constructor for TYPE_SEARCH_RESULTS.
     *
     * @param[in] key     The search key.
     * @param[in] itemMdl The search results. Takes over ownership.
     * @param[in] center  The search center coordinates, if any.
     * @param[in] label   The search center label, if any.
     * @param[in] layer   The search layer, if any.
     * @param[in] parent  The parent widget, if any.
     */
    InputDialog(const QString      &key,
                QStandardItemModel *itemMdl,
                const QString      &center,
                const QString      &label,
                const std::string  &layer,
                QWidget            *parent = 0);

    /**
     * Constructor for TYPE_TRACKING_INC_RESOURCES.
     *
     * @param[in] id     The Incident ID.
     * @param[in] start  The Incident start time.
     * @param[in] closed The Incident closed time, or empty string if not yet
     *                   closed.
     * @param[in] mdl    The resources.
     * @param[in] parent The parent widget, if any.
     */
    InputDialog(const QString      &id,
                const QString      &start,
                const QString      &closed,
                QStandardItemModel *mdl,
                QWidget            *parent = 0);

    /**
     * Constructor for TYPE_BRANCH_SELECT.
     *
     * @param[in] branches Allowed branches.
     * @param[in] ids      Selected branch IDs. Select all if contains -1.
     * @param[in] parent   The parent widget, if any.
     */
    InputDialog(const SubsData::BranchMapT &branches,
                QSet<int>                   ids,
                QWidget                    *parent = 0);

    ~InputDialog();

    /**
     * Gets the old and new passwords entered by the user.
     *
     * @param[out] oldP The old password.
     * @param[out] newP The new password.
     * @return true if successful.
     */
    bool getPasswords(QString &oldP, QString &newP);

    /**
     * Gets the selected DGNA GSSI.
     *
     * @return The GSSI.
     */
    int getDgnaGssi();

    /**
     * Gets the selected DGNA members.
     *
     * @return The members.
     */
    QStringList getDgnaMembers();

    /**
     * Gets the selected resources.
     *
     * @return The space-separated resources.
     */
    QString getSelectedResources();

    /**
     * Checks whether to use the Incident start or closed time.
     *
     * @param[in] start true for start time.
     * @return true if to be used.
     */
    bool getUseTimeFlag(bool start);

    /**
     * Gets the selected branches.
     *
     * @return The comma-separated branch IDs. "-" if all selected.
     */
    QString getSelectedBranches();

signals:
    void showItem(Props::ValueMapT   prs,
                  const std::string &id,
                  const std::string &layer);

private:
    int                 mType;
    std::string         mLayer;
    QList<QWidget *>    mFields;
    QListWidget        *mListWidget;
    QStandardItemModel *mItemMdl; //owned model

    /**
     * Sets the dialog style.
     */
    void init();

    /**
     * Creates a button box with OK and Cancel buttons.
     *
     * @param[in] noCancel true to exclude Cancel button.
     * @return The box.
     */
    QDialogButtonBox *getBtnBox(bool noCancel = false);
};
#endif //INPUTDIALOG_H
