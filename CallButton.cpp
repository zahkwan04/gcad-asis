/**
 * The CallButton UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: CallButton.cpp 1880 2024-11-15 08:24:57Z hazim.rujhan $
 * @author Mazdiana Makmor
 */
#include <assert.h>

#include "Style.h"
#include "ui_CallButton.h"
#include "CallButton.h"

CallButton::CallButton(CallWindow *callWindow, QWidget *parent) :
QWidget(parent), ui(new Ui::CallButton), mPriority(callWindow->getPriority()),
mCallWindow(callWindow)
{
    if (callWindow == 0)
    {
        assert("Bad param in CallButton::CallButton" == 0);
        return;
    }
    init();
}

CallButton::~CallButton()
{
    delete mCallWindow;
    delete ui;
}

void CallButton::setCallingParty(int type, int id)
{
    mCallWindow->setCallingParty(type, id);
    ui->callingPartyLabel->setText(mCallWindow->getCallingPartyName());
}

void CallButton::setCalledParty(const QString &name)
{
    ui->calledPartyLabel->setText(name);
    mCallWindow->setCalledParty(name);
}

void CallButton::setConnected(const QString &txParty, int priority)
{
    setPriority(priority);
    if (mCallWindow->setConnected(txParty, priority))
        ui->calledPartyLabel->setText(txParty);
}

void CallButton::changeOwnership(const QString &callingPartyName, int priority)
{
    ui->callingPartyLabel->setText((callingPartyName.isEmpty())?
                                   tr("Unknown"): callingPartyName);
    mCallWindow->changeOwnership(callingPartyName, priority);
    if (priority > 0)
        setPriority(priority);
}

void CallButton::resetCallWindow()
{
    mCallWindow->reset();
    mCallWindow = 0;
}

void CallButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        mCallWindow->doShow();
}

void CallButton::init()
{
    ui->setupUi(this);
    ui->calledLabel->setText(tr("From"));
    ui->callFrame->setStyleSheet(CallWindow::STYLE_BGCOLOR_PENDING);
    const QString &ss(Style::getStyle(Style::OBJ_LABEL_SMALL_BLACK));
    ui->calledLabel->setStyleSheet(ss);
    ui->callingPartyLabel->setStyleSheet(ss);
    ui->calledPartyLabel
      ->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_BLACK));
    ui->callingPartyLabel->setText(mCallWindow->getCallingPartyName());
    ui->calledPartyLabel->setText(mCallWindow->getCalledPartyName());
    installEventFilter(this);
}

void CallButton::setPriority(int priority)
{
    if (priority != MsgSp::Value::UNDEFINED)
        mPriority = priority;
    if (mPriority < MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_1)
        ui->callFrame->setStyleSheet(CallWindow::STYLE_BGCOLOR_ACTIVE);
    else if (mPriority < MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)
        ui->callFrame
          ->setStyleSheet(CallWindow::STYLE_BGCOLOR_ACTIVE_PREEMPTIVE);
    else
        ui->callFrame
          ->setStyleSheet(CallWindow::STYLE_BGCOLOR_ACTIVE_EMERGENCY);
}
