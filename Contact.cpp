/**
 * UI Quick Contact implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2021-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Contact.cpp 1880 2024-11-15 08:24:57Z hazim.rujhan $
 * @author Rosnin
 */
#include <algorithm>    //std::sort
#include <sstream>
#include <assert.h>

#include <QCheckBox>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMimeData>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "DbInt.h"
#include "MsgSp.h"
#include "QtUtils.h"
#include "ResourceButton.h"
#include "Settings.h"
#include "Style.h"
#include "SubsData.h"
#include "ui_Contact.h"
#include "Contact.h"

using namespace std;

enum eStyle
{
    STYLE_IDLE,
    STYLE_CALL_SETUP,
    STYLE_CALL_CONNECT, //call connected, but not in it
    STYLE_CALL_ACTIVE   //actively in the call
};

#ifdef SAVE2CFG
static const char    CFGDELIM(','); //delimiter between cfg values
static const char    VALDELIM(' '); //delimiter within cfg value
#endif
static const int     MAXCHARS = ResourceButton::NOTES_MAXCHARS;
static const int     NOTES_MAXWLEN = 10;
static const string  LOGPREFIX("Contact:: ");
static const QString TABNAMEDEFAULT("A1");

//comparison function compatible with std::sort()
static bool compare(const Contact *c1, const Contact *c2)
{
    static QList<int> l({ResourceData::TYPE_GROUP,
                         ResourceData::TYPE_DGNA_IND,
                         ResourceData::TYPE_DGNA_GRP,
                         ResourceData::TYPE_SUBSCRIBER,
                         ResourceData::TYPE_MOBILE,
                         ResourceData::TYPE_DISPATCHER});
    int v = l.indexOf(c1->getType()) - l.indexOf(c2->getType());
    if (v == 0)
        v = c1->getName().compare(c2->getName());
    return (v < 0);
}

string                 Contact::sUsername;
Logger                *Contact::sLogger(0);
ServerSession         *Contact::sSession(0);
Resources             *Contact::sResources(0);
QObject               *Contact::sEventFilter(0);
QTabWidget            *Contact::sTabWidget(0);
Contact::ContactMapT   Contact::sContactMap;
Contact::TabMapT       Contact::sTabMap;
Contact::MonCountMapT  Contact::sMonMaxMap;
Contact::MonCountMapT  Contact::sMonCountMap;

Contact::~Contact()
{
    delete ui;
}

bool Contact::selected() const
{
    return (ui->selectCb != 0 && ui->selectCb->isChecked());
}

bool Contact::sysDefined() const
{
    return !ui->userDefBtn->isHidden();
}

bool Contact::userDefined() const
{
    return ui->userDefBtn->isChecked();
}

QScrollArea *Contact::getTab() const
{
    //when in a layout, the parent is the child widget of the scroll area
    auto *w = parentWidget();
    auto *tab = qobject_cast<QScrollArea *>(w);
    while (tab == 0 && w != 0)
    {
        w = w->parentWidget();
        tab = qobject_cast<QScrollArea *>(w);
    }
    return tab;
}

void Contact::setMonitored(bool on)
{
    if (ui->monBtn != 0)
    {
        ui->monBtn->setChecked(on);
        if (on)
        {
            ++sMonCountMap[mIsGrp];
            //change tooltip only in user tab
            if (!sysDefined())
                ui->monBtn->setToolTip(tr("Stop monitoring"));
            LOGGER_DEBUG(sLogger, LOGPREFIX << "setMonitored: Monitored "
                         << ((mIsGrp)? "grps": "subs") << " = "
                         << sMonCountMap[mIsGrp]);
        }
        else if (!sysDefined())
        {
            ui->monBtn->setToolTip(tr("Monitor"));
        }
    }
}

bool Contact::monitored() const
{
    return (ui->monBtn != 0 && ui->monBtn->isChecked());
}

bool Contact::setNotes(QString &txt, bool isByUser)
{
    if (txt.length() > MAXCHARS)
    {
        if (isByUser)
            return false;
        txt = txt.left(MAXCHARS);
    }
    if (txt != mNotes)
    {
        mNotes = txt;
        if (mType == ResourceData::TYPE_DISPATCHER ||
            mType == ResourceData::TYPE_MOBILE)
            rscDspOptChanged(mType);
        //save only for contact on user tab
        if (isByUser && sTabMap[getTab()].usr)
            dataSave(0, this);
    }
    return true;
}

void Contact::setGrpActive()
{
    switch (SubsData::grpActive(mId))
    {
        case SubsData::GRP_STAT_ASSIGN:
            ui->grpStatLbl
              ->setPixmap(QPixmap(":/Images/images/icon_assign.png"));
            break;
        case SubsData::GRP_STAT_ATTACH:
            ui->grpStatLbl
              ->setPixmap(QPixmap(":/Images/images/icon_attach.png"));
            break;
        default:
            ui->grpStatLbl->clear();
            break;
    }
}

void Contact::callSetup(CallWindow *call)
{
    if (mCall != 0)
        return; //subsequent invocation in SETUP->PROCEEDING->ALERT
    mCall = call;
    if (call->isOutgoingCall())
    {
        if (ui->endBtn != 0)
            ui->endBtn->setEnabled(true);
        if (ui->pttBtn != 0 && !ui->pttBtn->isDown() &&
            (ui->pttBtn->isCheckable() || !mCall->isPttPressed()))
        {
            ui->pttBtn->setCheckable(false); //reset custom flag
            mCall->doPtt(false); //PTT already released
        }
    }
    else if (mIsGrp && ui->pttBtn != 0)
    {
        ui->joinBtn->show();
        ui->pttBtn->hide();
    }
    if (ui->callBtn != 0)
        ui->callBtn->setEnabled(false);
    QString s((call->getCallingParty() == mId)?
              call->getCalledPartyName(): call->getCallingPartyName());
    ui->callPartyBtn->setText(s);
    int rscType;
    if (ResourceData::isClient(s))
        rscType = CmnTypes::IDTYPE_DISPATCHER;
    else if (call->isOutgoingCall() || (call->isGrpCall() && !mIsGrp) ||
             call->getCallingParty() == mId)
        rscType = ResourceData::getType(call->getCalledParty());
    else
        rscType = ResourceData::getType(call->getCallingParty());
    ui->callPartyBtn->setIcon(QtUtils::getRscIcon(rscType));
    ui->callPartyBtn->show();
    setStyle(STYLE_CALL_SETUP);
}

void Contact::callConnect(CallWindow *call)
{
    if (call != 0)
    {
        if (mCall == 0)
        {
            //incoming call answered by this contact
            mCall = call;
            if (ui->callBtn != 0)
                ui->callBtn->setEnabled(false);
            ui->callPartyBtn->setText(call->getCallingPartyName());
            ui->callPartyBtn
              ->setIcon(QtUtils::getRscIcon(
                               ResourceData::getType(call->getCallingParty())));
            ui->callPartyBtn->show();
        }
        else if (call->getCallingParty() == mId)
        {
            //call from this contact - may be to dispatcher and answered by
            //another client, so update called party just in case
            ui->callPartyBtn->setText(call->getCalledPartyName());
        }
    }
    mCallTime.setHMS(0, 0, 0);
    mCallTimer.start();
    ui->timeIconLbl->show();
    ui->timeLbl->setText("00:00");
    ui->timeLbl->show();
    if (mIsGrp)
        pttUpdate();
    if (mCall != 0 && mCall->isOutgoingCall())
        setStyle(STYLE_CALL_ACTIVE);
    else if (mStyle != STYLE_CALL_ACTIVE)
        setStyle(STYLE_CALL_CONNECT);
}

void Contact::callInclude()
{
    if (ui->pttBtn != 0)
    {
        ui->joinBtn->hide();
        ui->pttBtn->show();
        pttUpdate();
    }
    if (ui->endBtn != 0)
        ui->endBtn->setEnabled(true);
    setStyle(STYLE_CALL_ACTIVE);
}

void Contact::pttUpdate()
{
    if (mCall == 0)
        return;
    QString tx;
    QIcon pttIcon;
    if (ui->pttBtn != 0)
    {
        ui->pttBtn->setEnabled(mCall->getPttInfo(tx, pttIcon));
        ui->pttBtn->setIcon(pttIcon);
    }
    if (ui->txLbl != 0)
    {
        if (tx.isEmpty())
        {
            ui->txIconLbl->hide();
            ui->txLbl->hide();
        }
        else
        {
            ui->txIconLbl->setPixmap(
                    QPixmap(QtUtils::getRscIconSrc((ResourceData::isClient(tx))?
                                                   CmnTypes::IDTYPE_DISPATCHER:
                                                   ResourceData::getType(tx))));
            ui->txLbl->setText(tx);
            ui->txIconLbl->show();
            ui->txLbl->show();
        }
    }
}

