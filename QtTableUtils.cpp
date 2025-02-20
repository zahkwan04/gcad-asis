/**
 * Qt utility functions implementation for table and database data.
 *
 * Copyright (C) Sapura Secured Technologies, 2019-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: QtTableUtils.cpp 1816 2024-02-19 06:45:36Z zulzaidi $
 * @author Zulzaidi Atan
 */
#include <assert.h>
#include <QAbstractItemModel>
#include <QDateTime>
#include <QFont>
#include <QGroupBox>
#include <QMap>
#include <QMessageBox>
#include <QObject>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "CmnTypes.h"
#include "DateTimeDelegate.h"
#ifdef INCIDENT
#include "IncidentData.h"
#endif
#include "MsgSp.h"
#include "QtUtils.h"
#include "ResourceData.h"
#include "QtTableUtils.h"

using namespace std;

//not to be translated because these are in database, set by server
const string QtTableUtils::CALLTYPE_AMBIENCE ("Ambience");
const string QtTableUtils::CALLTYPE_BROADCAST("Broadcast");
const string QtTableUtils::CALLTYPE_GROUP    ("Group");
const string QtTableUtils::CALLTYPE_INCOMING ("Incoming");
const string QtTableUtils::CALLTYPE_MOBILE   ("Mobile");
const string QtTableUtils::CALLTYPE_OUTGOING ("Outgoing");

bool QtTableUtils::tableContextMenu(QTableWidget *tw,
                                    const QPoint &pos,
                                    bool          unsaved,
                                    int          *numHighlighted)
{
    if (tw == 0)
        return false;
    QMenu cmenu(tw->parentWidget());
    //tw SelectionBehavior must be QAbstractItemView::SelectRows
    int count = tw->selectionModel()->selectedRows().size();
    if (count == 0)
        return false; //right-click not on table row
    QAction *actSel = 0;
    if (tw->rowCount() > count)
        actSel = cmenu.addAction(QObject::tr("Select all"));
    QAction *actMark = 0;
    if (numHighlighted != 0 && *numHighlighted > 0)
        actMark = cmenu.addAction(QObject::tr("Mark all read"));
    QAction *actRm = 0;
    if (count > 1)
        actRm = cmenu.addAction(QObject::tr("Remove selected rows"));
    else
        actRm = cmenu.addAction(QObject::tr("Remove this row"));
    QAction *act = cmenu.exec(tw->mapToGlobal(pos));
    if (act == 0)
        return false;

    count = 0;
    if (act == actSel)
        tw->selectAll();
    else if (act == actMark)
        count = tableNormal(tw);
    else if (act == actRm)
        count = tableRemoveRows(tw, unsaved);
    if (numHighlighted != 0 && count > 0)
    {
        *numHighlighted -= count;
        return true;
    }
    return false;
}

int QtTableUtils::tableHighlight(QTableWidget *tw, int row, bool enable)
{
    assert(tw != 0);
    if (row >= tw->rowCount())
        return 0;
    //first get existing font
    QFont font;
    QTableWidgetItem *itm = 0;
    int colCount = tw->columnCount();
    int c = 0;
    for (; c<colCount; ++c)
    {
        if (tw->isColumnHidden(c))
            continue;
        itm = tw->item(row, c);
        if (itm != 0)
        {
            font = tw->item(row, c)->font();
            if ((enable && font.bold()) || (!enable && !font.bold()))
                return false; //already in correct highlight mode
            font.setBold(enable);
            font.setItalic(enable);
            itm->setFont(font);
            break;
        }
    }
    if (itm == 0)
        return 0;
    //continue with remaining columns
    for (++c; c<colCount; ++c)
    {
        if (!tw->isColumnHidden(c))
        {
            itm = tw->item(row, c);
            if (itm != 0)
                itm->setFont(font);
        }
    }
    return 1;
}

int QtTableUtils::tableNormal(QTableWidget *tw, int row)
{
    assert(tw != 0);
    if (row >= tw->rowCount())
        return 0;
    int count = 0;
    int min = row;
    if (row < 0)
    {
        //do all rows
        row = tw->rowCount() - 1;
        min = 0;
    }
    for (; row>=min; --row)
    {
        count += tableHighlight(tw, row, false);
    }
    return count;
}

int QtTableUtils::tableRemoveRows(QTableWidget *tw, bool unsaved)
{
    assert(tw != 0);
    if (QMessageBox::question(tw->parentWidget(), QObject::tr("Confirm Delete"),
                   (unsaved)?
                   QObject::tr("Selected messages contain unsaved attachments. "
                               "Are you sure to delete them?"):
                   QObject::tr("Are you sure to remove the selected data?")) !=
        QMessageBox::Yes)
    {
        return 0;
    }
    int count = 0;
    QList<QTableWidgetItem *> l(tw->selectedItems());
    while (!l.empty())
    {
        if (l.at(0)->font().bold())
            ++count;
        tw->removeRow(l.at(0)->row());
        l = tw->selectedItems();
    }
    return count;
}

