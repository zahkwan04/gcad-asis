/**
 * Implementation of terminal tracking replay.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisTrackingReplay.cpp 1819 2024-02-29 01:50:24Z rosnin $
 * @author Zunnur Zafirah
 */
#include <QDateTime>
#include <assert.h>

#include "Style.h"
#include "GisTrackingReplay.h"
#include "ui_GisTrackingReplay.h"

using namespace std;

static const QString FMT("dd/MM/yyyy HH:mm:ss");

GisTrackingReplay::GisTrackingReplay(GisCanvas *canvas, QWidget *parent) :
QDialog(parent), ui(new Ui::GisTrackingReplay), mCanvas(canvas)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->speedCb->addItem("1x", 1);
    ui->speedCb->addItem("2x", 2);
    ui->speedCb->addItem("4x", 4);
    ui->speedCb->addItem("8x", 8);
    ui->speedCb->addItem("16x", 16);
    ui->speedCb->addItem("32x", 32);
    ui->speedCb->addItem("60x", 60);
    ui->speedCb->addItem("64x", 64);
    ui->speedCb->addItem("128x", 128);
    ui->speedCb->addItem("256x", 256);
    ui->speedCb->addItem("512x", 512);
    ui->speedCb->addItem("1024x", 1024);
    ui->speedCb->addItem("2048x", 2048);
    ui->speedCb->addItem("3600x", 3600);
    mSpeed = ui->speedCb->currentData().toInt();
    connect(ui->speedCb, qOverload<int>(&QComboBox::currentIndexChanged), this,
            [this]
            {
                mSpeed = ui->speedCb->currentData().toInt(); //set replay speed
            });
    connect(ui->playButton, &QPushButton::clicked, this,
            [this]
            {
                //start, pause or resume replay
                if (ui->playButton->isChecked())
                {
                    if (ui->replaySlider->value() == ui->replaySlider->maximum())
                        stop(true);
                    mTimer->start(1000); //1 second interval
                }
                else
                {
                    mTimer->stop();
                }
            });
    connect(ui->stopButton, &QPushButton::clicked, this,
            [this] { stop(true); });
    connect(ui->replaySlider, &QSlider::sliderMoved, this,
            [this](int pos)
            {
                //if moved backward, delete tracking lines beyond current
                //position, and set replay point to the position
                time_t curr = mTimeStart + pos;
                if (curr < mIt.key())
                {
                    //moved backward - delete points beyond current time
                    QMap<int, int> issiNum;
                    int issi;
                    int num = 1;
                    auto it = mIt;
                    foreach (issi, it.value().keys())
                    {
                        issiNum[issi] = 1;
                    }
                    //loop to count difference for each issi
                    for (--it; it.key()>curr; --it,++num)
                    {
                        foreach (issi, it.value().keys())
                        {
                            if (issiNum.contains(issi))
                                ++(issiNum[issi]);
                            else
                                issiNum[issi] = 1;
                        }
                    }
                    auto nIt = issiNum.begin();
                    for (; nIt!=issiNum.end(); ++nIt)
                    {
                        mCanvas->deleteTrackingLineSegments(nIt.key(),
                                                            nIt.value());
                    }
                    mIt = it;
                    mPointNum -= num;
                    updatePositionLabel();
                }
                //this is followed by invocation of replaySlider valueChanged()
            });
    connect(ui->replaySlider, &QSlider::valueChanged, this,
            [this](int val)
            {
                //add tracking lines up to current position, pause replay at
                //slider end
                time_t curr = mTimeStart + val;
                ui->currTimeLbl
                  ->setText(QDateTime::fromTime_t(curr).toString(FMT));
                auto it = mIt + 1;
                if (it == mTrackpoints.end() || it.key() > curr)
                    return;
                IssiPointsT::iterator pIt;
                for (; it!=mTrackpoints.end() && it.key()<=curr;
                     ++it,++mPointNum)
                {
                    ++mIt;
                    for (pIt=it.value().begin(); pIt!=it.value().end(); ++pIt)
                    {
                        mCanvas->addTrackingLine(pIt.key(), pIt.value(), false);
                    }
                }
                updatePositionLabel();
                if (curr == mTimeEnd && ui->playButton->isChecked())
                    ui->playButton->click(); //pause
            });
    connect(this, &QDialog::rejected, this,
            [this]
            {
                stop(false);
                mTrackpoints.clear();
                mIssis.clear();
            });
    mTimer = new QTimer(this);
    connect(mTimer, &QTimer::timeout, this,
            [this]
            {
                ui->replaySlider
                  ->setSliderPosition(ui->replaySlider->value() + mSpeed);
            });
    setTheme();
}

