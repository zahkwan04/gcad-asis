/**
 * UI ResourceSelector implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ResourceSelector.cpp 1885 2024-11-28 08:32:01Z hazim.rujhan $
 * @author Mazdiana Makmor
 */
#include <assert.h>
#include <QMessageBox>
#include <QRegularExpression>

#include "Style.h"
#include "ui_ResourceSelector.h"
#include "ResourceSelector.h"

//maximum number of visible items in completer
static const int     MAX_ITEMS = 10;
static const QString RECSEP(", "); //recipients separator

#define SETUPRB(rb, tp) \
    do \
    { \
        rb->setText(ResourceData::getTypeTxt(tp)); \
        connect(rb, &QRadioButton::clicked, this, [this] { setModel(tp); }); \
    } \
    while (0)

ResourceSelector::ResourceSelector(QWidget *parent) :
QWidget(parent), ui(new Ui::ResourceSelector)
{
    ui->setupUi(this);
    SETUPRB(ui->subscriber, ResourceData::TYPE_SUBSCRIBER);
    SETUPRB(ui->talkGroup, ResourceData::TYPE_GROUP);
    SETUPRB(ui->dgnaInd, ResourceData::TYPE_DGNA_IND);
#ifdef DGNA_OF_GRPS
    SETUPRB(ui->dgnaGrp, ResourceData::TYPE_DGNA_GRP);
#endif
    SETUPRB(ui->mobile, ResourceData::onlineMobileType());
    SETUPRB(ui->dispatcher, ResourceData::TYPE_DISPATCHER);
    connect(ui->nameEdit, &QLineEdit::textChanged, this,
            [this](const QString &txt)
            {
                //validate single-selection recipient ID and emit signal
                //indicating the recipient selections validity
                mSsi = 0;
                auto *mdl = static_cast<ResourceData::ListModel *>
                            (mCompleter->model());
                if (!txt.isEmpty() && mdl != 0)
                {
                    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
                    mCompleter->setFilterMode(Qt::MatchContains);
                    //check for valid resource
                    int type = mdl->type();
                    QString s(txt);
                    int ssi = ResourceData::matchName(true, s, type);
                    if (ssi != 0 )
                        mSsi = ssi;
                }
                ui->addButton->setEnabled(mSsi != 0);
                emit selectionChanged((mSsi != 0), hasSelection(false));
            });
    connect(ui->recEdit, &QLineEdit::textChanged, this,
            [this] { emit selectionChanged((mSsi != 0), hasSelection(false)); });
    connect(ui->viewButton, &QToolButton::clicked, this,
            [this]
            {
                //show unfiltered list in the drop-down
                mCompleter->setCompletionMode(
                                         QCompleter::UnfilteredPopupCompletion);
                mCompleter->complete();
            });
    connect(ui->addButton, &QToolButton::clicked, this,
            [this]
            {
                //add selected ID to recipient list
                appendSelection(ui->categoryGroup->checkedId(),
                                ui->nameEdit->text());
            });
    connect(ui->selRightButton, &QToolButton::clicked, this,
            [this]
            {
                //select next entry in recipient list
                if (mSelections.empty())
                    return;
                int idx = 0;
                QStringList l(ui->recEdit->text().split(RECSEP));
                QString s;
                //hasSelectedText() is true only for selection made here, and
                //not for user selection
                if (ui->recEdit->hasSelectedText())
                {
                    s = ui->recEdit->selectedText();
                    //select next item
                    for (auto it=l.begin(); it!=l.end(); ++it)
                    {
                        if (*it == s)
                        {
                            if (++it == l.end())
                            {
                                s = l.at(0); //cycle to front
                            }
                            else
                            {
                                s = *it;
                                idx = ui->recEdit->text().indexOf(s);
                            }
                            break;
                        }
                    }
                }
                else
                {
                    s = l.at(0);
                    //for unknown reason, must ensure recEdit does not have
                    //focus here, otherwise if recEdit has focus, the selection
                    //will disappear on focus change
                    ui->selRightButton->setFocus();
                }
                ui->recEdit->setSelection(idx, s.size());
            });
    connect(ui->selLeftButton, &QToolButton::clicked, this,
            [this]
            {
                //select previous entry in recipient list
                if (mSelections.empty())
                    return;
                QStringList l(ui->recEdit->text().split(RECSEP));
                QString s;
                if (ui->recEdit->hasSelectedText())
                {
                    s = ui->recEdit->selectedText();
                    //select previous item
                    for (auto it=l.rbegin(); it!=l.rend(); ++it)
                    {
                        if (*it == s)
                        {
                            if (++it == l.rend())
                                it = l.rbegin(); //cycle to back
                            s = *it;
                            break;
                        }
                    }
                }
                else
                {
                    s = *(l.rbegin());
                    ui->selLeftButton->setFocus();
                }
                ui->recEdit
                  ->setSelection(ui->recEdit->text().indexOf(s), s.size());
            });
    connect(ui->clrButton, &QToolButton::clicked, this,
            [this]
            {
                //clear recipient list
                if (ui->recEdit->hasSelectedText())
                {
                    QString s(ui->recEdit->selectedText());
                    for (auto &it : mSelections)
                    {
                        if (it.second.name == s)
                        {
                            mSelections.erase(it.first);
                            break;
                        }
                    }
                }
                else
                {
                    mSelections.clear();
                }
                setRecipients();
            });
    mCompleter = new QCompleter(this);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setMaxVisibleItems(MAX_ITEMS);
    ui->nameEdit->setCompleter(mCompleter);
    connect(mCompleter, qOverload<const QModelIndex &>(&QCompleter::activated),
            this,
            [this](const QModelIndex &idx)
            {
                //show selected item from the drop-down
                ui->nameEdit
                  ->setText(static_cast<ResourceData::ListModel *>
                            (mCompleter->model())->item(idx.row())->text());
            });
    setTheme();
}