void Contact::callRelease(bool ssic, CallWindow *call)
{
    if (call != 0 && mCall != 0 && call != mCall)
        return;
    if (ui->endBtn != 0)
        ui->endBtn->setEnabled(false);
    if (ssic)
    {
        if (ui->pttBtn != 0)
        {
            ui->joinBtn->show();
            ui->pttBtn->hide();
        }
        setStyle(STYLE_CALL_CONNECT);
        return;
    }
    mCall = 0;
    mCallTimer.stop();
    ui->callPartyBtn->hide();
    if (ui->txLbl != 0)
    {
        ui->txIconLbl->hide();
        ui->txLbl->hide();
    }
    ui->timeIconLbl->hide();
    ui->timeLbl->hide();
    if (ui->callBtn != 0)
        ui->callBtn->setEnabled(true);
    if (ui->pttBtn != 0)
    {
        ui->pttBtn->setCheckable(false); //custom flag fail-safe reset
        ui->pttBtn->setStyleSheet(mSsIdlePttBtn);
        ui->pttBtn->setIcon(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_PTT));
        ui->joinBtn->hide();
        ui->pttBtn->show();
    }
    setStyle(STYLE_IDLE);
}

void Contact::setTheme()
{
    sTabWidget->setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    for (auto &it : sContactMap)
    {
        it.second->setStyle();
    }
}

QWidget *Contact::init(Logger *logger, Resources *rscs, QObject *parent)
{
    assert(logger != 0 && rscs != 0);
    sLogger = logger;
    sResources = rscs;
    sEventFilter = parent;
    sTabWidget = new QTabWidget(qobject_cast<QWidget *>(parent));
    sTabWidget->setMovable(true);
    sTabWidget->setIconSize(QSize(28, 28));
    sTabWidget->setWindowTitle(tr("Quick Contacts"));
    sTabWidget->setDocumentMode(true); //ensure background color beyond tabs
    setTheme();
    auto *tBar = sTabWidget->tabBar();
    tBar->setAcceptDrops(true);
    tBar->installEventFilter(sEventFilter);
    auto *tb = new QToolButton();
    tb->setText("+");
    sTabWidget->addTab(new QLabel(), "");
    sTabWidget->setTabEnabled(0, false);
    tBar->setTabButton(0, QTabBar::RightSide, tb);
    connect(tb, &QToolButton::clicked, [] { tabName(); });
    connect(sTabWidget, &QTabWidget::tabBarDoubleClicked,
            [](int index)
            {
                if (qobject_cast<QScrollArea *>(sTabWidget->widget(index)) != 0)
                    tabName(index);
            });
    tBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tBar, &QTabBar::customContextMenuRequested, parent,
            [](const QPoint &pos)
            {
                auto *tab = qobject_cast<QScrollArea *>(
                          sTabWidget->widget(sTabWidget->tabBar()->tabAt(pos)));
                if (tab == 0)
                    return; //no menu for '+' tab
                QMenu menu(sTabWidget);
                TabData *data = &sTabMap[tab];
                if (!data->ctcList.isEmpty() && !data->sorted)
                    QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_SORT);
                QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_RENAME);
                if (data->usr)
                {
                    if (!data->ctcList.isEmpty())
                    {
                        //casting just for compiler
                        if (static_cast<int>(data->selection.size()) !=
                            data->ctcList.size())
                            QtUtils::addMenuAction(menu,
                                               CmnTypes::ACTIONTYPE_SELECT_ALL);
                        if (!data->selection.empty())
                        {
                            auto *m = menu.addMenu(tr("Selection"));
                            QtUtils::addMenuAction(*m,
                                              CmnTypes::ACTIONTYPE_SELECT_NONE);
                            menuMoveToTab(*m);
                            QtUtils::addMenuAction(*m,
                                               CmnTypes::ACTIONTYPE_DELETE_SEL);
                        }
                    }
                    QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_DELETE);
                }
                auto *act = menu.exec(sTabWidget->tabBar()->mapToGlobal(pos));
                if (act == 0)
                    return;
                switch (act->data().toInt())
                {
                    case CmnTypes::ACTIONTYPE_DELETE:
                        tabDelete(tab, true);
                        break;
                    case CmnTypes::ACTIONTYPE_DELETE_SEL:
                        tabDelete(tab, false);
                        break;
                    case CmnTypes::ACTIONTYPE_RENAME:
                        tabName(sTabWidget->indexOf(tab));
                        break;
                    case CmnTypes::ACTIONTYPE_SELECT_ALL:
                        for (auto &c : data->ctcList)
                        {
                            if (data->selection.count(c) == 0)
                                c->select(true);
                        }
                        break;
                    case CmnTypes::ACTIONTYPE_SELECT_NONE:
                        for (auto &c : data->ctcList)
                        {
                            if (data->selection.count(c) != 0)
                                c->select(false);
                        }
                        break;
                    case CmnTypes::ACTIONTYPE_SORT:
                        data->sorted = true;
                        tabLayout(tab);
                        break;
                    default:
                        //move selection to tab
                        tabMoveItems(sTabWidget
                                        ->findChild<QScrollArea *>(act->text()),
                                     tab);
                        break;
                }
            });
    return sTabWidget;
}

void Contact::setSession(ServerSession *session, bool doSave, bool onLogout)
{
    if (session == 0)
    {
        if (sSession != 0 && doSave)
            dataSave(1, 0);
        if (onLogout)
        {
            for (auto &it : sTabMap)
            {
                delete it.first;
            }
            sTabMap.clear();
            sContactMap.clear();
        }
        sMonCountMap.clear();
    }
    sSession = session;
    if (!onLogout)
    {
        //update all contacts
        for (auto &it : sContactMap)
        {
            it.second->setOnline();
        }
    }
}

bool Contact::add(ContactsT     &ctcs,
                  int            type,
                  bool           usr,
                  const MonMapT &monMap,
                  const QString &tabName)
{
    if (!usr && tabName.isEmpty())
    {
        LOGGER_ERROR(sLogger, LOGPREFIX << "add: Missing name for system tab");
        return false;
    }
    //for MOBILE, use correct type to check online status
    int altType = (type == ResourceData::TYPE_MOBILE)?
                  ResourceData::onlineMobileType(): type;
    int idx = sTabWidget->currentIndex();
    int tp;
    ServerSession::SsiSetT monIds;
    Contact *ctc;
    for (const auto &it : monMap)
    {
        ctc = get(it.first);
        if (ctc == 0)
        {
            if (type == ResourceData::TYPE_SUBS_OR_MOBILE)
            {
                tp = ResourceData::getType(it.first);
                altType = (tp == ResourceData::TYPE_MOBILE)?
                          ResourceData::onlineMobileType(): tp;
            }
            else
            {
                tp = type;
            }
            tabAddItem(tabName, tp, it.first, usr,
                       !ResourceData::hasId(altType, it.first), usr);
            ctcs.insert(get(it.first));
            if (it.second)
                monIds.insert(it.first);
        }
        else
        {
            if (it.second && !ctc->monitored())
                monIds.insert(it.first); //exists but not monitored
            if (!usr && sTabMap[ctc->getTab()].usr)
            {
                //contact exists as user-defined - move and change
                tabAddItem(tabGet(usr, tabName), ctc, false);
                ctc->setSysDefined();
            }
        }
    }
    if (!usr)
        sTabWidget->setCurrentIndex(idx);
    if (!monIds.empty())
        monitor(ResourceData::isGrp(type), true, monIds);
    return !ctcs.empty();
}

