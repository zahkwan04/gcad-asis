/**
 * Qt-specific utility functions implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2015-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: QtUtils.cpp 1908 2025-03-05 00:54:00Z rosnin $
 * @author Mohd Rozaimi
 * @author Mohd Rashid
 */
#include <assert.h>
#include <QApplication>
#include <QByteArray>
#include <QClipboard>
#include <QLocale>
#include <QRegularExpression>
#include <QStringList>
#include <QTcpSocket>
#include <QUrl>

#include "CmnTypes.h"
#include "QtUtils.h"

using std::map;

static const int HOST_CONNECT_TIMEOUT = 3000; //milliseconds

bool            QtUtils::gisOpen = false;
map<int, QIcon> QtUtils::actionIconMap;
map<int, QIcon> QtUtils::commsIconMap;
map<int, QIcon> QtUtils::rscIconMap;
QString QtUtils::timestampFormat           ("dd/MM/yyyy HH:mm:ss");
QString QtUtils::timestampFormatWithDayName("ddd dd/MM/yyyy HH:mm:ss");

QString QtUtils::fromHexUnicode(const QString &hexStr)
{
    return QString(QByteArray::fromHex(hexStr.toUtf8()));
}

QString QtUtils::fromHexUnicode(const std::string &hexStr)
{
    return fromHexUnicode(QString::fromStdString(hexStr));
}

bool QtUtils::setTimestampFormats(const QString &format,
                                  const QString &formatWithDayName)
{
    //QDateTime::toString() returns empty string for invalid format
    if (QDateTime::currentDateTime().toString(format).isEmpty() ||
        QDateTime::currentDateTime().toString(formatWithDayName).isEmpty())
        return false;
    timestampFormat = format;
    timestampFormatWithDayName = formatWithDayName;
    return true;
}

QString QtUtils::getTimestamp(const QDateTime &dt, bool showDay)
{
    if (showDay)
        return QLocale().toString(dt, timestampFormatWithDayName);
    return dt.toString(timestampFormat);
}

QString QtUtils::getTimestamp(bool showDay)
{
    return getTimestamp(QDateTime::currentDateTime(), showDay);
}

QDateTime QtUtils::getDateTime(const QString &ts, bool hasDay)
{
    return QDateTime::fromString(ts,
                                 (hasDay)? timestampFormatWithDayName:
                                           timestampFormat);
}

QStringList QtUtils::tokenize(const QString &str)
{
    QStringList l;
    bool isIn = false;
    for (const auto &phrase : str.split('"'))
    {
        if (!phrase.isEmpty())
        {
            if (isIn)
            {
                l << phrase; //quoted string
            }
            else
            {
                //unquoted string - find all words
                for (const auto &word :
                     phrase.split(QRegularExpression("\\s"),
                                  QString::SkipEmptyParts))
                {
                    l << word;
                }
            }
        }
        isIn = !isIn;
    }
    return l;
}

bool QtUtils::isHostAvailable(const QUrl &url)
{
    QTcpSocket socket;
    socket.connectToHost(url.host(), url.port());
    return socket.waitForConnected(HOST_CONNECT_TIMEOUT);
}

void QtUtils::addToComboBox(const QString &txt, QComboBox *cb)
{
    assert(cb != 0);
    int i = cb->findText(txt);
    if (i != 0)
    {
        //txt not at the top - either add or move it
        if (i > 0)
            cb->removeItem(i);
        cb->insertItem(0, txt);
        cb->setCurrentIndex(0);
    }
}

QString QtUtils::getCommsIconSrc(int type)
{
    switch (type)
    {
        case CmnTypes::COMMS_DIR_IN:
            return ":/Images/images/icon_comms_in.png";
        case CmnTypes::COMMS_DIR_MISSED:
            return ":/Images/images/icon_comms_missed.png";
        case CmnTypes::COMMS_DIR_MON:
            return ":/Images/images/icon_comms_mon.png";
        case CmnTypes::COMMS_DIR_OUT:
            return ":/Images/images/icon_comms_out.png";
        case CmnTypes::COMMS_MSG_MMS:
            return ":/Images/images/icon_mms.png";
        case CmnTypes::COMMS_MSG_SDS:
            return ":/Images/images/icon_sds.png";
        case CmnTypes::COMMS_MSG_STATUS:
            return ":/Images/images/icon_statusMsg.png";
        default:
            //just to have a default - but should not occur
            return ":/Images/images/icon_status_info.png";
    }
}

