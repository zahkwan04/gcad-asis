/**
 * BFT tracking UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisTracking.cpp 1833 2024-03-25 07:55:22Z hazim.rujhan $
 * @author Muhd Hashim Wahab
 */
#include <QDate>
#include <QIntValidator>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTime>

#include "DbInt.h"
#include "GisTrailingSelector.h"
#include "InputDialog.h"
#include "Style.h"
#include "Utils.h"
#include "ui_GisTracking.h"
#include "GisTracking.h"

using namespace std;

static const int     MAX_DAYS = 30;
static const QString DATE_FMT("dd/MM/yyyy");

GisTracking::GisTracking(GisCanvas *canvas, QWidget *parent) :
QWidget(parent), ui(new Ui::GisTracking), mReplay(0), mCanvas(canvas)
{
    ui->setupUi(this);
    ui->verticalLayout
      ->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum,
                                      QSizePolicy::Expanding));
#ifdef INCIDENT
    ui->incidentCb->setValidator(new QIntValidator(this));
    ui->incidentCb->setMaxCount(20);
#else
    delete ui->incidentLbl;
    delete ui->incidentCb;
    delete ui->rscButton;
#endif
    ui->subscriberCb->setToolTip(tr("Space-separated ISSIs"));
    ui->subscriberCb->setMaxCount(20);
    //set current date and time
    QDate d(QDate::currentDate());
    ui->startDate->setDate(d);
    ui->endDate->setDate(d);
    QTime now(QTime::currentTime());
    ui->startTime->setTime(now);
    ui->endTime->setTime(now);
#ifdef NO_DB
    ui->trackFrame->setEnabled(false);
#endif
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
    connect(ui->toggleBtn, &QToolButton::clicked, this,
            [this] { ui->trackFrame->setVisible(ui->toggleBtn->isChecked()); });
    ui->toggleBtn->click();
    connect(ui->trailingButton, &QPushButton::clicked, this,
            [this] { GisTrailingSelector::display(mCanvas, this); });
    connect(ui->submitButton, &QPushButton::clicked, this,
            [this]
            {
                //replay() with the entered ISSIs
                set<int> issis;
                if (getIssis(issis))
                    replay(issis);
            });
#ifdef INCIDENT
    connect(ui->rscButton, &QPushButton::clicked, this,
            [this]
            {
                //get incident resources and timestamps for replay
#ifndef NO_DB
                QString id(ui->incidentCb->currentText());
                if (id.isEmpty())
                    return;
                int i = ui->incidentCb->findText(id);
                if (i != 0)
                {
                    //id not at the top - either move or add it there
                    ui->incidentCb->insertItem(0, id);
                    ui->incidentCb->setCurrentIndex(0);
                    if (i > 0)
                        ui->incidentCb->removeItem(i + 1);
                }
                DbInt::QResult *res = DbInt::instance()
                                      .getIncidentHistory(id.toInt());
                if (res == 0 || res->getNumRows() == 0)
                {
                    //the ideal title begins with "Tracking Error: " but that
                    //gets truncated on display
                    QMessageBox::critical(this,
                                   tr("Tracking: ") + tr("Incident %1").arg(id),
                                   tr("Invalid Incident ID."));
                    delete res;
                    return;
                }
                string s(res->getFieldStr(DbInt::FIELD_INC_RESOURCES));
                if (s.empty())
                {
                    QMessageBox::critical(this,
                                   tr("Tracking: ") + tr("Incident %1").arg(id),
                                   tr("No resource assigned."));
                    delete res;
                    return;
                }
                auto *mdl = new QStandardItemModel();
                QStandardItem *itm;
                i = 0;
                QString qs;
                foreach (qs, QString::fromStdString(s).split(","))
                {
                    itm = new QStandardItem(qs);
                    itm->setCheckable(true);
                    itm->setSelectable(false);
                    itm->setData(Qt::Checked, Qt::CheckStateRole);
                    mdl->setItem(i++, itm);
                }
                QString start(QString::fromStdString(res->getFieldStr(
                                                 DbInt::FIELD_INC_RECVD_DATE)));
                QString closed(QString::fromStdString(res->getFieldStr(
                                                DbInt::FIELD_INC_CLOSED_DATE)));
                auto *d = new InputDialog(id, start, closed, mdl, this);
                if (d->exec() == QDialog::Accepted)
                {
                    QString issis(d->getSelectedResources());
                    qs = ui->subscriberCb->currentText().trimmed();
                    if (qs.isEmpty())
                    {
                        ui->subscriberCb->lineEdit()->setText(issis);
                    }
                    else
                    {
                        qs.append(" ").append(issis);
                        QStringList l(qs.split(QRegularExpression("\\D+"),
                                               QString::SkipEmptyParts));
                        l.removeDuplicates();
                        ui->subscriberCb->lineEdit()->setText(l.join(" "));
                    }
                    QStringList l;
                    if (d->getUseTimeFlag(true))
                    {
                        l = start.split(" ");
                        if (l.size() > 1)
                        {
                            ui->startDate
                              ->setDate(QDate::fromString(l.at(0), DATE_FMT));
                            ui->startTime->setTime(QTime::fromString(l.at(1)));
                        }
                    }
                    if (!closed.isEmpty() && d->getUseTimeFlag(false))
                    {
                        l = closed.split(" ");
                        if (l.size() > 1)
                        {
                            ui->endDate
                              ->setDate(QDate::fromString(l.at(0), DATE_FMT));
                            ui->endTime->setTime(QTime::fromString(l.at(1)));
                        }
                    }
                }
                delete d;
                delete res;
#endif //!NO_DB
            });
