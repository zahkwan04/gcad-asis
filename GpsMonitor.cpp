/**
 * GPS monitoring selector implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: GpsMonitor.cpp 1773 2023-10-26 07:34:48Z zulzaidi $
 * @author Mohd Fashan Abdul Munir
 * @author Mohd Rozaimi
 */
#include <algorithm>   //std::sort
#include <QList>
#include <QMessageBox>
#include <QRegExp>
#include <assert.h>

#include "CmnTypes.h"
#include "QtUtils.h"
#include "ResourceData.h"
#include "Style.h"
#include "SubsData.h"
#include "ui_GpsMonitor.h"
#include "GpsMonitor.h"

using namespace std;

enum
{
    OPT_NONE, //must have value 0
    OPT_ALL,
    OPT_SEL
};

bool               GpsMonitor::sMonGrps(false);
GpsMonitor::SsisT  GpsMonitor::sIssis;
ServerSession     *GpsMonitor::sSession(0);
Logger            *GpsMonitor::sLogger(0);

GpsMonitor::GpsMonitor(QWidget *parent) :
QDialog(parent), ui(new Ui::GpsMonitor)
{
    assert(sSession != 0);
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    //remove title bar help button and disable close button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
    const QString &ss(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->searchFrame->setStyleSheet(ss);
    ui->rscFrame->setStyleSheet(ss);
    ui->monFrame->setStyleSheet(ss);
    mSortModel = new QSortFilterProxyModel;
    mSortModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mResourceList = new DraggableListView(ResourceData::TYPE_SUBSCRIBER);
    mResourceList->setDragDropMode(QAbstractItemView::DragDrop);
    mResourceList->setFont(ui->searchEdit->font());
    mResourceList->installEventFilter(this);
    ui->rscLayout->addWidget(mResourceList);
    mMonList = new DraggableListView(ResourceData::TYPE_SUBSCRIBER);
    mMonList->setDragDropMode(QAbstractItemView::DragDrop);
    mMonList->installEventFilter(this);
    auto *mdl = ResourceData::createModel(ResourceData::TYPE_SUBS_OR_MOBILE,
                                          mMonList);
    mMonList->setModel(mdl);
    ui->monLayout->addWidget(mMonList);
    mMonList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mMonList, &QListView::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                auto idx(mMonList->indexAt(pos));
                if (!idx.isValid())
                    return;
                QList<int> actTypes;
                actTypes << CmnTypes::ACTIONTYPE_DELETE;
                if (mMonList->selectionModel()->selectedIndexes().count() == 1)
                    actTypes << CmnTypes::ACTIONTYPE_LOCATE;
                actTypes << CmnTypes::ACTIONTYPE_COPY;
                QMenu menu(this);
                QtUtils::addMenuActions(menu, actTypes);
                auto *act = menu.exec(mMonList->mapToGlobal(pos));
                if (act == 0)
                    return;
                switch (act->data().toInt())
                {
                    case CmnTypes::ACTIONTYPE_COPY:
                        QtUtils::copyToClipboard(mMonList);
                        break;
                    case CmnTypes::ACTIONTYPE_DELETE:
                        moveItems(false);
                        break;
                    case CmnTypes::ACTIONTYPE_LOCATE:
                        emit locateResource(
                                  ResourceData::getItemId(mMonList, idx.row()));
                        break;
                    default:
                        break; //do nothing
                }
            });
    connect(ui->searchEdit, &QLineEdit::textChanged, this,
            [this](const QString &txt)
            {
                QRegExp re(txt, Qt::CaseInsensitive); //filter resource list
                if (re.isValid())
                    mSortModel->setFilterRegExp(re);
            });
    connect(ui->addButton, &QRadioButton::clicked, this,
            [this]() { moveItems(true); });
    connect(ui->removeButton, &QRadioButton::clicked, this,
            [this]() { moveItems(false); });
    connect(ui->removeAllButton, &QRadioButton::clicked, this,
            [this]()
            {
                mMonList->selectAll();
                moveItems(false);
            });
    connect(ui->optAll, &QRadioButton::clicked, this,
            [this]() { setEnabled(false); });
    connect(ui->optSel, &QRadioButton::clicked, this,
            [this]()
            {
                setEnabled(true);
                ui->searchEdit->setFocus();
            });
    connect(ui->optMonGrps, &QRadioButton::clicked, this,
            [this]() { setEnabled(false); });
    connect(ui->okButton, &QRadioButton::clicked, this, [this]() { onOk(); });
    connect(ui->cancelButton, &QRadioButton::clicked, this,
            [this]() { close(); });
    if (sMonGrps)
    {
        mOpt = OPT_SEL;
        ui->optMonGrps->click();
    }
    else if (sIssis.empty())
    {
        mOpt = OPT_ALL;
        ui->optAll->click();
    }
    else if (*(sIssis.begin()) == OPT_NONE)
    {
        mOpt = OPT_NONE;
        ui->optSel->click();
    }
    else
    {
        mOpt = OPT_SEL;
        ui->optSel->click();
        //show current monitored
        for (auto i : sIssis)
        {
            ResourceData::addId(mdl, i);
        }
        mdl->sort(0);
    }
    //fill up source with subs and mobiles
    ResourceData::IdsT ids;
    mdl = ResourceData::getModel(ResourceData::TYPE_SUBSCRIBER);
    if (mdl != 0)
        mdl->getIds(ids);
    mdl = ResourceData::getModel(ResourceData::TYPE_MOBILE);
    if (mdl != 0)
        mdl->getIds(ids);
    if (!ids.empty())
    {
        mdl = ResourceData::createModel(ResourceData::TYPE_SUBS_OR_MOBILE,
                                        mResourceList);
        for (auto i : ids)
        {
            if (sIssis.count(i) == 0) //skip monitored ones
                ResourceData::addId(mdl, i);
        }
        mdl->sort(0);
        mSortModel->setSourceModel(mdl);
        mResourceList->setModel(mSortModel);
    }
    show();
}