void QtTableUtils::addDirFilter(QTableWidget *tw,
                                QPushButton  *btn,
                                int           dirCol,
                                IntSetT      *hideDirs,
                                QMenu        *menu,
                                bool          hasMissed)
{
    assert(tw != 0 && btn != 0 && hideDirs != 0 && menu != 0);
    QList<int> types({CmnTypes::COMMS_DIR_IN});
    QMap<int, QString> m;
    m[CmnTypes::COMMS_DIR_IN] = QObject::tr("Incoming");
    if (hasMissed)
    {
        types << CmnTypes::COMMS_DIR_MISSED;
        m[CmnTypes::COMMS_DIR_MISSED] = QObject::tr("Missed");
    }
    types << CmnTypes::COMMS_DIR_OUT << CmnTypes::COMMS_DIR_MON;
    m[CmnTypes::COMMS_DIR_OUT] = QObject::tr("Outgoing");
    m[CmnTypes::COMMS_DIR_MON] = QObject::tr("Monitored");
    auto *vbox = new QVBoxLayout();
    vbox->setSizeConstraint(QLayout::SetNoConstraint);
    for (auto type : types)
    {
        vbox->addWidget(addFilterCheckBox(tw, btn, dirCol, type, m[type],
                                          QtUtils::getCommsIcon(type),
                                          hideDirs, menu));
    }
    auto *gbox = new QGroupBox(QObject::tr("Directions"), menu);
    gbox->setLayout(vbox);
    auto *wAct = new QWidgetAction(menu);
    wAct->setDefaultWidget(gbox);
    menu->addAction(wAct);
}

QCheckBox *QtTableUtils::addFilterCheckBox(QTableWidget  *tw,
                                           QPushButton   *btn,
                                           int            col,
                                           int            value,
                                           const QString &text,
                                           const QIcon   &icon,
                                           IntSetT       *hideSet,
                                           QMenu         *menu)
{
    auto *chk = new QCheckBox(text, menu);
    chk->setObjectName(QString::number(value));
    chk->setIcon(icon);
    chk->setChecked(true);
    QObject::connect(chk, &QCheckBox::clicked,
        [=](bool isChecked)
        {
            int val = chk->objectName().toInt();
            bool hasHidden    = false; //whether there is hidden row
            bool rowFirstShow = false; //whether there is a row shown first time
            QWidget *w;
            //update rows with matching value while noting hidden row presence
            for (auto r=tw->rowCount()-1; r>=0; --r)
            {
                if (!isChecked || tw->isRowHidden(r))
                {
                    w = tw->cellWidget(r, col);
                    if (w != 0 && w->objectName().toInt() == val &&
                        updateFilteredRow(tw, r, col, !isChecked) &&
                        !rowFirstShow)
                        rowFirstShow = true;
                }
                if (!hasHidden && tw->isRowHidden(r))
                    hasHidden = true;
            }
            if (isChecked)
            {
                hideSet->erase(val);
                //see if any other checkbox is unchecked, i.e. active filter
                for (const auto &chk2 :
                     btn->menu()->findChildren<QCheckBox *>())
                {
                    if (!chk2->isChecked())
                    {
                        isChecked = false;
                        break;
                    }
                }
                if (rowFirstShow)
                    tw->resizeRowsToContents();
            }
            else
            {
                hideSet->insert(val);
            }
            updateFilterButtonText(btn, hasHidden, true, !isChecked);
        });
    return chk;
}

bool QtTableUtils::updateFilteredRow(QTableWidget *tw,
                                     int           row,
                                     int           col,
                                     bool          doAdd,
                                     QPushButton  *btn)
{
    assert(tw != 0);
    //last column holds a set of column IDs that cause this row to be hidden
    int lastCol = tw->columnCount() - 1;
    auto *itm = tw->item(row, lastCol);
    if (itm == 0)
    {
        itm = new QTableWidgetItem();
        tw->setItem(row, lastCol, itm);
    }
    auto cols = itm->data(Qt::UserRole).value<QSet<int>>();
    bool retVal = false;
    if (doAdd)
    {
        if (cols.empty())   //not yet hidden
        {
            tw->setRowHidden(row, true);
            if (btn != 0)
            {
                updateFilterButtonText(btn, true, true);
                //non-empty text to indicate newly added hidden row
                itm->setText("0");
            }
        }
        cols.insert(col);
    }
    else if (cols.remove(col) && cols.empty())
    {
        //col was the only cause - can show row now
        tw->setRowHidden(row, false);
        if (!itm->text().isEmpty())
        {
            //row was hidden on creation, now shown for first time
            itm->setText("");
            retVal = true;
        }
    }
    itm->setData(Qt::UserRole, QVariant::fromValue(cols));
    return retVal;
}

void QtTableUtils::updateFilterButtonText(QPushButton *btn,
                                          bool         hidden,
                                          bool         activeKnown,
                                          bool         active)
{
    QString s(btn->text());
    if (!activeKnown)
        active = s.endsWith("*");
    if (s.endsWith("*"))
    {
        if (s.endsWith("**"))
        {
            if (!active)
                s.chop(2);  //not filtered
            else if (!hidden)
                s.chop(1);  //filtered but no hidden row
        }
        else if (!active)
        {
            s.chop(1);      //not filtered
        }
        else if (hidden)
        {
            s.append("*");  //filtered with hidden row
        }
        btn->setText(s);
    }
    else if (active)
    {
        if (hidden)
            s.append("**");
        else
            s.append("*");
        btn->setText(s);
    }
}

