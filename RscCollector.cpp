/**
 * Resource collector dialog implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2024. All Rights Reserved.
 *
 * @file
 * @version $Id: RscCollector.cpp 1884 2024-11-25 09:10:00Z hazim.rujhan $
 * @author Hazim Mohd Rujhan
 */
#include <algorithm>   //std::sort
#include <QList>
#include <QRegExp>
#include <assert.h>

#include "CmnTypes.h"
#include "QtUtils.h"
#include "Style.h"
#include "Utils.h"
#include "ui_RscCollector.h"
#include "RscCollector.h"

using namespace std;

Logger *RscCollector::sLogger(0);

RscCollector::RscCollector(int type, SsisT *ssis, QWidget *parent) :
QDialog(parent), mSsis(ssis), ui(new Ui::RscCollector)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    if (ssis == 0 || sLogger == 0)
    {
        assert("Bad param in RscCollector" == 0);
        close();
        return;
    }
    setModal(true);
    //remove title bar help button and disable close button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
    setWindowTitle(windowTitle().append(ResourceData::getTypeTxt(type)));
    const QString &ss(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->searchLblFrame->setStyleSheet(ss);
    ui->rscFrame->setStyleSheet(ss);
    ui->selFrame->setStyleSheet(ss);
    mSortMdl = new QSortFilterProxyModel;
    mSortMdl->setFilterCaseSensitivity(Qt::CaseInsensitive);
    //change list type if necessary just to ensure correct drag icon
    int listType = (type == ResourceData::TYPE_GROUP_OR_DGNA)?
                   ResourceData::TYPE_GROUP: type;
    mRscList = new DraggableListView(listType);
    mRscList->setDragDropMode(QAbstractItemView::DragDrop);
    mRscList->setFont(ui->searchEdit->font());
    mRscList->installEventFilter(this);
    ui->rscLayout->addWidget(mRscList);
    mSelList = new DraggableListView(listType);
    mSelList->setDragDropMode(QAbstractItemView::DragDrop);
    mSelList->installEventFilter(this);
    auto *mdl = ResourceData::createModel(type, mSelList);
    mSelList->setModel(mdl);
    ui->selLayout->addWidget(mSelList);
    mSelList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mSelList, &QListView::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                auto idx(mSelList->indexAt(pos));
                if (!idx.isValid())
                    return;
                QList<int> actTypes;
                actTypes << CmnTypes::ACTIONTYPE_DELETE
                         << CmnTypes::ACTIONTYPE_COPY;
                QMenu menu(this);
                QtUtils::addMenuActions(menu, actTypes);
                auto *act = menu.exec(mSelList->mapToGlobal(pos));
                if (act == 0)
                    return;
                switch (act->data().toInt())
                {
                    case CmnTypes::ACTIONTYPE_COPY:
                        QtUtils::copyToClipboard(mSelList);
                        break;
                    case CmnTypes::ACTIONTYPE_DELETE:
                        moveItems(false);
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
                    mSortMdl->setFilterRegExp(re);
            });
    connect(ui->addBtn, &QPushButton::clicked, this,
            [this]() { moveItems(true); });
    connect(ui->rmvBtn, &QPushButton::clicked, this,
            [this]() { moveItems(false); });
    connect(ui->rmvAllBtn, &QPushButton::clicked, this,
            [this]()
            {
                mSelList->selectAll();
                moveItems(false);
            });
    connect(ui->okBtn, &QPushButton::clicked, this,
            [this]()
            {
                SsisT l;
                auto *mdl = ResourceData::model(mSelList);
                mdl->getIds(l);
                if (l != *mSsis)
                {
                    LOGGER_DEBUG(sLogger, "RscCollector:: Updated SSIs "
                                 << Utils::toStringWithRange(l));
                    *mSsis = l;
                    emit selChanged();
                }
                close();
            });
    connect(ui->cancelBtn, &QPushButton::clicked, this, [this]() { close(); });
    //show current collection
    for (auto i : *mSsis)
    {
        ResourceData::addId(mdl, i);
    }
    mdl->sort(0);
    //fill up source with specified type
    ResourceData::IdsT ids;
    switch (type)
    {
        case ResourceData::TYPE_GROUP_OR_DGNA:
            mdl = ResourceData::getModel(ResourceData::TYPE_GROUP);
            if (mdl != 0)
                mdl->getIds(ids);
            mdl = ResourceData::getModel(ResourceData::TYPE_DGNA_IND);
#ifdef DGNA_OF_GRPS
            if (mdl != 0)
                mdl->getIds(ids);
            mdl = ResourceData::getModel(ResourceData::TYPE_DGNA_GRP);
#endif
            break;
        case ResourceData::TYPE_SUBS_OR_MOBILE:
            mdl = ResourceData::getModel(ResourceData::TYPE_SUBSCRIBER);
            if (mdl != 0)
                mdl->getIds(ids);
            mdl = ResourceData::getModel(ResourceData::TYPE_MOBILE);
            break;
        case ResourceData::TYPE_UNKNOWN:
            assert("Invalid TYPE_UNKNOWN in RscCollector::RscCollector" == 0);
            mdl = 0;
            break;
        default:
            mdl = ResourceData::getModel(type);
            break;
    }
    if (mdl != 0)
        mdl->getIds(ids);
    if (!ids.empty())
    {
        mdl = ResourceData::createModel(type, mRscList);
        for (auto i : ids)
        {
            if (mSsis->count(i) == 0) //skip collected ones
                ResourceData::addId(mdl, i);
        }
        mdl->sort(0);
        mSortMdl->setSourceModel(mdl);
        mRscList->setModel(mSortMdl);
    }
    show();
}

RscCollector::~RscCollector()
{
    delete mRscList;
    delete mSortMdl;
    delete mSelList;
    delete ui;
}

bool RscCollector::eventFilter(QObject *obj, QEvent *evt)
{
    //accept only drag-and-drop between mRscList and mSelList
    if (evt->type() == QEvent::DragEnter)
    {
        auto *e = static_cast<QDragEnterEvent *>(evt);
        if (obj != e->source() &&
            (e->source() == mRscList || e->source() == mSelList))
        {
            e->acceptProposedAction();
            return true;
        }
    }
    else if (evt->type() == QEvent::Drop)
    {
        auto *e = static_cast<QDropEvent *>(evt);
        if (obj == e->source())
            return false;
        //source already validated in DragEnter event
        if (obj == mSelList)
        {
            moveItems(true);
            return true;
        }
        if (obj == mRscList)
        {
            moveItems(false);
            return true;
        }
    }
    return false;
}

void RscCollector::moveItems(bool add)
{
    QModelIndexList l((add)?
                      mRscList->selectionModel()->selectedIndexes():
                      mSelList->selectionModel()->selectedIndexes());
    if (l.empty())
        return;
    auto *rscMdl = static_cast<ResourceData::ListModel *>(
                                                       mSortMdl->sourceModel());
    auto *selMdl = ResourceData::model(mSelList);
    int id;
    ResourceData::IdsT ids;
    if (add)
    {
        for (auto &idx : l)
        {
            id = ResourceData::getItemId(
                                rscMdl->item(mSortMdl->mapToSource(idx).row()));
            ResourceData::addId(selMdl, id);
            ids.insert(id);
        }
        rscMdl->removeIds(ids);
        selMdl->sort(0);
    }
    else
    {
        for (auto &idx : l)
        {
            id = ResourceData::getItemId(selMdl->item(idx.row()));
            ResourceData::addId(rscMdl, id);
            ids.insert(id);
        }
        selMdl->removeIds(ids);
        rscMdl->sort(0);
    }
}