ResourceSelector::~ResourceSelector()
{
    delete mCompleter;
    delete ui;
}

void ResourceSelector::init(bool full)
{
    if (ui->categoryGroup->id(ui->talkGroup) == ResourceData::TYPE_GROUP)
        return; //already initialized
    ui->categoryGroup->setId(ui->talkGroup, ResourceData::TYPE_GROUP);
    ui->categoryGroup->setId(ui->mobile, ResourceData::onlineMobileType());
    ui->categoryGroup->setId(ui->dispatcher, ResourceData::TYPE_DISPATCHER);
    if (full)
    {
        ui->categoryGroup->setId(ui->subscriber, ResourceData::TYPE_SUBSCRIBER);
        ui->categoryGroup->setId(ui->dgnaInd, ResourceData::TYPE_DGNA_IND);
#ifdef DGNA_OF_GRPS
        ui->categoryGroup->setId(ui->dgnaGrp, ResourceData::TYPE_DGNA_GRP);
#else
        ui->dgnaInd->setText(tr("DGNA"));
        ui->dgnaGrp->setEnabled(false);
        ui->dgnaGrp->hide();
#endif //DGNA_OF_GRPS
    }
    else
    {
        ui->subscriber->hide();
        ui->dgnaInd->hide();
        ui->dgnaGrp->hide();
    }
}

void ResourceSelector::setTheme()
{
    mCompleter->popup()
              ->setStyleSheet(Style::getStyle(Style::OBJ_ABSTRACTITEMVIEW));
}

void ResourceSelector::setMobileType(bool online)
{
    ResourceData::setMobileStat(online);
    ui->categoryGroup->setId(ui->mobile, ResourceData::onlineMobileType());
}

void ResourceSelector::enableMultiSelect(bool enable)
{
    ui->addButton->setEnabled(enable); //for easier check
    if (enable)
        ui->recFrame->show();
    else
        ui->recFrame->hide();
}

bool ResourceSelector::setSelectedId(int                       type,
                                     const ResourceData::IdsT &ids,
                                     bool                      doAdd,
                                     bool                      doReplace)
{
    if (doReplace)
        ui->clrButton->click();
    if (type == ResourceData::TYPE_MOBILE ||
        type == ResourceData::TYPE_MOBILE_ONLINE)
        type = ResourceData::onlineMobileType();
    auto *btn = ui->categoryGroup->button(type);
    if (btn != 0)
    {
        btn->setChecked(true);
        setModel(type, false);
    }
    if (ids.size() > 1)
        return (ui->addButton->isEnabled() && appendSelection(type, ids));
    mSsi = *ids.begin();
    ui->nameEdit->setText(ResourceData::getDspTxt(mSsi, type));
    if (doAdd && !ui->recFrame->isHidden())
        appendSelection(type, ids);
    return true;
}

bool ResourceSelector::setSelectedId(int type, int id)
{
    if (type >= ResourceData::TYPE_UNKNOWN)
    {
        type = ResourceData::getType(id);
        if (type == ResourceData::TYPE_UNKNOWN)
            return false;
    }
    ResourceData::IdsT ids;
    ids.insert(id);
    setSelectedId(type, ids, false);
    return true;
}

bool ResourceSelector::getSelectedId(int &type, int &ssi, QString *name)
{
    if (mSsi == 0)
        return false;
    type = ui->categoryGroup->checkedId();
    ssi  = mSsi;
    if (name != 0)
        *name = ui->nameEdit->text();
    return true;
}

bool ResourceSelector::hasSelection(bool includeSingle)
{
    return ((includeSingle && mSsi != 0) ||
            (!ui->recFrame->isHidden() && !mSelections.empty()));
}