bool Contact::dataRestore(ContactsT &ctcs)
{
    ResourceData::IdsT monGssis;
    ResourceData::IdsT monIssis;
    if (!sContactMap.empty())
    {
        //either disconnected or updated subs data - restore monitoring
        int tp;
        for (const auto &it : sContactMap)
        {
            tp = ResourceData::getType(it.first);
            if (tp == ResourceData::TYPE_UNKNOWN)
            {
                it.second->activate(false); //id no longer valid
            }
            else
            {
                //if updated subs data, this could be a re-activated id
                it.second->activate(true);
                if (it.second->mMonitored)
                {
                    if (ResourceData::isGrp(tp))
                        monGssis.insert(it.first);
                    else
                        monIssis.insert(it.first);
                }
            }
        }
        //if updated subs data, must clear before re-adding,
        //if disconnected, already cleared
        sMonCountMap.clear();
    }
#ifndef NO_DB
    else
    {
        int id;
        int type; //saved type
        int tp;   //checked type
        int mon;
        string str;
        ResourceData::IdsT invIds;
        Settings &cfg(Settings::instance());
        bool doMon = cfg.get<bool>(Props::FLD_CFG_MONITOR_RETAIN);
#ifdef SAVE2CFG
        vector<string> data;
        if (cfg.getList(Props::FLD_CFG_CONTACTS, data, CFGDELIM) != 0)
        {
            LOGGER_DEBUG(sLogger, LOGPREFIX << "dataRestore:\n"
                         << Utils::toString(data, CFGDELIM));
            bool emptyTab = false;
            QString tabTxt(TABNAMEDEFAULT);
            istringstream is;
            for (auto &s : data)
            {
                if (Utils::trim(s) == 0)
                    continue;
                is.clear();
                is.str(s);
                if (s.find(VALDELIM) == string::npos)
                {
                    if (is >> str)
                    {
                        if (emptyTab)
                            (void) tabGet(true, tabTxt); //create it
                        tabTxt = QString::fromStdString(str);
                        emptyTab = true;
                    }
                }
                else if (is >> id >> type >> mon)
                {
                    tp = ResourceData::getType(id);
                    if (tp == ResourceData::TYPE_UNKNOWN)
                        invIds.insert(id);
                    else if (tp != type)
                        type = tp; //correct the type
                    if (tabAddItem(tabTxt, type, id, true,
                                   (tp == ResourceData::TYPE_UNKNOWN), false))
                    {
                        emptyTab = false;
                        ctcs.insert(get(id));
                        //no notes in cfg
                        if (doMon && mon != 0)
                        {
                            if (ResourceData::isGrp(type))
                                monGssis.insert(id);
                            else
                                monIssis.insert(id);
                        }
                    }
                }
            }
            //check for last empty tab entry
            if (emptyTab)
                (void) tabGet(true, tabTxt);
        }
#else
        auto *res = DbInt::instance().getContacts(sUsername);
        if (res != 0)
        {
            Contact *ctc;
            QString txt;
            int i = 0;
            int n = res->getNumRows();
            for (; i<n; ++i)
            {
                if (!res->getFieldValue(DbInt::FIELD_NAME, str, i) ||
                    !res->getFieldValue(DbInt::FIELD_SSI, id, i))
                    continue;
                if (id == 0)
                {
                    //create tab - it may be empty
                    (void) tabGet(true, QString::fromStdString(str));
                    continue;
                }
                if (!res->getFieldValue(DbInt::FIELD_TYPE, type, i) ||
                    !res->getFieldValue(DbInt::FIELD_MON, mon, i))
                    continue;
                tp = ResourceData::getType(id);
                if (tp == ResourceData::TYPE_UNKNOWN)
                    invIds.insert(id);
                else if (tp != type)
                    type = tp; //correct the type
                if (tp == ResourceData::TYPE_MOBILE &&
                    !ResourceData::hasId(ResourceData::onlineMobileType(), id))
                    tp = ResourceData::TYPE_UNKNOWN; //just to show offline
                txt = QString::fromStdString(str); //tab text
                if (!tabAddItem(txt, type, id, true,
                                (tp == ResourceData::TYPE_UNKNOWN), false))
                    continue;
                ctc = get(id);
                ctcs.insert(ctc);
                if (res->getFieldValue(DbInt::FIELD_NOTES, str, i) &&
                    !str.empty())
                {
                    txt = QString::fromStdString(str);
                    ctc->setNotes(txt);
                }
                if (doMon && mon != 0)
                {
                    if (ResourceData::isGrp(type))
                        monGssis.insert(id);
                    else
                        monIssis.insert(id);
                }
            }
            delete res;
        }
        else
        {
            MessageDialog::showStdMessage(tr("Contact Error"),
                                          tr("Failed to load user contacts "
                                             "because of database link error."),
                                          QMessageBox::Critical, false,
                                          sTabWidget);
        }
#endif //SAVE2CFG
        if (!invIds.empty())
            LOGGER_WARNING(sLogger, LOGPREFIX << "dataRestore: "
                           << "IDs no longer valid (deactivated): "
                           << Utils::toString(invIds, ","));
        if (sTabWidget->count() > 1)
            sTabWidget->setCurrentIndex(0);
    }
#endif //!NO_DB
    if (!monGssis.empty())
        monitor(true, true, monGssis);
    if (!monIssis.empty())
        monitor(false, true, monIssis);
    if (ctcs.empty())
        return false;
    //re-layout all as sorted
    for (auto &it : sTabMap)
    {
        it.second.sorted = true;
    }
    tabLayout();
    return true;
}

Contact *Contact::get(int id)
{
    return (sContactMap.count(id) == 0)? 0: sContactMap[id];
}

bool Contact::getAll(ContactsT &ctcs, int id1, int id2)
{
    if (sContactMap.count(id1) != 0 && sContactMap[id1]->mActive)
        ctcs.insert(sContactMap[id1]);
    if (id2 > 0 && sContactMap.count(id2) != 0 && sContactMap[id2]->mActive)
        ctcs.insert(sContactMap[id2]);
    return !ctcs.empty();
}

bool Contact::getAll(Contact::ContactsT &ctcs, CallWindow *call)
{
    assert(call != 0);
    for (const auto &it : sContactMap)
    {
        if (it.second->hasCall(call))
            ctcs.insert(it.second);
    }
    return !ctcs.empty();
}

void Contact::activate(int id, bool active)
{
    auto *ctc = get(id);
    if (ctc != 0)
        ctc->activate(active, false);
}

void Contact::refresh(int id)
{
    auto *ctc = get(id);
    if (ctc != 0)
        ctc->rscDspOptChanged(ctc->getType());
}

void Contact::show(int id)
{
    auto *ctc = get(id);
    if (ctc != 0)
    {
        auto *tab = ctc->getTab();
        sTabWidget->setCurrentWidget(tab);
        tab->ensureWidgetVisible(ctc);
    }
}

void Contact::move(Contact *ctc, Contact *dstCtc)
{
    assert(ctc != 0 && dstCtc != 0);
    auto *tab = dstCtc->getTab();
    auto *layout = tab->widget()->layout();
    int pos = layout->indexOf(dstCtc);
    if (pos >= 0)
    {
        auto &d(sTabMap[tab]);
        d.sorted = false;
        d.ctcList.move(layout->indexOf(ctc), pos);
        tabLayout(tab);
    }
}

void Contact::monitor(bool isGrp, bool start, const ResourceData::IdsT &ids)
{
    assert(!ids.empty());
    if (sSession == 0)
        return;
    ResourceData::IdsT s1(ids);
    if (!start)
    {
        sMonCountMap[isGrp] -= s1.size();
        LOGGER_DEBUG(sLogger, LOGPREFIX << "monitor: Monitored "
                     << ((isGrp)? "grps": "subs") << " = "
                     << sMonCountMap[isGrp]);
    }
    else if (sMonMaxMap[isGrp] != 0)
    {
        ResourceData::IdsT s2;
        int n = sMonMaxMap[isGrp] - sMonCountMap[isGrp];
        if (n < static_cast<int>(s1.size())) //casting just for compiler
        {
            if (n <= 0)
            {
                s2 = s1;
                s1.clear();
            }
            else
            {
                auto it = s1.begin();
                while (n-- > 0)
                {
                    ++it;
                }
                while (it != s1.end())
                {
                    s2.insert(*it);
                    it = s1.erase(it);
                }
            }
            Contact *ctc;
            for (auto i : s2)
            {
                ctc = get(i);
                if (ctc != 0)
                    ctc->setMonitored(false);
            }
            MessageDialog::showStdMessage(tr("Monitoring Error"),
                                          tr("%1: Maximum %2 already monitored."
                                             "\nCannot add %3.\n")
                                              .arg((isGrp)? "GSSI": "ISSI")
                                              .arg(sMonMaxMap[isGrp])
                                              .arg(QString::fromStdString(
                                                     Utils::toStringWithRange(
                                                                    s2, ", "))),
                                          QMessageBox::Critical, false,
                                          sTabWidget);
            if (s1.empty())
                return;
        }
    }
    Contact *ctc;
    //set desired monitoring
    for (auto i : s1)
    {
        ctc = get(i);
        if (ctc != 0)
            ctc->mMonitored = start;
    }
    if ((start)? !sSession->monitorStart(s1, isGrp):
                 !sSession->monitorStop(s1, isGrp))
    {
        start = !start;
        for (auto i : s1)
        {
            ctc = get(i);
            if (ctc != 0)
                ctc->setMonitored(start);
        }
        QMessageBox::critical(sTabWidget, tr("Monitoring Error"),
                              ((start)? tr("Failed to stop monitoring: "):
                                        tr("Failed to start monitoring: ")) +
                                  QString::fromStdString(
                                           Utils::toStringWithRange(s1, ", ")));
    }
    else if (!start)
    {
        //action may be from outside Contact
        for (auto i : s1)
        {
            ctc = get(i);
            if (ctc != 0)
                ctc->setMonitored(false);
        }
    }
}

