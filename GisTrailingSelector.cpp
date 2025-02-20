/**
 * GIS trailing selector implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisTrailingSelector.cpp 1819 2024-02-29 01:50:24Z rosnin $
 * @author Mohd Rozaimi
 * @author Muhd Hashim Wahab
 */
#include <algorithm>   //std::sort
#include <set>
#include <assert.h>
#include <QMessageBox>
#include <QRegExp>
#include <QStringListModel>

#include "ResourceData.h"
#include "Style.h"
#include "ui_GisTrailingSelector.h"
#include "GisTrailingSelector.h"

using namespace std;

GisTrailingSelector::~GisTrailingSelector()
{
    delete mRscList;
    delete mSortModel;
    delete mTrlList;
    delete ui;
}

void GisTrailingSelector::display(GisCanvas *canvas, QWidget *parent)
{
    if (canvas == 0)
    {
        assert("Bad param in GisTrailingSelector::display" == 0);
        return;
    }
    if (canvas->hasTerminals())
        new GisTrailingSelector(canvas, parent);
    else
        QMessageBox::information(parent, tr("GIS Trailing Selector"),
                                 tr("No terminal yet on the map."));
}

bool GisTrailingSelector::eventFilter(QObject *obj, QEvent *event)
{
    //accept only drag-and-drop between mRscList and mTrlList
    if (event->type() == QEvent::DragEnter)
    {
        auto *e = static_cast<QDragEnterEvent *>(event);
        if (obj != e->source() &&
            (e->source() == mRscList || e->source() == mTrlList))
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
        if (obj == mTrlList)
        {
            ui->addButton->click();
            return true;
        }
        if (obj == mRscList)
        {
            ui->removeButton->click();
            return true;
        }
    }
    return false;
}

GisTrailingSelector::GisTrailingSelector(GisCanvas *canvas, QWidget *parent) :
QDialog(parent), ui(new Ui::GisTrailingSelector), mCanvas(canvas)
{
    ui->setupUi(this);
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);
    //remove title bar help button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    const QString &ss(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->searchFrame->setStyleSheet(ss);
    ui->rscFrame->setStyleSheet(ss);
    ui->trlFrame->setStyleSheet(ss);
    mSortModel = new QSortFilterProxyModel;
    mSortModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    mRscList = new DraggableListView(ResourceData::TYPE_SUBSCRIBER);
    mRscList->setDragDropMode(QAbstractItemView::DragDrop);
    mRscList->setFont(ui->searchEdit->font());
    mRscList->installEventFilter(this);
    ui->rscLayout->addWidget(mRscList);
    mTrlList = new DraggableListView(ResourceData::TYPE_SUBSCRIBER);
    mTrlList->setDragDropMode(QAbstractItemView::DragDrop);
    mTrlList->installEventFilter(this);
    ui->trlLayout->addWidget(mTrlList);
    ui->searchEdit->setFocus();

    set<int> terminals;
    set<int> trailing;
    mCanvas->getTerminalTrailing(terminals, trailing);
    auto *mdl = ResourceData::createModel(ResourceData::TYPE_SUBS_OR_MOBILE,
                                          mRscList);
    ResourceData::addIds(mdl, terminals);
    mSortModel->setSourceModel(mdl);
    mRscList->setModel(mSortModel);
    mdl = ResourceData::createModel(ResourceData::TYPE_SUBS_OR_MOBILE,
                                    mTrlList);
    mTrlList->setModel(mdl);
    for (auto i : terminals)
    {
        if (trailing.count(i) != 0)
            ResourceData::addId(mdl, i);
    }
    mdl->sort(0);
    connect(ui->searchEdit, &QLineEdit::textChanged, this,
            [this](const QString &txt)
            {
                //filter resource list
                QRegExp re(txt, Qt::CaseInsensitive);
                if (re.isValid())
                    mSortModel->setFilterRegExp(re);
            });
    connect(ui->addButton, &QPushButton::clicked, this,
            [this]
            {
                //add selected ISSIs to trailing list
                QModelIndexList l(mRscList->selectionModel()->selectedIndexes());
                if (l.empty())
                    return;
                auto *mdl = static_cast<ResourceData::ListModel *>(
                                                     mSortModel->sourceModel());
                auto *trlMdl = ResourceData::model(mTrlList);
                int id;
                for (auto &idx : l)
                {
                    id = ResourceData::getItemId(
                                 mdl->item(mSortModel->mapToSource(idx).row()));
                    if (ResourceData::addId(trlMdl, id))
                        mCanvas->terminalTrailing(id, true);
                }
                trlMdl->sort(0);
            });
    connect(ui->removeButton, &QPushButton::clicked, this,
            [this]
            {
                //remove selected ISSIs from trailing list
                QModelIndexList l(mTrlList->selectionModel()->selectedIndexes());
                if (l.empty())
                    return;
                auto *mdl = ResourceData::model(mTrlList);
                int id;
                ResourceData::IdsT ids;
                for (auto &idx : l)
                {
                    id = ResourceData::getItemId(mdl->item(idx.row()));
                    mCanvas->terminalTrailing(id, false);
                    ids.insert(id);
                }
                mdl->removeIds(ids);
            });
    connect(ui->removeAllButton, &QPushButton::clicked, this,
            [this]
            {
                //remove all ISSIs from trailing list
                mTrlList->selectAll();
                ui->removeButton->click();
            });
    connect(ui->okButton, &QPushButton::clicked, this, [this] { close(); });
    show();
}