bool ResourceSelector::getSelection(SelectionsT &sel)
{
    if (!mSelections.empty())
    {
        //check for invalid entries
        ResourceData::IdsT inv;
        for (const auto &it : mSelections)
        {
            if (!ResourceData::hasId(it.second.type, it.first))
                inv.insert(it.first);
        }
        for (auto i : inv)
        {
            mSelections.erase(i);
        }
        sel = mSelections;
    }
    else if (mSsi != 0)
    {
        sel[mSsi] = Rsc(ui->categoryGroup->checkedId(), ui->nameEdit->text());
    }
    return (!sel.empty());
}

void ResourceSelector::setData(int type)
{
    if (type < 0)
    {
        //disable radiobutton if model is empty
        ui->subscriber
          ->setEnabled(ResourceData::hasData(ResourceData::TYPE_SUBSCRIBER));
        ui->talkGroup
          ->setEnabled(ResourceData::hasData(ResourceData::TYPE_GROUP));
        ui->dgnaInd
          ->setEnabled(ResourceData::hasData(ResourceData::TYPE_DGNA_IND));
        ui->mobile
          ->setEnabled(ResourceData::hasData(ResourceData::onlineMobileType()));
        ui->dispatcher
          ->setEnabled(ResourceData::hasData(ResourceData::TYPE_DISPATCHER));
#ifdef DGNA_OF_GRPS
        ui->dgnaGrp
          ->setEnabled(ResourceData::hasData(ResourceData::TYPE_DGNA_GRP));
#endif
        //select one
        auto *btn = ui->categoryGroup->checkedButton();
        if (btn != 0 && btn->isEnabled())
        {
            //preserve previous selection, if any and still valid
            int id = 0;
            QString prevName(ui->nameEdit->text());
            if (prevName.isEmpty())
                id = ResourceData::getId(prevName);
            if (id != 0)
                setSelectedId(ui->categoryGroup->checkedId(), id);
            else
                btn->click();
        }
        else if (ui->subscriber->isEnabled())
            ui->subscriber->click();
        else if (ui->talkGroup->isEnabled())
            ui->talkGroup->click();
        else if (ui->dgnaInd->isEnabled())
            ui->dgnaInd->click();
        else if (ui->dgnaGrp->isEnabled())
            ui->dgnaGrp->click();
        else if (ui->mobile->isEnabled())
            ui->mobile->click();
        else if (ui->dispatcher->isEnabled())
            ui->dispatcher->click();
        return;
    }
    QRadioButton *btn;
    switch (type)
    {
#ifdef DGNA_OF_GRPS
        case ResourceData::TYPE_DGNA_GRP:
            btn = ui->dgnaGrp;
            break;
#endif //DGNA_OF_GRPS
        case ResourceData::TYPE_DGNA_IND:
            btn = ui->dgnaInd;
            break;
        case ResourceData::TYPE_DISPATCHER:
            btn = ui->dispatcher;
            break;
        case ResourceData::TYPE_GROUP:
            btn = ui->talkGroup;
            break;
        case ResourceData::TYPE_MOBILE:
        case ResourceData::TYPE_MOBILE_ONLINE:
            type = ResourceData::onlineMobileType();
            btn = ui->mobile;
            break;
        case ResourceData::TYPE_SUBSCRIBER:
        default:
            btn = ui->subscriber;
            break;
    }
    auto *mdl = ResourceData::getModel(type);
    btn->setEnabled(mdl != 0 && mdl->rowCount() > 0);
    if (btn->isChecked())
    {
        mCompleter->setModel(mdl);
        if (!btn->isEnabled())
            ui->nameEdit->clear();
    }
}

void ResourceSelector::setModel(int type, bool showFirstId)
{
    auto *mdl = ResourceData::getModel(type);
    if (mdl != 0 && mdl != mCompleter->model())
    {
        mCompleter->setModel(mdl);
        if (showFirstId)
            ui->nameEdit->setText(mdl->index(0, 0).data().toString());
    }
}

void ResourceSelector::appendSelection(int type, const QString &rscs)
{
    QStringList l(rscs.split(",", QString::SkipEmptyParts));
    l.removeDuplicates();
    int id;
    int tp = type; //because this may be modified by matchName()
    for (auto &s : l)
    {
        tp = type;
        id = ResourceData::matchName(true, s, tp);
        if (id != 0)
            mSelections[id] = Rsc(tp, s);
    }
    setRecipients();
}

bool ResourceSelector::appendSelection(int type, const ResourceData::IdsT &ids)
{
    int tp;
    for (auto i : ids)
    {
        tp = ResourceData::chkType(type, i);
        if (ResourceData::hasId(tp, i))
            mSelections[i] = Rsc(tp, ResourceData::getDspTxt(i, tp));
    }
    setRecipients();
    return !mSelections.empty();
}

void ResourceSelector::setRecipients()
{
    if (mSelections.empty())
    {
        ui->recEdit->clear();
    }
    else
    {
        QStringList l;
        for (const auto &it : mSelections)
        {
            l << it.second.name;
        }
        l.sort();
        ui->recEdit->setText(l.join(RECSEP));
    }
}