QString QtUtils::getCommsText(int type)
{
    switch (type)
    {
        case CmnTypes::COMMS_DIR_IN:
            return QObject::tr("Incoming");
        case CmnTypes::COMMS_DIR_MISSED:
            return QObject::tr("Missed");
        case CmnTypes::COMMS_DIR_MON:
            return QObject::tr("Monitored");
        case CmnTypes::COMMS_DIR_OUT:
            return QObject::tr("Outgoing");
        case CmnTypes::COMMS_MSG_MMS:
            return QObject::tr("MMS");
        case CmnTypes::COMMS_MSG_SDS:
            return QObject::tr("SDS");
        case CmnTypes::COMMS_MSG_STATUS:
            return QObject::tr("Status Message");
        default:
            return "";
    }
}

const QIcon &QtUtils::getCommsIcon(int type)
{
    if (type < 0 || type > CmnTypes::COMMS_MAX)
        type = -1; //force a single invalid default
    if (commsIconMap.count(type) == 0)
        commsIconMap[type] = QIcon(getCommsIconSrc(type));
    return commsIconMap[type];
}

const QString QtUtils::getActionIconSrc(int type)
{
    switch (type)
    {
        case CmnTypes::ACTIONTYPE_ADD:
            return ":/Images/images/icon_add.png";
        case CmnTypes::ACTIONTYPE_ATT:
            return ":/Images/images/icon_att.png";
        case CmnTypes::ACTIONTYPE_CALL:
            return ":/Images/images/icon_call.png";
        case CmnTypes::ACTIONTYPE_CLEAR:
        case CmnTypes::ACTIONTYPE_DELETE:
        case CmnTypes::ACTIONTYPE_DELETE_SEL:
        case CmnTypes::ACTIONTYPE_MONSTOP:
        case CmnTypes::ACTIONTYPE_PB_DEL:
            return ":/Images/images/icon_clear.png";
        case CmnTypes::ACTIONTYPE_CLEAR_UNC_GRP_ATT:
            return ":/Images/images/icon_clear_unc_grp_att.png";
        case CmnTypes::ACTIONTYPE_COMMSREG:
            return ":/Images/images/icon_commsreg.png";
        case CmnTypes::ACTIONTYPE_CONTACT_ADD:
        case CmnTypes::ACTIONTYPE_CONTACT_SHOW:
            return ":/Images/images/icon_contacts.png";
        case CmnTypes::ACTIONTYPE_COPY:
        case CmnTypes::ACTIONTYPE_COPY_PATH:
            return ":/Images/images/icon_copy.png";
        case CmnTypes::ACTIONTYPE_DGNA_ASSG:
            return ":/Images/images/icon_dgna_ind.png";
        case CmnTypes::ACTIONTYPE_DGNA_DISSOLVE:
            return ":/Images/images/icon_dissolve.png";
        case CmnTypes::ACTIONTYPE_EDIT:
            return ":/Images/images/icon_edit.png";
        case CmnTypes::ACTIONTYPE_GIS:
        case CmnTypes::ACTIONTYPE_LOCATE:
            return ":/Images/images/icon_map.png";
        case CmnTypes::ACTIONTYPE_GRP_ATT:
            return ":/Images/images/icon_group.png";
        case CmnTypes::ACTIONTYPE_INCIDENT:
            return ":/Images/images/icon_incident.png";
        case CmnTypes::ACTIONTYPE_LOGOUT:
            return ":/Images/images/icon_logout.png";
        case CmnTypes::ACTIONTYPE_MMS:
            return ":/Images/images/icon_mms.png";
        case CmnTypes::ACTIONTYPE_MMS_DOWNLOAD:
            return ":/Images/images/icon_download.png";
        case CmnTypes::ACTIONTYPE_MON:
            return ":/Images/images/icon_monitoring.png";
        case CmnTypes::ACTIONTYPE_MSG:
        case CmnTypes::ACTIONTYPE_SDS:
            return ":/Images/images/icon_sds.png";
        case CmnTypes::ACTIONTYPE_NOTES:
        case CmnTypes::ACTIONTYPE_PB_ADD:
            return ":/Images/images/icon_phonebook.png";
        case CmnTypes::ACTIONTYPE_OSK:
            return ":/Images/images/icon_keyboard.png";
        case CmnTypes::ACTIONTYPE_PLAY:
            return ":/Images/images/icon_play.png";
        case CmnTypes::ACTIONTYPE_PLAY_VID:
            return ":/Images/images/icon_video.png";
        case CmnTypes::ACTIONTYPE_PRINT_EXCEL:
            return ":/Images/images/icon_excel.png";
        case CmnTypes::ACTIONTYPE_PRINT_PDF:
            return ":/Images/images/icon_pdf.png";
        case CmnTypes::ACTIONTYPE_PRINT_PRV:
            return ":/Images/images/icon_print.png";
        case CmnTypes::ACTIONTYPE_PTT:
            return ":/Images/images/icon_ptt.png";
        case CmnTypes::ACTIONTYPE_PTT_ACTIVE:
            return ":/Images/images/icon_ptt2.png";
        case CmnTypes::ACTIONTYPE_PTT_PENDING:
            return ":/Images/images/icon_ptt3.png";
        case CmnTypes::ACTIONTYPE_REFRESH:
            return ":/Images/images/icon_reload.png";
        case CmnTypes::ACTIONTYPE_RENAME:
            return ":/Images/images/icon_rename.png";
        case CmnTypes::ACTIONTYPE_REPORT:
            return ":/Images/images/icon_report.png";
        case CmnTypes::ACTIONTYPE_RESOURCES:
            return ":/Images/images/icon_resources.png";
        case CmnTypes::ACTIONTYPE_RSC_SELECT:
            return ":/Images/images/cursor_sel_resource.png";
        case CmnTypes::ACTIONTYPE_SAVE:
            return ":/Images/images/icon_save.png";
        case CmnTypes::ACTIONTYPE_SAVE_ALL:
            return ":/Images/images/icon_save_all.png";
        case CmnTypes::ACTIONTYPE_SELECT_ALL:
            return ":/Images/images/icon_select_all.png";
        case CmnTypes::ACTIONTYPE_SELECT_NONE:
            return ":/Images/images/icon_select_none.png";
        case CmnTypes::ACTIONTYPE_SETTINGS:
            return ":/Images/images/icon_settings.png";
        case CmnTypes::ACTIONTYPE_SORT:
            return ":/Images/images/icon_sort.png";
        case CmnTypes::ACTIONTYPE_STATUS:
            return ":/Images/images/icon_statusMsg.png";
        default:
            return ":/Images/images/icon_status_info.png";
    }
}

