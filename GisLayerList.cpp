/**
 * The implementation for layer list UI.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisLayerList.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <QStandardItemModel>
#include <assert.h>

#include "GisCanvas.h"
#include "GisQmlInt.h"
#include "Style.h"
#include "ui_GisLayerList.h"
#include "GisLayerList.h"

using namespace std;

//columns for map layers - must be in display order
enum
{
    COL_NUM,
    COL_NAME,
    COL_LABEL,
    COL_LAYER
};

GisLayerList::GisLayerList(Logger *logger, GisCanvas *canvas, QWidget *parent) :
QWidget(parent), ui(new Ui::GisLayerList), mCanvas(canvas), mLogger(logger),
mModel(0)
{
    ui->setupUi(this);
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
                //toggle Layer Selection frame display mode
                bool v = ui->toggleBtn->isChecked();
                ui->layerListFrame->setVisible(v);
                if (v)
                    ui->verticalLayout->removeItem(mSpacer);
                else
                    ui->verticalLayout->addSpacerItem(mSpacer);
            });
    ui->toggleBtn->click();
}

GisLayerList::~GisLayerList()
{
    if (ui->toggleBtn->isChecked())
        delete mSpacer; //it is not in ui
    delete mModel;
    delete ui;
}

void GisLayerList::populateLayerList()
{
    if (!mCanvas->isValid())
    {
        LOGGER_ERROR(mLogger, "GisLayerList::populateLayerList: "
                     "Failed to create layer list. Map canvas is not valid.");
        assert("GisLayerList::populateLayerList: Invalid map canvas." == 0);
        return;
    }
    if (mModel != 0)
        mModel->clear();
    else
        mModel = new QStandardItemModel();
    auto &mdls(mCanvas->getModelTypes());
    mModel->setRowCount(mdls.size());
    mModel->setHorizontalHeaderLabels(QStringList() << "" << ""
                                      << tr("Label") << tr("Layer"));
    int i = 0;
    for (auto &it : mdls)
    {
        mModel->setData(mModel->index(i, COL_NUM), -it.second);
        mModel->setData(mModel->index(i++, COL_NAME),
                        QString::fromStdString(it.first));
    }
    QStandardItem *itm;
    mModel->sort(COL_NUM, Qt::AscendingOrder);
    for (i=mModel->rowCount()-1; i>=0; --i)
    {
        mModel->setData(mModel->index(i, COL_NUM), QString::number(i + 1));
        itm = new QStandardItem();
        itm->setCheckable(true);
        itm->setData(Qt::Checked, Qt::CheckStateRole);
        mModel->setItem(i, COL_LABEL, itm);
        itm = new QStandardItem();
        itm->setCheckable(true);
        itm->setData(Qt::Checked, Qt::CheckStateRole);
        mModel->setItem(i, COL_LAYER, itm);
    }
    ui->treeView->setModel(mModel);
    for (i=COL_LAYER; i>=0; --i)
    {
        ui->treeView->resizeColumnToContents(i);
    }
    ui->treeView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(mModel, &QStandardItemModel::itemChanged, this,
            [this](QStandardItem *itm)
            {
                //show/hide map layer or label based on checkbox status
                bool sel = mModel->indexFromItem(itm)
                           .data(Qt::CheckStateRole).toBool();
                QString nm(mModel->data(mModel->index(itm->row(), COL_NAME))
                           .toString());
                string s(nm.toStdString());
                if (s == GisQmlInt::KEY_SEADEPTH || s == GisQmlInt::KEY_SEAMARKS)
                    mModel->setData(mModel->index(itm->row(),
                                                  (itm->column() == COL_LABEL)?
                                                      COL_LAYER: COL_LABEL),
                                    (sel)? Qt::Checked: Qt::Unchecked,
                                    Qt::CheckStateRole);
                mCanvas->updateView((itm->column() == COL_LABEL), sel, nm);
            });
}

void GisLayerList::setTheme()
{
    ui->layersTitleFrame
      ->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->treeView->setStyleSheet(Style::getStyle(Style::OBJ_TREEVIEW_CHECKBOX));
}

void GisLayerList::enableSeaMap(bool on)
{
#ifdef GIS_SEAMAP
    int i;
    int n = 2;
    string s;
    for (i=mModel->rowCount()-1; i>=0; --i)
    {
        s = mModel->data(mModel->index(i, COL_NAME)).toString().toStdString();
        if (s == GisQmlInt::KEY_SEADEPTH || s == GisQmlInt::KEY_SEAMARKS)
        {
            ui->treeView->setRowHidden(i, QModelIndex(), !on);
            mModel->setData(mModel->index(i, COL_LABEL), on, Qt::CheckStateRole);
            mModel->setData(mModel->index(i, COL_LAYER), on, Qt::CheckStateRole);
            if (--n == 0)
                break; //all relevant layers found
        }
    }
#endif
}