GisTrackingReplay::~GisTrackingReplay()
{
    delete ui;
    delete mTimer;
}

void GisTrackingReplay::setTheme()
{
    ui->replaySlider
      ->setStyleSheet(Style::getStyle(Style::OBJ_SLIDER_HORIZONTAL));
}

void GisTrackingReplay::resetData()
{
    if (!mIssis.empty())
    {
        stop(false);
        mIssis.clear();
    }
    mTrackpoints.clear();
    //set start time beyond now
    mTimeStart = QDateTime::currentDateTime().toTime_t() + 600;
    mTimeEnd = 0;
}

void GisTrackingReplay::start()
{
    if (mIssis.empty())
    {
        close();
        return;
    }
    QStringList l;
    for (auto i : qAsConst(mIssis))
    {
        l << QString::number(i);
    }
    setWindowTitle(tr("Tracking Replay: %1").arg(l.join(',')));
    ui->pointsLbl->setText("(" + QString::number(mTrackpoints.size()) + " " +
                           tr("points") + ")");
    reset(true);
    ui->replaySlider->setMaximum(mTimeEnd - mTimeStart);
    show();
    ui->playButton->setChecked(true);
    ui->playButton->click();
}

void GisTrackingReplay::addData(int issi, DbInt::QResult *res)
{
    if (res == 0)
    {
        assert("Bad param in GisTrackingReplay::addData" == 0);
        return;
    }
    mIssis.insert(issi);
    string val;
    //get first gps time
    res->getFieldValue(DbInt::FIELD_LOC_TIME, val);
    uint t = QDateTime::fromString(QString::fromStdString(val), FMT).toTime_t();
    if (t < mTimeStart)
    {
        mTimeStart = t;
        ui->startLbl->setText(tr("Start: ") + QString::fromStdString(val));
    }
    //start adding new points
    string coord;
    res->getFieldValue(DbInt::FIELD_LOC_LATLONG, coord);
    mTrackpoints[t][issi] = coord;
    //get last gps time
    int i = res->getNumRows() - 1;
    res->getFieldValue(DbInt::FIELD_LOC_TIME, val, i);
    t = QDateTime::fromString(QString::fromStdString(val), FMT).toTime_t();
    if (t > mTimeEnd)
    {
        mTimeEnd = t;
        ui->endLbl->setText(tr("End: ") + QString::fromStdString(val));
    }
    res->getFieldValue(DbInt::FIELD_LOC_LATLONG, coord, i);
    mTrackpoints[t][issi] = coord;
    //fill up track with the intermediate data points
    for (--i; i>0; --i)
    {
        res->getFieldValue(DbInt::FIELD_LOC_TIME, val, i);
        res->getFieldValue(DbInt::FIELD_LOC_LATLONG, coord, i);
        mTrackpoints[QDateTime::fromString(QString::fromStdString(val), FMT)
                .toTime_t()][issi] = coord;
    }
}

void GisTrackingReplay::reset(bool isFirstTime)
{
    mIt = mTrackpoints.begin();
    mPointNum = 1;
    ui->currTimeLbl->setText(QDateTime::fromTime_t(mTimeStart).toString(FMT));
    updatePositionLabel();
    ui->playButton->setChecked(false);
    ui->replaySlider->setValue(0);
    auto it = mIt.value().begin();
    mCanvas->addTrackingLine(it.key(), it.value(), isFirstTime);
    for (++it; it!=mIt.value().end(); ++it)
    {
        mCanvas->addTrackingLine(it.key(), it.value(), false);
    }
}

void GisTrackingReplay::stop(bool doReset)
{
    mTimer->stop();
    for (auto i : qAsConst(mIssis))
    {
        mCanvas->deleteTrackingLineSegments(i);
    }
    if (doReset)
        reset(false);
}

void GisTrackingReplay::updatePositionLabel()
{
    auto it = mIt.value().begin();
    QString txt(QString::fromStdString(it.value()));
    txt.append(" [").append(QString::number(mPointNum)).append("]");
    if (mIssis.size() > 1)
    {
        txt.prepend(": ").prepend(QString::number(it.key()));
        for (++it; it!=mIt.value().end(); ++it)
        {
            txt.append("\n").append(QString::number(it.key()))
               .append(": ").append(QString::fromStdString(it.value()));
        }
    }
    ui->currPosLbl->setText(txt);
}