GpsMonitor::~GpsMonitor()
{
    delete mResourceList;
    delete mSortModel;
    delete mMonList;
    delete ui;
}

void GpsMonitor::init(Logger        *logger,
                      ServerSession *session,
                      const string  &issiList)
{
    if (logger == 0 || session == 0)
    {
        assert("Bad param in GpsMonitor::init" == 0);
        return;
    }
    sLogger = logger;
    sSession = session;
    sIssis.clear();
    LOGGER_DEBUG(sLogger, "GpsMonitor::init: ISSIs "
                 << ((issiList.empty())? "<none>": issiList));
    //ISSIs:-empty = all
    //      -single entry OPT_NONE = none
    //      -OPT_NONE followed by another entry = monitored grps
    if (!issiList.empty() && Utils::fromStringWithRange(issiList, sIssis) > 0 &&
        *(sIssis.begin()) == OPT_NONE)
    {
        if (sIssis.size() > 1)
            sMonGrps = true; //for mon grps, also start with none
        session->gpsMonitorStop(SsisT()); //no monitoring
    }
    else
    {
        session->gpsMonitorStart(sIssis); //all or saved selection
    }
}

bool GpsMonitor::result(MsgSp &msg)
{
    if (!msg.isResultSuccessful())
    {
        LOGGER_ERROR(sLogger, "GpsMonitor::result: " << msg.getName()
                     << ": Failed to update monitoring with "
                     << msg.getFieldValueString(MsgSp::Field::ISSI_LIST) << ", "
                     << msg.getFieldValueString(MsgSp::Field::RESULT));
        QString s((msg.getType() == MsgSp::Type::GPS_MON_START)?
                  tr("Failed to add to monitoring"):
                  tr("Failed to remove from monitoring"));
        s.append(", ")
         .append(QString::fromStdString(msg.getFieldValueString(
                                                      MsgSp::Field::ISSI_LIST)))
         .append(",\n")
         .append(QString::fromStdString(msg.getFieldValueString(
                                                        MsgSp::Field::RESULT)));
        QMessageBox::critical(0, tr("GPS Monitoring Error"), s);
        return false;
    }
    bool retVal = false;
    SsisT ssis;
    switch (msg.getType())
    {
        case MsgSp::Type::GPS_MON_START:
        {
            if (!msg.hasField(MsgSp::Field::ISSI_LIST))
            {
                sIssis.clear(); //monitoring all
                retVal = true;
            }
            else if (Utils::fromStringWithRange(
                         msg.getFieldString(MsgSp::Field::ISSI_LIST), ssis) > 0)
            {
                if (sIssis.empty()) //changed from ALL
                    msg.removeField(MsgSp::Field::MSG_ACK); //special indicator
                else if (!sMonGrps)
                    sIssis.erase(OPT_NONE); //in case changing from no selection
                sIssis.insert(ssis.begin(), ssis.end());
                retVal = true;
            }
            LOGGER_DEBUG(sLogger, "GpsMonitor::result: " << msg.getName()
                         << " sIssis=" << getList(true));
            break;
        }
        case MsgSp::Type::GPS_MON_STOP:
        {
            if (!msg.hasField(MsgSp::Field::ISSI_LIST))
            {
                sIssis.clear();
                sIssis.insert(OPT_NONE); //not monitoring
                if (sMonGrps)
                {
                    sIssis.insert(OPT_ALL);
                    msg.removeField(MsgSp::Field::MSG_ACK); //special indicator
                }
                retVal = true;
                LOGGER_DEBUG(sLogger, "GpsMonitor::result: " << msg.getName()
                             << " sIssis=" << getList(true));
            }
            else if (Utils::fromStringWithRange(
                         msg.getFieldString(MsgSp::Field::ISSI_LIST), ssis) > 0)
            {
                for (auto i : ssis)
                {
                    sIssis.erase(i);
                }
                retVal = true;
                //sIssis should never be empty here
                LOGGER_DEBUG(sLogger, "GpsMonitor::result: " << msg.getName()
                             << " Erased "
                             << msg.getFieldString(MsgSp::Field::ISSI_LIST)
                             << ", sIssis=" << getList(true));
            }
            break;
        }
        default:
        {
            assert("Bad param in GpsMonitor::result" == NULL);
            break;
        }
    }
    return retVal;
}

