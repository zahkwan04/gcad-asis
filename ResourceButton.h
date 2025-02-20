/**
 * Resource button module.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: ResourceButton.h 1769 2023-10-17 03:02:03Z hazim.rujhan $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#ifndef RESOURCEBUTTON_H
#define RESOURCEBUTTON_H

#include <map>
#include <QIcon>
#include <QMouseEvent>
#include <QToolButton>

#include "Draggable.h"
#include "ResourceData.h"

class ResourceButton : public QToolButton, public Draggable
{
    Q_OBJECT

public:
    static const int NOTES_MAXCHARS = 200;

    /**
     * Constructor.
     *
     * @param[in] id          The resource ID.
     * @param[in] type        The resource type - ResourceData::eType.
     * @param[in] enableNotes true to enable notes.
     * @param[in] parent      Parent widget.
     */
    ResourceButton(int id, int type, bool enableNotes, QWidget *parent);

    /**
     * Refreshes the button state and text.
     *
     * @param[in] nwkUpdate   true if called due to network update.
     * @param[in] onlineKnown true if online status is known and passed in the
     *                        next parameter. Otherwise retrieves the status.
     * @param[in] online      true if online.
     * @param[in] doSetActive true to call setActive().
     */
    void refresh(bool nwkUpdate   = false,
                 bool onlineKnown = false,
                 bool online      = false,
                 bool doSetActive = false);

    /**
     * Saves phonebook entry to database.
     */
    void phonebookSave();

    /**
     * Deletes phonebook entry from database.
     */
    void phonebookDelete();

    /**
     * Sets or clears the active indicator on a group button.
     */
    void setActive();

    /**
     * Sets background color according to online status.
     *
     * @param[in] onlineKnown true if online status is known and passed in the
     *                        next parameter. Otherwise retrieves the status.
     * @param[in] online      true if online.
     * @param[in] init        true only for initial call from constructor.
     */
    void setOnline(bool onlineKnown = false,
                   bool online      = false,
                   bool init        = false);

    bool isOnline() const { return mOnline; }

    int getId() const { return mId; }

    int getType() const { return mType; }

    QString getDspTxt() const { return ResourceData::getDspTxt(mId, mType); }

    const QString &getLastOnlineTime() const { return mLastOnlineTime; }

    /**
     * Sets the notes text.
     *
     * @param[in] txt      The text.
     * @param[in] isByUser true if set by user.
     * @return false if set by user and the text length exceeds the maximum
     *         length.
     */
    bool setNotes(QString &txt, bool isByUser = false);

    const QString &getNotes() const { return mNotes; }

    static QSize getIconSize() { return sIconSize; }

    static void setUsername(const std::string &username)
    {
        sUsername = username;
    }

public slots:
    /**
     * Shows the notes dialog.
     */
    void showNotes();

protected:
    /**
     * Handles mouse press event on ToolButton.
     *
     * @param[in] event The event.
     */
    void mousePressEvent(QMouseEvent *event);

    /**
     * Handles mouse move event on ToolButton.
     *
     * @param[in] event The event.
     */
    void mouseMoveEvent(QMouseEvent *event);

private:
    bool    mOnline;
    int     mId;                   //ISSI, GSSI or Dispatcher ID
    QString mName;
    QString mNotes;
    QString mGrpMembers;
    QString mLastOnlineTime;
    QString mSsBase;
    QString mSsNormalBgColor;

    static std::string sUsername;
    static QSize       sIconSize;

    /**
     * Sets the style with a combination of the mSs* members.
     */
    void setStyle();
};
#endif //RESOURCEBUTTON_H