void QtTableUtils::clearTable(QTableView *tv, QWidget *parent)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::clearTable" == 0);
        return;
    }
    QAbstractItemModel *mdl = tv->model();
    //clearSelection() just to prevent Qt output message "Cannot creat (sic)
    //accessible child interface for object:  QTableView(..."
    tv->clearSelection();
    tv->setModel(0);
    delete mdl;
    if (parent != 0)
        parent->hide();
    else
        tv->hide();
}

QString QtTableUtils::getCallType(const string &type, const string &duplex)
{
    //process 'type' in order of probability;
    //duplex is "0" for simplex and "1" for duplex - just need to check for
    //simplex in outgoing and incoming calls
    if (type == CALLTYPE_GROUP)
        return QObject::tr("Group");
    if (type == CALLTYPE_OUTGOING)
    {
        if (duplex == "0")
            return QObject::tr("Individual PTT");
        return QObject::tr("Individual");
    }
    if (type == CALLTYPE_INCOMING)
    {
        if (duplex == "0")
            return QObject::tr("Incoming PTT");
        return QObject::tr("Incoming");
    }
    if (type.empty()) //special case for Report
        return QObject::tr("All");
    if (type == CALLTYPE_AMBIENCE)
        return QObject::tr("Ambience");
    if (type == CALLTYPE_MOBILE)
        return QObject::tr("Mobile");
    if (type == CALLTYPE_BROADCAST)
        return QObject::tr("Broadcast");
    return "";
}

QStringList QtTableUtils::getHeaders(int type, bool showType)
{
    QStringList l;
    switch (type)
    {
        case TBLTYPE_CALL:
            //order must follow eColCall
            l << QObject::tr("Time") << QObject::tr("Duration")
              << QObject::tr("From") << QObject::tr("To");
            if (showType)
                l << QObject::tr("Call Type");
            else
                l << QObject::tr("Type");
            break;
        case TBLTYPE_INCIDENT:
            //order must follow eColInc
            l << QObject::tr("ID") << QObject::tr("State")
              << QObject::tr("Priority") << QObject::tr("Category")
              << QObject::tr("Description") << QObject::tr("Current\nStatus")
              << QObject::tr("Resources");
            break;
        case TBLTYPE_LOC:
            //order must follow eColLoc
            l << QObject::tr("Type") << QObject::tr("ISSI")
              << QObject::tr("Time") << QObject::tr("Location");
            break;
        case TBLTYPE_MMS:
        case TBLTYPE_MSG:
        case TBLTYPE_SDS:
        case TBLTYPE_STS:
            //order must follow eCol
            l << QObject::tr("Type") << QObject::tr("Time")
              << QObject::tr("From") << QObject::tr("To")
              << QObject::tr("Message");
            break;
        case TBLTYPE_PTT:
            //order must follow eColPtt
            l << QObject::tr("Time Point") << QObject::tr("Seconds")
              << QObject::tr("Tx-Party");
            break;
        default:
            break; //unknown type - do nothing
    }
    return l;
}

bool QtTableUtils::getCallAudioData(QTableView *tv, int row, AudioData &data)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getCallAudioData" == 0);
        return false;
    }
    auto *mdl = static_cast<QStandardItemModel *>(tv->model());
    if (mdl == 0)
        return false;
    auto *itm = mdl->item(row, COL_CALL_AUDIO_PATH);
    if (itm == 0)
        return false;
    data.path     = itm->text();
    data.callKey  = mdl->item(row, COL_CALL_KEY)->text();
    data.from     = mdl->item(row, COL_CALL_FROM)->text();
    data.to       = mdl->item(row, COL_CALL_TO)->text();
    data.callType = mdl->item(row, COL_CALL_TYPE)->text();
    data.time     = DateTimeDelegate::getDateTime(
                          mdl->item(row, COL_CALL_TIME)->data(Qt::DisplayRole));
    itm = mdl->item(row, COL_CALL_GSSI);
    data.gssi = (itm != 0)? itm->text(): "";
    return true;
}

bool QtTableUtils::find(QTableView *tv, int col, const QString &str)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::find" == 0);
        return false;
    }
    auto *mdl = qobject_cast<QStandardItemModel *>(tv->model());
    return (mdl != 0 && !mdl->findItems(str, Qt::MatchExactly, col).isEmpty());
}

bool QtTableUtils::getCallData(int           from,
                               int           to,
                               bool          doAnd,
                               const string &startTime,
                               const string &endTime,
                               const string &type,
                               QTableView   *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getCallData(Report)" == 0);
        return false;
    }
    auto *res = DbInt::instance().getCallHistory(startTime, endTime, type, from,
                                                 to, doAnd);
    if (res == 0)
        return false;
    tv->setSortingEnabled(false);
    auto *mdl = new QStandardItemModel();
    fillCallData(res, false, SubsData::isMultiCluster(), mdl);
    if (mdl->rowCount() == 0)
        delete mdl;
    else
        setupTable(TBLTYPE_CALL, mdl, tv, false);
    return true;
}

