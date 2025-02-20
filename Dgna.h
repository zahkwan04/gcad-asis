/**
 * UI DGNA module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Dgna.h 1812 2024-02-14 03:07:20Z rosnin $
 * @author Mazdiana Makmor
 */
#ifndef DGNA_H
#define DGNA_H

#include <string>
#include <QDialog>
#include <QSortFilterProxyModel>
#include <QStringListModel>

#include "DraggableListView.h"
#include "Logger.h"
#include "MsgSp.h"
#include "ResourceData.h"
#include "ServerSession.h"

namespace Ui {
class Dgna;
}

class Dgna : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] logger App logger.
     * @param[in] parent Parent widget, if any.
     */
    explicit Dgna(Logger *logger, QWidget *parent = 0);

    ~Dgna();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    void setSession(ServerSession *session) { mSession = session; }

    /**
     * Adds members to a DGNA-Ind group.
     *
     * @param[in] gssi    The GSSI.
     * @param[in] members The member list.
     */
    void addMembers(int gssi, QStringList &members);

    /**
     * Removes members from a DGNA-Ind group.
     *
     * @param[in] gssi    The GSSI.
     * @param[in] members The member list, or empty list to clear the group.
     */
    void removeMembers(int gssi, QStringList &members);

    /**
     * Handles SSI assign or deassign result based on the server response
     * message.
     *
     * @param[in] msg The server message.
     */
    void dgnaResult(MsgSp *msg);

signals:
    void locateResource(int issi);

public slots:
    /**
     * Sets resources and DGNA group.
     *
     * @param[in] gssi The GSSI.
     * @param[in] type The DGNA group type - ResourceData::TYPE_DGNA_*.
     * @param[in] mdl  Subscriber or Talk Group data model.
     */
    void setDgnaItems(int gssi, int type, ResourceData::ListModel *mdl);

protected:
    //override
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::Dgna              *ui;
    DraggableListView     *mResourceList;
    DraggableListView     *mMemberList;
    QSortFilterProxyModel *mSortModel;
    int                    mGssi;
    int                    mType; //ResourceData::TYPE_DGNA_*
    //the following pointers are all owned by another module
    Logger                *mLogger;
    ServerSession         *mSession;

    /**
     * Adds members to a DGNA group.
     *
     * @param[in] gssi     The DGNA GSSI.
     * @param[in] type     The DGNA group type - ResourceData::TYPE_DGNA_*.
     * @param[in] members  The members.
     * @param[in] moveOnly true to only move items into the members list without
     *                     doing DGNA assignment.
     */
    void addMembers(int                 gssi,
                    int                 type,
                    ResourceData::IdsT &members,
                    bool                moveOnly);

    /**
     * Removes members from a DGNA group.
     *
     * @param[in] gssi     The DGNA GSSI.
     * @param[in] type     The DGNA group type - ResourceData::TYPE_DGNA_*.
     * @param[in] members  The members.
     * @param[in] moveOnly true to only move items out of the members list
     *                     without doing DGNA deassignment.
     */
    void removeMembers(int                 gssi,
                       int                 type,
                       ResourceData::IdsT &members,
                       bool                moveOnly);
};
#endif //DGNA_H
