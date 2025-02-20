/**
 * Dialog box for terminal point implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisMenuTerminal.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <assert.h>

#include "ResourceData.h"
#include "Style.h"
#include "ui_GisMenuTerminal.h"
#include "GisMenuTerminal.h"

using namespace std;

GisMenuTerminal::GisMenuTerminal(const Props::ValueMapT &props,
                                 QWidget                *parent) :
QDialog(parent), ui(new Ui::GisMenuTerminal)
{
    if (props.count(Props::FLD_KEY) == 0)
    {
        assert("Bad param in GisMenuTerminal::GisMenuTerminal" == 0);
        return;
    }
    ui->setupUi(this);
    setWindowOpacity(0.90);
    setTheme();
    int id;
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    id = Props::get<int>(props, Props::FLD_KEY);
    setWindowTitle(tr("ISSI ") + QString::number(id));
    ui->terminalLbl->setText(ResourceData::getDspTxt(id));
    ui->locationVal->setText(
        QString::fromStdString(Props::get(props, Props::FLD_LAT))
            .append(",")
            .append(QString::fromStdString(Props::get(props, Props::FLD_LON))));
    ui->lastUpdVal->setText(QString::fromStdString(
                                  Props::get(props, Props::FLD_LOC_TIMESTAMP)));
    ui->lastValidUpdVal->setText(QString::fromStdString(
                            Props::get(props, Props::FLD_LOC_TIMESTAMP_VALID)));
    connect(ui->okButton, &QPushButton::clicked, this,
            [this] { close(); });
}

GisMenuTerminal::~GisMenuTerminal()
{
    delete ui;
}

void GisMenuTerminal::setTheme()
{
    ui->titleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
}

QString GisMenuTerminal::getValidityAge(time_t t)
{
    if (t == 0)
        return "";
    QString s;
    int val = static_cast<int>(t/(24 * 60 * 60));
    if (val > 0)
        s.append(QString::number(val)).append(" ")
         .append((val > 1)? tr("days"): tr("day"));
    t %= (24 * 60 * 60);
    val = static_cast<int>(t/(60 * 60));
    if (val > 0)
    {
        if (!s.isEmpty())
            s.append(" ");
        s.append(QString::number(val)).append(" ")
         .append((val > 1)? tr("hours"): tr("hour"));
    }
    t %= (60 * 60);
    val = static_cast<int>(t/60);
    if (val > 0)
    {
        if (!s.isEmpty())
            s.append(" ");
        s.append(QString::number(val)).append(" ")
         .append((val > 1)? tr("minutes"): tr("minute"));
    }
    s.prepend("(").append(")");
    return s;
}
