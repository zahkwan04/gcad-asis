/**
 * The Bookmarks implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisBookmarks.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Mohd Fashan Abdul Munir
 */
#include <QAction>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>
#include <assert.h>

#include "CmnTypes.h"
#include "DbInt.h"
#include "GisPoint.h"
#include "QtUtils.h"
#include "Style.h"
#include "Utils.h"
#include "ui_GisBookmarks.h"
#include "GisBookmarks.h"

using namespace std;

GisBookmarks::GisBookmarks(GisCanvas     *canvas,
                           const QString &username,
                           QWidget       *parent) :
QWidget(parent), ui(new Ui::GisBookmarks), mCanvas(canvas), mUsername(username)
{
    if (canvas == 0)
    {
        assert("Bad param in GisBookmarks::GisBookmarks" == 0);
        return;
    }
    ui->setupUi(this);
    ui->stdListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->stdListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->stdListView->setToolTip(tr("Double click to display"));
    mStdModel = new QStandardItemModel(this);
    ui->stdListView->setModel(mStdModel);
    ui->usrListView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->usrListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->usrListView->setToolTip(tr("Double click to display"));
    mUsrModel = new QStandardItemModel(this);
    ui->usrListView->setModel(mUsrModel);
    ui->usrListView->setContextMenuPolicy(Qt::CustomContextMenu);
    loadData(false);
    loadData(true);
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
                //toggle Bookmarks frame display mode
                bool v = ui->toggleBtn->isChecked();
                ui->stdFrame->setVisible(v);
                ui->stdLblFrame->setVisible(v);
                ui->usrFrame->setVisible(v);
                ui->usrLblFrame->setVisible(v);
                if (v)
                    ui->verticalLayout->removeItem(mSpacer);
                else
                    ui->verticalLayout->addSpacerItem(mSpacer);
            });
    ui->toggleBtn->click();
    connect(ui->addBtn, &QPushButton::clicked, this,
            [this]
            {
                //add bookmark
                if (!mCanvas->isValid())
                    return;
                QString name(getInput(tr("Add Bookmark")));
                if (name.isEmpty())
                    return;
#ifndef NO_DB
                DbInt::RecordT data;
                data[DbInt::FIELD_USER_NAME] = mUsername.toStdString();
                data[DbInt::FIELD_NAME] = name.toStdString();
                data[DbInt::FIELD_COORDS] = GisPoint::getCoords(
                                        mCanvas->getGeomCenter()).toStdString();
                data[DbInt::FIELD_ZOOM] = Utils::toString(
                                                       mCanvas->getZoomLevel());
                if (!DbInt::instance().insertRecord(DbInt::DB_TABLE_D_BOOKMARK,
                                                    data))
                {
                    QMessageBox::critical(this, tr("Bookmark Error"),
                                          tr("Failed to save '%1' to database "
                                             "because of link error.")
                                              .arg(name));
                }
#endif //!NO_DB
                auto *itm = new QStandardItem(name);
                itm->setData(QVariant::fromValue(mCanvas->getGeomCenter()),
                             Qt::UserRole);
                itm->setData(QVariant::fromValue(mCanvas->getZoomLevel()),
                             Qt::UserRole + 1);
                mUsrModel->insertRow(0);
                mUsrModel->setItem(0, itm);
                mUsrModel->sort(Qt::AscendingOrder);
            });
    connect(ui->deleteBtn, &QPushButton::clicked, this,
            [this]
            {
                //delete bookmark
                QModelIndex idx(ui->usrListView->currentIndex());
                QString sel(idx.data().toString());
                if (QMessageBox::question(this, tr("Delete Bookmark"),
                                          tr("Are you sure to delete '%1'?")
                                              .arg(sel)) ==
                    QMessageBox::Yes)
                {
#ifndef NO_DB
                    DbInt::RecordT data;
                    data[DbInt::FIELD_USER_NAME] = mUsername.toStdString();
                    data[DbInt::FIELD_NAME] = sel.toStdString();
                    if (!DbInt::instance().deleteRecord(
                                              DbInt::DB_TABLE_D_BOOKMARK, data))
                    {
                        QMessageBox::critical(this, tr("Bookmark Error"),
                                              tr("Failed to delete '%1' from "
                                                 "database because of link "
                                                 "error.").arg(sel));
                        return;
                    }
#endif //!NO_DB
                    mUsrModel->removeRow(idx.row());
                }
            });
    connect(ui->renameBtn, &QPushButton::clicked, this,
            [this]
            {
                //rename bookmark
                QString currName(ui->usrListView
                                   ->currentIndex().data().toString());
                QString newName(getInput(tr("Rename Bookmark"), currName));
                if (newName.isEmpty())
                    return;
#ifndef NO_DB
                DbInt::RecordT data;
                data[DbInt::FIELD_NAME] = newName.toStdString();
                DbInt::RecordT keys;
                keys[DbInt::FIELD_USER_NAME] = mUsername.toStdString();
                keys[DbInt::FIELD_NAME] = currName.toStdString();
                if (!DbInt::instance().updateRecord(DbInt::DB_TABLE_D_BOOKMARK,
                                                    keys, data))
                {
                    QMessageBox::critical(this, tr("Bookmark Error"),
                                          tr("Failed to rename '%1' because of "
                                             "database link error.")
                                              .arg(currName));
                    return;
                }
#endif //!NO_DB
                mUsrModel->setData(ui->usrListView->currentIndex(), newName,
                                   Qt::DisplayRole);
                mUsrModel->sort(Qt::AscendingOrder);
            });
    connect(ui->stdListView, &QListView::doubleClicked, this,
            [this](const QModelIndex &idx)
            {
                //show the selected Standard bookmark map view
                QMap<int, QVariant> m(mStdModel->itemData(idx));
                mCanvas->setGeomCenter(m[Qt::UserRole].value<QPointF>(),
                                       m[Qt::UserRole + 1].toInt());
            });
    connect(ui->usrListView, &QListView::doubleClicked, this,
            [this](const QModelIndex &idx)
            {
                //show the selected User Defined bookmark map view
                QMap<int, QVariant> m(mUsrModel->itemData(idx));
                mCanvas->setGeomCenter(m[Qt::UserRole].value<QPointF>(),
                                       m[Qt::UserRole + 1].toInt());
            });
    connect(ui->usrListView->selectionModel(),
            &QItemSelectionModel::selectionChanged, this,
            [this]
            {
                ui->renameBtn->setEnabled(ui->usrListView->selectionModel()
                                            ->hasSelection());
                ui->deleteBtn->setEnabled(ui->renameBtn->isEnabled());
            });
    connect(ui->usrListView, &QListView::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                QMenu menu(this);
                QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_ADD);
                //show item-specific actions only when clicking on an item
                if (ui->usrListView->indexAt(pos).isValid())
                {
                    QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_EDIT,
                                           ui->renameBtn->text());
                    QtUtils::addMenuAction(menu, CmnTypes::ACTIONTYPE_DELETE);
                }
                QAction *act = menu.exec(ui->usrListView->mapToGlobal(pos));
                if (act == 0)
                    return;
                switch (act->data().toInt())
                {
                    case CmnTypes::ACTIONTYPE_ADD:
                        ui->addBtn->click();
                        break;
                    case CmnTypes::ACTIONTYPE_DELETE:
                        ui->deleteBtn->click();
                        break;
                    case CmnTypes::ACTIONTYPE_EDIT:
                    default:
                        ui->renameBtn->click();
                        break;
                }
            });
}