#endif //INCIDENT
}

GisTracking::~GisTracking()
{
    delete ui;
    delete mReplay;
}

void GisTracking::setTheme()
{
    ui->trackTitleFrame
      ->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    if (mReplay != 0)
        mReplay->setTheme();
}

void GisTracking::replay(const std::set<int> &issis)
{
    if (issis.empty())
        return;
#ifndef NO_DB
    if (mReplay == 0)
        mReplay = new GisTrackingReplay(mCanvas, this);
    mReplay->resetData();
    int issi;
    QStringList fails;
    DbInt::QResult *res;
    auto it = issis.begin();
    while (it != issis.end())
    {
        issi = *it;
        res = DbInt::instance().getGps(Utils::toString(issi),
                                     ui->startDate->text().toStdString() + " " +
                                         ui->startTime->text().toStdString(),
                                     ui->endDate->text().toStdString() + " " +
                                         ui->endTime->text().toStdString());
        ++it;
        if (res == 0 || res->getNumRows() == 0)
            fails << QString::number(issi);
        else
            mReplay->addData(issi, res);
        delete res;
    }
    if (!fails.isEmpty())
        QMessageBox::critical(this, "Tracking: " + tr("Replay"),
                              tr("No tracking data for %1.")
                                  .arg(fails.join(',')));
    mReplay->start();
#endif
}

bool GisTracking::getIssis(std::set<int> &issis)
{
    QString qs(ui->subscriberCb->currentText().trimmed());
    //replace non-digits with a space
    qs.replace(QRegularExpression("\\D+"), " ");
    qs = qs.trimmed();
    if (qs.isEmpty())
        return false;
    int i = ui->subscriberCb->findText(qs);
    if (i != 0)
    {
        //qs not at the top - either move or add it there
        ui->subscriberCb->insertItem(0, qs);
        ui->subscriberCb->setCurrentIndex(0);
        if (i > 0)
            ui->subscriberCb->removeItem(i + 1);
    }
    int diff = ui->startDate->date().daysTo(ui->endDate->date());
    if (diff > MAX_DAYS)
    {
        QMessageBox::critical(this, tr("Tracking: ") + tr("Invalid Date"),
                              tr("Date range must not exceed %1 days.")
                                  .arg(MAX_DAYS));
        return false;
    }
    if (diff < 0)
    {
        QMessageBox::critical(this, tr("Tracking: ") + tr("Invalid Date"),
                              tr("End date must not be earlier than Start "
                                 "date."));
        return false;
    }
    if (diff == 0 && (ui->endTime->time() <= ui->startTime->time()))
    {
        QMessageBox::critical(this, tr("Tracking: ") + tr("Invalid Time"),
                              tr("End time must be later than Start time."));
        return false;
    }
    return Utils::fromString(qs.toStdString(), issis, ' ');
}
