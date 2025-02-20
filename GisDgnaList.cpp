/**
 * DGNA list implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisDgnaList.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItemModel>
#include <assert.h>

#include "CmnTypes.h"
#include "GisQmlInt.h"
#include "InputDialog.h"
#include "QtUtils.h"
#include "Style.h"
#include "ui_GisDgnaList.h"
#include "GisDgnaList.h"

using namespace std;

GisDgnaList::GisDgnaList(Resources *resources,
                         Dgna      *dgna,
                         GisCanvas *canvas,
                         QWidget   *parent) :
QWidget(parent), ui(new Ui::GisDgnaList), mCanvas(canvas), mDgna(dgna),
mResources(resources)
{
    if (resources == 0 || dgna == 0 || canvas == 0)
    {
        assert("Bad param in GisDgnaList::GisDgnaList" == 0);
        return;
    }
    ui->setupUi(this);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mModel = ResourceData::createModel(ResourceData::TYPE_DGNA_IND,
                                       ui->listView);
    ui->listView->setModel(mModel);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, &QListView::customContextMenuRequested, this,
            [this](const QPoint &pt)
            {
                QMenu menu(this);
                QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_RSC_SELECT,
                                       ui->assignBtn->text());
                //show item-specific actions only when clicking on an item
                if (ui->listView->indexAt(pt).isValid())
                {
                    QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_DGNA_ASSG,
                                           ui->viewBtn->text());
                    QtUtils::addMenuAction(menu,
                                           CmnTypes::ACTIONTYPE_DGNA_DISSOLVE);
                    QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_DELETE,
                                           ui->removeBtn->text());
                }
                auto *act = menu.exec(ui->listView->mapToGlobal(pt));
                if (act != 0)
                    onBtnClick(act->data().toInt());
            });
    connect(ui->listView->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            [this]
            {
                bool b = (ui->listView->selectionModel()->hasSelection());
                ui->viewBtn->setEnabled(b);
                ui->dissolveBtn->setEnabled(b);
                ui->removeBtn->setEnabled(b);
            });
    setWindowTitle(parentWidget()->windowTitle()); //for use in connect() below
    connect(ui->floatButton, &QToolButton::clicked, this,
            [this]
            {
                static QSize dlgSize; //to preserve size between floats
                if (ui->floatButton->isChecked())
                {
                    //move self from parent into new floating dialog
                    auto *layout = new QVBoxLayout();
                    layout->setMargin(0);
                    layout->addWidget(this);
                    auto *dlg = new QDialog(parentWidget());
                    dlg->setWindowTitle(windowTitle());
                    //remove title bar help button and disable close button
                    dlg->setWindowFlags(dlg->windowFlags() &
                                        ~Qt::WindowContextHelpButtonHint &
                                        ~Qt::WindowCloseButtonHint);
                    dlg->setLayout(layout);
                    if (dlgSize.isValid())
                        dlg->resize(dlgSize);
                    else
                        dlg->resize(width(), height());
                    dlg->show();
                    ui->floatButton->setToolTip(tr("Dock"));
                    if (!ui->toggleBtn->isChecked())
                        ui->toggleBtn->click();
                }
                else
                {
                    auto *dlg = qobject_cast<QDialog *>(parentWidget());
                    if (dlg != 0)
                    {
                        //dock back into parent, which will delete dialog
                        dlgSize = dlg->size();
                        ui->floatButton->setToolTip(tr("Float"));
                        emit dock(dlg);
                    }
                }
            });
    mSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
                              QSizePolicy::Expanding);
    connect(ui->toggleBtn, &QToolButton::clicked, this,
            [this]
            {
                bool visible = ui->toggleBtn->isChecked();
                ui->dgnaFrame->setVisible(visible);
                if (visible)
                    ui->verticalLayout->removeItem(mSpacer);
                else
                    ui->verticalLayout->addSpacerItem(mSpacer);
            });
    ui->toggleBtn->click();
    connect(ui->assignBtn, &QPushButton::clicked, this,
            [this] { onBtnClick(CmnTypes::ACTIONTYPE_RSC_SELECT); });
    connect(ui->viewBtn, &QPushButton::clicked, this,
            [this] { onBtnClick(CmnTypes::ACTIONTYPE_DGNA_ASSG); });
    connect(ui->dissolveBtn, &QPushButton::clicked, this,
            [this] { onBtnClick(CmnTypes::ACTIONTYPE_DGNA_DISSOLVE); });
    connect(ui->removeBtn, &QPushButton::clicked, this,
            [this] { onBtnClick(CmnTypes::ACTIONTYPE_DELETE); });
}

GisDgnaList::~GisDgnaList()
{
    if (ui->toggleBtn->isChecked())
        delete mSpacer; //it is not in ui
    delete ui;
}

void GisDgnaList::setTheme()
{
    ui->dgnaLblFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
}

void GisDgnaList::onDgnaAssign(ResourceData::ListModel *mdl)
{
    if (mdl == 0 || mdl->rowCount() == 0)
    {
        assert("Bad param in GisDgnaList::onDgnaAssign" == 0);
        delete mdl;
        return;
    }
    auto *d = new InputDialog(
                            ResourceData::getModel(ResourceData::TYPE_DGNA_IND),
                            mdl, this);
    while (d->exec() == QDialog::Accepted)
    {
        QStringList members(d->getDgnaMembers());
        if (!members.isEmpty())
        {
            int gssi = d->getDgnaGssi();
            mDgna->addMembers(gssi, members);
            ResourceData::addId(mModel, gssi, true);
            break;
        }
        QMessageBox::critical(this, tr("DGNA Assignment Error"),
                              tr("No member selected."));
    }
    delete d;
    delete mdl;
    mCanvas->setMode(GisQmlInt::MODE_SELECT);
}

void GisDgnaList::onBtnClick(int actType)
{
    switch (actType)
    {
        case CmnTypes::ACTIONTYPE_DELETE:
        {
            QModelIndex idx(ui->listView->currentIndex());
            if (QMessageBox::question(this, tr("Remove DGNA Group"),
                                      tr("Are you sure to remove '%1'?\n"
                                         "This does not dissolve the group.")
                                          .arg(idx.data().toString())) ==
                QMessageBox::Yes)
                mModel->removeRow(idx.row());
            break;
        }
        case CmnTypes::ACTIONTYPE_DGNA_ASSG:
        {
            mResources->selectDgnaInd(ResourceData::getItemId(
                            ResourceData::model(ui->listView)
                                ->itemFromIndex(ui->listView->currentIndex())));
            break;
        }
        case CmnTypes::ACTIONTYPE_DGNA_DISSOLVE:
        {
            QModelIndex idx(ui->listView->currentIndex());
            QString grpName(idx.data().toString());
            if (QMessageBox::question(this, tr("Dissolve DGNA Group"),
                                      tr("Are you sure to dissolve '%1'?")
                                          .arg(grpName)) ==
                QMessageBox::Yes)
            {
                auto *mdl = ResourceData::model(ui->listView);
                QStringList emptyList;
                mDgna->removeMembers(ResourceData::getItemId(
                                                       mdl->itemFromIndex(idx)),
                                     emptyList);
                mModel->removeRow(idx.row());
            }
            break;
        }
        case CmnTypes::ACTIONTYPE_RSC_SELECT:
        default:
        {
            mCanvas->setMode(GisQmlInt::MODE_RESOURCES,
                             GisQmlInt::TYPEID_RSC_DGNA);
            break;
        }
    }
}