QString GpsMonitor::monGrpsStart(const SsisT &gssis)
{
    QString err;
    if (!sMonGrps)
        return err;
    sIssis.insert(OPT_NONE);
    SsisT issis;
    for (auto i : gssis)
    {
        SubsData::getGrpAttachedMembers(i, issis);
    }
    //exclude issis already monitored
    auto it = issis.begin();
    while (it != issis.end())
    {
        if (sIssis.count(*it) == 0)
            ++it;
        else
            it = issis.erase(it);
    }
    if (!issis.empty() && !sSession->gpsMonitorStart(issis))
    {
        string s(Utils::toStringWithRange(issis));
        LOGGER_ERROR(sLogger, "GpsMonitor::monGrpsStart: Failed to send "
                     "monitoring update request to add " << s);
        err.append(tr("Failed to send update request to %1").arg(tr("add ")))
           .append(QString::fromStdString(s));
    }
    return err;
}

QString GpsMonitor::monGrpsStop(const SsisT &gssis, SsisT &affSsis)
{
    QString err;
    if (!sMonGrps)
        return err;
    SsisT issis; //members of stopped grps
    for (auto i : affSsis)
    {
        SubsData::getGrpAttachedMembers(i, issis);
    }
    //exclude issis not monitored, and members of monitored grps
    auto it = issis.begin();
    while (it != issis.end())
    {
        if (sIssis.count(*it) == 0 ||
            (!gssis.empty() && SubsData::isGrpAttachedMember(*it, gssis)))
            it = issis.erase(it);
        else
            ++it;
    }
    if (!issis.empty() && !sSession->gpsMonitorStop(issis))
    {
        string s(Utils::toStringWithRange(issis));
        LOGGER_ERROR(sLogger, "GpsMonitor::monGrpsStop: Failed to send "
                     "monitoring update request to remove " << s);
        err.append((tr("Failed to send update request to %1"))
                    .arg(tr("remove ")))
           .append(QString::fromStdString(s));
    }
    else
    {
        affSsis = issis;
    }
    return err;
}

QString GpsMonitor::monGrpsAttDet(int issi, const SsisT &gssis)
{
    string log;
    QString err;
    if (sIssis.count(issi) == 0)
    {
        //not monitored - add if attached to monitored grps
        if (SubsData::isGrpAttachedMember(issi, gssis) &&
            !sSession->gpsMonitorStart(SsisT({issi})))
        {
            log.append("add ");
            err.append(tr("add "));
        }
    }
    //monitored - remove if not attached to monitored grps
    else if (!SubsData::isGrpAttachedMember(issi, gssis) &&
             !sSession->gpsMonitorStop(SsisT({issi})))
    {
        log.append("remove ");
        err.append(tr("remove "));
    }
    if (!log.empty())
    {
        LOGGER_ERROR(sLogger, "GpsMonitor::monGrpsAttDet: Failed to send "
                     "monitoring update request to " << log << issi);
        err = tr("Failed to send update request to %1").arg(err);
        err.append(QString::number(issi));
    }
    return err;
}

string GpsMonitor::getList(bool forLog)
{
    if (!forLog && sMonGrps)
    {
        SsisT ssis;
        ssis.insert(OPT_NONE);
        ssis.insert(OPT_ALL);
        return Utils::toStringWithRange(ssis);
    }
    return Utils::toStringWithRange(sIssis);
}