void Contact::monitorRestore()
{
    ResourceData::IdsT monGssis;
    ResourceData::IdsT monIssis;
    for (const auto &it : sContactMap)
    {
        if (it.second->mMonitored && !it.second->monitored())
        {
            if (ResourceData::isGrp(it.second->getType()))
                monGssis.insert(it.first);
            else
                monIssis.insert(it.first);
        }
    }
    if (!monGssis.empty())
        monitor(true, true, monGssis);
    if (!monIssis.empty())
        monitor(false, true, monIssis);
}

bool Contact::monitored(int id)
{
    auto *ctc = get(id);
    return (ctc != 0 && ctc->monitored());
}

void Contact::setGrpActive(const ResourceData::IdsT &ids)
{
    Contact *ctc;
    for (auto i : ids)
    {
        ctc = get(i);
        if (ctc != 0)
            ctc->setGrpActive();
    }
}

void Contact::callEvent(int type, CallWindow *call)
{
    if (call == 0)
    {
        assert("Bad param in Contact::callEvent" == 0);
        return;
    }
    ContactsT ctcs;
    switch (type)
    {
        case MsgSp::Type::CALL_ALERT:
        case MsgSp::Type::CALL_PROCEEDING:
            //outgoing - called ctc only
            if (getAll(ctcs, call->getCalledParty()))
            {
                for (auto &c : ctcs)
                {
                    c->callSetup(call);
                }
            }
            break;
        case MsgSp::Type::CALL_CONNECT:
            //outgoing/incoming - called/caller ctc only
            if (getAll(ctcs, call->getCalledParty()))
            {
                for (auto &c : ctcs)
                {
                    c->callConnect();
                }
            }
            else if (getAll(ctcs, call->getCallingParty()))
            {
                for (auto &c : ctcs)
                {
                    c->callConnect(call);
                }
            }
            break;
        case MsgSp::Type::CALL_RELEASE:
            if (getAll(ctcs, call->getCallingParty(), call->getCalledParty()))
            {
                for (auto &c : ctcs)
                {
                    c->callRelease(false, call);
                }
            }
            break;
        case MsgSp::Type::CALL_SETUP:
            //incoming - caller ctc only
            if (getAll(ctcs, call->getCallingParty()))
            {
                for (auto &c : ctcs)
                {
                    c->callSetup(call);
                }
            }
            break;
        case MsgSp::Type::CALL_TX_CEASED:
        case MsgSp::Type::CALL_TX_GRANTED:
        case MsgSp::Type::MON_TX_CEASED:
        case MsgSp::Type::MON_TX_GRANTED:
            if (getAll(ctcs, call))
            {
                for (auto &c : ctcs)
                {
                    c->pttUpdate();
                }
            }
            break;
        case MsgSp::Type::MON_CONNECT:
            if (getAll(ctcs, call->getCallingParty(), call->getCalledParty()))
            {
                for (auto &c : ctcs)
                {
                    c->callConnect(call);
                }
            }
            break;
        case MsgSp::Type::MON_SETUP:
            if (getAll(ctcs, call->getCallingParty(), call->getCalledParty()))
            {
                for (auto &c : ctcs)
                {
                    c->callSetup(call);
                }
            }
            break;
        case MsgSp::Type::SSIC_INCL:
            if (getAll(ctcs, call))
            {
                for (auto &c : ctcs)
                {
                    c->callInclude();
                }
            }
            break;
        case MsgSp::Type::SSIC_RELEASE:
            if (getAll(ctcs, call))
            {
                for (auto &c : ctcs)
                {
                    c->callRelease(true, call);
                }
            }
            break;
        default:
            break;
    }
}

void Contact::onRscDspOptChanged(int type)
{
    for (auto &it : sContactMap)
    {
        it.second->rscDspOptChanged(type);
    }
    tabLayout(); //re-sort tabs that are already sorted
}

bool Contact::doEventFilter(QWidget   *dst,
                            QEvent    *evt,
                            bool      &complete,
                            ContactsT &ctcs)
{
    complete = false;
    bool onTabBar = (qobject_cast<QTabBar *>(dst) != 0);
    if (evt->type() == QEvent::DragEnter)
    {
        auto *e = static_cast<QDragEnterEvent *>(evt);
        //for the tab bar, need to accept DragEnter to enable DragMove
        if (onTabBar)
            e->acceptProposedAction();
        if (e->mimeData()->hasFormat(Draggable::MIMETYPE) &&
            canDrop(e->source(), dst, e->pos()))
        {
            if (!onTabBar)
                e->acceptProposedAction();
            complete = true;
        }
        return true;
    }
    if (onTabBar && evt->type() == QEvent::DragMove)
    {
        auto *e = static_cast<QDragMoveEvent *>(evt);
        if (canDrop(e->source(), dst, e->pos()))
        {
            e->acceptProposedAction();
            complete = true;
        }
        else
        {
            e->ignore();
        }
        return true;
    }
    if (evt->type() == QEvent::Drop)
    {
        auto *e = static_cast<QDropEvent *>(evt);
        if (!canDrop(e->source(), dst, e->pos()))
            return true;
        auto *srcCtc = qobject_cast<Contact *>(e->source());
        auto *dstCtc = qobject_cast<Contact *>(dst);
        if (srcCtc == 0)
        {
            //dragging resources
            //dropping on a contact actually adds to its tab
            if (dstCtc != 0)
                dst = dstCtc->getTab();
            else if (dst == sTabWidget->tabBar())
                dst = sTabWidget->widget(sTabWidget->tabBar()->tabAt(e->pos()));
            QDataStream ds(e->mimeData()->data(Draggable::MIMETYPE));
            int idType;
            ds >> idType;
            int id;
            Contact::MonMapT idMap;
            while (ds.status() == QDataStream::Ok)
            {
                ds >> id;
                if (id != 0)
                    idMap[id] = false;
            }
            add(ctcs, idType, true, idMap, dst->objectName());
        }
        else if (dstCtc != 0)
        {
            //dragging a contact onto another - move within tab
            auto *tab = dstCtc->getTab();
            auto &d(sTabMap[tab]);
            auto *l = tab->widget()->layout();
            d.ctcList.move(l->indexOf(srcCtc), l->indexOf(dstCtc));
            d.sorted = false;
            tabLayout(tab);
        }
        else if (dst == sTabWidget->tabBar())
        {
            //dragging a contact onto tab bar - move to tab
            dst = sTabWidget->widget(sTabWidget->tabBar()->tabAt(e->pos()));
            tabAddItem(qobject_cast<QScrollArea *>(dst), srcCtc, false);
        }
        complete = true;
        return true;
    }
    return false;
}

void Contact::mousePressEvent(QMouseEvent *event)
{
    handleMousePress(event);
}

void Contact::mouseMoveEvent(QMouseEvent *event)
{
    if (handleMouseMove(event))
    {
        appendData(mId);
        startDrag(this);
    }
}