bool QtTableUtils::getMsgData(int           from,
                              int           to,
                              int           msgType,
                              bool          doAnd,
                              const string &startTime,
                              const string &endTime,
                              const string &input,
                              QTableView   *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getMsgData(Report)" == 0);
        return false;
    }
    DbInt::QResult *res;
    int tblType;
    switch (msgType)
    {
        case CmnTypes::COMMS_MSG_MMS:
            tblType = TBLTYPE_MMS;
            res = DbInt::instance().getMmsHistory(startTime, endTime, input,
                                                  from, to, doAnd);
            break;
        case CmnTypes::COMMS_MSG_SDS:
            tblType = TBLTYPE_SDS;
            res = DbInt::instance().getSdsHistory(startTime, endTime, input,
                                                  from, to, doAnd);
            break;
        case CmnTypes::COMMS_MSG_STATUS:
            tblType = TBLTYPE_STS;
            res = DbInt::instance().getStsMsgHistory(startTime, endTime, input,
                                                     from, to, doAnd);
            break;
        default:
            tblType = TBLTYPE_MSG;
            res = DbInt::instance().getMsgHistory(startTime, endTime, from, to,
                                                  doAnd);
            break;
    }
    if (res == 0)
        return false;
    tv->setSortingEnabled(false);
    auto *mdl = new QStandardItemModel();
    fillMsgData(res, msgType, SubsData::isMultiCluster(), mdl,
                (tblType == TBLTYPE_MSG));
    if (mdl->rowCount() == 0)
        delete mdl;
    else
        setupTable(tblType, mdl, tv);
    return true;
}

int QtTableUtils::getLocData(const string &issis,
                             const string &types,
                             const string &startTime,
                             const string &endTime,
                             QTableView   *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getLocData" == 0);
        return -1;
    }
    auto *res = DbInt::instance().getGps(issis, types, startTime, endTime);
    if (res == 0)
        return -1;
    int n = res->getNumRows();
    if (n > MAX_RES)
    {
        delete res;
        return n;
    }
    tv->setSortingEnabled(false);
    auto *mdl = new QStandardItemModel();
    fillLocData(res, SubsData::isMultiCluster(), mdl);
    if (mdl->rowCount() == 0)
        delete mdl;
    else
        setupTable(TBLTYPE_LOC, mdl, tv, false);
    return 0;
}

void QtTableUtils::setupTable(int                 type,
                              QStandardItemModel *mdl,
                              QTableView         *tv,
                              bool                showType)
{
    assert(mdl != 0 && tv != 0);
    tv->setModel(mdl);
    tv->setSortingEnabled(true);
    int sortColumn = COL_TIME;
    switch (type)
    {
        case TBLTYPE_CALL:
            tv->hideColumn(COL_CALL_KEY);
            tv->hideColumn(COL_CALL_AUDIO_PATH);
            tv->hideColumn(COL_CALL_GSSI);
            break;
        case TBLTYPE_INCIDENT:
            sortColumn = COL_INC_ID;
            break;
        case TBLTYPE_MMS:
        case TBLTYPE_SDS:
        case TBLTYPE_STS:
            tv->hideColumn(COL_TYPE);
            break;
        case TBLTYPE_PTT:
            tv->setSortingEnabled(false);
            sortColumn = COL_PTT_TIME;
            break;
        default:
            break; //do nothing
    }
    mdl->setHorizontalHeaderLabels(getHeaders(type, showType));
    if (type == TBLTYPE_CALL)
        mdl->horizontalHeaderItem(COL_CALL_DURATION)
           ->setToolTip(QObject::tr("Double click row with Play Icon to play "
                                    "audio"));
    tv->sortByColumn(sortColumn, Qt::AscendingOrder);
    tv->resizeRowsToContents();
}

void QtTableUtils::fillCallData(DbInt::QResult     *res,
                                bool                allToGssi,
                                bool                doCheckBranch,
                                QStandardItemModel *mdl)
{
    assert(res != 0 && mdl != 0);
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        fillCallRow(res, i, allToGssi, doCheckBranch, mdl);
    }
    delete res;
    mdl->sort(COL_CALL_TIME);
}

void QtTableUtils::fillMsgData(DbInt::QResult     *res,
                               int                 msgType,
                               bool                doCheckBranch,
                               QStandardItemModel *mdl,
                               bool                showType)
{
    assert(res != 0 && mdl != 0);
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        fillMsgRow(res, i, msgType, doCheckBranch, mdl, showType);
    }
    delete res;
    mdl->sort(COL_TIME);
}

void QtTableUtils::fillLocData(DbInt::QResult     *res,
                               bool                doCheckBranch,
                               QStandardItemModel *mdl)
{
    assert(res != 0 && mdl != 0);
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        fillLocRow(res, i, doCheckBranch, mdl);
    }
    delete res;
    mdl->sort(COL_LOC_TIME);
}

#define SETITEM(field, dbRow, col) \
    do \
    { \
        val.clear(); \
        if (res->getFieldValue(field, val, dbRow) && !val.empty()) \
            mdl->setItem(0, col, \
                         new QStandardItem(QString::fromStdString(val))); \
    } \
    while (0)

#define SETITEM_ID(idField, typeField, dbRow, col) \
    do \
    { \
        if (res->getFieldValue(idField, id, dbRow)) \
        { \
            if (res->getFieldValue(typeField, idType, dbRow)) \
                idType = CmnTypes::fromMsgSpIdentityType(idType); \
            else \
                idType = CmnTypes::IDTYPE_UNKNOWN; \
            item = new QStandardItem(ResourceData::getDspTxt(id, idType)); \
            item->setData(id); \
            mdl->setItem(0, col, item); \
        } \
    } \
    while (0)

