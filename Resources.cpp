/**
 * UI Resources implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Resources.cpp 1885 2024-11-28 08:32:01Z hazim.rujhan $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QWidgetAction>
#include <assert.h>

#include "CmnTypes.h"
#include "Contact.h"
#include "DbInt.h"
#include "FlowLayout.h"
#include "MessageDialog.h"
#include "QtUtils.h"
#include "Style.h"
#include "ui_Resources.h"
#include "Resources.h"

using namespace std;

enum eRadioButtonId
{
    //order must be exactly as in UI
    RB_LIST,
    RB_BUTTONS
};

static const string LOGPREFIX("Resources:: ");

//comparison function compatible with std::sort()
static bool compare(const ResourceButton *b1, const ResourceButton *b2)
{
    static QMap<int, int> typeOrder({{CmnTypes::IDTYPE_SUBSCRIBER, 1},
                                     {CmnTypes::IDTYPE_GROUP,      2},
                                     {CmnTypes::IDTYPE_DGNA_IND,   3},
                                     {CmnTypes::IDTYPE_DGNA_GRP,   4},
                                     {CmnTypes::IDTYPE_MOBILE,     5},
                                     {CmnTypes::IDTYPE_DISPATCHER, 6}});
    int v = typeOrder[b1->getType()] - typeOrder[b2->getType()];
    if (v == 0)
        v = b1->text().compare(b2->text());
    return (v < 0);
}

#define SETTABTXT(tab, tp) \
    do \
    { \
        ui->tabWidget->setTabText(ui->tabWidget->indexOf(tab), \
                                  ResourceData::getTypeTxt(tp)); \
    } \
    while (0)

Resources::Resources(Logger *logger, QWidget *parent) :
QWidget(parent), ui(new Ui::Resources), mLogger(logger), mDgnaMembersModel(0)
{
    ui->setupUi(this);
    SETTABTXT(ui->subsTab, ResourceData::TYPE_SUBSCRIBER);
    SETTABTXT(ui->talkgroupTab, ResourceData::TYPE_GROUP);
    SETTABTXT(ui->dgnaIndTab, ResourceData::TYPE_DGNA_IND);
#ifdef DGNA_OF_GRPS
    SETTABTXT(ui->dgnaGrpTab, ResourceData::TYPE_DGNA_GRP);
#endif
    SETTABTXT(ui->mobileTab, ResourceData::TYPE_MOBILE);
    SETTABTXT(ui->dispatcherTab, ResourceData::TYPE_DISPATCHER);
    connect(ui->searchButton, &QToolButton::clicked, this,
            [this]
            {
                //search for the text entered in searchEdit
                QString s(ui->searchCombo->currentText().trimmed());
                QRegularExpression re(s,
                                      QRegularExpression::CaseInsensitiveOption);
                if (!re.isValid() ||
                    QRegularExpression("^[.*+]+$").match(s).hasMatch())
                {
                    QMessageBox::critical(this, tr("Search Error"),
                                          tr("Invalid search string."));
                    return;
                }
                QtUtils::addToComboBox(s, ui->searchCombo);
                ui->searchItem->setText(s);
                if (!s.startsWith(".*") && !s.startsWith("^"))
                    s.prepend(".*");
                if (!s.endsWith(".*") && !s.endsWith("$"))
                    s.append(".*");
                //must go from right to left
                search(ResourceData::TYPE_DISPATCHER, s);
                search(ResourceData::TYPE_MOBILE, s);
                bool f = ResourceData::isFullMode();
                if (f)
                {
#ifdef DGNA_OF_GRPS
                    search(ResourceData::TYPE_DGNA_GRP, s);
#endif
                    search(ResourceData::TYPE_DGNA_IND, s);
                }
                search(ResourceData::TYPE_GROUP, s);
                if (f)
                    search(ResourceData::TYPE_SUBSCRIBER, s);
            });
    connect(ui->searchCombo, &QComboBox::editTextChanged, this,
            [this](const QString &txt)
            {
                //enable search if input present, else disable
                ui->searchButton->setEnabled(!txt.trimmed().isEmpty());
            });
    connect(ui->viewAllButton, &QPushButton::clicked, this,
            [this]
            {
                //show all data for current tab in list or button view
                int type;
                QWidget *w = ui->tabWidget->currentWidget();
                if (w == ui->subsTab)
                    type = ResourceData::TYPE_SUBSCRIBER;
                else if (w == ui->talkgroupTab)
                    type = ResourceData::TYPE_GROUP;
                else if (w == ui->dgnaIndTab)
                    type = ResourceData::TYPE_DGNA_IND;
#ifdef DGNA_OF_GRPS
                else if (w == ui->dgnaGrpTab)
                    type = ResourceData::TYPE_DGNA_GRP;
#endif
                else if (w == ui->mobileTab)
                    type = ResourceData::TYPE_MOBILE;
                else if (w == ui->dispatcherTab)
                    type = ResourceData::TYPE_DISPATCHER;
                else
                    return; //cannot occur
                if (ui->viewGroup->checkedId() == RB_LIST)
                    showList(type, false);
                else
                    showButtons(type, false);
            });
    connect(ui->tabWidget, &QTabWidget::currentChanged, this,
            [this]
            {
                //enable/disable radio btns and 'View All' btn for current tab
                QWidget *w = ui->tabWidget->currentWidget();
                ui->listView->setEnabled(w == ui->talkgroupTab ||
                                         w == ui->dgnaIndTab   ||
                                         w == ui->dgnaGrpTab   ||
                                         w == ui->mobileTab    ||
                                         w == ui->dispatcherTab);
                ui->buttonsView->setEnabled(ui->listView->isEnabled());
                ui->viewAllButton->setEnabled(w != ui->phonebookTab);
                if (w == ui->subsTab)
                {
                    ui->listView->setChecked(true);
                }
                else if (w == ui->phonebookTab)
                {
                    ui->buttonsView->setChecked(true);
                }
                else
                {
                    if (w == ui->talkgroupTab)
                        w = ui->talkgroupScroll->widget();
                    else if (w == ui->dgnaIndTab)
                        w = ui->dgnaIndScroll->widget();
                    else if (w == ui->dgnaGrpTab)
                        w = ui->dgnaGrpScroll->widget();
                    else if (w == ui->mobileTab)
                        w = ui->mobileScroll->widget();
                    else
                        w = ui->dispScroll->widget();
                    if (qobject_cast<DraggableListView *>(w) != 0)
                        ui->listView->setChecked(true);
                    else
                        ui->buttonsView->setChecked(true);
                }
            });
    setTheme();
    ui->searchCombo->setFocus();
    //invoke searchButton click when Enter pressed in searchCombo
    connect(ui->searchCombo->lineEdit(), &QLineEdit::returnPressed, this,
            [this]
            {
                if (ui->searchButton->isEnabled())
                    ui->searchButton->click();
            });
    ui->viewGroup->setId(ui->listView, RB_LIST);
    ui->viewGroup->setId(ui->buttonsView, RB_BUTTONS);
    ui->listView->setChecked(true);
    ui->listView->setEnabled(false);
    ui->buttonsView->setEnabled(false);
}

Resources::~Resources()
{
    ResourceData::cleanup(true);
    delete mDgnaMembersModel;
    phonebookClear(false);
    delete ui;
}

void Resources::init(bool full)
{
    if (!ui->resultLayout->isEmpty())
        return; //already initialized
    auto *tw = ui->tabWidget;
    if (full)
    {
        addSearchResBtn(ResourceData::TYPE_SUBSCRIBER, ui->subsTab);
        tw->setCurrentWidget(ui->subsTab);
        addSearchResBtn(ResourceData::TYPE_GROUP, ui->talkgroupTab);
        addSearchResBtn(ResourceData::TYPE_DGNA_IND, ui->dgnaIndTab);
#ifdef DGNA_OF_GRPS
        addSearchResBtn(ResourceData::TYPE_DGNA_GRP, ui->dgnaGrpTab);
#else
        tw->removeTab(tw->indexOf(ui->dgnaGrpTab));
#endif
        mDgnaMembersModel = ResourceData::createModel();
    }
    else
    {
        tw->removeTab(tw->indexOf(ui->subsTab));
        tw->removeTab(tw->indexOf(ui->dgnaIndTab));
        tw->removeTab(tw->indexOf(ui->dgnaGrpTab));
        addSearchResBtn(ResourceData::TYPE_GROUP, ui->talkgroupTab);
        tw->setCurrentWidget(ui->talkgroupTab);
    }
    addSearchResBtn(ResourceData::TYPE_MOBILE, ui->mobileTab);
    addSearchResBtn(ResourceData::TYPE_DISPATCHER, ui->dispatcherTab);
    tw->setTabIcon(tw->indexOf(ui->phonebookTab),
                   QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_PB_ADD));
    tw->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tw->tabBar(), &QTabBar::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                QTabWidget *tw = ui->tabWidget;
                QWidget *w = tw->widget(tw->tabBar()->tabAt(pos));
                QScrollArea *sa = 0;
                if (w == ui->subsTab)
                    sa = ui->subsScroll;
                else if (w == ui->talkgroupTab)
                    sa = ui->talkgroupScroll;
                else if (w == ui->dgnaIndTab)
                    sa = ui->dgnaIndScroll;
                else if (w == ui->dgnaGrpTab)
                    sa = ui->dgnaGrpScroll;
                else if (w == ui->mobileTab)
                    sa = ui->mobileScroll;
                else if (w == ui->dispatcherTab)
                    sa = ui->dispScroll;
                else if (w == ui->phonebookTab)
                    sa = ui->phonebookScroll;
                //children() size is at least 1 when the tab is empty
                if (sa == 0 || sa->widget() == 0 ||
                    sa->widget()->children().size() <= 1)
                    return;
                QMenu cmenu(this);
                QtUtils::addMenuAction(cmenu, CmnTypes::ACTIONTYPE_CLEAR);
                if (ResourceData::isFullMode() && w == ui->talkgroupTab &&
                    SubsData::hasGrpUncAttach())
                    QtUtils::addMenuAction(cmenu,
                                        CmnTypes::ACTIONTYPE_CLEAR_UNC_GRP_ATT);
                auto *act = cmenu.exec(tw->tabBar()->mapToGlobal(pos));
                if (act != 0)
                {
                    if (act->data().toInt() ==
                        CmnTypes::ACTIONTYPE_CLEAR_UNC_GRP_ATT)
                    {
                        if (QMessageBox::question(this,
                                      tr("Clear Unconfirmed Group Attachments"),
                                      tr("Are you sure to proceed?")) ==
                            QMessageBox::Yes)
                            SubsData::grpUncDetach();
                    }
                    else if (w != ui->phonebookTab)
                    {
                        delete sa->takeWidget();
                    }
                    else if (QMessageBox::question(this,
                                                tr("Clear Phonebook Content"),
                                                tr("Are you sure to delete "
                                                   "all Phonebook entries?")) ==
                             QMessageBox::Yes)
                    {
                        phonebookClear(true);
                    }
                }
            });
}

void Resources::setTheme()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    ui->searchButton->setStyleSheet(Style::getStyle(Style::OBJ_TOOLBUTTON));
    //ensure clear QToolButton text
    QString s(Style::getStyle(Style::OBJ_RESOURCEBUTTON));
    QToolButton *btn;
    int i = ui->resultLayout->count() - 1;
    for (; i>=0; --i)
    {
        btn = qobject_cast<QToolButton *>(ui->resultLayout->itemAt(i)->widget());
        if (btn != 0)
            btn->setStyleSheet(s);
    }
}

void Resources::loadData(const QString &username)
{
    deactivate(false);
    ResourceData::reload();
    bool sameUser = (username == mUsername);
    if (!sameUser)
    {
        mUsername = username;
        phonebookClear(true); //clear content for a different user
    }
#ifndef NO_DB
    string s(username.toStdString());
    ResourceButton::setUsername(s);
    DbInt::QResult *res = DbInt::instance().getPhonebook(s);
    if (res != 0)
    {
        set<int> validIds;
        QString str;
        string ssi;
        string notes;
        int    id;
        int    type;
        int    i = 0;
        int    count = res->getNumRows();
        for (; i<count; ++i)
        {
            if (res->getFieldValue(DbInt::FIELD_TYPE, type, i) &&
                res->getFieldValue(DbInt::FIELD_SSI, ssi, i) && ssi != "0" &&
                res->getFieldValue(DbInt::FIELD_NOTES, notes, i))
            {
                id = Utils::fromString<int>(ssi);
                if (sameUser)
                    validIds.insert(id);
                str = QString::fromStdString(notes);
                phonebookAddItem(type, id, &str);
            }
        }
        delete res;
        if (sameUser)
            phonebookRemoveItems(validIds); //remove those no longer in DB
    }
    else
    {
        MessageDialog::showStdMessage(tr("Phonebook Error"),
                                      tr("Failed to load user phonebook "
                                         "because of database link error."),
                                      QMessageBox::Critical, false, this);
#else
    {
#endif //!NO_DB
        if (sameUser)
            phonebookUpdate();
    }
    emit sendData(-1);
}

void Resources::reloadData(int type, int id)
{
    ResourceData::reload(type);
    phonebookUpdate(type, id);
    emit sendData(type);
}

void Resources::addRemoveId(bool doAdd, int id, int type)
{
    if (!ResourceData::addRemoveId(doAdd, type, id))
        return;
    //phonebook does not have TYPE_MOBILE_ONLINE
    phonebookUpdate((type == ResourceData::TYPE_MOBILE_ONLINE)?
                        ResourceData::TYPE_MOBILE: type,
                    id, doAdd);
    emit sendData(type);
    //update tab if applicable
    if (doAdd && type != ResourceData::TYPE_MOBILE_ONLINE)
        return; //cannot add item here because tab may be showing search results
    QWidget *w = 0;
    switch (type)
    {
        case ResourceData::TYPE_DGNA_GRP:
            w = ui->dgnaGrpScroll->widget();
            break;
        case ResourceData::TYPE_DGNA_IND:
            w = ui->dgnaIndScroll->widget();
            break;
        case ResourceData::TYPE_DISPATCHER:
            w = ui->dispScroll->widget();
            break;
        case ResourceData::TYPE_GROUP:
            w = ui->talkgroupScroll->widget();
            break;
        case ResourceData::TYPE_MOBILE:
        case ResourceData::TYPE_MOBILE_ONLINE:
            w = ui->mobileScroll->widget();
            break;
        case ResourceData::TYPE_SUBSCRIBER:
            w = ui->subsScroll->widget();
            break;
        default:
            break; //do nothing
    }
    if (w == 0)
        return;
    auto *list = qobject_cast<DraggableListView *>(w);
    if (list != 0)
    {
        //other than TYPE_MOBILE_ONLINE, do nothing if list is showing all
        //items, because it is updated automatically through the model
        if (type != ResourceData::TYPE_MOBILE_ONLINE &&
            list->model() == ResourceData::getModel(type))
            return;
        //find item to remove or update
        auto *itm = ResourceData::getItem(list, id);
        if (itm != 0)
        {
            if (type == ResourceData::TYPE_MOBILE_ONLINE)
            {
                setMobOnlineStatus(itm, (doAdd)? 1: 0);
            }
            else
            {
                //cannot use qobject_cast here
                auto *m = dynamic_cast<ResourceData::ListModel *>(list->model());
                if (m != 0)
                    m->removeId(id);
            }
        }
    }
    else
    {
        //find button to remove or update
        auto *layout(w->layout());
        if (layout == 0)
            return;
        ResourceButton *rb = 0;
        int i = layout->count() - 1;
        for (; i>=0; --i)
        {
            w = layout->itemAt(i)->widget();
            if (w != 0)
            {
                rb = qobject_cast<ResourceButton *>(w);
                if (rb != 0 && rb->getId() == id)
                {
                    if (type == ResourceData::TYPE_MOBILE_ONLINE)
                        rb->setOnline(true, doAdd);
                    else
                        delete rb;
                    break;
                }
            }
        }
    }
}

void Resources::deactivate(bool onLogoutNoExit)
{
    clearTab(ResourceData::TYPE_UNKNOWN); //clear all
    ResourceData::cleanup();
    //clear search
    for (auto &it : mSearchResultMap)
    {
        delete it.second;
    }
    mSearchResultMap.clear();
    ui->searchCombo->clear();
    ui->searchItem->clear();
    QToolButton *btn;
    int i = ui->resultLayout->count() - 1;
    for (; i>=0; --i)
    {
        btn = qobject_cast<QToolButton *>(ui->resultLayout->itemAt(i)->widget());
        if (btn != 0)
        {
            btn->setText("");
            btn->setEnabled(false);
        }
    }
    if (onLogoutNoExit)
    {
        //update all phonebook items with offline status
        for (auto &it : mPhonebookButtons)
        {
            it->setOnline(true, false);
        }
    }
}

void Resources::selectDgnaInd(int gssi)
{
    selectedDgna(ResourceData::TYPE_DGNA_IND, gssi);
}

void Resources::selectedDgna(int type, int gssi)
{
    SubsData::IdSetT ssis;
    mDgnaMembersModel->clear();
    if (type == ResourceData::TYPE_DGNA_IND)
    {
        SubsData::getDgnaAllowedIssis(gssi, ssis);
        mDgnaMembersModel->setType(ResourceData::TYPE_SUBSCRIBER);
    }
    else
    {
        SubsData::getDgnaAllowedGssis(gssi, ssis);
        mDgnaMembersModel->setType(ResourceData::TYPE_GROUP_OR_DGNA);
    }
    if (!ssis.empty())
        ResourceData::addIds(mDgnaMembersModel, ssis);
    LOGGER_DEBUG(mLogger, LOGPREFIX << "selectedDgna: " << gssi << " VPN "
                 << SubsData::getGssiVpn(gssi) << " (" << ssis.size() << ")\n"
                 << ((ssis.size() < 100)?
                     Utils::toString(ssis, ','): "<too many to list>"));
    emit dgnaSelected(gssi, type, mDgnaMembersModel);
}

void Resources::setGrpActive(int gssi)
{
    QWidget *w = 0;
    int type = ResourceData::getType(gssi);
    switch (type)
    {
#ifdef DGNA_OF_GRPS
        case ResourceData::TYPE_DGNA_GRP:
            w = ui->dgnaGrpScroll->widget();
            break;
#endif
        case ResourceData::TYPE_DGNA_IND:
            w = ui->dgnaIndScroll->widget();
            break;
        case ResourceData::TYPE_GROUP:
            w = ui->talkgroupScroll->widget();
            break;
        default:
            return; //do nothing - should not occur
    }
    //it is possible that buttons/list not yet generated
    if (w != 0)
    {
        auto *list = qobject_cast<DraggableListView *>(w);
        if (list != 0)
        {
            //find the list item to change appearance
            auto *p = ResourceData::getItem(list, gssi);
            if (p != 0)
                setGrpStatus(p, gssi);
        }
        else
        {
            //find the button to change appearance
            ResourceButton *rb;
            QLayout *layout(w->layout());
            int i = layout->count() - 1;
            for (; i>=0; --i)
            {
                w = layout->itemAt(i)->widget();
                if (w != 0)
                {
                    rb = qobject_cast<ResourceButton *>(w);
                    if (rb != 0 && rb->getId() == gssi)
                    {
                        rb->setActive();
                        break;
                    }
                }
            }
        }
    }
    auto *btn = phonebookGetItem(type, gssi);
    if (btn != 0)
        btn->setActive();
}

void Resources::showGrpAttachedMembers(int gssi, QWidget *parent)
{
    int type = ResourceData::getType(gssi);
    QString msg(ResourceData::getDspTxt(gssi, type));
    msg.append("\n").append(QtUtils::getTimestamp());
    QString s(QString::fromStdString(
                           SubsData::getGrpAttachedMembers(gssi, false, true)));
    if (s.isEmpty())
        msg.append(tr("\nNo attached members."));
    else
        msg.append(tr("\nAttached members:"));
    if (type == ResourceData::TYPE_UNKNOWN)
        type = ResourceData::TYPE_GROUP; //for getRscIcon()
    if (ResourceData::isFullMode())
    {
        string s2(SubsData::getGrpUncAttach(gssi));
        if (!s2.empty())
            s.append("\n\n").append(tr("Unconfirmed:")).append("\n")
             .append(QString::fromStdString(s2));
    }
    MessageDialog::showMessage(tr("Group Attachments"), msg, s,
                               QtUtils::getRscIcon(type),
                               (parent == 0)? this: parent);
}

void Resources::showEvent(QShowEvent *)
{
    ui->searchCombo->setFocus();
}

inline bool Resources::hasActiveState(int type) const
{
    switch (type)
    {
        case ResourceData::TYPE_DGNA_GRP:
        case ResourceData::TYPE_DGNA_IND:
        case ResourceData::TYPE_GROUP:
            return true;
        default:
            break;
    }
    return false;
}

void Resources::search(int type, const QString &re)
{
    QWidget *w;
    switch (type)
    {
        case ResourceData::TYPE_DGNA_GRP:
            w = ui->dgnaGrpTab;
            break;
        case ResourceData::TYPE_DGNA_IND:
            w = ui->dgnaIndTab;
            break;
        case ResourceData::TYPE_DISPATCHER:
            w = ui->dispatcherTab;
            break;
        case ResourceData::TYPE_GROUP:
            w = ui->talkgroupTab;
            break;
        case ResourceData::TYPE_MOBILE:
            w = ui->mobileTab;
            break;
        case ResourceData::TYPE_SUBSCRIBER:
            w = ui->subsTab;
            break;
        default:
            return; //cannot occur
    }
    auto *mdl = ResourceData::filter(type, re);
    delete mSearchResultMap[type];
    mSearchResultMap[type] = mdl;
    auto *btn = qobject_cast<QToolButton *>(
                 ui->resultLayout->itemAt(ui->tabWidget->indexOf(w))->widget());
    if (mdl->empty())
    {
        btn->setText("");
        btn->setEnabled(false);
    }
    else
    {
        btn->setText(QString::number(mdl->rowCount()));
        btn->setEnabled(true);
        btn->click();
    }
}

void Resources::showSearchResults(int type)
{
    if (type == ResourceData::TYPE_SUBSCRIBER || ui->listView->isChecked())
        showList(type, true);
    else
        showButtons(type, true);
}

int Resources::showList(int type, bool filtered)
{
    auto *mdl = (filtered)? mSearchResultMap[type]: ResourceData::getModel(type);
    if (mdl == 0)
        return 0;
    auto *list = new DraggableListView(type);
    list->setContextMenuPolicy(Qt::CustomContextMenu);
    list->setModel(mdl);
    QScrollArea *sa = 0;
    switch (type)
    {
        case ResourceData::TYPE_DGNA_IND:
            sa = ui->dgnaIndScroll;
            //fallthrough
        case ResourceData::TYPE_DGNA_GRP:
            if (sa == 0)
                sa = ui->dgnaGrpScroll;
            connect(list, &DraggableListView::doubleClicked, this,
                    [this](const QModelIndex &idx)
                    {
                        //emit selected DGNA grp data
                        auto *mdl = ResourceData::model(
                          qobject_cast<DraggableListView *>(QObject::sender()));
                        selectedDgna(mdl->type(),
                                     ResourceData::getItemId(mdl, idx.row()));
                        auto *l = qobject_cast<DraggableListView *>
                                  (QObject::sender());
                        showGrpAttachedMembers(
                                         ResourceData::getItemId(l, idx.row()));
                    });
            break;
        case ResourceData::TYPE_DISPATCHER:
            sa = ui->dispScroll;
            break;
        case ResourceData::TYPE_GROUP:
            sa = ui->talkgroupScroll;
            connect(list, &DraggableListView::doubleClicked, this,
                    [this](const QModelIndex &idx)
                    {
                        //show attached grp members
                        auto *l = qobject_cast<DraggableListView *>
                                  (QObject::sender());
                        showGrpAttachedMembers(
                                         ResourceData::getItemId(l, idx.row()));
                    });
            break;
        case ResourceData::TYPE_MOBILE:
            sa = ui->mobileScroll;
            break;
        case ResourceData::TYPE_SUBSCRIBER:
        default:
            sa = ui->subsScroll;
            break;
    }
    sa->setWidget(list);
    //list already has scrollbar
    sa->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(list, &DraggableListView::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                showContextMenu(
                           qobject_cast<DraggableListView *>(QObject::sender()),
                           0, pos);
            });
    if (type == ResourceData::TYPE_MOBILE)
    {
        if (ResourceData::hasMobileStat())
        {
            //highlight the online ones - reuse 'type'
            for (type=mdl->rowCount()-1; type>=0; --type)
            {
                setMobOnlineStatus(mdl->item(type));
            }
        }
    }
    else if (hasActiveState(type))
    {
        //highlight the active ones
        for (type=mdl->rowCount()-1; type>=0; --type)
        {
            setGrpStatus(mdl->item(type));
        }
    }
    return mdl->rowCount();
}

int Resources::showButtons(int type, bool filtered)
{
    auto *mdl = (filtered)? mSearchResultMap[type]: ResourceData::getModel(type);
    if (mdl == 0)
        return 0;
    auto *widget = new QWidget();
    widget->setLayout(new FlowLayout(5, 4, 4));
    int n = mdl->rowCount();
    for (auto i=0; i<n; ++i)
    {
        createButton(type, ResourceData::getItemId(mdl, i), false, widget);
    }
    QScrollArea *sa;
    switch (type)
    {
        case ResourceData::TYPE_DGNA_GRP:
            sa = ui->dgnaGrpScroll;
            break;
        case ResourceData::TYPE_DGNA_IND:
            sa = ui->dgnaIndScroll;
            break;
        case ResourceData::TYPE_DISPATCHER:
            sa = ui->dispScroll;
            break;
        case ResourceData::TYPE_GROUP:
            sa = ui->talkgroupScroll;
            break;
        case ResourceData::TYPE_MOBILE:
            sa = ui->mobileScroll;
            break;
        case ResourceData::TYPE_SUBSCRIBER:
        default:
            sa = ui->subsScroll;
            break;
    }
    sa->setWidget(widget);
    sa->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    return n;
}

ResourceButton *Resources::createButton(int      type,
                                        int      id,
                                        bool     enableNotes,
                                        QWidget *parent,
                                        QString *notes)
{
    auto *btn = new ResourceButton(id, type, enableNotes, parent);
    switch (type)
    {
        case ResourceData::TYPE_DGNA_GRP:
        case ResourceData::TYPE_DGNA_IND:
            connect(btn, &ResourceButton::clicked, this,
                    [this]
                    {
                        //emit selected DGNA grp data
                        auto *btn = qobject_cast<ResourceButton *>
                                    (QObject::sender());
                        if (btn->isOnline())
                            selectedDgna(btn->getType(), btn->getId());
                    });
            break;
        case ResourceData::TYPE_GROUP:
            connect(btn, &ResourceButton::clicked, this,
                    [this]
                    {
                        //show attached grp members
                        showGrpAttachedMembers(qobject_cast<ResourceButton *>
                                               (QObject::sender())->getId());
                    });
            break;
        case ResourceData::TYPE_MOBILE:
            btn->setOnline();
            break;
        default:
            break;
    }
    if (hasActiveState(type))
        btn->setActive();
    if (enableNotes && (notes != 0) && !notes->isEmpty())
        btn->setNotes(*notes);
    btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(btn, &ResourceButton::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                showContextMenu(0, qobject_cast<ResourceButton *>(sender()),
                                pos);
            });
    if (parent != 0)
        parent->layout()->addWidget(btn);
    return btn;
}

ResourceButton *Resources::phonebookGetItem(int type, int id)
{
    //items are grouped by type - first find the type with an empty loop
    auto it = mPhonebookButtons.begin();
    for (; it!=mPhonebookButtons.end() && (*it)->getType()!=type; ++it);
    for (; it!=mPhonebookButtons.end() && (*it)->getType()==type; ++it)
    {
        if ((*it)->getId() == id)
            return *it;
    }
    return 0;
}

bool Resources::phonebookAddItem(int type, int id, QString *notes)
{
    bool retVal = true;
    auto *btn = phonebookGetItem(type, id);
    if (btn != 0)
    {
        if (notes != 0)
            btn->setNotes(*notes);
        btn->refresh(true, false, false, hasActiveState(type));
        retVal = false;
    }
    else
    {
        btn = createButton(type, id, true, ui->phonebookScroll->widget(), notes);
        btn->setOnline();
        mPhonebookButtons << btn;
    }
    phonebookSort();
    return retVal;
}

void Resources::phonebookAddItems(const DraggableListView *list,
                                  const QModelIndexList   &idxList)
{
    if (idxList.empty())
        return;
    int             type = list->getType();
    int             id;
    ResourceButton *btn;
    QWidget        *widget = ui->phonebookScroll->widget();
    for (const auto &idx : idxList)
    {
        id = ResourceData::getItemId(list, idx.row());
        btn = phonebookGetItem(type, id);
        if (btn == 0)
        {
            btn = createButton(type, id, true, widget);
            btn->phonebookSave();
            mPhonebookButtons << btn;
        }
        btn->setOnline(true, true);
    }
    phonebookSort();
}

void Resources::phonebookRemoveItem(ResourceButton *btn)
{
    mPhonebookButtons.removeOne(btn);
    btn->phonebookDelete();
    delete btn;
}

void Resources::phonebookRemoveItems(const set<int> &ids)
{
    //ids are from DB, so no DB delete needed when deleting entries
    for (auto &it : mPhonebookButtons)
    {
        if (ids.count(it->getId()) == 0)
        {
            delete it;
            it = 0; //for removeAll()
        }
    }
    mPhonebookButtons.removeAll(0);
}

void Resources::phonebookClear(bool doCreateLayout)
{
    mPhonebookButtons.clear(); //buttons destroyed with widget below
    if (doCreateLayout)
    {
        auto *widget = new QWidget();
        widget->setLayout(new FlowLayout(5, 4, 4));
        ui->phonebookScroll->setWidget(widget); //destroys the old widget
    }
    else
    {
        delete ui->phonebookScroll->takeWidget();
    }
}

void Resources::phonebookUpdate(int type, int id, bool online)
{
    if (mPhonebookButtons.isEmpty())
        return;
    if (type == ResourceData::TYPE_UNKNOWN)
    {
        //update all items
        for (auto &it : mPhonebookButtons)
        {
            it->refresh(true, false, false, hasActiveState(it->getType()));
        }
    }
    else if (id > 0)
    {
        //update one item
        auto *btn = phonebookGetItem(type, id);
        if (btn == 0)
            return;
        btn->refresh(true, true, online, hasActiveState(type));
    }
    else
    {
        //update one type - first find the type with an empty loop
        auto it = mPhonebookButtons.begin();
        for (; it!=mPhonebookButtons.end() && (*it)->getType()!=type; ++it);
        if (it == mPhonebookButtons.end())
            return; //type not found
        bool doSetActive = hasActiveState(type);
        for (; it!=mPhonebookButtons.end() && (*it)->getType()==type; ++it)
        {
            (*it)->refresh(true, false, false, doSetActive);
        }
    }
    //sort because positions may have changed due to name change
    phonebookSort();
}

void Resources::phonebookSort()
{
    sort(mPhonebookButtons.begin(), mPhonebookButtons.end(), compare);
    //add all buttons from mPhonebookButtons to layout - the displayed buttons
    //will follow the list order
    auto *layout = ui->phonebookScroll->widget()->layout();
    for (const auto &it : mPhonebookButtons)
    {
        //calling addWidget for a widget already in the layout does not
        //duplicate the widget in the layout
        layout->addWidget(it);
    }
}

void Resources::clearTab(int type)
{
    switch (type)
    {
        case ResourceData::TYPE_SUBSCRIBER:
            delete ui->subsScroll->takeWidget();
            break;
        case ResourceData::TYPE_GROUP:
            delete ui->talkgroupScroll->takeWidget();
            break;
        case ResourceData::TYPE_DGNA_IND:
            delete ui->dgnaIndScroll->takeWidget();
            break;
        case ResourceData::TYPE_DGNA_GRP:
            delete ui->dgnaGrpScroll->takeWidget();
            break;
        case ResourceData::TYPE_MOBILE:
            delete ui->mobileScroll->takeWidget();
            break;
        case ResourceData::TYPE_DISPATCHER:
            delete ui->dispScroll->takeWidget();
            break;
        default:
            delete ui->subsScroll->takeWidget();
            delete ui->talkgroupScroll->takeWidget();
            delete ui->dgnaIndScroll->takeWidget();
            delete ui->dgnaGrpScroll->takeWidget();
            delete ui->mobileScroll->takeWidget();
            delete ui->dispScroll->takeWidget();
            break;
    }
}

void Resources::showContextMenu(DraggableListView *list,
                                ResourceButton    *btn,
                                const QPoint      &pos)
{
    int     type;
    int     id;
    bool    single = true;
    QString idStr;
    QPoint  pt;
    if (list != 0)
    {
        type = list->getType();
        QModelIndex index(list->indexAt(pos));
        if (!index.isValid())
            return;
        pt = list->mapToGlobal(pos);
        id = ResourceData::getItemId(list, index.row());
        idStr = index.data().toString();
        single = (list->selectionModel()->selectedIndexes().count() == 1);
    }
    else if (btn != 0)
    {
        type = btn->getType();
        pt = btn->mapToGlobal(pos);
        id = btn->getId();
        idStr = btn->getDspTxt();
    }
    else
    {
        return;  //should never occur
    }
    QMenu cmenu(this);
    QList<int> actTypes;
    if (btn == 0 || btn->isOnline())
    {
        //for mobiles in list, check for online status
        if (btn != 0 || type != ResourceData::TYPE_MOBILE ||
            ResourceData::hasId(ResourceData::onlineMobileType(), id))
        {
            if (single)
                QtUtils::addMenuAction(cmenu, CmnTypes::ACTIONTYPE_CALL,
                                       tr("Call %1").arg(idStr));
            actTypes << CmnTypes::ACTIONTYPE_SDS << CmnTypes::ACTIONTYPE_STATUS;
        }
        if (type != ResourceData::TYPE_DISPATCHER)
        {
            if (!Contact::monitored(id))
                actTypes << CmnTypes::ACTIONTYPE_MON;
            if (single)
            {
                if (type == ResourceData::TYPE_SUBSCRIBER ||
                    type == ResourceData::TYPE_MOBILE)
                    actTypes << CmnTypes::ACTIONTYPE_LOCATE;
                else if (type == ResourceData::TYPE_GROUP ||
                         type == ResourceData::TYPE_DGNA_IND)
                    actTypes << CmnTypes::ACTIONTYPE_GRP_ATT;
            }
        }
    }
    if (!Contact::hasId(id))
        actTypes << CmnTypes::ACTIONTYPE_CONTACT_ADD;
    if (btn != 0 && btn->parent() == ui->phonebookScroll->widget())
    {
        if (!btn->isOnline())
        {
            QString s(btn->getLastOnlineTime());
            if (!s.isEmpty())
            {
                s.prepend(tr("Last online "));
                auto *lbl = new QLabel(s, this);
                lbl->setStyleSheet(Style::getStyle(Style::OBJ_LABEL));
                auto *wact = new QWidgetAction(&cmenu);
                wact->setDefaultWidget(lbl);
                cmenu.addAction(wact);
            }
        }
        actTypes << CmnTypes::ACTIONTYPE_NOTES << CmnTypes::ACTIONTYPE_PB_DEL;
    }
    else if (!single || phonebookGetItem(type, id) == 0)
    {
        actTypes << CmnTypes::ACTIONTYPE_PB_ADD;
    }
    actTypes << CmnTypes::ACTIONTYPE_COPY;
    QtUtils::addMenuActions(cmenu, actTypes);
    QAction *act = cmenu.exec(pt);
    if (act == 0)
        return;
    int actType = act->data().toInt();
    switch (actType)
    {
        case CmnTypes::ACTIONTYPE_CALL:
        case CmnTypes::ACTIONTYPE_CONTACT_ADD:
        case CmnTypes::ACTIONTYPE_MON:
        case CmnTypes::ACTIONTYPE_MSG:
        case CmnTypes::ACTIONTYPE_SDS:
        case CmnTypes::ACTIONTYPE_STATUS:
        {
            ResourceData::IdsT ids({id});
            if (!single)
            {
                for (auto &idx : list->selectionModel()->selectedIndexes())
                {
                    ids.insert(ResourceData::getItemId(list, idx.row()));
                }
            }
            emit startAction(actType, type, ids);
            break;
        }
        case CmnTypes::ACTIONTYPE_COPY:
        {
            if (single)
                QtUtils::copyToClipboard(idStr);
            else
                QtUtils::copyToClipboard(list);
            break;
        }
        case CmnTypes::ACTIONTYPE_GRP_ATT:
        {
            showGrpAttachedMembers(id);
            break;
        }
        case CmnTypes::ACTIONTYPE_LOCATE:
        {
            emit locateResource(id);
            break;
        }
        case CmnTypes::ACTIONTYPE_NOTES:
        {
            btn->showNotes();
            break;
        }
        case CmnTypes::ACTIONTYPE_PB_ADD:
        {
            if (list != 0)
                phonebookAddItems(list,
                                  list->selectionModel()->selectedIndexes());
            else if (phonebookAddItem(type, btn->getId()))
                btn->phonebookSave();
            break;
        }
        case CmnTypes::ACTIONTYPE_PB_DEL:
        {
            if (QMessageBox::question(this, tr("Delete Phonebook Entry"),
                                      tr("Are you sure to delete %1?")
                                          .arg(btn->getDspTxt())) ==
                QMessageBox::Yes)
                phonebookRemoveItem(btn);
            break;
        }
        default:
        {
            break; //do nothing - should never occur
        }
    } //switch (actType)
}

void Resources::setGrpStatus(QStandardItem *item, int gssi)
{
    assert(item != 0);
    bool isUpdate = (gssi != 0); //true if updating list, false if creating
    if (!isUpdate)
        gssi = ResourceData::getItemId(item);
    switch (SubsData::grpActive(gssi))
    {
        case SubsData::GRP_STAT_ASSIGN:
            item->setBackground(Qt::darkYellow);
            if (isUpdate)
                item->setToolTip("");
            break;
        case SubsData::GRP_STAT_ATTACH:
            //color taken from ResourceButton CmnTypes::IDTYPE_GROUP
            item->setBackground(QColor(117, 209, 129));
            item->setToolTip(QString::fromStdString(
                                 SubsData::getGrpAttachedMembers(gssi, false)));
            break;
        default:
            item->setBackground(Qt::transparent);
            if (isUpdate)
                item->setToolTip("");
            break;
    }
}

void Resources::setMobOnlineStatus(QStandardItem *item, int stat)
{
    //highlight color is from ResourceButton constructor for TYPE_MOBILE
    if (stat == 0)
        item->setBackground(Qt::transparent);
    else if (stat > 0 ||
             ResourceData::hasId(ResourceData::onlineMobileType(),
                                 ResourceData::getItemId(item)))
        item->setBackground(QColor(135, 159, 217));
}

void Resources::addSearchResBtn(int type, QWidget *tab)
{
    auto *btn = new QToolButton(this);
    btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    btn->setIconSize(QSize(30, 30));
    btn->setIcon(QtUtils::getRscIcon(type));
    int i = ui->tabWidget->indexOf(tab);
    btn->setToolTip(ui->tabWidget->tabText(i));
    ui->tabWidget->setTabIcon(i, btn->icon());
    ui->resultLayout->insertWidget(i, btn);
    connect(btn, &QToolButton::clicked, this,
            [type, tab, this]
            {
                showSearchResults(type);
                ui->tabWidget->setCurrentWidget(tab);
            });
}