Contact::Contact(int type, int id, bool usr, QScrollArea *tab) :
QWidget(tab), Draggable(type), ui(new Ui::Contact), mCall(0), mId(id),
mStyle(STYLE_IDLE), mActive(true), mIsGrp(ResourceData::isGrp(type)),
mMonitored(false)
{
    mLogPrefix.assign("Contact[").append(Utils::toString(mId)).append("]:: ");
    ui->setupUi(this);
    ui->mainLayout->setAlignment(Qt::AlignTop);
    connect(ui->selectCb, &QCheckBox::toggled, this,
            [this](bool checked)
            {
                //add/remove this from the tab selection
                auto *tab = getTab();
                auto &sel(sTabMap[tab].selection);
                if (checked)
                    sel.insert(this);
                else
                    sel.erase(this);
                sTabWidget->setTabToolTip(sTabWidget->indexOf(tab),
                          (sel.empty())? "": tr("%1 selected").arg(sel.size()));
            });
    ui->iconLbl->setPixmap(QPixmap(QtUtils::getRscIconSrc(type)));
    setStyle(STYLE_IDLE);
    rscDspOptChanged(mType);
    ui->userDefBtn->setChecked(usr);
    if (usr)
        ui->userDefBtn->hide();
    else
        setSysDefined();
    setMonitored(false);
    if (!ResourceData::hasId(mType, mId))
        activate(false, false);
    bool canLocate = false;
    switch (mType)
    {
        case ResourceData::TYPE_DISPATCHER:
            delete ui->monBtn; //cannot be monitored
            ui->monBtn = 0;
            break;
        case ResourceData::TYPE_MOBILE:
            if (SubsData::getIssiType(mId) == SubsData::TERMINALTYPE_BWC)
            {
                delete ui->callBtn;
                ui->callBtn = 0;
                delete ui->msgBtn;
                ui->msgBtn = 0;
                delete ui->endBtn;
                ui->endBtn = 0;
            }
            //fallthrough
        case ResourceData::TYPE_SUBSCRIBER:
            canLocate = true;
            break;
        case ResourceData::TYPE_DGNA_GRP:
        case ResourceData::TYPE_DGNA_IND:
        case ResourceData::TYPE_GROUP:
        default:
            break; //do nothing
    }
    if (ui->endBtn != 0)
    {
        connect(ui->endBtn, &QToolButton::clicked, this,
                [this]
                {
                    mCall->end(true); //either end call or leave grp call
                    ui->endBtn->setEnabled(false);
                });
    }
    if (mIsGrp)
    {
        delete ui->callBtn;
        ui->callBtn = 0;
        setGrpActive();
        connect(ui->joinBtn, &QPushButton::clicked, this,
                [this]
                {
                    //join group call
                    if (mCall != 0)
                        mCall->startCall();
                });
        mSsIdlePttBtn = ui->pttBtn->styleSheet();
        mPttTimer.setSingleShot(true);
        connect(&mPttTimer, &QTimer::timeout, this,
                [this]
                {
                    if (mCall != 0)
                        mCall->doPtt(true);
                    else
                        emit newCall(mType, mId, true);
                });
        connect(ui->pttBtn, &QPushButton::pressed, this,
                [this]
                {
                    mPttTimer.start(150); //delay action (ms) until confirmed
                });
        connect(ui->pttBtn, &QPushButton::released, this,
                [this]
                {
                    if (mPttTimer.isActive())
                        mPttTimer.stop(); //pressed too briefly - do nothing
                    else if (mCall != 0)
                        mCall->doPtt(false);
                    else
                        ui->pttBtn->setCheckable(true); //custom flag
                });
    }
    else
    {
        delete ui->joinBtn;
        ui->joinBtn = 0;
        delete ui->pttBtn;
        ui->pttBtn = 0;
        delete ui->txLbl;
        ui->txLbl = 0;
        delete ui->txIconLbl;
        ui->txIconLbl = 0;
        if (ui->callBtn != 0)
        {
            connect(ui->callBtn, &QPushButton::clicked, this,
                    [this]
                    {
                        //start individual call
                        if (mCall != 0)
                            mCall->doShow();
                        else
                            emit newCall(mType, mId, true);
                    });
        }
    }
    if (ui->msgBtn != 0)
    {
        connect(ui->msgBtn, &QPushButton::clicked, this,
                [this]
                {
                    //add ID to message recipients list and switch to Message
                    emit startAction(CmnTypes::ACTIONTYPE_SDS, mType,
                                     ResourceData::IdsT({mId}));
                });
    }
    connect(ui->callPartyBtn, &QPushButton::clicked, this,
            [this]
            {
                if (mCall != 0)
                    mCall->show();
            });
    mCallTimer.setInterval(1000);
    connect(&mCallTimer, &QTimer::timeout, this,
            [this]
            {
                mCallTime = mCallTime.addSecs(1);
                ui->timeLbl->setText(mCallTime.toString((mCallTime.hour() > 0)?
                                                        "h:mm:ss": "mm:ss"));
            });
    if (ui->monBtn != 0)
    {
        connect(ui->monBtn, &QToolButton::clicked, this,
                [this]
                {
                    if (!sysDefined())
                    {
                        //toggle monitoring
                        monitor(mIsGrp, ui->monBtn->isChecked(),
                                ResourceData::IdsT({mId}));
                        if (!ui->monBtn->isChecked())
                            setMonitored(false);
                    }
                    else
                    {
                        ui->monBtn->setChecked(true);
                    }
                });
    }
    //menuBtn menu
    auto *menu = new QMenu(this);
    QAction *act = QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_CALL);
    mActiveActions.insert(act);
    connect(act, &QAction::triggered, this,
            [this]
            {
                //open CallWindow without starting call
                emit startAction(CmnTypes::ACTIONTYPE_CALL, mType,
                                 ResourceData::IdsT({mId}));
            });
    if (mIsGrp && mType != ResourceData::TYPE_DGNA_GRP)
    {
        act = QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_GRP_ATT);
        mActiveActions.insert(act);
        connect(act, &QAction::triggered, this,
                [this] { sResources->showGrpAttachedMembers(mId, this); });
    }
    if (mType == ResourceData::TYPE_DGNA_GRP ||
        mType == ResourceData::TYPE_DGNA_IND)
    {
        act = QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_DGNA_ASSG);
        mActiveActions.insert(act);
        connect(act, &QAction::triggered, this,
                [this] { sResources->selectedDgna(mType, mId); });
    }
    if (canLocate)
    {
        //cannot use QtUtils::addMenuAction() here because ACTIONTYPE_LOCATE is
        //ignored if GisWindow not open
        act = menu->addAction(QtUtils::getActionIcon(
                                                   CmnTypes::ACTIONTYPE_LOCATE),
                              tr("Locate on map"));
        mActiveActions.insert(act);
        connect(act, &QAction::triggered, this,
                [act, this]
                {
                    if (QtUtils::isGisOpen())
                        emit startAction(CmnTypes::ACTIONTYPE_LOCATE, mType,
                                         ResourceData::IdsT({mId}));
                    else
                        QMessageBox::critical(this, act->text(),
                                              tr("Map Window not yet loaded."));
                });
    }
    connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_NOTES),
            &QAction::triggered, this, [this] { showNotes(); });
    connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_COPY,
                                   tr("Copy name")),
            &QAction::triggered, this,
            [this] { QtUtils::copyToClipboard(getName()); });
    if (usr)
    {
        connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_DELETE),
                &QAction::triggered, this,
                [this]
                {
                    if (QMessageBox::question(this, tr("Delete Contact"),
                                              tr("Are you sure to delete %1?")
                                                  .arg(getName())) ==
                        QMessageBox::Yes)
                    {
                        auto *myTab = getTab();
                        auto &data(sTabMap[myTab]);
                        data.ctcList.removeOne(this);
                        if (data.usr)
                            dataSave(-1, this); //must do before removeWidget()
                        myTab->widget()->layout()->removeWidget(this);
                        sContactMap.erase(getId());
                        activate(false, true);
                        //cannot just delete because triggered by itself
                        deleteLater();
                    }
                });
        //create submenu to move to other tabs on every menu invocation because
        //tabs may change at any time,
        //and a usr Contact may be moved automatically to system tab, so must
        //check again before each action below
        connect(menu, &QMenu::aboutToShow, this,
                [this]
                {
                    if (!sysDefined())
                        menuMoveToTab(*ui->menuBtn->menu(), this);
                });
        connect(menu, &QMenu::aboutToHide, this,
                [this]
                {
                    //remove added submenu
                    if (!sysDefined())
                    {
                        auto *m = ui->menuBtn->menu();
                        for (auto *sm : m->findChildren<QMenu *>())
                        {
                            m->removeAction(sm->menuAction());
                        }
                    }
                });
    }
    ui->menuBtn->setMenu(menu);
}

void Contact::select(bool selected)
{
    if (ui->selectCb != 0)
        ui->selectCb->setChecked(selected);
}

void Contact::activate(bool active, bool onDelete)
{
    if (!active && monitored())
        ui->monBtn->click();
    if (!onDelete)
    {
        mActive = active;
        setOnline();
        for (auto &act : mActiveActions)
        {
            act->setVisible(active);
        }
    }
}

void Contact::setOnline()
{
    bool enabled = (mActive && sSession != 0);
    if (ui->monBtn != 0)
        ui->monBtn->setEnabled(enabled);
    if (ui->callBtn != 0)
        ui->callBtn->setEnabled(enabled);
    if (ui->pttBtn != 0)
        ui->pttBtn->setEnabled(enabled);
    if (ui->msgBtn != 0)
        ui->msgBtn->setEnabled(enabled);
}

void Contact::setSysDefined()
{
    ui->userDefBtn->show();
    //system contact cannot be selected
    delete ui->selectCb;
    ui->selectCb = 0;
    //remove 'delete' menu item when changing an existing contact
    auto *menu = ui->menuBtn->menu();
    if (menu != 0)
    {
        for (auto *act : menu->findChildren<QAction *>())
        {
            if (act->data().toInt() == CmnTypes::ACTIONTYPE_DELETE)
            {
                menu->removeAction(act);
                break;
            }
        }
    }
}