//QDateTime::fromString() must use date-time format from db instead of
//QtUtils::timestampFormat
#define SETDATETIME(field, dbRow, col) \
    do \
    { \
        val.clear(); \
        if (res->getFieldValue(field, val, dbRow) && !val.empty()) \
        { \
            item = new QStandardItem(); \
            item->setData(QDateTime::fromString(QString::fromStdString(val), \
                                                "dd/MM/yyyy HH:mm:ss"), \
                          Qt::DisplayRole); \
            mdl->setItem(0, col, item); \
        } \
    } \
    while (0)

void QtTableUtils::fillCallRow(DbInt::QResult     *res,
                               int                 dbRow,
                               bool                allToGssi,
                               bool                doCheckBranch,
                               QStandardItemModel *mdl)
{
    assert(res != 0 && mdl != 0);
    if (doCheckBranch &&
        !checkBranch(res, dbRow, DbInt::FIELD_FROM, DbInt::FIELD_FROM_TYPE) &&
        !checkBranch(res, dbRow, DbInt::FIELD_TO, DbInt::FIELD_TO_TYPE))
        return; //data row not applicable
    mdl->insertRow(0);
    QStandardItem *item;
    string val;
    SETDATETIME(DbInt::FIELD_TIME, dbRow, COL_CALL_TIME);
    SETITEM(DbInt::FIELD_CALL_DURATION, dbRow, COL_CALL_DURATION);
    int id = 0;
    int idType = 0;
    SETITEM_ID(DbInt::FIELD_FROM, DbInt::FIELD_FROM_TYPE, dbRow, COL_CALL_FROM);
    SETITEM_ID(DbInt::FIELD_TO, DbInt::FIELD_TO_TYPE, dbRow, COL_CALL_TO);
    if (allToGssi || idType == CmnTypes::IDTYPE_GROUP)
    {
        mdl->setItem(0, COL_CALL_GSSI,
                     new QStandardItem(QString::number(id)));
    }
    if (res->getFieldValue(DbInt::FIELD_CALL_TYPE, val, dbRow) &&
        !val.empty())
    {
        string val2;
        res->getFieldValue(DbInt::FIELD_CALL_SIMPLEX_DUPLEX, val2, dbRow);
        mdl->setItem(0, COL_CALL_TYPE,
                     new QStandardItem(getCallType(val, val2)));
    }
    SETITEM(DbInt::FIELD_ID, dbRow, COL_CALL_KEY);
    SETITEM(DbInt::FIELD_AUDIO_PATH, dbRow, COL_CALL_AUDIO_PATH);
    if (!val.empty()) //audio path available
        mdl->item(0, COL_CALL_DURATION)
           ->setIcon(QtUtils::getActionIcon(
                                 (QString::fromStdString(val).endsWith(".mp4"))?
                                 CmnTypes::ACTIONTYPE_PLAY_VID:
                                 CmnTypes::ACTIONTYPE_PLAY));
}

void QtTableUtils::fillMsgRow(DbInt::QResult     *res,
                              int                 dbRow,
                              int                 msgType,
                              bool                doCheckBranch,
                              QStandardItemModel *mdl,
                              bool                showType)
{
    assert(res != 0 && mdl != 0);
    if (doCheckBranch &&
        !checkBranch(res, dbRow, DbInt::FIELD_FROM, DbInt::FIELD_FROM_TYPE) &&
        !checkBranch(res, dbRow, DbInt::FIELD_TO, DbInt::FIELD_TO_TYPE))
        return; //data row not applicable
    mdl->insertRow(0);
    QStandardItem *item;
    string val;
    SETDATETIME(DbInt::FIELD_TIME, dbRow, COL_TIME);
    int id;
    int idType;
    SETITEM_ID(DbInt::FIELD_FROM, DbInt::FIELD_FROM_TYPE, dbRow, COL_FROM);
    SETITEM_ID(DbInt::FIELD_TO, DbInt::FIELD_TO_TYPE, dbRow, COL_TO);
    int stsCode = -1;
    string txt;
    if (msgType < 0)
    {
        //in a combined SDS/MMS/Status query result, the status code field is
        //present only for Status, filename is only for MMS, and the MMS/Status
        //text is in DbInt::FIELD_SDS_MSG instead of FIELD_TEXT/STATUSCODE_TEXT
        if (res->hasField(DbInt::FIELD_STATUSCODE_CODE, dbRow))
        {
            msgType = CmnTypes::COMMS_MSG_STATUS;
            res->getFieldValue(DbInt::FIELD_STATUSCODE_CODE, stsCode, dbRow);
            res->getFieldValue(DbInt::FIELD_SDS_MSG, txt, dbRow);
        }
        else if (res->hasField(DbInt::FIELD_FILENAME, dbRow))
        {
            msgType = CmnTypes::COMMS_MSG_MMS;
            stsCode = 0;
        }
        else
        {
            msgType = CmnTypes::COMMS_MSG_SDS;
        }
    }
    if (msgType == CmnTypes::COMMS_MSG_SDS)
    {
        if (showType)
            mdl->setItem(0, COL_TYPE,
                         new QStandardItem(QtUtils::getCommsIcon(msgType),
                                           QObject::tr("SDS")));
        SETITEM(DbInt::FIELD_SDS_MSG, dbRow, COL_MSG);
    }
    else if (msgType == CmnTypes::COMMS_MSG_MMS)
    {
        if (showType)
            mdl->setItem(0, COL_TYPE,
                         new QStandardItem(QtUtils::getCommsIcon(msgType),
                                           QObject::tr("MMS")));
        val.clear();
        res->getFieldValue((stsCode < 0)? DbInt::FIELD_TEXT:
                                          DbInt::FIELD_SDS_MSG,
                           val, dbRow);
        res->getFieldValue(DbInt::FIELD_FILENAME, txt, dbRow);
        if (!val.empty() || !txt.empty())
        {
            QString s(QString::fromStdString(val));
            if (!txt.empty())
            {
                if (!val.empty())
                    s.append(" ");
                s.append("[").append(QObject::tr("attached file "))
                 .append(QString::fromStdString(txt)).append("]");
            }
            mdl->setItem(0, COL_MSG, new QStandardItem(s));
        }
    }
    else
    {
        if (showType)
            mdl->setItem(0, COL_TYPE,
                         new QStandardItem(QtUtils::getCommsIcon(msgType),
                                           QObject::tr("Status")));
        //show message as "text [code]"
        if (stsCode < 0)
        {
            res->getFieldValue(DbInt::FIELD_STATUSCODE_TEXT, txt, dbRow);
            res->getFieldValue(DbInt::FIELD_STATUSCODE_CODE, stsCode, dbRow);
        }
        QString s(QString::fromStdString(txt));
        if (stsCode >= 0)
            s.append(" [").append(QString::number(stsCode)).append("]");
        mdl->setItem(0, COL_MSG, new QStandardItem(s));
    }
}

