/**
 * The Call UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Call.cpp 1806 2024-01-30 01:46:19Z hazim.rujhan $
 * @author Mazdiana Makmor
 */
#include <assert.h>

#include "Style.h"
#include "VideoStream.h"
#include "ui_Call.h"
#include "Call.h"

Call::Call(Logger           *logger,
           ResourceSelector *rscSelector,
           CommsRegister    *commsReg,
           QWidget          *parent) :
QWidget(parent), ui(new Ui::Call), mCommsReg(commsReg), mLogger(logger),
mResourceSelector(rscSelector)
{
    ui->setupUi(this);
    ui->callButton->setText(tr("Call", "Button"));
    ui->verticalLayout1->addWidget(mCommsReg);
    connect(ui->callButton, &QToolButton::clicked, this,
            [this]
            {
                int type;
                int ssi;
                if (mResourceSelector->getSelectedId(type, ssi))
                    emit newCall(type, ssi);
            });
    connect(ui->broadcastButton, &QToolButton::clicked, this,
            [this] { emit newBroadcast(); });
    connect(mCommsReg, &CommsRegister::startAction, this,
            [this](int, int idType, int id, const QString &)
            {
                mResourceSelector->setSelectedId(idType, id);
                ui->callButton->click();
            });
    connect(mCommsReg, &CommsRegister::floatOrDock, this,
            [this](QDialog *dlg)
            {
                if (dlg == 0)
                {
                    //mCommsReg floated out - fill void with vertical spacer
                    ui->inpLayout->addSpacerItem(
                                     new QSpacerItem(0, 0, QSizePolicy::Minimum,
                                                     QSizePolicy::Expanding));
                }
                else
                {
                    //delete spacer
                    delete ui->inpLayout->takeAt(ui->inpLayout->count() - 1);
                    //dock mCommsReg back into ui, delete its floating parent
                    ui->verticalLayout1->addWidget(mCommsReg);
                    delete dlg; //ui is now parent widget
                }
            });
    setTheme();
    //ensure that mResourceSelector is assigned to a screen to guarantee its
    //destruction
    activate();
    VideoStream::setLogger(logger);
}

Call::~Call()
{
    delete ui;
}

void Call::activate()
{
    mResourceSelector->enableMultiSelect(false);
    ui->inpLayout->insertWidget(0, mResourceSelector);
}

void Call::setTheme()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    ui->callButton->setStyleSheet(Style::getStyle(Style::OBJ_CALLBUTTON));
    if (!ui->broadcastButton->isHidden())
        ui->broadcastButton
          ->setStyleSheet(Style::getStyle(Style::OBJ_CALLBUTTON));
    mCommsReg->setTheme();
}

void Call::setBroadcastPermission(bool allowed)
{
    ui->broadcastButton->setVisible(allowed);
}

void Call::openCallWindow()
{
    ui->callButton->click();
}

void Call::onSelectionChanged(bool isValidSingle, bool)
{
    ui->callButton->setEnabled(isValidSingle);
}