const QIcon &QtUtils::getActionIcon(int type)
{
    if (type < 0 || type > CmnTypes::ACTIONTYPE_MAX)
        type = -1; //force a single default
    if (actionIconMap.count(type) == 0)
        actionIconMap[type] = QIcon(getActionIconSrc(type));
    return actionIconMap[type];
}

const QString QtUtils::getRscIconSrc(int type)
{
    switch (type)
    {
        case CmnTypes::IDTYPE_DGNA_GRP:
            return ":/Images/images/icon_dgna_grp.png";
        case CmnTypes::IDTYPE_DGNA_IND:
            return ":/Images/images/icon_dgna_ind.png";
        case CmnTypes::IDTYPE_DISPATCHER:
            return ":/Images/images/icon_disp.png";
        case CmnTypes::IDTYPE_GROUP:
            return ":/Images/images/icon_group.png";
        case CmnTypes::IDTYPE_MOBILE:
        case CmnTypes::IDTYPE_MOBILE_ONLINE:
            return ":/Images/images/icon_mobile_phone.png";
        case CmnTypes::IDTYPE_SUBSCRIBER:
        default:
            return ":/Images/images/icon_mobile.png";
    }
}

const QIcon &QtUtils::getRscIcon(int type)
{
    if (type < 0 || type >= CmnTypes::IDTYPE_UNKNOWN)
        type = CmnTypes::IDTYPE_SUBSCRIBER;
    if (rscIconMap.count(type) == 0)
        rscIconMap[type] = QIcon(getRscIconSrc(type));
    return rscIconMap[type];
}