void Contact::rscDspOptChanged(int type)
{
    switch (mType)
    {
        case ResourceData::TYPE_DGNA_GRP:
        case ResourceData::TYPE_DGNA_IND:
        case ResourceData::TYPE_GROUP:
            if (type != mType && type != ResourceData::TYPE_GROUP)
                return;
            break;
        case ResourceData::TYPE_DISPATCHER:
        case ResourceData::TYPE_MOBILE:
        case ResourceData::TYPE_SUBSCRIBER:
            if (type != mType)
                return;
            break;
        default:
            return; //do nothing
    }
    mName = ResourceData::getDspTxt(mId, mType);
    //for dispatcher and mobile, show first word of notes in title
    QString s;
    if (!mNotes.isEmpty() &&
        (mType == ResourceData::TYPE_DISPATCHER ||
         mType == ResourceData::TYPE_MOBILE))
    {
        s = mNotes.section(QRegularExpression("[\\s\n]"), 0, 0);
        if (s.length() > NOTES_MAXWLEN)
            s.replace(NOTES_MAXWLEN - 2, s.length() - NOTES_MAXWLEN + 2, "...");
        ui->titleLbl->setText(mName + " " + s);
    }
    else
    {
        ui->titleLbl->setText(mName);
    }
    //tooltip shows only what's not shown in title
    s = QString::number(mId);
    if (mName.contains(s))
    {
        s = ResourceData::getName(mId, mType);
        if (mName.contains(s))
            s.clear();
    }
    ui->titleLbl->setToolTip(s);
}

void Contact::setStyle(int style)
{
    if (style < 0)
    {
        if (mStyle > STYLE_IDLE)
            return; //set theme-dependent style only when idle
        style = STYLE_IDLE;
    }
    mStyle = style;
    QString ss("background-color:");
    switch (style)
    {
        case STYLE_CALL_ACTIVE:
            ss.append("rgb(84, 151, 251)"); //blue
            break;
        case STYLE_CALL_CONNECT:
            ss.append("rgb(57, 181, 74)"); //green from CallWindow
            break;
        case STYLE_CALL_SETUP:
            ss.append("rgb(255, 210, 0)"); //yellow from CallWindow
            break;
        case STYLE_IDLE:
        default:
            ss.append(Style::getStyle(Style::OBJ_BACKGROUND));
            break;
    }
    ss.append("}");
    ui->mainFrame->setStyleSheet("QFrame#mainFrame{border:1px solid "
                                 "rgb(109, 110, 113)}"
                                 "QFrame {border-radius:8px;" + ss);
}

void Contact::showNotes()
{
    QInputDialog d(this, Qt::WindowTitleHint);
    d.setWindowIcon(QIcon(*ui->iconLbl->pixmap()));
    d.setWindowTitle(getName());
    d.setLabelText(tr("Notes: (max %1 characters)").arg(MAXCHARS));
    d.setOptions(QInputDialog::UsePlainTextEditForTextInput);
    d.setTextValue(mNotes);
    d.setStyleSheet(Style::getStyle(Style::OBJ_INPUTDIALOG));
    while (d.exec() == QDialog::Accepted)
    {
        QString txt(d.textValue().trimmed());
        if (setNotes(txt, true))
            break;
        MessageDialog::showStdMessage(tr("Notes Error"),
                                      tr("Text length %1 exceeds maximum %2")
                                          .arg(txt.length()).arg(MAXCHARS),
                                      QMessageBox::Critical, true, this);
    }
}

QScrollArea *Contact::tabGet(bool usr, const QString &txt)
{
    auto *tab = (txt.isEmpty())? 0: sTabWidget->findChild<QScrollArea *>(txt);
    if (tab != 0)
        return tab;
    if (txt.isEmpty())
    {
        if (!sTabMap.empty())
        {
            //return current tab if of the right type (usr/sys)
            tab = qobject_cast<QScrollArea *>(sTabWidget->currentWidget());
            if (sTabMap[tab].usr == usr)
                return tab;
            //return first tab of the type
            int n = sTabWidget->count();
            for (int i=0; i<n; ++i)
            {
                if (sTabWidget->isTabEnabled(i))
                {
                    tab = qobject_cast<QScrollArea *>(sTabWidget->widget(i));
                    if (sTabMap[tab].usr == usr)
                        return tab;
                }
            }
        }
        if (!usr)
        {
            LOGGER_ERROR(sLogger, LOGPREFIX
                         << "tabGet: Missing name for system tab");
            return 0;
        }
    }
    //create
    auto *w = new QWidget(); //scroll area must have a widget to work
    auto *l = new QVBoxLayout();
    l->setAlignment(Qt::AlignTop);
    l->setMargin(3);
    l->setSpacing(3);
    w->setLayout(l);
    tab = new QScrollArea(sTabWidget);
    tab->setWidget(w);
    //empty txt means creating the first user tab with default name
    tab->setObjectName((txt.isEmpty())? TABNAMEDEFAULT: txt);
    tab->setWidgetResizable(true);
    tab->setAcceptDrops(usr);
    tab->installEventFilter(sEventFilter);
    sTabMap[tab] = TabData(usr);
    //if last tab is '+', insert before it, otherwise insert last
    int i = sTabWidget->count() - 1;
    if (sTabWidget->isTabEnabled(i))
        ++i;
    sTabWidget->insertTab(i, tab,
                          QIcon((usr)? ":/Images/images/icon_contacts_usr.png":
                                       ":/Images/images/icon_contacts_sys.png"),
                          tab->objectName());
    sTabWidget->setCurrentWidget(tab);
    return tab;
}

void Contact::tabName(int idx)
{
    QString old;
    if (idx >= 0)
        old = sTabWidget->tabText(idx);
    bool ok;
    QString s(QInputDialog::getText(sTabWidget,
                                    (idx < 0)? tr("New Tab"): tr("Rename Tab"),
                                    tr("Name:"), QLineEdit::Normal, old, &ok,
                                    Qt::WindowTitleHint).trimmed());
    if (ok && !s.isEmpty() && s != old && tabValidate(s))
    {
        if (idx < 0)
        {
            dataSave(1, 0, tabGet(true, s));
        }
        else
        {
            sTabWidget->setTabText(idx, s);
            auto *tab = qobject_cast<QScrollArea *>(sTabWidget->widget(idx));
            tab->setObjectName(s);
            if (sTabMap[tab].usr)
                dataSave(0, 0, tab, &old);
        }
    }
}

bool Contact::tabValidate(const QString &name)
{
    QString s;
#ifdef SAVE2CFG
    if (name.contains(QRegularExpression(QString("[").append("\\s")
                                                .append(CFGDELIM).append("]"))))
    {
        s = tr("Name must not contain space or '%1'.").arg(CFGDELIM);
    }
#else
    if (name.contains(QRegularExpression(QString("[\\s]"))))
    {
        s = tr("Name must not contain space.");
    }
#endif //SAVE2CFG
    else
    {
        int i = sTabWidget->count() - 1;
        for (; i>=0; --i)
        {
            if (sTabWidget->tabText(i).compare(name, Qt::CaseInsensitive) == 0)
            {
                s = tr("'%1' already exists.").arg(name);
                break;
            }
        }
    }
    if (!s.isEmpty())
    {
        QMessageBox::critical(sTabWidget, tr("Name Error"), s);
        return false;
    }
    return true;
}

bool Contact::tabAddItem(const QString &tabTxt,
                         int            type,
                         int            id,
                         bool           usr,
                         bool           offline,
                         bool           doSave)
{
    bool retVal = false;
    auto *tab = tabGet(usr, tabTxt);
    Contact *ctc = get(id);
    if (ctc == 0)
    {
        LOGGER_DEBUG(sLogger, LOGPREFIX << "tabAddItem(Tab "
                     << tab->objectName().toStdString() << ", " << type << ", "
                     << id << ", " << doSave << ")");
        ctc = new Contact(type, id, usr, tab);
        sContactMap[id] = ctc;
        ctc->setAcceptDrops(true);
        ctc->installEventFilter(sEventFilter);
        if (offline)
            ctc->activate(false);
        retVal = true;
    }
    tabAddItem(tab, ctc, doSave);
    return retVal;
}

void Contact::tabAddItem(QScrollArea *tab, Contact *ctc, bool doSave)
{
    assert(tab != 0 && ctc != 0);
    auto &dstList(sTabMap[tab].ctcList);
    if (dstList.count(ctc) != 0)
        return; //already there
    ctc->select(false);
    dstList << ctc;
    auto *srcTab = ctc->getTab();
    if (srcTab != tab)
        sTabMap[srcTab].ctcList.removeOne(ctc); //from another tab
    tabLayout(tab);
    if (doSave && srcTab == tab && sTabMap[tab].usr)
        dataSave(1, ctc); //new ctc by user onto a user tab
    sTabWidget->setCurrentWidget(tab);
    tab->ensureWidgetVisible(ctc);
}