void QtTableUtils::fillLocRow(DbInt::QResult     *res,
                              int                 dbRow,
                              bool                doCheckBranch,
                              QStandardItemModel *mdl)
{
    assert(res != 0 && mdl != 0);
    if (doCheckBranch)
    {
        int id;
        if (res->getFieldValue(DbInt::FIELD_LOC_ISSI, id, dbRow))
        {
            int type = CmnTypes::toMsgSpSubsType(ResourceData::getType(id));
            switch (type)
            {
                case MsgSp::Value::SUBS_TYPE_ISSI:
                    if (!SubsData::validIssi(id))
                        return; //data row not applicable
                    break;
                case MsgSp::Value::SUBS_TYPE_MOBILE:
                    if (!SubsData::isValidClient(type, id))
                        return;
                    break;
                default:
                    return; //should not occur
            }
        }
    }
    mdl->insertRow(0);
    QStandardItem *item;
    string val;
    SETITEM(DbInt::FIELD_TYPE_DESC, dbRow, COL_LOC_TYPE);
    SETITEM(DbInt::FIELD_LOC_ISSI, dbRow, COL_LOC_ISSI);
    SETDATETIME(DbInt::FIELD_LOC_TIME, dbRow, COL_LOC_TIME);
    SETITEM(DbInt::FIELD_LOC_LATLONG, dbRow, COL_LOC_LOC);
}

bool QtTableUtils::checkBranch(DbInt::QResult *res,
                               int             row,
                               int             idFld,
                               int             tpFld)
{
    assert(res != 0);
    int id;
    int type;
    if (res->getFieldValue(idFld, id, row) &&
        res->getFieldValue(tpFld, type, row))
    {
        type = CmnTypes::toMsgSpSubsType(CmnTypes::fromMsgSpIdentityType(type));
        switch (type)
        {
            case MsgSp::Value::SUBS_TYPE_DISPATCHER:
            case MsgSp::Value::SUBS_TYPE_MOBILE:
                return SubsData::isValidClient(type, id);
            case MsgSp::Value::SUBS_TYPE_GRP:
                return SubsData::validGssi(id);
            case MsgSp::Value::SUBS_TYPE_ISSI:
                return SubsData::validIssi(id);
            default:
                break; //do nothing
        }
    }
    return false;
}

inline QString QtTableUtils::getToField(DbInt::QResult *res, int row, int *gssi)
{
    assert(res != 0);
    int id;
    int idType;
    if (res->getFieldValue(DbInt::FIELD_TO, id, row) &&
        res->getFieldValue(DbInt::FIELD_TO_TYPE, idType, row))
    {
        if (idType == MsgSp::Value::IDENTITY_TYPE_GSSI)
        {
            if (gssi != 0)
                *gssi = id;
            return ResourceData::getDspTxt(id, CmnTypes::IDTYPE_GROUP);
        }
        return QString::number(id);
    }
    return "";
}

#ifdef INCIDENT
QString QtTableUtils::getIncidentStatus(int status)
{
    switch (status)
    {
        case IncidentData::STATE_CLOSED:
            return QObject::tr("Closed");
        case IncidentData::STATE_DISPATCHED:
            return QObject::tr("Dispatched");
        case IncidentData::STATE_ON_SCENE:
            return QObject::tr("On Scene");
        case IncidentData::STATE_OPEN:
            return QObject::tr("Open");
        case IncidentData::STATE_RECEIVED:
            return QObject::tr("Received");
        default:
            break; //do nothing
    }
    return "";
}