GisBookmarks::~GisBookmarks()
{
    if (ui->toggleBtn->isChecked())
        delete mSpacer; //it is not in ui
    delete mStdModel;
    delete mUsrModel;
    delete ui;
}

void GisBookmarks::setTheme()
{
    ui->stdLblFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE2));
    ui->usrLblFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE2));
    ui->lblFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
}

void GisBookmarks::setUser(const QString &username)
{
    if (mUsername != username)
    {
        mUsername = username;
        //clear old user data and reload
        if (mUsrModel->rowCount() > 0)
            mUsrModel->removeRows(0, mUsrModel->rowCount());
        loadData(true);
    }
}

void GisBookmarks::loadData(bool user)
{
#ifndef NO_DB
    DbInt &db(DbInt::instance());
    DbInt::QResult *res = (user)?
                          db.getUsrBookmarks(mUsername.toStdString()) :
                          db.getStdBookmarks();
    if (res == 0)
        return;
    QStandardItemModel *mdl = (user)? mUsrModel: mStdModel;
    QStandardItem *itm;
    QPointF p;
    string name;
    string coord;
    int zoom;
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        if (res->getFieldValue(DbInt::FIELD_NAME, name, i) &&
            res->getFieldValue(DbInt::FIELD_COORDS, coord, i) &&
            res->getFieldValue(DbInt::FIELD_ZOOM, zoom, i) &&
            (GisPoint::checkCoords(QString::fromStdString(coord), p) ==
             GisPoint::COORD_VALID))
        {
            itm = new QStandardItem(QString::fromStdString(name));
            itm->setData(QVariant::fromValue(p), Qt::UserRole);
            itm->setData(QVariant::fromValue(zoom), Qt::UserRole + 1);
            mdl->insertRow(0);
            mdl->setItem(0, itm);
        }
    }
    delete res;
    mdl->sort(Qt::AscendingOrder);
#endif //!NO_DB
}

QString GisBookmarks::getInput(const QString &title, const QString &curr)
{
    bool ok;
    QString name(QInputDialog::getText(this, title, tr("Enter name:"),
                                       QLineEdit::Normal, curr, &ok,
                                       windowFlags() &
                                           ~Qt::WindowContextHelpButtonHint &
                                           ~Qt::WindowMinMaxButtonsHint));
    if (ok && !name.isEmpty())
    {
        if (name == curr)
        {
            name.clear();
        }
        else if (!mUsrModel->findItems(name).isEmpty())
        {
            QMessageBox::critical(this, tr("%1 Error").arg(title),
                                 tr("The name '%1' already exists.").arg(name));
            name.clear();
        }
    }
    return name;
}