bool GpsMonitor::eventFilter(QObject *obj, QEvent *event)
{
    //accept only drag-and-drop between mResourceList and mMonList
    if (event->type() == QEvent::DragEnter)
    {
        auto *e = static_cast<QDragEnterEvent *>(event);
        if (obj != e->source() &&
            (e->source() == mResourceList || e->source() == mMonList))
        {
            e->acceptProposedAction();
            return true;
        }
    }
    else if (event->type() == QEvent::Drop)
    {
        auto *e = static_cast<QDropEvent *>(event);
        if (obj == e->source())
            return false;
        //source already validated in DragEnter event
        if (obj == mMonList)
        {
            moveItems(true);
            return true;
        }
        if (obj == mResourceList)
        {
            moveItems(false);
            return true;
        }
    }
    return false;
}

void GpsMonitor::setEnabled(bool enabled)
{
    mResourceList->setEnabled(enabled);
    mMonList->setEnabled(enabled);
    ui->searchEdit->setEnabled(enabled);
    ui->addButton->setEnabled(enabled);
    ui->removeButton->setEnabled(enabled);
    ui->removeAllButton->setEnabled(enabled);
}

void GpsMonitor::moveItems(bool add)
{
    QModelIndexList l((add)?
                      mResourceList->selectionModel()->selectedIndexes():
                      mMonList->selectionModel()->selectedIndexes());
    if (l.empty())
        return;
    auto *rscMdl = static_cast<ResourceData::ListModel *>(
                                                     mSortModel->sourceModel());
    auto *monMdl = ResourceData::model(mMonList);
    int id;
    ResourceData::IdsT ids;
    if (add)
    {
        for (auto &idx : l)
        {
            id = ResourceData::getItemId(
                              rscMdl->item(mSortModel->mapToSource(idx).row()));
            ResourceData::addId(monMdl, id);
            ids.insert(id);
        }
        rscMdl->removeIds(ids);
        monMdl->sort(0);
    }
    else
    {
        for (auto &idx : l)
        {
            id = ResourceData::getItemId(monMdl->item(idx.row()));
            ResourceData::addId(rscMdl, id);
            ids.insert(id);
        }
        monMdl->removeIds(ids);
        rscMdl->sort(0);
    }
}

void GpsMonitor::onOk()
{
    string log;
    QString err;
    SsisT newList;
    if (ui->optSel->isChecked())
    {
        auto *mdl = ResourceData::model(mMonList);
        if (mdl->getIds(newList))
        {
            SsisT updList;
            if (mOpt == OPT_SEL && !sIssis.empty())
            {
                if (sMonGrps)
                {
                    sIssis.erase(OPT_NONE); //changing from mon grps
                    sIssis.erase(OPT_ALL);
                }
                //update removed items
                for (auto i : sIssis)
                {
                    if (newList.count(i) == 0)
                        updList.insert(i);
                }
                if (!updList.empty() && !sSession->gpsMonitorStop(updList))
                {
                    log.assign(Utils::toStringWithRange(updList));
                    err.append(tr("remove "))
                       .append(QString::fromStdString(log));
                    log.insert(0, "remove ");
                }
                updList.clear();
            }
            //update added items
            for (auto i : newList)
            {
                if (sIssis.count(i) == 0)
                    updList.insert(i);
            }
            if (!updList.empty() && !sSession->gpsMonitorStart(updList))
            {
                if (!err.isEmpty())
                {
                    log.append(", and ");
                    err.append(tr(", and "));
                }
                string s(Utils::toStringWithRange(updList));
                log.append("add ").append(s);
                err.append(tr("add ")).append(QString::fromStdString(s));
            }
        }
        //none selected - stop all if was monitoring before
        else if (mOpt != OPT_NONE && !sSession->gpsMonitorStop(newList))
        {
            log = "stop monitoring";
            err = tr("stop monitoring");
        }
    }
    else if (ui->optAll->isChecked())
    {
        if (mOpt != OPT_ALL && !sSession->gpsMonitorStart(newList))
        {
            log = "monitor all";
            err = tr("monitor all");
        }
    }
    //optMonGrps
    else if (!sMonGrps)
    {
        if (mOpt == OPT_NONE)
        {
            sIssis.insert(OPT_ALL); //from NONE to mon grps
            emit listChanged(getList()); //because not sending svr msg
        }
        //stop all first - grp members to be selected upon svr response
        else if (!sSession->gpsMonitorStop(newList))
        {
            log = "stop monitoring";
            err = tr("stop monitoring");
        }
    }
    if (log.empty())
    {
        sMonGrps = ui->optMonGrps->isChecked();
    }
    else
    {
        LOGGER_ERROR(sLogger, "GpsMonitor::onOk: Failed to send monitoring "
                     "update request to " << log);
        QMessageBox::critical(this, tr("GPS Monitoring Error"),
                              tr("Failed to send update request to %1")
                                  .arg(err));
    }
    close();
}