void Contact::tabMoveItems(QScrollArea *tab, QScrollArea *src)
{
    assert(tab != 0 && src != 0);
    auto &dstList(sTabMap[tab].ctcList);
    //make a copy because select() changes selection in loop
    ContactsT sel(sTabMap[src].selection);
    auto &srcList(sTabMap[src].ctcList);
    for (auto &c : sel)
    {
        c->select(false);
        dstList << c;
        srcList.removeOne(c);
    }
    tabLayout(tab);
}

void Contact::tabLayout(QScrollArea *tab)
{
    int max = 0; //max label text width
    int w;
    QLayout *layout = 0;
    for (auto &it : sTabMap)
    {
        if (tab != 0 && it.first != tab)
            continue;
        //when doing all tabs, do only sorted tabs
        if (it.second.sorted)
            std::sort(it.second.ctcList.begin(), it.second.ctcList.end(),
                      compare);
        else if (tab == 0)
            continue;
        layout = it.first->widget()->layout();
        //add all items from list to layout - display will follow list order
        for (auto &ctc : it.second.ctcList)
        {
            //a widget can only be in one layout, so no risk of duplication here
            layout->addWidget(ctc);
            //determine maximum title width
            w = ctc->ui->titleLbl->sizeHint().width();
            if (w > max)
                max = w;
        }
        //set same max width to all titles for uniform display
        for (auto &ctc : it.second.ctcList)
        {
            ctc->ui->titleLbl->setMinimumWidth(max);
        }
        if (tab != 0)
            break;
    }
}

void Contact::tabDelete(QScrollArea *tab, bool deleteTab)
{
    assert(tab != 0);
    if (QMessageBox::question(sTabWidget,
             (deleteTab)? tr("Delete Tab"): tr("Delete Selection"),
             (deleteTab)? ((sTabMap[tab].ctcList.isEmpty())?
                          tr("Are you sure to delete '%1'?")
                              .arg(tab->objectName()):
                          tr("Are you sure to delete '%1' and its content?")
                              .arg(tab->objectName())):
                         tr("Are you sure to delete selection in '%1'?")
                             .arg(tab->objectName())) ==
        QMessageBox::Yes)
    {
        if (deleteTab)
        {
            for (auto &ctc : sTabMap[tab].ctcList)
            {
                sContactMap.erase(ctc->mId);
                ctc->activate(false, true); //stop monitoring
                delete ctc; //automatically removed from layout
            }
            dataSave(-1, 0, tab); //must be done before erasing tab
            sTabMap.erase(tab);
            delete tab;
            int idx = sTabWidget->currentIndex();
            if (!sTabWidget->isTabEnabled(idx))
            {
                //current tab is '+', choose the next if available
                if (idx < sTabWidget->count() - 1)
                    sTabWidget->setCurrentIndex(++idx);
                else if (idx > 0)
                    sTabWidget->setCurrentIndex(--idx);
            }
        }
        else
        {
            for (auto &ctc : sTabMap[tab].selection)
            {
                sContactMap.erase(ctc->mId);
                sTabMap[tab].ctcList.removeOne(ctc);
                dataSave(-1, ctc);
                ctc->activate(false, true);
                delete ctc;
            }
            sTabMap[tab].selection.clear();
        }
    }
}

bool Contact::menuMoveToTab(QMenu &menu, Contact *ctc)
{
    int n = sTabWidget->count();
    if (n < 3)
        return false; //no tabs other than current and '+'
    int curr = sTabWidget->currentIndex();
    //check for other user tabs for target
    int i = n - 1;
    for (; i>=0; --i)
    {
        if (i != curr && sTabWidget->isTabEnabled(i) &&
            sTabWidget->widget(i)->acceptDrops())
            break;
    }
    if (i < 0)
        return false; //no target tab, so no submenu
    auto *subMenu = menu.addMenu(tr("Move to tab"));
    QAction *act;
    for (i=0; i<n; ++i)
    {
        if (i == curr || !sTabWidget->isTabEnabled(i) ||
            !sTabWidget->widget(i)->acceptDrops())
            continue;
        act = subMenu->addAction(sTabWidget->tabText(i));
        if (ctc == 0)
            act->setData(-1); //just to differentiate from other actions
        else
            connect(act, &QAction::triggered, &menu,
                    [act,ctc]
                    {
                        tabAddItem(
                              sTabWidget->findChild<QScrollArea *>(act->text()),
                              ctc, false);
                    });
    }
    return true;
}

bool Contact::canDrop(QObject *src, QWidget *dst, const QPoint &pos)
{
    assert(src != 0 && dst != 0);
    //a contact may only be dropped on either:
    //-another contact to take its position
    //-a different tab (via the tab bar) to move there
    if (dst == sTabWidget->tabBar())
    {
        dst = sTabWidget->widget(sTabWidget->tabBar()->tabAt(pos));
        if (dst == 0)
            return false;
    }
    if (!dst->acceptDrops())
        return false;
    auto *srcCtc = qobject_cast<Contact *>(src);
    auto *dstCtc = qobject_cast<Contact *>(dst);
    if (srcCtc == 0)
    {
        if (dstCtc == 0 && qobject_cast<QScrollArea *>(dst) == 0)
            return false; //unknown dst
    }
    else if (dstCtc == 0)
    {
        if (srcCtc->sysDefined() || srcCtc->getTab() == dst)
            return false; //system ctc to any tab, or ctc onto its own tab
    }
    else if (srcCtc == dstCtc)
    {
        return false; //ctc on itself
    }
    return true;
}

#ifdef ENABLE_DEBUG_LOG
//convenience function to show DB save operation
static string dbprint(DbInt::RecordT &d)
{
    ostringstream oss;
    for (const auto &it : d)
    {
        switch (it.first)
        {
            case DbInt::FIELD_SSI:
                oss << "SSI";
                break;
            case DbInt::FIELD_TYPE:
                oss << "TYPE";
                break;
            case DbInt::FIELD_MON:
                oss << "MON";
                break;
            case DbInt::FIELD_NAME:
                oss << "NAME";
                break;
            case DbInt::FIELD_NOTES:
                oss << "NOTES";
                break;
            case DbInt::FIELD_USER_NAME:
                oss << "USER_NAME";
                break;
        }
        oss << "(" << it.second << ") ";
    }
    return oss.str();
}
#endif //ENABLE_DEBUG_LOG

