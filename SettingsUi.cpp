/**
 * The Settings UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: SettingsUi.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Mazdiana Makmor
 */
#include <assert.h>
#ifdef DEBUG
#include <QDir>
#else
#include <QStandardPaths>
#endif //DEBUG
#include <QFileDialog>
#include <QIntValidator>
#include <QMessageBox>
#include <QStringList>

#include "AudioDevice.h"
#include "InputDialog.h"
#include "Props.h"
#include "ResourceData.h"
#include "Settings.h"
#include "Style.h"
#include "Version.h"
#include "VideoDevice.h"
#include "ui_SettingsUi.h"
#include "SettingsUi.h"

using namespace std;

enum
{
    //must be in UI page widget order
    PAGE_GENERAL,
    PAGE_RESOURCE,
    PAGE_SERVER,
    PAGE_MAP,
    PAGE_MEDIA,
    PAGE_ABOUT
};

static const QString GROUP_GENERAL ("General");
static const QString GROUP_RESOURCE("Resource");
static const QString GROUP_MAP     ("Map");
static const QString GROUP_SERVER  ("Server");
static const QString ORGANIZATION  ("SST");

static const char CFGDELIM(','); //delimiter between cfg values
static const char VALDELIM(' '); //delimiter within cfg value

#define SETOBJNAME(obj, key) \
    ui->obj->setObjectName(QString::number(Props::FLD_CFG_##key))

#define RESTOREVAL(key) \
    QString::fromStdString(cfg.get<string>(Props::FLD_CFG_##key))

#define RESTOREBOOL(key) cfg.get<bool>(Props::FLD_CFG_##key)

SettingsUi::SettingsUi(QWidget *parent) :
QDialog(parent), ui(new Ui::SettingsUi), mLogger(0), mSession(0), mInDevice(0),
mOutDevice(0)
{
#ifdef SETTINGS_INIFILE
    mPath.append("/").append(Version::APP_TITLE).append(".ini");
#else
    mPath.append("HKEY_LOCAL_MACHINE\\Software\\").append(ORGANIZATION)
         .append("\\").append(Version::APP_TITLE).append("\\");
#endif
    ui->setupUi(this);
    ui->buttonsFrame
      ->setStyleSheet("background-color:" +
                      Style::getStyle(Style::OBJ_BACKGROUND_PANEL));
    ui->themeCombo->addItems(Style::getThemeNames());
    ui->grpCallAutoJoinCheck->setText(
               tr("Auto-Join Group Call\n(always enabled for Broadcast Call)"));
    ui->grpCallAutoJoinCheck->setToolTip(
        tr("Automatically join a monitored group call if the concurrent call "
           "limit has not been reached.\nThis is always enabled for a "
           "Broadcast call regardless of call limit."));
    ui->monRetainCheck->setText(tr("Remember Monitoring"));
    ui->monRetainCheck->setToolTip(
        tr("Remember the monitored groups and subscribers to restore upon next "
           "login."));
    ui->grpDspCombo
      ->addItems(ResourceData::dspOptLabels(ResourceData::TYPE_GROUP));
    QStringList l(ResourceData::dspOptLabels(ResourceData::TYPE_SUBSCRIBER));
    ui->subsDspCombo->addItems(l);
    ui->termLblCombo->addItems(l);
    ui->termChangeEdit->setValidator(new QIntValidator(ui->termChangeEdit));
    ui->termRemoveEdit->setValidator(ui->termChangeEdit->validator());
    ui->termChangeEdit->setToolTip(tr("0 to disable, otherwise minimum %1")
                                   .arg(Settings::TERMINAL_TIMER_MIN_MINUTES));
    ui->termRemoveEdit->setToolTip(ui->termChangeEdit->toolTip() +
                                   tr(" plus '%1'")
                                       .arg(ui->termChangeLabel->text()));
    ui->ctrRscCheck->setText(tr("Center On Resource In Call"));
    ui->ctrRscCheck->setToolTip(tr("Automatically center map view on the most "
                                   "recent highlighted resource in a call."));
#ifdef GIS_SEAMAP
    ui->seaMapCheck->setText(tr("Enable Sea Map Layers"));
    ui->seaMapCheck->setToolTip(tr("Include sea map in layer list."));
#else
    ui->seaMapCheck->setVisible(false);
#endif
    //remove the help button from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QCoreApplication::setOrganizationName(ORGANIZATION);
    QCoreApplication::setApplicationName(Version::APP_TITLE);
    ui->titleLabel->setText(Version::APP_NAME_VERSION + '\n' +
                            Version::copyright());
    ui->registrationLabel->hide(); //hide license info label for now
    load();
    setStyle();
    SETOBJNAME(logEdit, LOGFILE);
    SETOBJNAME(dlEdit, MMS_DOWNLOADDIR);
    SETOBJNAME(pttEdit, PTT_CHAR);
    SETOBJNAME(serverIpEdit, SERVERIP);
    SETOBJNAME(serverPortEdit, SERVERPORT);
    SETOBJNAME(logLevelCombo, LOGLEVEL);
    SETOBJNAME(themeCombo, COLORTHEME);
    SETOBJNAME(grpCallAutoJoinCheck, GRPCALL_AUTOJOIN);
    SETOBJNAME(monRetainCheck, MONITOR_RETAIN);
    SETOBJNAME(pttCtrlCheck, PTT_CTRL);
    SETOBJNAME(pttAltCheck, PTT_ALT);
    SETOBJNAME(grpDspCombo, RSC_DSP_GRP);
    SETOBJNAME(subsDspCombo, RSC_DSP_SUBS);
    SETOBJNAME(termLblCombo, MAP_TERM_LBL);
    SETOBJNAME(termChangeEdit, MAP_TERM_STALE1);
    SETOBJNAME(termRemoveEdit, MAP_TERM_STALELAST);
    SETOBJNAME(ctrRscCheck, MAP_CTR_RSC_CALL);
    SETOBJNAME(seaMapCheck, MAP_SEA);
    SETOBJNAME(micCombo, AUDIO_IN);
    SETOBJNAME(speakerCombo, AUDIO_OUT);
    SETOBJNAME(camCombo, CAMERA);
    SETOBJNAME(resCombo, CAMERA_RES);
    connect(ui->logEdit, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->dlEdit, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->dlClrButton, &QToolButton::clicked, this,
            [this] { ui->dlEdit->clear(); });
    connect(ui->dlBrowseButton, &QPushButton::clicked, this,
            [this] { selectDir(ui->dlEdit, true); });
    connect(ui->pttEdit, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->serverIpEdit, SIGNAL(textEdited(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->serverPortEdit, SIGNAL(textEdited(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->logLevelCombo, SIGNAL(currentTextChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->termChangeEdit, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->termRemoveEdit, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->themeCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(onComboIndexChanged(int)));
    connect(ui->grpDspCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(onComboIndexChanged(int)));
    connect(ui->subsDspCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(onComboIndexChanged(int)));
    connect(ui->termLblCombo, SIGNAL(currentIndexChanged(int)),
            SLOT(onComboIndexChanged(int)));
    connect(ui->grpCallAutoJoinCheck, SIGNAL(clicked(bool)),
            SLOT(onCheckBoxClicked(bool)));
    connect(ui->monRetainCheck, SIGNAL(clicked(bool)),
            SLOT(onCheckBoxClicked(bool)));
    connect(ui->pttAltCheck, SIGNAL(clicked(bool)),
            SLOT(onCheckBoxClicked(bool)));
    connect(ui->pttCtrlCheck, &QCheckBox::clicked, this,
            [this](bool checked)
            {
                onCheckBoxClicked(checked);
                //Ctrl must always be with Alt
                if (checked)
                {
                    if (!ui->pttAltCheck->isChecked())
                        ui->pttAltCheck->click();
                    ui->pttAltCheck->setDisabled(true);
                }
                else
                {
                    ui->pttAltCheck->setEnabled(true);
                }
            });
    connect(ui->ctrRscCheck, SIGNAL(clicked(bool)),
            SLOT(onCheckBoxClicked(bool)));
    connect(ui->seaMapCheck, SIGNAL(clicked(bool)),
            SLOT(onCheckBoxClicked(bool)));
    delete ui->mapLabel;
    delete ui->mapEdit;
    delete ui->browseButton;
    setPage();
    ui->audioInBar->setMaximum(AudioDevice::getMaxPeak());
    connect(ui->generalButton, &QToolButton::clicked, this,
            [this] { setPage(PAGE_GENERAL); });
    connect(ui->resourceButton, &QToolButton::clicked, this,
            [this] { setPage(PAGE_RESOURCE); });
    connect(ui->aboutButton, &QToolButton::clicked, this,
            [this] { setPage(PAGE_ABOUT); });
    connect(ui->serverButton, &QToolButton::clicked, this,
            [this] { setPage(PAGE_SERVER); });
    connect(ui->mapButton, &QToolButton::clicked, this,
            [this] { setPage(PAGE_MAP); });
    connect(ui->mediaButton, &QToolButton::clicked, this,
            [this]
            {
                setPage(PAGE_MEDIA);
                if (mInDevice == 0)
                {
                    mInDevice = new AudioDevice(AudioDevice::TYPE_INPUT,
                                                mLogger);
                    connect(mInDevice, &AudioDevice::received, this,
                            [this](const char *data, qint64 len)
                            {
                                //set the audio progress bar value
                                ui->audioInBar
                                  ->setValue(AudioDevice::getPeak(data, len));
                            });
                    ui->micCombo->addItems(mInDevice->getDeviceNames());
                    int idx = ui->micCombo->findText(QString::fromStdString(
                                  Settings::instance().get<string>(
                                                     Props::FLD_CFG_AUDIO_IN)));
                    if (idx != -1)
                        ui->micCombo->setCurrentIndex(idx);
                }
                else if (mInDevice->getState() != QAudio::ActiveState)
                {
                    mInDevice->start();
                }
                if (mOutDevice == 0)
                {
                    mOutDevice = new AudioDevice(AudioDevice::TYPE_OUTPUT,
                                                 mLogger);
                    ui->speakerCombo->addItems(mOutDevice->getDeviceNames());
                    int idx = ui->speakerCombo->findText(QString::fromStdString(
                                  Settings::instance().get<string>(
                                                    Props::FLD_CFG_AUDIO_OUT)));
                    if (idx != -1)
                        ui->speakerCombo->setCurrentIndex(idx);
                }
#ifdef NO_VIDEO
                ui->camLabel->hide();
                ui->camCombo->hide();
                ui->resLabel->hide();
                ui->resCombo->hide();
#else
                auto &dev(VideoDevice::instance());
                ui->camCombo->setEnabled(dev.isValid() && !dev.isBusy());
                ui->resCombo->setEnabled(dev.isValid() && !dev.isBusy());
                if (!dev.isValid())
                {
                    ui->camCombo->addItem(tr("Unavailable"));
                    ui->resCombo->addItem(tr("Unavailable"));
                    return;
                }
                if (ui->camCombo->count() == 0)
                {
                    //first load
                    ui->camCombo->addItems(dev.getDeviceNames());
                    int idx = ui->camCombo->findText(QString::fromStdString(
                                  Settings::instance().get<string>(
                                                       Props::FLD_CFG_CAMERA)));
                    if (idx != -1)
                        ui->camCombo->setCurrentIndex(idx);
                }
                if (ui->camCombo->isEnabled())
                {
                    dev.setCallback(this, previewCb);
                    dev.setCamera(this, true);
                }
#endif
            });
    connect(ui->okButton, &QPushButton::clicked, this,
            [this]
            {
                //save current data and close window
                if (save())
                {
                    hide();
                    ui->generalButton->click();
                    ui->okButton->setAttribute(Qt::WA_UnderMouse, false);
                    emit isFinished(true);
                }
            });
    connect(ui->cancelButton, &QPushButton::clicked, this,
            [this]
            {
                //close window without saving changes
                hide();
                ui->generalButton->click();
                ui->cancelButton->setAttribute(Qt::WA_UnderMouse, false);
                Settings &cfg(Settings::instance());
                ui->logEdit->setText(RESTOREVAL(LOGFILE));
                ui->logLevelCombo->setCurrentText(RESTOREVAL(LOGLEVEL));
                ui->dlEdit->setText(RESTOREVAL(MMS_DOWNLOADDIR));
                ui->themeCombo->setCurrentIndex(RESTOREVAL(COLORTHEME).toInt());
                ui->grpCallAutoJoinCheck
                  ->setChecked(RESTOREBOOL(GRPCALL_AUTOJOIN));
                ui->monRetainCheck->setChecked(RESTOREBOOL(MONITOR_RETAIN));
                ui->pttCtrlCheck->setChecked(RESTOREBOOL(PTT_CTRL));
                ui->pttAltCheck->setChecked(RESTOREBOOL(PTT_ALT));
                ui->pttEdit->setText(RESTOREVAL(PTT_CHAR));
                ui->serverIpEdit->setText(RESTOREVAL(SERVERIP));
                ui->serverPortEdit->setText(RESTOREVAL(SERVERPORT));
                ui->grpDspCombo
                  ->setCurrentIndex(RESTOREVAL(RSC_DSP_GRP).toInt());
                ui->subsDspCombo
                  ->setCurrentIndex(RESTOREVAL(RSC_DSP_SUBS).toInt());
                ui->termLblCombo
                  ->setCurrentIndex(RESTOREVAL(MAP_TERM_LBL).toInt());
                ui->termChangeEdit->setText(RESTOREVAL(MAP_TERM_STALE1));
                ui->termRemoveEdit->setText(RESTOREVAL(MAP_TERM_STALELAST));
                ui->ctrRscCheck->setChecked(RESTOREBOOL(MAP_CTR_RSC_CALL));
                ui->seaMapCheck->setChecked(RESTOREBOOL(MAP_SEA));
                ui->camCombo->setCurrentText(RESTOREVAL(CAMERA));
                ui->resCombo->setCurrentText(RESTOREVAL(CAMERA_RES));
                mChangedKeys.clear();
                ui->okButton->setEnabled(false);
                ui->applyButton->setEnabled(false);
                if (mInDevice != 0)
                    mInDevice->stop();
                if (mOutDevice != 0 && ui->playButton->isChecked())
                    ui->playButton->click();
                if (ui->camCombo->isEnabled())
                {
                    auto &dev(VideoDevice::instance());
                    dev.removeCallback(this);
                    dev.setCamera(this, false);
                }
                emit isFinished(false);
            });
    connect(ui->applyButton, &QPushButton::clicked, this,
            [this]
            {
                //save current data
                if (save())
                    emit isFinished(true);
            });
    connect(ui->logBrowseButton, &QPushButton::clicked, this,
            [this]
            {
                //show Open file dialog
                QString filter(tr("Text Files (*.log"));
                QString fn(ui->logEdit->text().trimmed());
                if (!fn.isEmpty())
                {
                    QString ext(QFileInfo(fn).suffix());
                    if (!ext.isEmpty() && ext != "log")
                        filter.append(" *.").append(ext);
                }
                filter.append(")");
                fn = QFileDialog::getSaveFileName(this, tr("Select File"), fn,
                                             filter, 0,
                                             QFileDialog::DontConfirmOverwrite);
                if (!fn.isEmpty())
                    ui->logEdit->setText(fn);
            });
    connect(ui->logLevelDefButton, &QPushButton::clicked, this,
            [this]
            {
                ui->logLevelCombo->setCurrentText(
                            QString::fromStdString(mLogger->getDefaultLevel()));
            });
    connect(ui->gpsMonButton, &QPushButton::clicked, this,
            [this] { emit gpsMon(); });
    connect(ui->branchButton, &QPushButton::clicked, this,
            [this]
            {
                //show Branch Selection dialog
                SubsData::BranchMapT branches;
                if (!SubsData::getBranchData(branches))
                {
                    //SubsData not ready, read from config
                    vector<string> data;
                    Settings &cfg(Settings::instance());
                    if (cfg.getList(Props::FLD_CFG_BRANCH_ALLOWED, data,
                                    CFGDELIM) != 0)
                    {
                        int id;
                        string name; //in hex
                        istringstream is;
                        for (auto &s : data)
                        {
                            if (Utils::trim(s) == 0)
                                continue;
                            is.clear();
                            is.str(s);
                            if (is >> id >> name)
                                branches[id] = name;
                        }
                    }
                    if (branches.empty())
                        return; //no allowed branch
                }
                QSet<int> ids;
                if (mBranches == "-")
                {
                    ids.insert(-1); //select all
                }
                else
                {
                    for (auto &it : mBranches.split(','))
                    {
                        ids.insert(it.toInt());
                    }
                }
                auto *d = new InputDialog(branches, ids, this);
                if (d->exec() == QDialog::Accepted)
                {
                    auto s(d->getSelectedBranches());
                    if (mBranches != s)
                    {
                        mBranches = s;
                        setModState(Props::FLD_CFG_BRANCH, true);
                    }
                }
                delete d;
            });
    connect(ui->changePswdButton, &QPushButton::clicked, this,
            [this]
            {
                //show Change Password dialog
                auto *d = new InputDialog(InputDialog::TYPE_CHANGE_PASSWORD,
                                          this);
                if (d->exec() == QDialog::Accepted)
                {
                    if (mSession != 0)
                    {
                        QString currP;
                        QString newP;
                        if (d->getPasswords(currP, newP))
                            mSession->changePasswd(currP.toStdString(),
                                                   newP.toStdString());
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("Change Password Error"),
                                              tr("Unable to proceed due to "
                                                 "server disconnection."));
                    }
                }
                delete d;
            });
    connect(ui->playButton, &QPushButton::clicked, this,
            [this](bool checked)
            {
                //start/stop audio output testing
                if (!checked)
                    mOutDevice->stop();
                else if (mOutDevice->getState() != QAudio::ActiveState)
                    mOutDevice->start();
                mOutDevice->testOutput(checked);
            });
    connect(ui->micCombo,
            qOverload<const QString &>(&QComboBox::currentIndexChanged), this,
            [this](const QString &name)
            {
                //set the audio input device
                ui->micCombo->setToolTip(name);
                mInDevice->setDevice(name);
                mInDevice->start();
                int k = QObject::sender()->objectName().toInt();
                setModState(k,
                            (Settings::instance().get<string>(k) !=
                             name.toStdString()));
            });
    connect(ui->speakerCombo,
            qOverload<const QString &>(&QComboBox::currentIndexChanged), this,
            [this](const QString &name)
            {
                //set the audio output device
                ui->speakerCombo->setToolTip(name);
                mOutDevice->setDevice(name);
                if (ui->playButton->isChecked())
                    mOutDevice->start();
                int k = QObject::sender()->objectName().toInt();
                setModState(k,
                            (Settings::instance().get<string>(k) !=
                             name.toStdString()));
            });
    connect(ui->camCombo,
            qOverload<const QString &>(&QComboBox::currentIndexChanged), this,
            [this](const QString &name)
            {
                //set the video input device
                ui->videoLabel->clear();
                ui->camCombo->setToolTip(name);
                auto &dev(VideoDevice::instance());
                if (ui->camCombo->isEnabled())
                    dev.setDevice(name);
                ui->resCombo->clear();
                ui->resCombo->addItems(dev.getResolutionList());
                //reset resolution to previous selection
                int idx = ui->resCombo->findText(QString::fromStdString(
                              Settings::instance().get<string>(
                                                   Props::FLD_CFG_CAMERA_RES)));
                if (idx != -1)
                    ui->resCombo->setCurrentIndex(idx);
                int k = QObject::sender()->objectName().toInt();
                setModState(k,
                            (Settings::instance().get<string>(k) !=
                             name.toStdString()));
            });
    connect(ui->resCombo,
            qOverload<const QString &>(&QComboBox::currentIndexChanged), this,
            [this](const QString &res)
            {
                //set the video resolution
                if (res.isEmpty())
                    return;
                if (ui->resCombo->isEnabled())
                    VideoDevice::instance().setResolution(res);
                int k = QObject::sender()->objectName().toInt();
                setModState(k,
                            (Settings::instance().get<string>(k) !=
                             res.toStdString()));
            });
}

SettingsUi::~SettingsUi()
{
    delete mInDevice;
    delete mOutDevice;
    delete ui;
}

void SettingsUi::setLogger(Logger *logger)
{
    mLogger = logger;
    Settings &cfg(Settings::instance());
    string lvl(cfg.get<string>(Props::FLD_CFG_LOGLEVEL));
    if (lvl.empty())
    {
        lvl = logger->getLevel();
    }
    else
    {
        //set the configured level, and if not valid, fix it
        if (!logger->setLevel(lvl))
            lvl = logger->getLevel();
        cfg.set(Props::FLD_CFG_LOGLEVEL, lvl);
    }
    ui->logLevelCombo
      ->addItems(QString::fromStdString(logger->getValidLevels()).split("|"));
    //select the actual level in use
    ui->logLevelCombo->setCurrentText(QString::fromStdString(lvl));
    ui->logLevelCombo->setEnabled(true);
    ui->logLevelDefButton->setEnabled(true);
}

#define GETVAL(key) \
    do \
    { \
        qStr = qs.value(QString::fromStdString( \
                                   Props::getFieldName(Props::FLD_CFG_##key))) \
                 .toString(); \
        cfg.set(Props::FLD_CFG_##key, qStr.toStdString()); \
    } \
    while (0)

#define GETBOOL(key) \
    do \
    { \
        flag = qs.value(QString::fromStdString( \
                                   Props::getFieldName(Props::FLD_CFG_##key))) \
                 .toBool(); \
        cfg.set(Props::FLD_CFG_##key, flag); \
    } \
    while (0)

#define GETPATH(key) \
    do \
    { \
        qStr = qs.value(QString::fromStdString( \
                                   Props::getFieldName(Props::FLD_CFG_##key))) \
                 .toString().trimmed(); \
        if (validatePath(qStr)) \
            cfg.set(Props::FLD_CFG_##key, qStr.toStdString()); \
    } \
    while (0)

#define GETFILE(key, cond) \
    do \
    { \
        qStr = qs.value(QString::fromStdString( \
                                   Props::getFieldName(Props::FLD_CFG_##key))) \
                 .toString().trimmed(); \
        flag = validateFileName(qStr, cond); \
        if (flag) \
            cfg.set(Props::FLD_CFG_##key, qStr.toStdString()); \
    } \
    while (0)

void SettingsUi::load()
{
#ifdef SETTINGS_INIFILE
#ifdef DEBUG
    QSettings qs(QDir::currentPath() + mPath, QSettings::IniFormat);
#else
    QSettings qs(QStandardPaths::writableLocation(
                                            QStandardPaths::AppConfigLocation) +
                     mPath,
                 QSettings::IniFormat);
#endif //DEBUG
#else
    QSettings qs(QSettings::NativeFormat, QSettings::SystemScope, ORGANIZATION,
                 Version::APP_TITLE);
#endif
    Settings &cfg(Settings::instance());
    QString   qStr;
    bool      flag;
    qs.beginGroup(GROUP_GENERAL);
    GETFILE(LOGFILE, true);
    if (flag)
        ui->logEdit->setText(qStr);
    GETVAL(LOGLEVEL);
    GETVAL(MSG_TMR_INTERVAL);
    int readValue = qStr.toInt();
    //if enabled, must be within [10,50] ms
    if ((readValue >= 0 && readValue < 10) || readValue > 50)
        cfg.set(Props::FLD_CFG_MSG_TMR_INTERVAL, 20); //default is 20ms
    GETPATH(MMS_DOWNLOADDIR);
    ui->dlEdit->setText(qStr);
    GETVAL(AUDIO_IN);
    GETVAL(AUDIO_OUT);
    GETVAL(CAMERA);
    GETVAL(CAMERA_RES);
    GETVAL(BRANCH);
    GETVAL(BRANCH_ALLOWED);
    GETVAL(COLORTHEME);
    readValue = qStr.toInt();
    Style::setTheme(readValue);
    int value = Style::getTheme();
    ui->themeCombo->setCurrentIndex(value);
    if (value != readValue) //invalid readValue - save correct one
        cfg.set(Props::FLD_CFG_COLORTHEME, value);
    GETBOOL(GRPCALL_AUTOJOIN);
    ui->grpCallAutoJoinCheck->setChecked(flag);
    GETVAL(CONTACTS);
    GETBOOL(MONITOR_RETAIN);
    ui->monRetainCheck->setChecked(flag);
    GETVAL(GPS_MON);
    GETBOOL(PTT_CTRL);
    ui->pttCtrlCheck->setChecked(flag);
    if (flag)
    {
        //Ctrl must always be with Alt
        cfg.set(Props::FLD_CFG_PTT_ALT, true);
        ui->pttAltCheck->setChecked(true);
        ui->pttAltCheck->setDisabled(true);
    }
    else
    {
        GETBOOL(PTT_ALT);
        ui->pttAltCheck->setChecked(flag);
    }
    GETVAL(PTT_CHAR);
    ui->pttEdit->setText(qStr);
    GETVAL(HELPDESKNUM);
    GETVAL(SDSTEMPLATE);
    GETVAL(INC_ICONDIR);
    GETVAL(INCFILTER_ADDRSTATE);
    GETVAL(INCFILTER_CATEGORY);
    GETVAL(INCFILTER_PRIORITY);
    GETVAL(INCFILTER_STATE);
    qs.endGroup();
    qs.beginGroup(GROUP_RESOURCE);
    GETVAL(RSC_DSP_GRP);
    readValue = qStr.toInt();
    value = ResourceData::setGrpDspOpt(readValue);
    ui->grpDspCombo->setCurrentIndex(value);
    if (value != readValue) //invalid readValue - save correct one
        cfg.set(Props::FLD_CFG_RSC_DSP_GRP, value);
    GETVAL(RSC_DSP_SUBS);
    readValue = qStr.toInt();
    value = ResourceData::setSubsDspOpt(readValue);
    ui->subsDspCombo->setCurrentIndex(value);
    if (value != readValue) //invalid readValue - save correct one
        cfg.set(Props::FLD_CFG_RSC_DSP_SUBS, value);
    GETVAL(MAP_TERM_LBL);
    readValue = qStr.toInt();
    value = ResourceData::setMapSubsDspOpt(readValue);
    ui->termLblCombo->setCurrentIndex(value);
    if (value != readValue) //invalid readValue - save correct one
        cfg.set(Props::FLD_CFG_MAP_TERM_LBL, value);
    qs.endGroup();
    qs.beginGroup(GROUP_SERVER);
    GETVAL(SERVERIP);
    ui->serverIpEdit->setText(qStr);
    GETVAL(SERVERPORT);
    ui->serverPortEdit->setText(qStr);
    qs.endGroup();
    qs.beginGroup(GROUP_MAP);
    GETVAL(MAP_TERM_STALE1);
    if (checkNegative(true, ui->termChangeEdit, qStr))
        cfg.set(Props::FLD_CFG_MAP_TERM_STALE1, qStr.toStdString());
    GETVAL(MAP_TERM_STALELAST);
    if (checkNegative(true, ui->termRemoveEdit, qStr))
        cfg.set(Props::FLD_CFG_MAP_TERM_STALELAST, qStr.toStdString());
    GETBOOL(MAP_CTR_RSC_CALL);
    ui->ctrRscCheck->setChecked(flag);
    GETBOOL(MAP_SEA);
    ui->seaMapCheck->setChecked(flag);
    GETVAL(MAP_MAXSCALE);
    qs.endGroup();
    if (!cfg.validate(value))
        showSettingsError(value);
}

#define SETVAL_UNC(key, val, isValid) \
    do \
    { \
        if (isValid && \
            cfg.validate(Props::FLD_CFG_##key, val.toStdString(), true)) \
        { \
            qs.setValue(QString::fromStdString( \
                                   Props::getFieldName(Props::FLD_CFG_##key)), \
                        val); \
            isChanged = true; \
        } \
        else \
        { \
            qs.endGroup(); \
            showSettingsError(Props::FLD_CFG_##key); \
            return false; \
        } \
    } \
    while (0)

#define SETVAL(key, val, isValid) \
    do \
    { \
        isChanged = false; \
        oldVal = qs.value(QString::fromStdString( \
                                   Props::getFieldName(Props::FLD_CFG_##key))) \
                   .toString(); \
        if (oldVal.isEmpty() || val != oldVal) \
        { \
            SETVAL_UNC(key, val, isValid); \
        } \
    } \
    while (0)

#define SETBOOL(key, flag) \
    do \
    { \
        cfg.set(Props::FLD_CFG_##key, flag); \
        qs.setValue(QString::fromStdString( \
                                   Props::getFieldName(Props::FLD_CFG_##key)), \
                    flag); \
    } \
    while (0)

bool SettingsUi::save()
{
#ifdef SETTINGS_INIFILE
#ifdef DEBUG
    QSettings qs(QDir::currentPath() + mPath, QSettings::IniFormat);
#else
    QSettings qs(QStandardPaths::writableLocation(
                                            QStandardPaths::AppConfigLocation) +
                     mPath,
                 QSettings::IniFormat);
#endif //DEBUG
#else
    QSettings qs(QSettings::NativeFormat, QSettings::SystemScope, ORGANIZATION,
                 Version::APP_TITLE);
#endif
    Settings &cfg(Settings::instance());
    bool isChanged = false;
    QString oldVal;
    qs.beginGroup(GROUP_GENERAL);
    QString qStr(ui->logEdit->text().trimmed());
    SETVAL(LOGFILE, qStr, validateFileName(qStr, true));
    if (mLogger != 0)
    {
        if (isChanged)
            mLogger->setFilename(cfg.get<string>(Props::FLD_CFG_LOGFILE));
        SETVAL(LOGLEVEL, ui->logLevelCombo->currentText(), true);
        if (isChanged)
            mLogger->setLevel(cfg.get<string>(Props::FLD_CFG_LOGLEVEL));
    }
    qStr = ui->dlEdit->text().trimmed();
    SETVAL(MMS_DOWNLOADDIR, qStr, qStr.isEmpty() || validatePath(qStr));
    //isChanged is true even when unchanged if both qStr & oldVal are empty
    if (isChanged && (!qStr.isEmpty() || !oldVal.isEmpty()))
    {
        cfg.set(Props::FLD_CFG_MMS_DOWNLOADDIR, qStr.toStdString());
        emit mmsDownloadDirChanged();
    }
    qStr = ui->micCombo->currentText();
    if (!qStr.isEmpty())
    {
        SETVAL(AUDIO_IN, qStr, true);
        if (isChanged)
            emit audioInChanged(qStr);
    }
    qStr = ui->speakerCombo->currentText();
    if (!qStr.isEmpty())
    {
        SETVAL(AUDIO_OUT, qStr, true);
        if (isChanged)
            emit audioOutChanged(qStr);
    }
    qStr = ui->camCombo->currentText();
    if (!qStr.isEmpty())
        SETVAL(CAMERA, qStr, true);
    qStr = ui->resCombo->currentText();
    if (!qStr.isEmpty())
        SETVAL(CAMERA_RES, qStr, true);
    int value = ui->themeCombo->currentIndex();
    if (Style::setTheme(value))
    {
        SETVAL(COLORTHEME, QString::number(value), true);
        setStyle();
        emit setTheme();
    }
    SETBOOL(GRPCALL_AUTOJOIN, ui->grpCallAutoJoinCheck->isChecked());
    //cannot use macro because entry accepts empty string
    qs.setValue(QString::fromStdString(
                                  Props::getFieldName(Props::FLD_CFG_CONTACTS)),
                QString::fromStdString(
                                     cfg.get<string>(Props::FLD_CFG_CONTACTS)));
    qs.setValue(QString::fromStdString(
                            Props::getFieldName(Props::FLD_CFG_BRANCH_ALLOWED)),
                QString::fromStdString(
                               cfg.get<string>(Props::FLD_CFG_BRANCH_ALLOWED)));
    if (mChangedKeys.count(Props::FLD_CFG_BRANCH) != 0)
    {
        //save from dialog
        qs.setValue(QString::fromStdString(
                                    Props::getFieldName(Props::FLD_CFG_BRANCH)),
                    mBranches);
        cfg.set(Props::FLD_CFG_BRANCH, mBranches.toStdString());
        emit branchChanged();
    }
    else
    {
        qs.setValue(QString::fromStdString(
                                    Props::getFieldName(Props::FLD_CFG_BRANCH)),
                    QString::fromStdString(
                                       cfg.get<string>(Props::FLD_CFG_BRANCH)));
    }
    SETBOOL(MONITOR_RETAIN, ui->monRetainCheck->isChecked());
    qs.setValue(QString::fromStdString(
                                   Props::getFieldName(Props::FLD_CFG_GPS_MON)),
                QString::fromStdString(
                                      cfg.get<string>(Props::FLD_CFG_GPS_MON)));
    SETBOOL(PTT_CTRL, ui->pttCtrlCheck->isChecked());
    SETBOOL(PTT_ALT, ui->pttAltCheck->isChecked());
    SETVAL(PTT_CHAR, ui->pttEdit->text(), true);
    qStr = QString::fromStdString(cfg.get<string>(Props::FLD_CFG_HELPDESKNUM));
    if (!qStr.isEmpty())
        SETVAL(HELPDESKNUM, qStr, true);
    SETVAL(SDSTEMPLATE,
           QString::fromStdString(cfg.get<string>(Props::FLD_CFG_SDSTEMPLATE)),
           true);
    SETVAL(INC_ICONDIR,
           QString::fromStdString(cfg.get<string>(Props::FLD_CFG_INC_ICONDIR)),
           true);
    SETVAL(INCFILTER_ADDRSTATE,
           QString::fromStdString(cfg.get<string>(
                                           Props::FLD_CFG_INCFILTER_ADDRSTATE)),
           true);
    SETVAL(INCFILTER_CATEGORY,
           QString::fromStdString(cfg.get<string>(
                                            Props::FLD_CFG_INCFILTER_CATEGORY)),
           true);
    SETVAL(INCFILTER_PRIORITY,
           QString::fromStdString(cfg.get<string>(
                                            Props::FLD_CFG_INCFILTER_PRIORITY)),
           true);
    SETVAL(INCFILTER_STATE,
           QString::fromStdString(cfg.get<string>(
                                               Props::FLD_CFG_INCFILTER_STATE)),
           true);
    SETVAL(MSG_TMR_INTERVAL,
           QString::fromStdString(cfg.get<string>(
                                              Props::FLD_CFG_MSG_TMR_INTERVAL)),
           true);
    qs.endGroup();
    qs.beginGroup(GROUP_RESOURCE);
    value = ui->grpDspCombo->currentIndex();
    SETVAL(RSC_DSP_GRP, QString::number(value), true);
    if (isChanged)
    {
        ResourceData::setGrpDspOpt(value);
        emit rscDspOptChanged(ResourceData::TYPE_GROUP);
    }
    value = ui->subsDspCombo->currentIndex();
    SETVAL(RSC_DSP_SUBS, QString::number(value), true);
    if (isChanged)
    {
        ResourceData::setSubsDspOpt(value);
        emit rscDspOptChanged(ResourceData::TYPE_SUBSCRIBER);
    }
    value = ui->termLblCombo->currentIndex();
    SETVAL(MAP_TERM_LBL, QString::number(value), true);
    if (isChanged)
    {
        ResourceData::setMapSubsDspOpt(value);
        emit terminalLblOptChanged();
    }
    qs.endGroup();
    qs.beginGroup(GROUP_SERVER);
    SETVAL(SERVERIP, ui->serverIpEdit->text().trimmed(), true);
    SETVAL(SERVERPORT, ui->serverPortEdit->text().trimmed(), true);
    qs.endGroup();
    qs.beginGroup(GROUP_MAP);
    checkNegative(false, ui->termChangeEdit, qStr);
    SETVAL(MAP_TERM_STALE1, qStr, true);
    checkNegative(false, ui->termRemoveEdit, qStr);
    if (isChanged)
    {
        //must check STALELAST even if unchanged, because the new STALE1 may
        //make STALELAST invalid
        SETVAL_UNC(MAP_TERM_STALELAST, qStr, true);
        emit terminalCheckTimeChanged();
    }
    else
    {
        SETVAL(MAP_TERM_STALELAST, qStr, true);
        if (isChanged)
            emit terminalCheckTimeChanged();
    }
    SETBOOL(MAP_CTR_RSC_CALL, ui->ctrRscCheck->isChecked());
    SETBOOL(MAP_SEA, ui->seaMapCheck->isChecked());
    qs.endGroup();
    qs.sync();
    QString err;
    switch (qs.status())
    {
        case QSettings::AccessError:
            err = "Access Error";
            break;
        case QSettings::FormatError:
            err = "Format Error";
            break;
        case QSettings::NoError:
        default:
            break; //do nothing
    }
    if (!err.isEmpty())
    {
        LOGGER_ERROR(mLogger, "SettingsUi::save: QSettings "
                     << err.toStdString());
        QMessageBox::critical(this,
                              tr("%1 Settings Error").arg(Version::APP_TITLE),
                              tr("Failed to save settings.\n") + err);
        return false;
    }
    mChangedKeys.clear();
    ui->okButton->setEnabled(false);
    ui->applyButton->setEnabled(false);
    return true;
}

void SettingsUi::setSession(ServerSession *ss)
{
    ui->gpsMonButton->setEnabled(ss != 0);
    ui->changePswdButton->setEnabled(ss != 0);
    mSession = ss;
}

void SettingsUi::setOrganization(const QString &org)
{
    ui->agencyNameLabel->setText(org);
}

void SettingsUi::gpsMonChanged(const string &issiList)
{
    Settings::instance().set(Props::FLD_CFG_GPS_MON, issiList);
    setModState(Props::FLD_CFG_GPS_MON, true);
}

void SettingsUi::setAllowedBranches(const SubsData::BranchMapT &branches)
{
    ostringstream oss;
    string s;
    for (auto &it : branches)
    {
        oss << it.first << VALDELIM << it.second << CFGDELIM;
    }
    s.assign(oss.str());
    if (!s.empty())
        s.pop_back(); //remove trailing CFGDELIM
    Settings::instance().set(Props::FLD_CFG_BRANCH_ALLOWED, s);
}

void SettingsUi::onVideoPreviewReceived(QPixmap frame)
{
    ui->videoLabel->setPixmap(frame);
}

void SettingsUi::previewCb(void *obj, QPixmap frame)
{
    if (obj == 0)
    {
        assert("Bad param in SettingsUi::previewCb" == 0);
        return;
    }
    static_cast<SettingsUi *>(obj)->onVideoPreviewReceived(frame);
}

void SettingsUi::onTextChanged(const QString &text)
{
    int key = QObject::sender()->objectName().toInt();
    setModState(key,
                (Settings::instance().get<string>(key) !=
                 text.trimmed().toStdString()));
}

void SettingsUi::onComboIndexChanged(int idx)
{
    int key = QObject::sender()->objectName().toInt();
    setModState(key, (Settings::instance().get<int>(key) != idx));
}

void SettingsUi::onCheckBoxClicked(bool isChecked)
{
    int key = QObject::sender()->objectName().toInt();
    setModState(key, (Settings::instance().get<bool>(key) != isChecked));
}

void SettingsUi::showEvent(QShowEvent *)
{
    if (ui->stackedWidget->currentWidget() == ui->mediaPage)
        mInDevice->start();
    if (ui->stackedWidget->currentWidget() == ui->generalPage)
    {
        Settings &cfg(Settings::instance());
        ui->branchButton->setEnabled(SubsData::getBranchCount() > 0 ||
                                     !cfg.get<string>(
                                                  Props::FLD_CFG_BRANCH_ALLOWED)
                                         .empty());
        mBranches = QString::fromStdString(cfg.get<string>(
                                                        Props::FLD_CFG_BRANCH));
    }
}

void SettingsUi::setModState(int key, bool isChanged)
{
    if (isChanged)
    {
        mChangedKeys.insert(key);
    }
    else
    {
        mChangedKeys.erase(key);
        isChanged = !mChangedKeys.empty();
    }
    ui->okButton->setEnabled(isChanged);
    ui->applyButton->setEnabled(isChanged);
}

void SettingsUi::setStyle()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    const QString &ss(Style::getStyle(Style::OBJ_TOOLBUTTON));
    ui->generalButton->setStyleSheet(ss);
    ui->resourceButton->setStyleSheet(ss);
    ui->serverButton->setStyleSheet(ss);
    ui->mapButton->setStyleSheet(ss);
    ui->mediaButton->setStyleSheet(ss);
    ui->aboutButton->setStyleSheet(ss);
    ui->titleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->agencyLabel->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_LOGO));
}

void SettingsUi::showSettingsError(int key)
{
    bool doAppend = false;
    QString msg;
    switch (key)
    {
        case Props::FLD_CFG_LOGFILE:
            msg.append(ui->logLabel->text()).append(": ")
               .append(tr("Must be either empty or a filename in an existing "
                          "folder"));
            break;
        case Props::FLD_CFG_MAP_TERM_STALE1:
            msg.append(ui->termChangeLabel->text()).append(": ")
               .append(tr("Must be either 0 or at least %1")
                       .arg(Settings::TERMINAL_TIMER_MIN_MINUTES));
            break;
        case Props::FLD_CFG_MAP_TERM_STALELAST:
            msg.append(ui->termRemoveLabel->text()).append(": ")
               .append(tr("Must be either 0 or at least %1")
                       .arg(Settings::TERMINAL_TIMER_MIN_MINUTES +
                            Settings::instance().get<int>(
                                              Props::FLD_CFG_MAP_TERM_STALE1)));
            break;
        case Props::FLD_CFG_MMS_DOWNLOADDIR:
            msg.append(ui->dlLabel->text()).append(": ")
               .append(tr("Selected folder no longer exists. "
                          "Please select another."));
            break;
        case Props::FLD_CFG_SERVERIP:
            msg = ui->serverIPLabel->text();
            doAppend = true;
            break;
        case Props::FLD_CFG_SERVERPORT:
            msg = ui->portLabel->text();
            doAppend = true;
            break;
        default:
            break; //do nothing
    }
    if (doAppend)
        msg.append(": ").append(tr("Invalid value"));
    QMessageBox::critical(this, tr("%1 Settings Error").arg(Version::APP_TITLE),
                          msg);
}

void SettingsUi::closeEvent(QCloseEvent *event)
{
    event->ignore();
    ui->cancelButton->click();
}

void SettingsUi::selectDir(QLineEdit *le, bool writeable)
{
    assert(le != 0);
    QString dir(QFileDialog::getExistingDirectory(this, tr("Select Folder"),
                                                  le->text() + "/..",
                                                  QFileDialog::ShowDirsOnly));
    if (!dir.isEmpty())
    {
        if (writeable)
        {
            //test file creation in dir
            QFile f(dir + "/z.txt");
            if (f.open(QIODevice::WriteOnly))
            {
                f.remove();
                writeable = false;
            }
        }
        if (!writeable)
            le->setText(dir);
        else
            QMessageBox::critical(this, tr("Folder Error"),
                                  tr("Unable to create file in %1").arg(dir));
    }
}

bool SettingsUi::validatePath(QString &name)
{
    if (name.isEmpty() || !QFileInfo(name).isDir())
        return false;
    if (!name.endsWith('/'))
        name.append('/');
    return true;
}

bool SettingsUi::validateFileName(const QString &name, bool isEmptyValid)
{
    if (name.isEmpty())
        return isEmptyValid;
    QFileInfo fi(name);
    return (!fi.isDir() && !fi.fileName().isEmpty() && fi.dir().exists());
}

bool SettingsUi::checkNegative(bool onLoad, QLineEdit *lnEdit, QString &str)
{
    assert(lnEdit != 0);
    bool retVal = false;
    if (!onLoad)
        str = lnEdit->text(); //no need to trim
    if (str.startsWith('-'))
    {
        str = "0";
        retVal = true;
    }
    if (onLoad || retVal)
        lnEdit->setText(str);
    return retVal;
}

void SettingsUi::setPage(int page)
{
    if (page >= 0)
    {
        auto *btn = ui->generalButton;
        int idx = ui->stackedWidget->currentIndex();
        switch (idx)
        {
            case PAGE_ABOUT:
                btn = ui->aboutButton;
                break;
            case PAGE_MAP:
                btn = ui->mapButton;
                break;
            case PAGE_MEDIA:
                btn = ui->mediaButton;
                break;
            case PAGE_RESOURCE:
                btn = ui->resourceButton;
                break;
            case PAGE_SERVER:
                btn = ui->serverButton;
                break;
            case PAGE_GENERAL:
            default:
                break;
        }
        if (page == idx)
        {
            btn->setChecked(true); //restore
            return; //no change
        }
        btn->setChecked(false); //deactivate the old one
    }
    //activate the new one
    switch (page)
    {
        case PAGE_ABOUT:
            ui->aboutButton->setChecked(true);
            ui->stackedWidget->setCurrentWidget(ui->aboutPage);
            ui->categoryLabel->setText(tr("About"));
            break;
        case PAGE_MAP:
            ui->mapButton->setChecked(true);
            ui->stackedWidget->setCurrentWidget(ui->mapPage);
            ui->categoryLabel->setText(tr("Map Settings"));
            break;
        case PAGE_MEDIA:
            ui->mediaButton->setChecked(true);
            ui->stackedWidget->setCurrentWidget(ui->mediaPage);
            ui->categoryLabel->setText(tr("Media Settings"));
            break;
        case PAGE_RESOURCE:
            ui->resourceButton->setChecked(true);
            ui->stackedWidget->setCurrentWidget(ui->resourcePage);
            ui->categoryLabel->setText(tr("Resource Settings"));
            break;
        case PAGE_SERVER:
            ui->serverButton->setChecked(true);
            ui->stackedWidget->setCurrentWidget(ui->connectionPage);
            ui->categoryLabel->setText(tr("Server Settings"));
            break;
        case PAGE_GENERAL:
        default:
            ui->generalButton->setChecked(true);
            ui->stackedWidget->setCurrentWidget(ui->generalPage);
            ui->categoryLabel->setText(tr("General Settings"));
            break;
    }
    if (page != PAGE_MEDIA && ui->camCombo->isEnabled())
    {
        //stop camera
        auto &dev(VideoDevice::instance());
        if (dev.removeCallback(this))
        {
            dev.setCamera(this, false);
            ui->videoLabel->clear();
        }
    }
}
