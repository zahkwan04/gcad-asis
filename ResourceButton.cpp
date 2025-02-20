/**
 * Resource button implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ResourceButton.cpp 1817 2024-02-22 07:30:31Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#include <assert.h>
#include <QInputDialog>
#include <QRegularExpression>

#include "DbInt.h"
#include "MessageDialog.h"
#include "QtUtils.h"
#include "Style.h"
#include "SubsData.h"
#include "ResourceButton.h"

using namespace std;

static const int NOTES_MAXWLEN = 10;

string ResourceButton::sUsername;
QSize  ResourceButton::sIconSize(60, 40);

ResourceButton::ResourceButton(int      id,
                               int      type,
                               bool     enableNotes,
                               QWidget *parent) :
QToolButton(parent), Draggable(type), mOnline(false), mId(id),
mName(ResourceData::getName(id, type))
{
    mSsBase.append("QToolButton {color:black;")
           .append(Style::getStyle(Style::OBJ_FONT3))
           .append("border-radius:8px;background-color:rgb(");
    switch (mType)
    {
        case ResourceData::TYPE_SUBSCRIBER:
            mSsNormalBgColor = "135,200,217";
            break;
        case ResourceData::TYPE_GROUP:
            mSsNormalBgColor = "105,191,116";
            enableNotes = false; //still enabled but not by clicking
            break;
        case ResourceData::TYPE_DGNA_IND:
            mSsNormalBgColor = "139,128,0";
            enableNotes = false;
            break;
        case ResourceData::TYPE_DGNA_GRP:
            mSsNormalBgColor = "230,172,0";
            enableNotes = false;
            break;
        case ResourceData::TYPE_MOBILE:
            mSsNormalBgColor = "135,159,217";
            break;
        case ResourceData::TYPE_DISPATCHER:
        default:
            mSsNormalBgColor = "147,112,219";
            break;
    }
    setOnline(true, true, true); //init
    if (enableNotes)
        connect(this, SIGNAL(clicked()), SLOT(showNotes()));
    setIcon(QtUtils::getRscIcon(mType));
    refresh();
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setMinimumWidth(100);
    setMaximumHeight(100);
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
    setIconSize(sIconSize);
    setAutoRaise(true);
}

void ResourceButton::refresh(bool nwkUpdate,
                             bool onlineKnown,
                             bool online,
                             bool doSetActive)
{
    if (nwkUpdate)
        mName = ResourceData::getName(mId, mType);
    QString s(mName);
    //if no name, show first word of notes as name
    if (s.isEmpty() && !mNotes.isEmpty())
    {
        s = mNotes.section(QRegularExpression("[\\s\n]"), 0, 0);
        if (s.length() > NOTES_MAXWLEN)
            s.replace(NOTES_MAXWLEN - 2, s.length() - NOTES_MAXWLEN + 2, "...");
    }
    //prevent shortcut creation caused by single ampersand
    if (!s.isEmpty())
        s.replace('&', "&&");
    s.append("\n").append(QString::number(mId));
    setText(s);
    if (nwkUpdate)
        setOnline(onlineKnown, online);
    if (doSetActive)
        setActive();
}

void ResourceButton::phonebookSave()
{
#ifndef NO_DB
    DbInt::RecordT data;
    data[DbInt::FIELD_USER_NAME] = sUsername;
    data[DbInt::FIELD_SSI] = Utils::toString(mId);
    data[DbInt::FIELD_TYPE] = Utils::toString(mType);
    if (!DbInt::instance().insertRecord(DbInt::DB_TABLE_D_PHONEBOOK, data))
    {
        QMessageBox::critical(this, tr("Phonebook Error"),
                              tr("Failed to save '%1' to database because "
                                 "of link error.").arg(mId));
    }
#endif
}

void ResourceButton::phonebookDelete()
{
#ifndef NO_DB
    DbInt::RecordT data;
    data[DbInt::FIELD_USER_NAME] = sUsername;
    data[DbInt::FIELD_SSI] = Utils::toString(mId);
    data[DbInt::FIELD_TYPE] = Utils::toString(mType);
    if (!DbInt::instance().deleteRecord(DbInt::DB_TABLE_D_PHONEBOOK, data))
    {
        QMessageBox::critical(this, tr("Phonebook Error"),
                              tr("Failed to delete '%1' from database "
                                 "because of link error.").arg(mId));
    }
#endif
}

void ResourceButton::setActive()
{
    switch (SubsData::grpActive(mId))
    {
        case SubsData::GRP_STAT_ASSIGN:
            setIcon(QIcon(QPixmap(":/Images/images/icon_dgna_ind_assign.png")));
            mGrpMembers.clear();
            setToolTip(mNotes);
            break;
        case SubsData::GRP_STAT_ATTACH:
            if (mType == ResourceData::TYPE_GROUP)
                setIcon(QIcon(QPixmap(":/Images/images/icon_group_attach.png")));
            else //ResourceData::TYPE_DGNA_IND
                setIcon(QIcon(
                          QPixmap(":/Images/images/icon_dgna_ind_attach.png")));
            mGrpMembers = QString::fromStdString(
                                   SubsData::getGrpAttachedMembers(mId, false));
            if (mGrpMembers.isEmpty())
                setToolTip(mNotes);
            else if (mNotes.isEmpty())
                setToolTip(mGrpMembers);
            else
                setToolTip(mNotes + "\n" + mGrpMembers);
            break;
        default:
            setIcon(QtUtils::getRscIcon(mType));
            mGrpMembers.clear();
            setToolTip(mNotes);
            break;
    }
    setStyle();
}

void ResourceButton::setOnline(bool onlineKnown, bool online, bool init)
{
    //for mobile not known to be offline, check actual status, excluding
    //constructor init call
    if (!init && mType == ResourceData::TYPE_MOBILE && (!onlineKnown || online))
        online = ResourceData::hasId(ResourceData::onlineMobileType(), mId);
    else if (!onlineKnown)
        online = ResourceData::hasId(mType, mId);
    if (online && !init) //exclude constructor init call
        mLastOnlineTime = QtUtils::getTimestamp();
    if (online == mOnline)
        return;
    mOnline = online;
    //set time when going offline after having been online before
    if (!online && !mLastOnlineTime.isEmpty())
        mLastOnlineTime = QtUtils::getTimestamp();
    setStyle();
}

bool ResourceButton::setNotes(QString &txt, bool isByUser)
{
    if (txt.length() > NOTES_MAXCHARS)
    {
        if (isByUser)
            return false;
        txt = txt.left(NOTES_MAXCHARS);
    }
    if (txt != mNotes)
    {
        mNotes = txt;
        if (mGrpMembers.isEmpty())
            setToolTip(mNotes);
        else if (mNotes.isEmpty())
            setToolTip(mGrpMembers);
        else
            setToolTip(mNotes + "\n" + mGrpMembers);
        if (mName.isEmpty())
            refresh();
#ifndef NO_DB
        if (isByUser)
        {
            DbInt::RecordT data;
            data[DbInt::FIELD_NOTES] = txt.toStdString();
            DbInt::RecordT keys;
            keys[DbInt::FIELD_USER_NAME] = sUsername;
            keys[DbInt::FIELD_SSI] = Utils::toString(mId);
            keys[DbInt::FIELD_TYPE] = Utils::toString(mType);
            if (!DbInt::instance().updateRecord(DbInt::DB_TABLE_D_PHONEBOOK,
                                                keys, data))
            {
                QMessageBox::critical(this, tr("Phonebook Error"),
                                      tr("Failed to update '%1' notes to "
                                         "database because of link error.")
                                          .arg(mId));
            }
        }
#endif
    }
    return true;
}

void ResourceButton::showNotes()
{
    QInputDialog d(this, windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    d.setWindowIcon(icon());
    d.setWindowTitle(tr("Phonebook Entry ") + getDspTxt());
    d.setLabelText(tr("Notes: (max %1 characters)").arg(NOTES_MAXCHARS));
    d.setOptions(QInputDialog::UsePlainTextEditForTextInput);
    d.setTextValue(mNotes);
    d.setStyleSheet(Style::getStyle(Style::OBJ_INPUTDIALOG));
    while (d.exec() == QDialog::Accepted)
    {
        QString txt(d.textValue().trimmed());
        if (setNotes(txt, true))
            break;
        MessageDialog::showStdMessage(tr("Phonebook Notes Error"),
                                      tr("Text length = %1 exceeds the maximum "
                                         "%2")
                                          .arg(txt.length()).arg(NOTES_MAXCHARS),
                                      QMessageBox::Critical, true, this);
    }
}

void ResourceButton::mousePressEvent(QMouseEvent *event)
{
    handleMousePress(event);
    QToolButton::mousePressEvent(event);
}

void ResourceButton::mouseMoveEvent(QMouseEvent *event)
{
    if (handleMouseMove(event))
    {
        appendData(mId);
        startDrag(this);
        setDown(false); //otherwise it stays pressed-down
    }
}

inline void ResourceButton::setStyle()
{
    QString ss(mSsBase);
    setStyleSheet(ss.append((mOnline)? mSsNormalBgColor: "150,150,150")
                    .append(")}"));
}
