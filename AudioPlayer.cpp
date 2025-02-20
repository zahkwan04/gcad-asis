/**
 * Implementation of MP3 audio playback.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: AudioPlayer.cpp 1869 2024-08-08 08:51:17Z hazim.rujhan $
 * @author Zulzaidi Atan
 */
#include <assert.h>

#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QRect>
#include <QStandardItemModel>
#include <QTime>
#include <QToolTip>
#include <QVideoWidget>
#include <QWidgetAction>

#include "CmnTypes.h"
#include "DbInt.h"
#include "QtUtils.h"
#include "Style.h"
#include "ui_AudioPlayer.h"
#include "AudioPlayer.h"

using namespace std;

static const string LOGPREFIX("AudioPlayer:: ");
static const QIcon  ICON_SPKR(":/Images/images/icon_speaker.png");
static const QIcon  ICON_MUTE(":/Images/images/icon_speaker_mute.png");

AudioPlayer::AudioPlayer(Logger *logger, QWidget *parent) :
QDialog(parent), ui(new Ui::AudioPlayer), mLogger(logger),
mMediaPlayer(new QMediaPlayer), mMsgDlg(0), mMovie(0), mRetry(0)
{
    if (logger == 0)
    {
        assert("Bad param in AudioPlayer::AudioPlayer" == 0);
        return;
    }
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
    setWindowIcon(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_CALL));
    mMediaPlayer->setVideoOutput(ui->vidWidget);
    int vol = mMediaPlayer->volume();
    ui->volumeSlider->setValue(vol);
    ui->volumeSlider->setToolTip(QString::number(vol) + "%");
    auto *action = new QWidgetAction(ui->volumeToolButton);
    action->setDefaultWidget(ui->volumeFrame);
    auto *menu = new QMenu(ui->volumeToolButton);
    menu->addAction(action);
    ui->volumeToolButton->setMenu(menu);
    mMediaPlayer->setNotifyInterval(50);
    ui->vidWidget->installEventFilter(this);
    connect(ui->playButton, &QPushButton::clicked, this,
            [this]
            {
                //start/resume audio play
                if (ui->playButton->isChecked())
                    mMediaPlayer->play();
                else
                    mMediaPlayer->pause();
            });
    connect(ui->stopButton, &QPushButton::clicked, this,
            [this]
            {
                //stop playing, and reset position to start
                mMediaPlayer->stop();
                resetUi();
            });
    connect(ui->audioSlider, SIGNAL(sliderMoved(int)),
            SLOT(onAudioSliderMoved(int)));
    connect(ui->audioSlider, &QSlider::valueChanged, this,
            [this](int val)
            {
                ui->elapsedTimeLabel->setText(getTimeString(val));
                //set player position only not during playback, because this is
                //also triggered by the player position
                if (!ui->playButton->isChecked())
                    mMediaPlayer->setPosition(val);
            });
    connect(ui->volumeSlider, &QSlider::valueChanged, this,
            [this](int val)
            {
                mMediaPlayer->setVolume(val);
                ui->volumeToolButton->setIcon((val == 0)? ICON_MUTE: ICON_SPKR);
                QString pct(QString::number(val) + "%");
                QToolTip::showText(QCursor::pos(), pct, ui->volumeSlider,
                                   ui->volumeSlider->rect());
                ui->volumeSlider->setToolTip(pct);
            });
    connect(ui->pttTable, &QTableView::doubleClicked, this,
            [this](const QModelIndex &idx)
            {
                //jump to the selected time point
                int ms = QTime::fromString(
                               qobject_cast<QStandardItemModel *>(
                                                          ui->pttTable->model())
                                   ->item(idx.row(), QtTableUtils::COL_PTT_TIME)
                                   ->text(),
                               "mm:ss").msecsSinceStartOfDay();
                if (ui->playButton->isChecked())
                    onAudioSliderMoved(ms);
                else
                    ui->audioSlider->setValue(ms);
            });
    connect(ui->okButton, &QPushButton::clicked, this,
            [this]
            {
                //stop playing and hide this dialog
                ui->stopButton->click();
                setVisibility(false);
            });
    connect(this, &QDialog::rejected, this,
            [this]
            {
                //stop playing
                if (mMediaPlayer->state() != QMediaPlayer::StoppedState)
                    mMediaPlayer->stop();
            });
    connect(mMediaPlayer, &QMediaPlayer::durationChanged, this,
            [this](qint64 len)
            {
                //set the UI showing audio duration (len in ms)
                //note: when switching media, this is always called with len 0
                //first before called again with the new media length
                if (len == 0)
                {
                    //QMediaPlayer::Error raised in case of actual empty media
                    setVisibility(false);
                }
                else
                {
                    delete mMsgDlg;
                    mMsgDlg = 0;
                    mRetry = 0;
                    ui->endTimeLabel->setText(getTimeString(len));
                    ui->audioSlider->setMaximum(len);
                    setVisibility(true);
                    ui->playButton->click();
                }
            });
    connect(mMediaPlayer, &QMediaPlayer::positionChanged, this,
            [this](qint64 pos)
            {
                //set slider position
                ui->audioSlider->setValue(pos);
                if (ui->idLabel->isVisible())
                    showTxParty(pos/1000);
            });
    connect(mMediaPlayer, &QMediaPlayer::mediaStatusChanged, this,
            [this](QMediaPlayer::MediaStatus stat)
            {
                //set the UI based on media status
                if (stat == QMediaPlayer::EndOfMedia)
                {
                    ui->playButton->setChecked(false);
                    ui->idLabel->setText("");
                    mMediaPlayer->pause();
                }
            });
    connect(mMediaPlayer, qOverload<QMediaPlayer::Error>(&QMediaPlayer::error),
            this,
            [this](QMediaPlayer::Error e)
            {
                //raise and log error
                string err;
                switch (e)
                {
                    case QMediaPlayer::NoError:
                        return;
                    case QMediaPlayer::ResourceError:
                        err = "The media resource either could not be resolved "
                              "or is empty.";
                        break;
                    case QMediaPlayer::FormatError:
                        err = "The format of the media resource is not (fully) "
                              "supported.";
                        break;
                    case QMediaPlayer::NetworkError:
                        err = "A network error occurred.";
                        break;
                    case QMediaPlayer::AccessDeniedError:
                        err = "There are no appropriate permissions to play "
                              "the media resource.";
                        break;
                    case QMediaPlayer::ServiceMissingError:
                        err = "A valid playback service was not found.";
                        break;
                    case QMediaPlayer::MediaIsPlaylist:
                        //should not occur, but just to eliminate Qt warning
                    default:
                        err = "Undefined error.";
                        break;
                }
                if (mRetry < 3)
                {
                    ++mRetry;
                    PalThread::sleep(1);
                    setDetails(mAudData);
                }
                else
                {
                    delete mMsgDlg;
                    mMsgDlg = 0;
                    mRetry = 0;
                    LOGGER_ERROR(mLogger, LOGPREFIX << "QMediaPlayer::Error: "
                                 << err << "\n" << mPath.toStdString());
                    QMessageBox::critical((isVisible())? this: parentWidget(),
                                          tr("Playback Error"),
                                          tr("Error encountered when attempting "
                                             "to open the media."));
                    if (mMediaPlayer->state() != QMediaPlayer::StoppedState)
                        ui->stopButton->click();
                }
            });
    setTheme();
}

