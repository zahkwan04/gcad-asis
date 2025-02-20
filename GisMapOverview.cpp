/**
 * The map overview implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisMapOverview.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <assert.h>

#include <qquickwidget.h>
#include <QQuickView>

#include "Style.h"
#include "ui_GisMapOverview.h"
#include "GisMapOverview.h"

GisMapOverview::GisMapOverview(Logger *logger, QWidget *parent) :
QWidget(parent), mCanvas(0), mLogger(logger), ui(new Ui::GisMapOverview)
{
    if (logger == 0)
    {
        assert("Bad param in GisMapOverview::GisMapOverview" == 0);
        return;
    }
    ui->setupUi(this);
    mSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
                              QSizePolicy::Expanding);
    connect(ui->hideTbtn, &QToolButton::clicked, this,
            [this] ()
            {
                if (ui->overviewFrame->isHidden())
                {
                    ui->overviewFrame->setHidden(false);
                    ui->mainLayout->removeItem(mSpacer);
                }
                else
                {
                    ui->overviewFrame->setHidden(true);
                    ui->mainLayout->addSpacerItem(mSpacer);
                }
            });
    auto *qw = new QQuickWidget(ui->overviewFrame);
    ui->mapLayout->addWidget(qw);
    mCanvas = new GisCanvas(logger, "", true, qw);
    onSetTheme();
}

GisMapOverview::~GisMapOverview()
{
    if (ui->overviewFrame->isVisible())
        delete mSpacer; //it is not in ui
    delete mCanvas;
    delete ui;
}

void GisMapOverview::onSetTheme()
{
    ui->titleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
}

void GisMapOverview::reload()
{
    mCanvas->refreshMap();
}