void Contact::dataSave(int mode, Contact *ctc, QScrollArea *tab, QString *old)
{
#ifndef NO_DB
    //note: must use DB FIELD_SSI instead of FIELD_ID for contact ID because
    //      FIELD_ID is set as unique
    DbInt &db(DbInt::instance());
    DbInt::RecordT dbKey;
    DbInt::RecordT dbData;
    string tabTxt;
    if (tab == 0 && ctc == 0)
    {
        //save all
        TabData *data;
        ostringstream oss;
        string s;
        ContactsT ctcs; //user contacts in system tab
        int count = sTabWidget->count();
        int i = 0;
#ifdef SAVE2CFG
        //preserve tab order by iterating through sTabWidget
        for (; i<count; ++i)
        {
            if (!sTabWidget->isTabEnabled(i))
                continue;
            tab = qobject_cast<QScrollArea *>(sTabWidget->widget(i));
            data = &sTabMap[tab];
            if (!data->usr)
            {
                //collect user contacts in system tab to save in last user tab
                for (auto &c : data->ctcList)
                {
                    if (c->userDefined())
                        ctcs.insert(c);
                }
                continue;
            }
            tabTxt = tab->objectName().toStdString();
            //tabtxt<CFGDELIM>id type mon<CFGDELIM>
            //id type mon<CFGDELIM>tabtxt<CFGDELIM>...
            oss << tabTxt << CFGDELIM;
            for (auto &c : data->ctcList)
            {
                oss << c->getId() << VALDELIM << c->getType() << VALDELIM
                    << c->monitored() << CFGDELIM;
            }
        }
        if (!ctcs.empty())
        {
            if (tabTxt.empty()) //no user tab, add one
                oss << TABNAMEDEFAULT.toStdString() << CFGDELIM;
            for (auto &c : ctcs)
            {
                oss << c->getId() << VALDELIM << c->getType() << VALDELIM
                    << c->monitored() << CFGDELIM;
            }
        }
        s.assign(oss.str());
        if (!s.empty())
            s.pop_back(); //remove trailing CFGDELIM
#ifdef DEBUG
        else
            return; //do not save empty string
#endif
        Settings::instance().set(Props::FLD_CFG_CONTACTS, s);
        LOGGER_DEBUG(sLogger, LOGPREFIX << "dataSave: " << s);
        oss.str("");
        oss.clear();
        i = 0;
#endif //SAVE2CFG
        for (; i<count; ++i)
        {
            if (!sTabWidget->isTabEnabled(i))
                continue;
            tab = qobject_cast<QScrollArea *>(sTabWidget->widget(i));
            data = &sTabMap[tab];
            if (!data->usr)
            {
                //collect user contacts in system tab to save in last user tab
                for (auto &c : data->ctcList)
                {
                    if (c->userDefined())
                        ctcs.insert(c);
                }
                continue;
            }
            if (data->ctcList.isEmpty())
                continue;
            tabTxt = tab->objectName().toStdString();
            dbData.clear();
            dbKey.clear();
            dbKey[DbInt::FIELD_USER_NAME] = sUsername;
            //to minimize DB transactions, data are not saved when moving
            //items between tabs, so here tab text is in data instead of key
            dbData[DbInt::FIELD_NAME] = tabTxt;
            for (auto &c : data->ctcList)
            {
                dbKey[DbInt::FIELD_SSI] = Utils::toString(c->mId);
                dbData[DbInt::FIELD_MON] = Utils::toString(c->mMonitored);
                //exclude notes because that is saved immediately when changed
                LOGGER_DEBUG(sLogger, LOGPREFIX << "dataSave tab-item " << mode
                             << " updateRecord\ndbKey: " << dbprint(dbKey)
                             << "\ndata: " << dbprint(dbData));
#ifndef SAVE2CFG
                if (!db.updateRecord(DbInt::DB_TABLE_D_CONTACTS, dbKey, dbData))
                    oss << " tab-" << tabTxt << '-' << c->mId;
#endif
            }
        }
        if (!ctcs.empty())
        {
            if (tabTxt.empty()) //no user tab, add one
                tabTxt = TABNAMEDEFAULT.toStdString();
            dbKey.clear();
            dbKey[DbInt::FIELD_USER_NAME] = sUsername;
            dbData[DbInt::FIELD_NAME] = tabTxt;
            for (auto &c : ctcs)
            {
                dbKey[DbInt::FIELD_SSI] = Utils::toString(c->mId);
                dbData[DbInt::FIELD_MON] = Utils::toString(c->mMonitored);
                //changed notes not saved from system tab - include here
                dbData[DbInt::FIELD_NOTES] = c->mNotes.toStdString();
                LOGGER_DEBUG(sLogger, LOGPREFIX << "dataSave notes " << mode
                             << " updateRecord (sys)\ndbKey: " << dbprint(dbKey)
                             << "\ndata: " << dbprint(dbData));
#ifndef SAVE2CFG
                if (!db.updateRecord(DbInt::DB_TABLE_D_CONTACTS, dbKey, dbData))
                {
                    //most likely entry not yet created because it was added
                    //directly to system tab - try creating
                    dbKey[DbInt::FIELD_NAME] = tabTxt;
                    dbKey[DbInt::FIELD_TYPE] = Utils::toString(c->getType());
                    dbKey[DbInt::FIELD_MON] = dbData[DbInt::FIELD_MON];
                    dbKey[DbInt::FIELD_NOTES] = dbData[DbInt::FIELD_NOTES];
                    if (!db.insertRecord(DbInt::DB_TABLE_D_CONTACTS, dbKey))
                        oss << " tab-" << tabTxt << '-' << c->mId << "-sys";
                }
#endif
            }
        }
        s = oss.str();
        if (!s.empty())
            LOGGER_ERROR(sLogger, LOGPREFIX << "dataSave: Failed to update:"
                         << s);
        return;
    } //if (tab == 0 && ctc == 0)
    QString err;
    if (tab == 0)
        tab = ctc->getTab();
    //system tab items cannot be saved individually
    assert(sTabMap[tab].usr);
    tabTxt = tab->objectName().toStdString();
    dbKey[DbInt::FIELD_USER_NAME] = sUsername;
    dbKey[DbInt::FIELD_NAME] = tabTxt;
    if (mode > 0)
    {
        //new ctc or tab
        if (ctc != 0)
        {
            dbKey[DbInt::FIELD_SSI] = Utils::toString(ctc->mId);
            dbKey[DbInt::FIELD_TYPE] = Utils::toString(ctc->mType);
        }
        else
        {
            dbKey[DbInt::FIELD_SSI] = "0";
        }
        LOGGER_DEBUG(sLogger, LOGPREFIX << "dataSave new " << mode
                     << " insertRecord\ndbKey: " << dbprint(dbKey));
#ifndef SAVE2CFG
        if (!db.insertRecord(DbInt::DB_TABLE_D_CONTACTS, dbKey))
        {
            if (ctc != 0)
            {
                err = tr("Failed to save '%1' to database because of link "
                         "error.").arg(ctc->mName);
                LOGGER_ERROR(sLogger, LOGPREFIX << "dataSave: "
                             "Failed to save tab-" << tabTxt << '-' << ctc->mId);
            }
            else
            {
                err = tr("Failed to save tab '%1' to database because of link "
                         "error.").arg(tab->objectName());
                LOGGER_ERROR(sLogger, LOGPREFIX << "dataSave: "
                             "Failed to save tab-" << tabTxt);
            }
        }
#endif //SAVE2CFG
    }
    else if (mode < 0)
    {
        //delete ctc or tab
        if (ctc != 0)
            dbKey[DbInt::FIELD_SSI] = Utils::toString(ctc->mId);
        LOGGER_DEBUG(sLogger, LOGPREFIX << "dataSave " << mode
                     << " deleteRecord\ndbKey: " << dbprint(dbKey));
#ifndef SAVE2CFG
        if (!db.deleteRecord(DbInt::DB_TABLE_D_CONTACTS, dbKey))
        {
            if (ctc != 0)
            {
                err = tr("Failed to delete '%1' from database because of link "
                         "error.").arg(ctc->mName);
                LOGGER_ERROR(sLogger, LOGPREFIX << "dataSave: "
                             "Failed to delete tab-" << tabTxt << '-'
                             << ctc->mId);
            }
            else
            {
                err = tr("Failed to delete tab '%1' from database because of "
                         "link error.").arg(tab->objectName());
                LOGGER_ERROR(sLogger, LOGPREFIX << "dataSave: "
                             "Failed to delete tab-" << tabTxt);
            }
        }
#endif //SAVE2CFG
    }
    else if (ctc != 0)
    {
        //update ctc notes
        dbKey[DbInt::FIELD_SSI] = Utils::toString(ctc->mId);
        //might as well save other data too
        dbData[DbInt::FIELD_MON] = Utils::toString(ctc->mMonitored);
        dbData[DbInt::FIELD_NOTES] = ctc->mNotes.toStdString();
        LOGGER_DEBUG(sLogger, LOGPREFIX << "dataSave notes " << mode
                     << " updateRecord\ndbKey: " << dbprint(dbKey) << "\ndata: "
                     << dbprint(dbData));
#ifndef SAVE2CFG
        if (!db.updateRecord(DbInt::DB_TABLE_D_CONTACTS, dbKey, dbData))
        {
            err = tr("Failed to update '%1' notes to database because of link "
                     "error.").arg(ctc->mName);
            LOGGER_ERROR(sLogger, LOGPREFIX << "dataSave: "
                         "Failed to update notes for tab-" << tabTxt << '-'
                         << ctc->mId);
        }
#endif
    }
    else if (old != 0)
    {
        //rename tab
        dbKey[DbInt::FIELD_NAME] = (*old).toStdString();
        dbData[DbInt::FIELD_NAME] = tabTxt;
        LOGGER_DEBUG(sLogger, LOGPREFIX << "dataSave tab-rename " << mode
                     << " updateRecord\ndbKey: " << dbprint(dbKey) << "\ndata: "
                     << dbprint(dbData));
#ifndef SAVE2CFG
        if (!db.updateRecord(DbInt::DB_TABLE_D_CONTACTS, dbKey, dbData))
        {
            err = tr("Failed to rename tab '%1' to '%2' in database because of "
                     "link error.").arg(*old, tab->objectName());
            LOGGER_ERROR(sLogger, LOGPREFIX << "dataSave: Failed to rename tab-"
                         << dbKey[DbInt::FIELD_NAME] << " to " << tabTxt);
        }
#endif
    }
    if (!err.isEmpty())
        QMessageBox::critical(sTabWidget, tr("Contact Error"), err);
#endif //NO_DB
}