bool QtTableUtils::getCallData(const QStringList &ids,
                               const string      &startTime,
                               const string      &endTime,
                               IntSetT           &gssis,
                               QTableView        *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getCallData(Incident)" == 0);
        return false;
    }
    int i;
    int id;
    int idType;
    int src;
    int dst;
    bool wasEmpty = false;
    IntSetT idSet;
    DbInt::QResult *res;
    DbInt &db(DbInt::instance());
    tv->setSortingEnabled(false);
    auto *mdl = qobject_cast<QStandardItemModel *>(tv->model());
    if (mdl == 0)
    {
        mdl = new QStandardItemModel();
        wasEmpty = true;
    }
    bool doCheckBranch = SubsData::isMultiCluster();
    for (const auto &idStr : ids) //ids non-empty only in first pass
    {
        id = idStr.toInt();
        //calls by or to each id
        res = db.getCallHistory(startTime, endTime, "", id, id, false);
        if (res == 0)
            continue;
        for (i=res->getNumRows()-1; i>=0; --i)
        {
            src = 0;
            if (res->getFieldValue(DbInt::FIELD_FROM, src, i) &&
                src != id && idSet.count(src) != 0)
            {
                continue; //already added
            }
            dst = 0;
            if (res->getFieldValue(DbInt::FIELD_TO, dst, i) &&
                res->getFieldValue(DbInt::FIELD_TO_TYPE, idType, i))
            {
                if (idType == MsgSp::Value::IDENTITY_TYPE_GSSI)
                {
                    gssis.insert(dst); //collect gssis for second pass
                    continue;
                }
                if (dst != id && idSet.count(dst) != 0)
                    continue; //already added
            }
            fillCallRow(res, i, false, doCheckBranch, mdl);
        }
        idSet.insert(id);
        delete res;
    }
    if (ids.isEmpty())
    {
        //this is second pass for group calls
        for (auto gssi : gssis)
        {
            //calls to each gssi from anybody
            res = db.getCallHistory(startTime, endTime, "", 0, gssi, true);
            if (res != 0)
                fillCallData(res, true, doCheckBranch, mdl);
        }
    }
    if (mdl->rowCount() == 0)
    {
        if (!wasEmpty)
            tv->setModel(0);
        delete mdl;
        return false;
    }
    setupTable(TBLTYPE_CALL, mdl, tv, true);
    return true;
}

bool QtTableUtils::getMsgData(const QStringList &ids,
                              const string      &startTime,
                              const string      &endTime,
                              IntSetT           &gssis,
                              QTableView        *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getMsgData(Incident)" == 0);
        return false;
    }
    int i;
    int id;
    int idType;
    int src;
    int dst;
    bool wasEmpty = false;
    IntSetT idSet;
    DbInt::QResult *res;
    DbInt &db(DbInt::instance());
    tv->setSortingEnabled(false);
    auto *mdl = qobject_cast<QStandardItemModel *>(tv->model());
    if (mdl == 0)
    {
        mdl = new QStandardItemModel();
        wasEmpty = true;
    }
    bool doCheckBranch = SubsData::isMultiCluster();
    for (const auto &idStr : ids) //ids non-empty only in first pass
    {
        id = idStr.toInt();
        res = db.getMsgHistory(startTime, endTime, id, id, false);
        if (res != 0)
        {
            for (i=res->getNumRows()-1; i>=0; --i)
            {
                src = 0;
                if (res->getFieldValue(DbInt::FIELD_FROM, src, i) &&
                    src != id && idSet.count(src) != 0)
                {
                    continue; //already added
                }
                dst = 0;
                if (res->getFieldValue(DbInt::FIELD_TO, dst, i) &&
                    res->getFieldValue(DbInt::FIELD_TO_TYPE, idType, i))
                {
                    if (idType == MsgSp::Value::IDENTITY_TYPE_GSSI)
                    {
                        gssis.insert(dst); //collect gssis for second pass
                        continue;
                    }
                    if (dst != id && idSet.count(dst) != 0)
                        continue; //already added
                }
                fillMsgRow(res, i, -1, doCheckBranch, mdl, true);
            }
            idSet.insert(id);
            delete res;
        }
    }
    if (ids.isEmpty())
    {
        //this is second pass for group messages
        for (auto gssi : gssis)
        {
            res = db.getMsgHistory(startTime, endTime, 0, gssi, true);
            if (res != 0)
                fillMsgData(res, -1, doCheckBranch, mdl, true);
        }
    }
    if (mdl->rowCount() == 0)
    {
        if (!wasEmpty)
            tv->setModel(0);
        delete mdl;
        return false;
    }
    setupTable(TBLTYPE_MSG, mdl, tv, true);
    return true;
}

