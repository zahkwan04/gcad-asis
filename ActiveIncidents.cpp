/**
 * UI Active Incidents implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2020-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ActiveIncidents.cpp 1838 2024-04-03 01:11:56Z rosnin $
 * @author Rosnin Mustaffa
 */
#include <assert.h>
#include <QActionGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QMessageBox>
#include <QWidgetAction>

#include "Settings.h"
#include "Style.h"
#include "ui_ActiveIncidents.h"
#include "ActiveIncidents.h"

using namespace std;

ActiveIncidents::ActiveIncidents(bool showFull, QWidget *parent) :
QWidget(parent), ui(new Ui::ActiveIncidents)
{
    ui->setupUi(this);
    setTheme();
    if (showFull)
    {
        ui->titleLabel->setText(tr("Active Incidents"));
        delete ui->posBtn;
        connect(ui->toggleShowBtn, &QToolButton::clicked, this,
                [this]
                {
                    ui->scrollArea->setHidden(!ui->scrollArea->isHidden());
                });
    }
    else
    {
        ui->titleLabel->setText(tr("Active"));
        delete ui->toggleShowBtn;
        addPositionMenu();
    }
    mFlowLayout = new FlowLayout(ui->scrollWidget, 0, 0, 0);
    auto *menu = new QMenu(this);
    ui->iconBtn->setMenu(menu);
    connect(ui->iconBtn, &QToolButton::clicked, this,
            [this] { ui->iconBtn->showMenu(); });
    auto *act = menu->addAction(tr("Set Custom"));
    menu->setToolTipsVisible(true);
    connect(act, &QAction::triggered, this,
            [this, act]
            {
                Settings &cfg(Settings::instance());
                QString old(QString::fromStdString(
                                  cfg.get<string>(Props::FLD_CFG_INC_ICONDIR)));
                QString dir(QFileDialog::getExistingDirectory(parentWidget(),
                     tr("Select Icon Folder"), old, QFileDialog::ShowDirsOnly));
                if (!dir.isEmpty() && dir != old)
                {
                    QString tooltip;
                    QStringList l(IncidentData::getCategoryList(true));
                    if (IncidentButton::setCustomIcons(l, dir, tooltip))
                    {
                        cfg.set(Props::FLD_CFG_INC_ICONDIR, dir.toStdString());
                        act->setToolTip(tooltip);
                        refreshIcons();
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("Incident Icons"),
                                              tr("No valid icons in\n  %1.\n"
                                                 "Expecting files named %2, "
                                                 "where %3 is one of:\n  %4")
                                                  .arg(dir,
                                                       "'icon_inc_<cat>.png'",
                                                       "<cat>", l.join(", ")));
                    }
                }
            });
    connect(menu->addAction(tr("Reset Default")), &QAction::triggered, this,
            [this, act]
            {
                Settings &cfg(Settings::instance());
                //do nothing if already default
                if (!cfg.get<string>(Props::FLD_CFG_INC_ICONDIR).empty())
                {
                    IncidentButton::setDefaultIcons();
                    cfg.set(Props::FLD_CFG_INC_ICONDIR, "");
                    act->setToolTip("");
                    refreshIcons();
                }
            });
}

ActiveIncidents::~ActiveIncidents()
{
    delete mFlowLayout;
    delete ui;
}

void ActiveIncidents::setTheme()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    ui->titleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    for (auto &it : mIncidentMap)
    {
        it.second->setStyle();
    }
}