AudioPlayer::~AudioPlayer()
{
    delete mMediaPlayer;
    delete mMsgDlg;
    delete mMovie;
    delete ui;
}

void AudioPlayer::setDetails(const QtTableUtils::AudioData &data)
{
    mAudData = data;
    QUrl url(data.path);
    if (!url.isLocalFile() && !QtUtils::isHostAvailable(url))
    {
        delete mMsgDlg;
        mMsgDlg = 0;
        setVisibility(false);
        LOGGER_ERROR(mLogger, LOGPREFIX << "Audio server not available at "
                     << data.path.toStdString());
        QMessageBox::critical(parentWidget(), tr("Playback Error"),
                              tr("Audio server is not available."));
        return;
    }
    mPath = data.path;
    setWindowTitle(tr("Call Recording: %1->%2, %3")
                   .arg(data.from, data.to, data.callType));
    ui->endTimeLabel->setText("");
    ui->dateLabel->setText(data.time);
    resetUi();
    mPttHistory.clear();
    QtTableUtils::clearTable(ui->pttTable);
    mMediaPlayer->setMedia(QUrl()); //reset video
    ui->vidWidget->hide();
    if (!data.gssi.isEmpty() && getPttData(data.callKey))
    {
        ui->pttTable->show();
        adjustSize();
        setMaximumHeight(QWIDGETSIZE_MAX); //no limit
        setMaximumWidth(width());
    }
    else if (data.path.endsWith(".mp4"))
    {
        ui->vidWidget->show();
        adjustSize();
        setMaximumHeight(QWIDGETSIZE_MAX);
        setMaximumWidth(QWIDGETSIZE_MAX);
        if (mRetry == 0)
        {
            if (mMovie == 0)
            {
                mMovie = new QMovie(":/Images/images/icon_processing.gif");
                if (!mMovie->isValid())
                {
                    delete mMovie;
                    mMovie = 0;
                }
            }
            delete mMsgDlg;
            mMsgDlg = new MessageDialog(windowTitle(),
                              tr("Retrieving data...") + "\n" + data.time,
                              QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_CALL),
                              false, this);
            QLabel *lbl = 0;
            if (mMovie != 0)
            {
                lbl = new QLabel(mMsgDlg);
                lbl->setAlignment(Qt::AlignCenter);
                lbl->setMovie(mMovie); //lbl does not take ownership
                mMovie->start();
            }
            mMsgDlg->showNoOk(lbl);
        }
    }
    else
    {
        adjustSize();
        setMaximumHeight(height());
        setMaximumWidth(width());
    }
    ui->txPartyLabel->setVisible(!mPttHistory.isEmpty());
    ui->idLabel->setVisible(!mPttHistory.isEmpty());
    mMediaPlayer->setMedia(url);
}