bool QtTableUtils::getLastData(int         tblType,
                               int         msgType,
                               int         from,
                               int         to,
                               QTableView *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getLastData()" == 0);
        return false;
    }
    tv->setSortingEnabled(false);
    bool retVal = false;
    auto *mdl = qobject_cast<QStandardItemModel *>(tv->model());
    if (mdl == 0)
    {
        mdl = new QStandardItemModel();
        retVal = true;
    }
    bool doCheckBranch = SubsData::isMultiCluster();
    int rows = mdl->rowCount();
    DbInt::QResult *res = 0;
    switch (tblType)
    {
        case TBLTYPE_CALL:
            res = DbInt::instance().getLastCall(from, to);
            if (res != 0)
                fillCallData(res, false, doCheckBranch, mdl);
            break;
        case TBLTYPE_MMS:
            res = DbInt::instance().getLastMms(from, to);
            if (res != 0)
                fillMsgData(res, CmnTypes::COMMS_MSG_MMS, doCheckBranch, mdl);
            break;
        case TBLTYPE_MSG:
            if (msgType == CmnTypes::COMMS_MSG_SDS)
                res = DbInt::instance().getLastSds(from, to);
            else if (msgType == CmnTypes::COMMS_MSG_STATUS)
                res = DbInt::instance().getLastSts(from, to);
            else
                res = DbInt::instance().getLastMms(from, to);
            if (res != 0)
                fillMsgData(res, msgType, doCheckBranch, mdl, true);
            break;
        case TBLTYPE_SDS:
            res = DbInt::instance().getLastSds(from, to);
            if (res != 0)
                fillMsgData(res, CmnTypes::COMMS_MSG_SDS, doCheckBranch, mdl);
            break;
        case TBLTYPE_STS:
            res = DbInt::instance().getLastSts(from, to);
            if (res != 0)
                fillMsgData(res, CmnTypes::COMMS_MSG_STATUS, doCheckBranch, mdl);
            break;
        default:
            assert("Bad type in QtTableUtils::getLastData()" == 0);
            break;
    }
    if (!retVal)
    {
        tv->setSortingEnabled(true);
        if (mdl->rowCount() > rows)
            tv->resizeRowsToContents();
    }
    else if (mdl->rowCount() == 0)
    {
        delete mdl;
        retVal = false;
    }
    else
    {
        setupTable(tblType, mdl, tv, true);
    }
    return retVal;
}

bool QtTableUtils::getIncidentData(int id, QTableView *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getIncidentData(id)" == 0);
        return false;
    }
    auto *res = DbInt::instance().getIncidentHistory(id);
    if (res == 0)
        return false;
    fillIncidentData(res, tv);
    return true;
}

bool QtTableUtils::getIncidentData(int           status,
                                   const string &startTime,
                                   const string &endTime,
                                   const string &state,
                                   const string &priority,
                                   const string &category,
                                   const string &desc,
                                   const string &resources,
                                   QTableView   *tv)
{
    if (tv == 0)
    {
        assert("Bad param in QtTableUtils::getIncidentData" == 0);
        return false;
    }
    auto *res = DbInt::instance().getIncidentHistory(startTime, endTime, state,
                                   priority, category, desc, resources, status);
    if (res == 0)
        return false;
    fillIncidentData(res, tv);
    return true;
}

void QtTableUtils::fillIncidentData(DbInt::QResult *res, QTableView *tv)
{
    assert(res != 0 && tv != 0);
    tv->setSortingEnabled(false);
    IncidentData *data;
    auto *mdl = new QStandardItemModel();
    bool doCheckBranch = SubsData::isMultiCluster();
    string val;
    int creator;
    int status;
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        //check for either creator or updater in common branches if defined
        if (doCheckBranch &&
            (!res->getFieldValue(DbInt::FIELD_CREATED_BY, creator, i) ||
             !SubsData::isValidClient(MsgSp::Value::SUBS_TYPE_DISPATCHER,
                                      creator)) &&
            //reuse 'status' for updater
            (!res->getFieldValue(DbInt::FIELD_MODIFIED_BY, status, i) ||
             status == creator ||
             !SubsData::isValidClient(MsgSp::Value::SUBS_TYPE_DISPATCHER,
                                      status)))
        {
            continue; //data not applicable
        }
        status = -1; //init to invalid
        if (res->getFieldValue(DbInt::FIELD_INC_STATUS, status, i) &&
            status == IncidentData::STATE_CLOSED)
        {
            data = new IncidentData(res, i);
            if (!data->isValid())
            {
                delete data;
                continue;
            }
        }
        else
        {
            data = 0;
        }
        mdl->insertRow(0);
        mdl->setItem(0, COL_INC_STATUS,
                     new QStandardItem(getIncidentStatus(status)));
        SETITEM(DbInt::FIELD_ID, i, COL_INC_ID);
        SETITEM(DbInt::FIELD_STATE_DESC, i, COL_INC_STATE);
        SETITEM(DbInt::FIELD_INC_PRIORITY, i, COL_INC_PRIORITY);
        SETITEM(DbInt::FIELD_INC_CATEGORY, i, COL_INC_CATEGORY);
        SETITEM(DbInt::FIELD_INC_DESC, i, COL_INC_DESCRIPTION);
        if (res->getFieldValue(DbInt::FIELD_INC_RESOURCES, val, i) &&
            !val.empty())
        {
            //need a space between list items to enable multiline display
            mdl->setItem(0, COL_INC_RESOURCES,
                         new QStandardItem(QString::fromStdString(val)
                                           .replace(',', ", ")));
        }
        if (data != 0)
            mdl->item(0, COL_INC_ID) //put data in ID item as UserRole
               ->setData(QVariant::fromValue(data), Qt::UserRole);
    }
    if (mdl->rowCount() == 0)
        delete mdl;
    else
        setupTable(TBLTYPE_INCIDENT, mdl, tv);
}
#endif //INCIDENT