void ActiveIncidents::setFilterMenu()
{
    //first, check for custom icons
    QString dir(QString::fromStdString(
                 Settings::instance().get<string>(Props::FLD_CFG_INC_ICONDIR)));
    if (!dir.isEmpty())
    {
        QString tooltip;
        if (IncidentButton::setCustomIcons(IncidentData::getCategoryList(true),
                                           dir, tooltip))
            ui->iconBtn->menu()->actions().front()->setToolTip(tooltip);
    }
    delete ui->filterBtn->menu(); //must delete old menu first
    auto *menu = new QMenu(this);
    ui->filterBtn->setMenu(menu);
    connect(ui->filterBtn, &QToolButton::clicked, this,
            [this] { ui->filterBtn->showMenu(); });
    addFilter(menu, tr("State"), IncidentButton::FIELD_ADDRSTATE);
    addFilter(menu, tr("Category"), IncidentButton::FIELD_CATEGORY);
    addFilter(menu, tr("Status"), IncidentButton::FIELD_STATE);
    addFilter(menu, tr("Priority"), IncidentButton::FIELD_PRIORITY);
    auto *menu2 = menu->addMenu(tr("Address2"));
    auto *cb = new QComboBox(this);
    cb->setMaxCount(20);
    cb->setEditable(true);
    cb->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    auto *wact = new QWidgetAction(this);
    wact->setDefaultWidget(cb);
    menu2->addAction(wact);
    connect(cb, &QComboBox::currentTextChanged, this,
            [cb, this](const QString &txt)
            {
                QString s(txt.trimmed());
                if (!s.isEmpty())
                {
                    int i = cb->findText(s, Qt::MatchFixedString);
                    if (i == 0)
                        return;
                    //s not at the top - either move or add it there
                    cb->insertItem(0, s);
                    cb->setCurrentIndex(0);
                    if (i > 0)
                        cb->removeItem(i + 1);
                    else
                        cb->parentWidget()->setFixedWidth(cb->width() + 10);
                    //if appended a character, remove item(1),
                    //else if removed last character, remove item(1) only if
                    //it is also a substring of item(2) (to preserve the
                    //longest typed string)
                    if ((cb->count() > 1 && s.startsWith(cb->itemText(1))) ||
                        (cb->count() > 2 && cb->itemText(1).startsWith(s) &&
                         cb->itemText(2).startsWith(cb->itemText(1))))
                    {
                        cb->removeItem(1);
                    }
                }
                else if (cb->count() > 1 &&
                         cb->itemText(1).startsWith(cb->itemText(0)))
                {
                    //entire text deleted - remove item(0) which is a
                    //substring of item(1)
                    cb->setCurrentIndex(-1); //ensure box remains empty
                    cb->removeItem(0);
                }
                IncidentButton::updateFilter(IncidentButton::FIELD_ADDR2, s,
                                             true);
                for (auto &it : mIncidentMap)
                {
                    it.second->setFilteredVisible();
                }
    });
}

void ActiveIncidents::clear()
{
    for (auto &it : mIncidentMap)
    {
        delete it.second;
    }
    mIncidentMap.clear();
}

void ActiveIncidents::refreshIcons()
{
    IncidentButton *b;
    int i = mFlowLayout->count() - 1;
    for (; i>=0; --i)
    {
        b = qobject_cast<IncidentButton *>(mFlowLayout->itemAt(i)->widget());
        if (b != 0)
            b->refreshIcon();
    }
}

IncidentData *ActiveIncidents::getData(int id)
{
    if (mIncidentMap.count(id) != 0)
        return mIncidentMap[id]->getData();
    return 0;
}

void ActiveIncidents::updateCase(IncidentData *data)
{
    int id = data->getId();
    if (data->isClosed())
    {
        if (mIncidentMap.count(id) != 0)
        {
            delete mIncidentMap[id];
            mIncidentMap.erase(id);
        }
    }
    else if (mIncidentMap.count(id) == 0)
    {
        auto *btn = new IncidentButton(data);
        connect(btn, &IncidentButton::showData, this,
                [btn, this] { emit showData(btn); });
        mFlowLayout->addWidget(btn);
        mIncidentMap[id] = btn;
        btn->setFilteredVisible();
    }
    else
    {
        auto *btn = mIncidentMap[id];
        btn->updateData(data);
        btn->setFilteredVisible();
    }
}

void ActiveIncidents::closeCase(int id)
{
    if (id != 0 && mIncidentMap.count(id) != 0)
    {
        delete mIncidentMap[id];
        mIncidentMap.erase(id);
    }
}