void AudioPlayer::setTheme()
{
    ui->audioSlider
      ->setStyleSheet(Style::getStyle(Style::OBJ_SLIDER_HORIZONTAL));
    ui->volumeSlider
      ->setStyleSheet(Style::getStyle(Style::OBJ_SLIDER_VERTICAL));
}

bool AudioPlayer::eventFilter(QObject *obj, QEvent *evt)
{
    switch (evt->type())
    {
        case QEvent::KeyRelease:
            switch (static_cast<QKeyEvent *>(evt)->key())
            {
                case Qt::Key_Escape:
                    if (ui->vidWidget->isFullScreen())
                        ui->vidWidget->setFullScreen(false);
                    return true;
                case Qt::Key_Left:
                    //rewind 5 secs
                    mMediaPlayer->setPosition(mMediaPlayer->position() - 5000);
                    return true;
                case Qt::Key_Right:
                    //forward 5 secs
                    mMediaPlayer->setPosition(mMediaPlayer->position() + 5000);
                    return true;
                case Qt::Key_Space:
                    ui->playButton->click();
                    return true;
                default:
                    break; //do nothing
            }
            break;
        case QEvent::MouseButtonDblClick:
            if (static_cast<QMouseEvent *>(evt)->button() == Qt::LeftButton)
            {
                ui->vidWidget->setFullScreen(!ui->vidWidget->isFullScreen());
                return true;
            }
            break;
        default:
            break; //do nothing
    }
    return QDialog::eventFilter(obj, evt);
}

void AudioPlayer::onAudioSliderMoved(int pos)
{
    //set player position only during playback
    if (ui->playButton->isChecked())
        mMediaPlayer->setPosition(pos);
    else if (ui->idLabel->isVisible())
        showTxParty(pos/1000);
}

void AudioPlayer::resetUi()
{
    ui->audioSlider->setValue(0);
    ui->playButton->setChecked(false);
}

inline QString AudioPlayer::getTimeString(int time)
{
    return QTime(0, 0).addMSecs(time)
                      .toString((time > 3600000)? "h:mm:ss": "mm:ss");
}

bool AudioPlayer::getPttData(const QString &callKey)
{
    auto *res = DbInt::instance().getPttHistory(callKey.toStdString());
    if (res == 0)
        return false;
    auto *mdl = new QStandardItemModel();
    QString tx;
    string txParty;
    int start;
    int dur;
    int row = 0;
    int i = res->getNumRows() - 1;
    for (; i>=0; --i)
    {
        if (res->getFieldValue(DbInt::FIELD_TX_PARTY, txParty, i) &&
            res->getFieldValue(DbInt::FIELD_START, start, i) &&
            res->getFieldValue(DbInt::FIELD_CALL_DURATION, dur, i))
        {
            tx = QString::fromStdString(txParty);
            mPttHistory.append(PttData(tx, start, start + dur));
            mdl->setItem(row, QtTableUtils::COL_PTT_TIME,
                         new QStandardItem(getTimeString(start * 1000)));
            mdl->setItem(row, QtTableUtils::COL_PTT_SECONDS,
                         new QStandardItem(QString::number(dur)));
            mdl->setItem(row++, QtTableUtils::COL_PTT_TX,
                         new QStandardItem(tx));
        }
    }
    delete res;
    if (row == 0)
    {
        delete mdl;
        return false;
    }
    QtTableUtils::setupTable(QtTableUtils::TBLTYPE_PTT, mdl, ui->pttTable);
    return true;
}

void AudioPlayer::showTxParty(int position)
{
    auto it = mPttHistory.begin();
    for (; it!=mPttHistory.end(); ++it)
    {
        if (position >= it->start && position <= it->end)
        {
            ui->idLabel->setText(it->txParty);
            break;
        }
    }
    if (it == mPttHistory.end())
        ui->idLabel->setText("");
}

void AudioPlayer::setVisibility(bool visible)
{
    static QRect geo; //last position
    if (visible)
    {
        if (!geo.isNull()) //null in first call
            setGeometry(geo);
        show();
    }
    else if (isVisible())
    {
        geo = geometry();
        hide();
    }
}