QAction *QtUtils::addMenuAction(QMenu &menu, int type, const QString &txt)
{
    const QIcon &icon(getActionIcon(type));
    QString s(txt);
    if (s.isEmpty())
    {
        switch (type)
        {
            case CmnTypes::ACTIONTYPE_ADD:
                s = QObject::tr("Add");
                break;
            case CmnTypes::ACTIONTYPE_CALL:
                s = QObject::tr("Call");
                break;
            case CmnTypes::ACTIONTYPE_CLEAR:
                s = QObject::tr("Clear content");
                break;
            case CmnTypes::ACTIONTYPE_CLEAR_UNC_GRP_ATT:
                s = QObject::tr("Clear Unconfirmed Group Attachments");
                break;
            case CmnTypes::ACTIONTYPE_CONTACT_ADD:
                s = QObject::tr("Add to Quick Contacts");
                break;
            case CmnTypes::ACTIONTYPE_CONTACT_SHOW:
                s = QObject::tr("Show in Quick Contacts");
                break;
            case CmnTypes::ACTIONTYPE_COPY:
                s = QObject::tr("Copy text");
                break;
            case CmnTypes::ACTIONTYPE_COPY_PATH:
                s = QObject::tr("Copy path");
                break;
            case CmnTypes::ACTIONTYPE_DELETE:
                s = QObject::tr("Delete");
                break;
            case CmnTypes::ACTIONTYPE_DELETE_SEL:
                s = QObject::tr("Delete selection");
                break;
            case CmnTypes::ACTIONTYPE_DGNA_ASSG:
                s = QObject::tr("DGNA assignment");
                break;
            case CmnTypes::ACTIONTYPE_DGNA_DISSOLVE:
                s = QObject::tr("Dissolve");
                break;
            case CmnTypes::ACTIONTYPE_EDIT:
                s = QObject::tr("Edit");
                break;
            case CmnTypes::ACTIONTYPE_GRP_ATT:
                s = QObject::tr("Group member attachments");
                break;
            case CmnTypes::ACTIONTYPE_LOCATE:
                if (!isGisOpen())
                    return 0;
                s = QObject::tr("Locate on map");
                break;
            case CmnTypes::ACTIONTYPE_MMS_DOWNLOAD:
                s = QObject::tr("Retry download");
                break;
            case CmnTypes::ACTIONTYPE_MON:
            case CmnTypes::ACTIONTYPE_MONSTART:
                s = QObject::tr("Monitor");
                break;
            case CmnTypes::ACTIONTYPE_MONSTOP:
                s = QObject::tr("Stop monitoring");
                break;
            case CmnTypes::ACTIONTYPE_MSG:
                s = QObject::tr("Message");
                break;
            case CmnTypes::ACTIONTYPE_NOTES:
                s = QObject::tr("Notes");
                break;
            case CmnTypes::ACTIONTYPE_PB_ADD:
                s = QObject::tr("Add to Phonebook");
                break;
            case CmnTypes::ACTIONTYPE_PB_DEL:
                s = QObject::tr("Delete from Phonebook");
                break;
            case CmnTypes::ACTIONTYPE_PRINT_EXCEL:
                s = QObject::tr("Excel");
                break;
            case CmnTypes::ACTIONTYPE_PRINT_PDF:
                s = QObject::tr("PDF");
                break;
            case CmnTypes::ACTIONTYPE_PRINT_PRV:
                s = QObject::tr("Preview");
                break;
            case CmnTypes::ACTIONTYPE_REFRESH:
                s = QObject::tr("Refresh");
                break;
            case CmnTypes::ACTIONTYPE_RENAME:
                s = QObject::tr("Rename");
                break;
            case CmnTypes::ACTIONTYPE_RSC_SELECT:
                s = QObject::tr("Select");
                break;
            case CmnTypes::ACTIONTYPE_SAVE:
                s = QObject::tr("Save");
                break;
            case CmnTypes::ACTIONTYPE_SAVE_ALL:
                s = QObject::tr("Save all");
                break;
            case CmnTypes::ACTIONTYPE_SDS:
                s = QObject::tr("SDS");
                break;
            case CmnTypes::ACTIONTYPE_SELECT_ALL:
                s = QObject::tr("Select all");
                break;
            case CmnTypes::ACTIONTYPE_SELECT_NONE:
                s = QObject::tr("Clear selection");
                break;
            case CmnTypes::ACTIONTYPE_SORT:
                s = QObject::tr("Sort");
                break;
            case CmnTypes::ACTIONTYPE_STATUS:
                s = QObject::tr("Status Message");
                break;
            default:
                break;
        }
    }
    auto *act = menu.addAction(icon, s);
    act->setData(type);
    return act;
}

void QtUtils::addMenuActions(QMenu &menu, const QList<int> &types)
{
    for (int i : types)
    {
        addMenuAction(menu, i);
    }
}

void QtUtils::copyToClipboard(const QString &txt)
{
    QApplication::clipboard()->setText(txt);
}

void QtUtils::copyToClipboard(const QListView *list)
{
    assert(list != 0);
    QStringList l;
    for (const auto &idx : list->selectionModel()->selectedIndexes())
    {
        l << idx.data().toString();
    }
    QApplication::clipboard()->setText(l.join("\n"));
}

void QtUtils::setGisOpen(bool open)
{
    gisOpen = open;
}

bool QtUtils::isGisOpen()
{
    return gisOpen;
}