void ActiveIncidents::setLockHolder(int id, int lockHolder)
{
    if (mIncidentMap.count(id) != 0)
        mIncidentMap[id]->setLockHolder(lockHolder);
}

QList<IncidentData *> ActiveIncidents::getAllData()
{
    QList<IncidentData *> l;
    for (const auto &it : mIncidentMap)
    {
        l << it.second->getData();
    }
    return l;
}

void ActiveIncidents::addPositionMenu()
{
    auto *menu = new QMenu(this);
    auto *grpAct = new QActionGroup(menu);
    auto *act = new QAction(tr("Left"), grpAct);
    act->setCheckable(true);
    act->setChecked(true);
    connect(act, &QAction::triggered, this,
            [this] { emit setPosition(POS_LEFT); });
    menu->addAction(act);
    act = new QAction(tr("Top"), grpAct);
    act->setCheckable(true);
    connect(act, &QAction::triggered, this,
            [this] { emit setPosition(POS_TOP); });
    menu->addAction(act);
    act = new QAction(tr("Right"), grpAct);
    act->setCheckable(true);
    connect(act, &QAction::triggered, this,
            [this] { emit setPosition(POS_RIGHT); });
    menu->addAction(act);
    act = new QAction(tr("Bottom"), grpAct);
    act->setCheckable(true);
    connect(act, &QAction::triggered, this,
            [this] { emit setPosition(POS_BOTTOM); });
    menu->addAction(act);
    ui->posBtn->setMenu(menu);
}

QStringList ActiveIncidents::getFilterData(int field) const
{
    switch (field)
    {
        case IncidentButton::FIELD_ADDRSTATE:
            return IncidentData::getAddrStateList();
        case IncidentButton::FIELD_CATEGORY:
            return IncidentData::getCategoryList();
        case IncidentButton::FIELD_PRIORITY:
            return IncidentData::getPriorityList();
        case IncidentButton::FIELD_STATE:
            return IncidentData::getStateList();
        default:
            break; //do nothing
    }
    return QStringList();
}

void ActiveIncidents::addFilter(QMenu *topMenu, const QString &txt, int field)
{
    auto *menu = topMenu->addMenu(txt);
    auto *vbox = new QVBoxLayout();
    auto *chkAll = new QCheckBox(tr("All"), menu);
    vbox->addWidget(chkAll);
    QCheckBox *chk;
    bool allChecked = true;
    foreach (QString value, getFilterData(field))
    {
        chk = new QCheckBox(value, menu);
        chk->setObjectName(QString::number(field));
        chk->setChecked(IncidentButton::isInFilter(field, value));
        if (allChecked && !chk->isChecked())
            allChecked = false;
        connect(chk, &QCheckBox::toggled, this,
                [chk, this](bool isChecked)
                {
                    IncidentButton::updateFilter(chk->objectName().toInt(),
                                                 chk->text(), isChecked);
                    for (auto &it : mIncidentMap)
                    {
                        it.second->setFilteredVisible();
                    }
                });
        //on chk click, check chkAll if all boxes are checked, and uncheck
        //it if any is unchecked
        connect(chk, &QCheckBox::clicked, this,
                [=](bool isChecked)
                {
                    if (isChecked)
                    {
                        for (auto *&c : menu->findChildren<QCheckBox *>())
                        {
                            if (c != chkAll && !c->isChecked())
                            {
                                isChecked = false;
                                break;
                            }
                        }
                    }
                    chkAll->setChecked(isChecked);
                });
        //chkAll click handling
        connect(chkAll, SIGNAL(clicked(bool)), chk, SLOT(setChecked(bool)));
        vbox->addWidget(chk);
    }
    chkAll->setChecked(allChecked);
    auto *gbox = new QGroupBox();
    gbox->setLayout(vbox);
    auto *wAct = new QWidgetAction(menu);
    wAct->setDefaultWidget(gbox);
    menu->addAction(wAct);
}
