/**
 * The implementation for routing UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisRouting.cpp 1899 2025-01-22 05:42:07Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <QMessageBox>
#include <assert.h>

#include "GisCanvas.h"
#include "GisPoint.h"
#include "GisQmlInt.h"
#include "Style.h"
#include "ui_GisRouting.h"
#include "GisRouting.h"

GisRouting::GisRouting(GisCanvas *canvas, QWidget *parent) :
QWidget(parent), mMapCanvas(canvas), ui(new Ui::GisRouting)
{
    if (canvas == 0)
    {
        assert("Bad param in GisRouting::GisRouting" == 0);
        return;
    }
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
                bool visible = ui->toggleBtn->isChecked();
                ui->routeFrame->setVisible(visible);
                if (visible)
                    ui->verticalLayout->removeItem(mSpacer);
                else
                    ui->verticalLayout->addSpacerItem(mSpacer);
            });
    ui->toggleBtn->click();
    connect(ui->srcBtn, &QToolButton::clicked, this,
            [this]
            {
                mMapCanvas->setMode(GisQmlInt::MODE_ROUTE,
                                    GisQmlInt::TYPEID_ROUTE_START);
            });
    connect(ui->dstBtn, &QToolButton::clicked, this,
            [this]
            {
                mMapCanvas->setMode(GisQmlInt::MODE_ROUTE,
                                    GisQmlInt::TYPEID_ROUTE_END);
            });
    connect(ui->routeBtn, &QPushButton::clicked, this,
            [this]
            {
                //fail silently if either input missing
                QString src(ui->sourceEdit->text().trimmed());
                if (src.isEmpty())
                    return;
                QString dst(ui->destinationEdit->text().trimmed());
                if (dst.isEmpty())
                    return;
                /**
                 * @todo Check input types whether location name/coordinates.
                 *       Rosnin Mustaffa 10/01/2025
                 */
                QPointF srcPt;
                if (GisPoint::checkCoords(this, tr("Routing Error"), src, false,
                                          srcPt) !=
                    GisPoint::COORD_VALID)
                {
                    QMessageBox::critical(this, tr("Routing Error"),
                                          tr("Please check source location."),
                                          QMessageBox::Ok);
                    return;
                }
                QPointF dstPt;
                if (GisPoint::checkCoords(this, tr("Routing Error"), dst, false,
                                          dstPt) !=
                    GisPoint::COORD_VALID)
                {
                    QMessageBox::critical(this, tr("Routing Error"),
                                          tr("Please check destination."),
                                          QMessageBox::Ok);
                    return;
                }
                emit getRouting(srcPt.y(), srcPt.x(), dstPt.y(), dstPt.x());
            });
    connect(ui->clearBtn, &QPushButton::clicked, this,
            [this]
            {
                removePoints();
                emit clearResult();
            });
}

GisRouting::~GisRouting()
{
    if (ui->toggleBtn->isChecked())
        delete mSpacer; //it is not in ui
    delete ui;
}

void GisRouting::setTheme()
{
    ui->routeTitleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->routeSuggestFrame
      ->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE2));
    ui->routeDetailsFrame
      ->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE2));
}

void GisRouting::removePoints()
{
    mMapCanvas->typeCancelled(GisQmlInt::TYPEID_ROUTE_START);
    mMapCanvas->typeCancelled(GisQmlInt::TYPEID_ROUTE_END);
    ui->sourceEdit->clear();
    ui->destinationEdit->clear();
}

void GisRouting::onCoordReceived(int typeId, double x, double y)
{
    switch (typeId)
    {
        case GisQmlInt::TYPEID_ROUTE_START:
            ui->sourceEdit->setText(GisPoint::getCoords(y, x));
            break;
        case GisQmlInt::TYPEID_ROUTE_END:
            ui->destinationEdit->setText(GisPoint::getCoords(y, x));
            break;
        default:
            break; //do nothing
    }
}
