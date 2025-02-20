/**
 * The Incident UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Incident.cpp 1862 2024-06-04 06:33:19Z rosnin $
 * @author Mazdiana Makmor
 */
#include <assert.h>
#include <QAction>
#include <QCheckBox>
#include <QGroupBox>
#include <QIntValidator>
#include <QList>
#include <QMenu>
#include <QMessageBox>
#include <QRegExp>
#include <QSet>
#include <QSpinBox>
#include <QTimer>
#include <QToolTip>
#include <QWidgetAction>

#include "CmnTypes.h"
#include "Contact.h"
#include "DateTimeDelegate.h"
#include "Document.h"
#include "GisPoint.h"
#include "IncidentButton.h"
#include "QtTableUtils.h"
#include "QtUtils.h"
#include "Sds.h"
#include "Settings.h"
#include "Style.h"
#include "Utils.h"
#include "ui_Incident.h"
#include "Incident.h"

using namespace std;

static const char   PROP_ID[]  = "FID";
static const string LOGPREFIX("Incident:: ");

#define SETOBJFIELD(obj, field) \
    ui->obj->setProperty(PROP_ID, DbInt::FIELD_INC_##field)

#define ADJUSTDT(curr, ref) \
    do \
    { \
        refDate = ui->ref##Date->date(); \
        if (ui->curr##Date->date() < refDate) \
            ui->curr##Date->setDate(refDate); \
        refTime = ui->ref##Time->time(); \
        if (ui->curr##Date->date() == refDate && \
            ui->curr##Time->time() < refTime) \
            ui->curr##Time->setTime(refTime); \
    } \
    while (0)

Incident::Incident(AudioPlayer     *audioPlayer,
                   Logger          *logger,
                   ActiveIncidents *activeInc,
                   QWidget         *parent) :
QWidget(parent), ui(new Ui::Incident), mActiveInc(activeInc),
mAudioPlayer(audioPlayer), mLogger(logger), mSession(0), mGis(0), mTrmMdl(0),
mMobMdl(0), mTrmList(0), mMobList(0), mData(0), mRscSelEnabled(false)
{
    IncidentData::init();
    ui->setupUi(this);
    connect(ui->locButton, &QToolButton::clicked, this,
            [this]
            {
                //validate the coordinates and, if valid, update icon on map
                double lat = ui->latEdit->text().toDouble();
                double lon = ui->lonEdit->text().toDouble();
                if (ui->editButton->isEnabled() && !ui->editButton->isChecked())
                {
                    if (mGis != 0)
                        mGis->onZoomTo(lat, lon);
                }
                else if (validateCoords())
                {
                    if (mGis != 0)
                        mGis->incidentPlot(lat, lon);
                    lookupAddress(lat, lon);
                }
            });
    connect(ui->callRadioBtn, &QRadioButton::clicked, this,
            [this]
            {
                //enable fields for Call Received state and disable for others
                setStatusFields(false, false, false,
                                IncidentData::STATE_RECEIVED);
            });
    connect(ui->dispatchRadioBtn, &QRadioButton::clicked, this,
            [this]
            {
                //enable fields for Call Received and Dispatched states, and
                //disable for others
                setStatusFields(true, false, false,
                                IncidentData::STATE_DISPATCHED);
                QDate refDate;
                QTime refTime;
                ADJUSTDT(dispatch, call);
            });
    connect(ui->sceneRadioBtn, &QRadioButton::clicked, this,
            [this]
            {
                //enable fields for Call Received, Dispatched and On Scene
                //states, and disable for others
                setStatusFields(true, true, false, IncidentData::STATE_ON_SCENE);
                QDate refDate;
                QTime refTime;
                ADJUSTDT(dispatch, call);
                ADJUSTDT(scene, dispatch);
            });
    connect(ui->closeRadioBtn, &QRadioButton::clicked, this,
            [this]
            {
                //enable fields for all states
                setStatusFields(true, true, true, IncidentData::STATE_CLOSED);
                QDate refDate;
                QTime refTime;
                ADJUSTDT(dispatch, call);
                ADJUSTDT(scene, dispatch);
                ADJUSTDT(close, scene);
            });
    connect(ui->nowButton, &QPushButton::clicked, this,
            [this]
            {
                //update selected time to current
                if (ui->callRadioBtn->isChecked())
                {
                    ui->callDate->setDate(QDate::currentDate());
                    ui->callTime->setTime(QTime::currentTime());
                }
                else if (ui->dispatchRadioBtn->isChecked())
                {
                    ui->dispatchDate->setDate(QDate::currentDate());
                    ui->dispatchTime->setTime(QTime::currentTime());
                }
                else if (ui->sceneRadioBtn->isChecked())
                {
                    ui->sceneDate->setDate(QDate::currentDate());
                    ui->sceneTime->setTime(QTime::currentTime());
                }
                else
                {
                    ui->closeDate->setDate(QDate::currentDate());
                    ui->closeTime->setTime(QTime::currentTime());
                }
            });
    connect(ui->newButton, &QPushButton::clicked, this, [this] { startNew(); });
    connect(ui->createButton, &QPushButton::clicked, this,
            [this]
            {
                //enable/disable necessary fields after incident data creation
                if (!validate())
                    return;
                ui->lastUpdateEdit->setText(QtUtils::getTimestamp());
#ifndef NO_DB
                QString text;
                DbInt::RecordT data;
                text = ui->latEdit->text();
                if (!text.isEmpty())
                    data[DbInt::FIELD_INC_LAT] = text.toStdString();
                text = ui->lonEdit->text();
                if (!text.isEmpty())
                    data[DbInt::FIELD_INC_LON] = text.toStdString();
                text = ui->address1Edit->text().trimmed();
                if (!text.isEmpty())
                    data[DbInt::FIELD_INC_ADDR1] = text.toStdString();
                text = ui->address2Edit->text().trimmed();
                if (!text.isEmpty())
                    data[DbInt::FIELD_INC_ADDR2] = text.toStdString();
                text = ui->descriptionEdit->toPlainText().trimmed();
                if (!text.isEmpty())
                    data[DbInt::FIELD_INC_DESC] = text.toStdString();
                text = ui->callCardNumEdit->text().trimmed();
                if (text.isEmpty())
                {
                    text = "0";
                    ui->callCardNumEdit->setText(text);
                }
                data[DbInt::FIELD_INC_CALLCARD_NO] = text.toStdString();
                data[DbInt::FIELD_CREATED_BY] =
                                        ui->createdByEdit->text().toStdString();
                data[DbInt::FIELD_CREATED_DATE] =
                                       ui->lastUpdateEdit->text().toStdString();
                data[DbInt::FIELD_INC_STATE] =
                                         mStates[ui->stateCombo->currentText()];
                data[DbInt::FIELD_INC_PRIORITY] =
                                 ui->priorityCombo->currentText().toStdString();
                data[DbInt::FIELD_INC_CATEGORY] =
                                 ui->categoryCombo->currentText().toStdString();
                updateStatusDateTime(data);
                set<string> resources;
                if (!ResourceData::isEmpty(mRscAssigned))
                    getResources(resources);
                int id = DbInt::instance().createIncident(data, resources);
                if (id < 0)
                {
                    QMessageBox::critical(this, tr("Incident: Creation Error"),
                                          tr("Failed to create incident "
                                             "because of database link error."));
                    return;
                }
                if (mSession->incidentUpdate(id) <= 0)
                {
                    LOGGER_ERROR(mLogger, LOGPREFIX
                                 << "on_createButton_clicked: "
                                    "Failed to notify server.");
                }
                ui->idEdit->setText(QString::number(id));
#else
                static int id = 0;
                ui->idEdit->setText(QString::number(++id));
#endif //!NO_DB
                ui->frameIdLabel->setText(ui->idEdit->text());
                mData = new IncidentData(id, ui->priorityCombo->currentIndex(),
                                         ui->categoryCombo->currentText());
                setData();
                ui->locButton->setEnabled(!ui->latEdit->text().isEmpty() &&
                                          !ui->lonEdit->text().isEmpty());
                ui->closeRadioBtn->setEnabled(ui->sceneRadioBtn->isChecked());
                ui->editButton->setEnabled(true);
                ui->printButton->setEnabled(true);
                setEditable(false);
                closeCase(0);
                updateCase(mData);
                if (mGis != 0)
                    mGis->modeReset();
                if (ui->sdsButton->menu()
                      ->findChild<QCheckBox *>(QString(), Qt::FindDirectChildrenOnly)
                      ->isChecked())
                    notifyResources(true);
                enableDisableSendNow();
            });
    connect(ui->editButton, &QPushButton::clicked, this,
            [this](bool checked)
            {
                if (!checked)
                {
                    if (ui->updateButton->isEnabled())
                    {
                        if (QMessageBox::question(this,
                                                  tr("Warning: Edit Incident"),
                                                  tr("You have unsaved changes "
                                                     "in Incident %1.\nDiscard?")
                                                      .arg(ui->idEdit->text()))
                            != QMessageBox::Yes)
                        {
                            ui->editButton->setChecked(true);
                            return;
                        }
                        showData(mData);
                        mUpdatedFields.clear();
                    }
                    if (mGis != 0)
                        mGis->modeReset();
                }
                if (mSession->incidentLock(ui->idEdit->text().toInt(), checked)
                    <= 0)
                {
                    LOGGER_ERROR(mLogger, LOGPREFIX
                                 << "editButton clicked: Failed to send "
                                 << ((checked)? "lock ": "unlock ")
                                 << "request to server for Incident "
                                 << ui->idEdit->text().toStdString() << ".");
                    ui->editButton->setChecked(!checked);
                    return;
                }
                ui->editButton->setEnabled(false);
            });
    connect(ui->updateButton, &QPushButton::clicked, this,
            [this]
            {
                //update the necessary incident data
                if (!validate())
                    return;
                if (mUpdatedFields.empty())
                    return;
                ui->updatedByEdit->setText(mUserName);
                ui->lastUpdateEdit->setText(QtUtils::getTimestamp());
#ifndef NO_DB
                DbInt::RecordT data;
                QString text;
                string value;
                string desc;
                string editData(ui->idLabel->text().toStdString() +
                                ui->idEdit->text().trimmed().toStdString());
                for (auto &it : mUpdatedFields)
                {
                    switch (it)
                    {
                        case DbInt::FIELD_INC_ADDR1:
                            value = ui->address1Edit
                                      ->text().trimmed().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->addressLabel
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_ADDR2:
                            value = ui->address2Edit
                                      ->text().trimmed().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->address2Label
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_CALLCARD_NO:
                            text = ui->callCardNumEdit->text();
                            if (text.isEmpty())
                            {
                                text = "0";
                                ui->callCardNumEdit->setText(text);
                            }
                            value = text.toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->callCardLabel
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_CATEGORY:
                            value = ui->categoryCombo
                                      ->currentText().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->categoryLabel
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_CLOSED_DATE:
                            value = ui->closeDate->text().toStdString() + " " +
                                    ui->closeTime->text().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->closeRadioBtn
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_DESC:
                            //already validated
                            desc = ui->descriptionEdit
                                     ->toPlainText().toStdString();
                            data[it] = desc;
                            replace(desc.begin(), desc.end(), '\n', ' ');
                            break;
                        case DbInt::FIELD_INC_DISP_DATE:
                            value = ui->dispatchDate->text().toStdString() +
                                    " " +
                                    ui->dispatchTime->text().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->dispatchRadioBtn
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_LAT:
                            //already validated
                            value = ui->latEdit->text().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->latLabel->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_LON:
                            //already validated
                            value = ui->lonEdit->text().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->longLabel->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_ONSCENE_DATE:
                            value = ui->sceneDate->text().toStdString() + " " +
                                    ui->sceneTime->text().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->sceneRadioBtn
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_PRIORITY:
                            value = ui->priorityCombo
                                      ->currentText().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->priorityLabel
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_RECVD_DATE:
                            value = ui->callDate->text().toStdString() + " " +
                                    ui->callTime->text().toStdString();
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->callRadioBtn
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_STATE:
                            value = mStates[ui->stateCombo->currentText()];
                            data[it] = value;
                            editData.append(",")
                                    .append(ui->stateLabel
                                              ->text().toStdString())
                                    .append(value);
                            break;
                        case DbInt::FIELD_INC_STATUS:
                            //clear status dates and times because any disabled dates and
                            //times should be empty in database
                            data[DbInt::FIELD_INC_RECVD_DATE]   = "";
                            data[DbInt::FIELD_INC_DISP_DATE]    = "";
                            data[DbInt::FIELD_INC_ONSCENE_DATE] = "";
                            data[DbInt::FIELD_INC_CLOSED_DATE]  = "";
                            text.append(tr("Status:"));
                            editData.append(",").append(text.toStdString())
                                    .append(updateStatusDateTime(data));
                            break;
                        default:
                            break; //do nothing
                    } //switch (it)
                } //for (auto &it : mUpdatedFields)
                if (!desc.empty())
                    editData.append(",")
                            .append(ui->descriptionLabel->text().toStdString())
                            .append(desc);
                data[DbInt::FIELD_MODIFIED_BY] = ui->updatedByEdit
                                                   ->text().toStdString();
                data[DbInt::FIELD_MODIFIED_DATE] = ui->lastUpdateEdit
                                                     ->text().toStdString();
                set<string> resources;
                if (!ResourceData::isEmpty(mRscAssigned))
                    getResources(resources);
                string id(ui->idEdit->text().toStdString());
                if (!DbInt::instance().updateIncident(id, data, resources))
                {
                    QMessageBox::critical(this, tr("Incident: Update Error"),
                                          tr("Failed to update the record "
                                             "because of database link error."));
                    return;
                }
                if (mSession->incidentUpdate(Utils::fromString<int>(id),
                                             editData) <= 0)
                {
                    LOGGER_ERROR(mLogger, LOGPREFIX
                                 << "on_updateButton_clicked: "
                                    "Failed to notify server.");
                }
                mUpdatedFields.clear();
#endif //!NO_DB
                setData();
                ui->updateButton->setEnabled(false);
                if (ui->closeRadioBtn->isChecked())
                {
                    closeCase(mData->getId());
                    mData = 0;
                    startNew();
                }
                else
                {
                    closeCase(0);
                    updateCase(mData);
                }
            });
    connect(ui->priorityCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            [this](int idx)
            {
                if (mData != 0 && !ui->createButton->isEnabled())
                    addRemoveField(DbInt::FIELD_INC_PRIORITY,
                                   (idx != mData->getPriority()));
            });
    connect(ui->descriptionEdit, &QTextEdit::textChanged, this,
            [this]
            {
                //marks whether description has changed against saved settings
                if (mData != 0 && !ui->createButton->isEnabled())
                    addRemoveField(DbInt::FIELD_INC_DESC,
                                (ui->descriptionEdit->toPlainText().trimmed() !=
                                 mData->getDescription()));
            });
    connect(ui->filterEdit, &QLineEdit::textChanged, this,
            [this](const QString &txt)
            {
                //filter available resources
                QRegExp re(txt, Qt::CaseInsensitive);
                if (re.isValid())
                {
                    if (mTrmMdl != 0)
                        mTrmMdl->setFilterRegExp(re);
                    mMobMdl->setFilterRegExp(re);
                }
            });
    connect(ui->selectButton, &QToolButton::clicked, this,
            [this]
            {
                if (mGis != 0)
                    mGis->modeResources();
            });
    connect(ui->autoSelButton, &QToolButton::clicked, this,
            [this]
            {
                //if both lat and lon are empty, validateCoords() would return
                //true, whereas if either one is empty, it would produce a
                //different error message not applicable to this action,
                //so check and handle those cases here
                if (ui->latEdit->text().trimmed().isEmpty() ||
                    ui->lonEdit->text().trimmed().isEmpty())
                {
                    QMessageBox::critical(this,
                                          tr("Incident: Resource Assignment"),
                                          tr("Location not set - unable to "
                                             "select nearby resources."));
                }
                else if (mGis != 0)
                {
                    QPointF pt;
                    if (validateCoords(&pt))
                        mGis->selectResources(pt,
                                              ui->autoSelButton->objectName());
                }
            });
    connect(ui->addButton, &QToolButton::clicked, this,
            [this]
            {
                //take selections from both terminals and mobiles
                QModelIndexList l;
                int id;
                int i = 1;
                auto *v = mTrmList;
                auto *m = mTrmMdl;
                auto *asgMdl = ResourceData::model(mRscAssigned);
                ResourceData::IdsT ids;
                for (; i>=0; --i)
                {
                    if (v == 0)
                    {
                        if (i == 0)
                            break;
                        i = 0;
                    }
                    if (i == 0)
                    {
                        v = mMobList;
                        if (v == 0)
                            break; //should not occur
                        m = mMobMdl;
                        l.clear();
                        ids.clear();
                    }
                    l.append(v->selectionModel()->selectedIndexes());
                    if (l.empty())
                        continue;
                    v->clearSelection();
                    //add all selected into assigned
                    auto *mdl = static_cast<ResourceData::ListModel *>(
                                                              m->sourceModel());
                    for (auto &idx : l)
                    {
                        id = ResourceData::getItemId(
                                          mdl->item(m->mapToSource(idx).row()));
                        if (ResourceData::addId(asgMdl, id))
                            ids.insert(id);
                    }
                    if (!ids.empty())
                    {
                        asgMdl->sort(0);
                        handleResourceAssign(ids);
                    }
                }
            });
    connect(ui->removeButton, &QToolButton::clicked, this,
            [this]
            {
                //remove all selected assigned resources
                QModelIndexList l(mRscAssigned->selectionModel()
                                              ->selectedIndexes());
                if (l.empty())
                    return;
                auto *mdl = ResourceData::model(mRscAssigned);
                ResourceData::IdsT ids;
                for (auto &idx : l)
                {
                    ids.insert(ResourceData::getItemId(mdl->item(idx.row())));
                }
                mdl->removeIds(ids);
                if (mData != 0 && !ui->createButton->isEnabled())
                {
                    //user updates the resources
                    mUpdatedFields.insert(DbInt::FIELD_IR_ISSI);
                    ui->updateButton->setEnabled(true);
                }
                enableDisableSendNow();
            });
    connect(ui->tabWidget, &QTabWidget::tabBarDoubleClicked, this,
            [this](int idx)
            {
                if (ui->tabWidget->indexOf(ui->commsTab) == idx)
                {
                    if (ui->callTable->isVisible())
                        ui->callTable->resizeRowsToContents();
                    if (ui->msgTable->isVisible())
                        ui->msgTable->resizeRowsToContents();
                }
            });
    connect(ui->callTable, &QTableView::doubleClicked, this,
            [this](const QModelIndex &idx)
            {
                QtTableUtils::AudioData data;
                if (QtTableUtils::getCallAudioData(ui->callTable, idx.row(),
                                                   data))
                    mAudioPlayer->setDetails(data);
            });
    if (activeInc == 0)
    {
        mActiveInc = new ActiveIncidents(false, this);
        ui->mainSplitter->insertWidget(0, mActiveInc);
        connect(mActiveInc, &ActiveIncidents::setPosition, this,
                [this](int pos)
                {
                    if (pos == ActiveIncidents::POS_TOP ||
                        pos == ActiveIncidents::POS_BOTTOM)
                        ui->mainSplitter->setOrientation(Qt::Vertical);
                    else
                        ui->mainSplitter->setOrientation(Qt::Horizontal);
                    if (pos == ActiveIncidents::POS_LEFT ||
                        pos == ActiveIncidents::POS_TOP)
                        ui->mainSplitter->insertWidget(0, mActiveInc);
                    else
                        ui->mainSplitter->insertWidget(1, mActiveInc);
                });
    }
    else
    {
        //delete unused splitter
        ui->mainFrame->setParent(this);
        ui->outerVLayout->addWidget(ui->mainFrame);
        delete ui->mainSplitter;
    }
    connect(mActiveInc, &ActiveIncidents::showData, this,
            [this](IncidentButton *btn) { showData(btn); });
    setTheme();
    ui->splitter->setStretchFactor(1, 1);
    ui->locButton->setEnabled(false);
#ifdef NO_DB
    QStringList list(QStringList() << tr("Accident") << tr("Fire")
                     << tr("Rescue") << tr("Robbery") << tr("Others"));
    ui->categoryCombo->addItems(list);
    IncidentData::setCategories(list);

    list.clear();
    list << "Johor" << "Kedah" << "Kelantan" << "Melaka" << "Negeri Sembilan"
         << "Pahang" << "Perak" << "Perlis" << "Pulau Pinang" << "Sabah"
         << "Sarawak" << "Selangor" << "Terengganu"
         << "WP Kuala Lumpur"  << "WP Labuan" << "WP Putrajaya";
    ui->stateCombo->addItems(list);
#endif
    ui->callCardNumEdit->setValidator(new QIntValidator(this));
    ui->priorityCombo->addItems(IncidentData::getPriorityList());
    ui->priorityCombo->setCurrentIndex(IncidentData::getDefaultPriority());
    ui->callRadioBtn->setChecked(true);
    ui->closeRadioBtn->setEnabled(false);
    ui->nowButton->setToolTip(tr("Set selected date and time to now"));
    auto menu = new QMenu(this);
    //add checkbox to select whether to include comms tab in printout, using
    //QWidgetAction to allow ticking without dismissing menu
    auto chk = new QCheckBox(ui->tabWidget->tabText(
                                          ui->tabWidget->indexOf(ui->commsTab)),
                             menu);
    auto wact = new QWidgetAction(menu);
    wact->setDefaultWidget(chk);
    menu->addAction(wact);
    menu->addSeparator(); //because only one chk without groupbox
    //print menu
    connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_PRINT_PRV),
            &QAction::triggered, this,
            [this] { doPrint(Document::PRINTTYPE_PREVIEW); });
    connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_PRINT_PDF),
            &QAction::triggered, this,
            [this] { doPrint(Document::PRINTTYPE_PDF); });
    connect(QtUtils::addMenuAction(*menu, CmnTypes::ACTIONTYPE_PRINT_EXCEL),
            &QAction::triggered, this,
            [this] { doPrint(Document::PRINTTYPE_EXCEL); });
    ui->printButton->setMenu(menu);
    setCurrentDateTime();
    SETOBJFIELD(latEdit, LAT);
    SETOBJFIELD(lonEdit, LON);
    SETOBJFIELD(address1Edit, ADDR1);
    SETOBJFIELD(address2Edit, ADDR2);
    SETOBJFIELD(stateCombo, STATE);
    SETOBJFIELD(callCardNumEdit, CALLCARD_NO);
    SETOBJFIELD(categoryCombo, CATEGORY);
    SETOBJFIELD(callDate, RECVD_DATE);
    SETOBJFIELD(callTime, RECVD_DATE);
    SETOBJFIELD(dispatchDate, DISP_DATE);
    SETOBJFIELD(dispatchTime, DISP_DATE);
    SETOBJFIELD(sceneDate, ONSCENE_DATE);
    SETOBJFIELD(sceneTime, ONSCENE_DATE);
    SETOBJFIELD(closeDate, CLOSED_DATE);
    SETOBJFIELD(closeTime, CLOSED_DATE);
    connect(ui->latEdit, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->lonEdit, SIGNAL(textChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->address1Edit, SIGNAL(textEdited(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->address2Edit, SIGNAL(textEdited(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->stateCombo, SIGNAL(currentTextChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->callCardNumEdit, SIGNAL(textEdited(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->categoryCombo, SIGNAL(currentTextChanged(QString)),
            SLOT(onTextChanged(QString)));
    connect(ui->callDate, SIGNAL(dateChanged(QDate)),
            SLOT(onDateChanged(QDate)));
    connect(ui->callTime, SIGNAL(timeChanged(QTime)),
            SLOT(onTimeChanged(QTime)));
    connect(ui->dispatchDate, SIGNAL(dateChanged(QDate)),
            SLOT(onDateChanged(QDate)));
    connect(ui->dispatchTime, SIGNAL(timeChanged(QTime)),
            SLOT(onTimeChanged(QTime)));
    connect(ui->sceneDate, SIGNAL(dateChanged(QDate)),
            SLOT(onDateChanged(QDate)));
    connect(ui->sceneTime, SIGNAL(timeChanged(QTime)),
            SLOT(onTimeChanged(QTime)));
    connect(ui->closeDate, SIGNAL(dateChanged(QDate)),
            SLOT(onDateChanged(QDate)));
    connect(ui->closeTime, SIGNAL(timeChanged(QTime)),
            SLOT(onTimeChanged(QTime)));
    //resources tab
    mRscAssigned = new DraggableListView(ResourceData::TYPE_SUBS_OR_MOBILE,
                                         this);
    mRscAssigned->setAcceptDrops(true);
    mRscAssigned->installEventFilter(this);
    mRscAssigned->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->assignVLayout->addWidget(mRscAssigned);
    connect(mRscAssigned, &QListView::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                auto idx(mRscAssigned->indexAt(pos));
                if (!idx.isValid())
                    return;
                bool single = (mRscAssigned->selectionModel()
                                           ->selectedIndexes().count() == 1);
                QMenu mn(this);
                if (single)
                    QtUtils::addMenuAction(mn, CmnTypes::ACTIONTYPE_CALL,
                                           tr("Call %1")
                                               .arg(idx.data().toString()));
                QList<int> actTypes;
                actTypes << CmnTypes::ACTIONTYPE_SDS
                         << CmnTypes::ACTIONTYPE_STATUS;
                int id = ResourceData::getItemId(mRscAssigned, idx.row());
                if (!single || !Contact::hasId(id))
                    actTypes << CmnTypes::ACTIONTYPE_CONTACT_ADD;
                int rscType = mRscAssigned->getType();
                if (single)
                {
                    if (rscType == ResourceData::TYPE_SUBS_OR_MOBILE)
                        rscType = ResourceData::getType(id);
                    if (!Contact::monitored(id))
                        actTypes << CmnTypes::ACTIONTYPE_MON;
                    if (mGis != 0)
                        actTypes << CmnTypes::ACTIONTYPE_LOCATE;
                }
                actTypes << CmnTypes::ACTIONTYPE_COPY;
                QtUtils::addMenuActions(mn, actTypes);
                auto *act = mn.exec(mRscAssigned->mapToGlobal(pos));
                if (act == 0)
                    return;
                int tp = act->data().toInt();
                switch (tp)
                {
                    case CmnTypes::ACTIONTYPE_CALL:
                    case CmnTypes::ACTIONTYPE_CONTACT_ADD:
                    case CmnTypes::ACTIONTYPE_MON:
                    case CmnTypes::ACTIONTYPE_MSG:
                    case CmnTypes::ACTIONTYPE_SDS:
                    case CmnTypes::ACTIONTYPE_STATUS:
                    {
                        ResourceData::IdsT ids({id});
                        if (!single)
                        {
                            for (auto &idx2 :
                                 mRscAssigned->selectionModel()
                                             ->selectedIndexes())
                            {
                                ids.insert(ResourceData::getItemId(mRscAssigned,
                                                                   idx2.row()));
                            }
                        }
                        emit startAction(tp, rscType, ids);
                        break;
                    }
                    case CmnTypes::ACTIONTYPE_COPY:
                    {
                        QtUtils::copyToClipboard(mRscAssigned);
                        break;
                    }
                    case CmnTypes::ACTIONTYPE_LOCATE:
                    {
                        assert(mGis != 0);
                        mGis->onTerminalLocate(id);
                        break;
                    }
                    default:
                    {
                        break; //do nothing
                    }
                }
            });
    //SDS button menu start
    ui->sdsButton->setToolTip(tr("SDS"));
    ui->sdsButton->setPopupMode(QToolButton::MenuButtonPopup);
    menu = new QMenu(this);
    chk = new QCheckBox(tr("Send automatically"), menu);
    wact = new QWidgetAction(menu);
    wact->setDefaultWidget(chk);
    menu->addAction(wact);
    menu->addSeparator();
    //sub-menu start
    auto menu2 = menu->addMenu(tr("Auto text content"));
    auto vbox = new QVBoxLayout();
    chk = new QCheckBox(ui->addressLabel->text().remove(':'), menu2);
    chk->setObjectName(QString::number(DbInt::FIELD_INC_ADDR1));
    chk->setChecked(true);
    vbox->addWidget(chk);
    chk = new QCheckBox(tr("Coordinates"), menu2);
    chk->setObjectName(QString::number(DbInt::FIELD_INC_LAT));
    chk->setChecked(true);
    vbox->addWidget(chk);
    chk = new QCheckBox(ui->descriptionLabel->text().remove(':'), menu2);
    chk->setObjectName(QString::number(DbInt::FIELD_INC_DESC));
    chk->setToolTip(tr("May be truncated to fit size limit %1")
                    .arg(Sds::SDSMAXLEN));
    chk->setChecked(true);
    vbox->addWidget(chk);
    auto gbox = new QGroupBox();
    gbox->setLayout(vbox);
    wact = new QWidgetAction(menu2);
    wact->setDefaultWidget(gbox);
    menu2->addAction(wact);
    //sub-menu end
    auto *act = new QAction(tr("Send now - auto text"), menu);
    act->setObjectName("auto");
    connect(act, &QAction::triggered, this, [this] { notifyResources(true); });
    menu->addAction(act);
    act = new QAction(tr("Send now - custom text"), menu);
    act->setObjectName("custom");
    connect(act, &QAction::triggered, this, [this] { notifyResources(false); });
    menu->addAction(act);
    ui->sdsButton->setMenu(menu);
    connect(ui->sdsButton, &QToolButton::clicked, this,
            [this] { ui->sdsButton->showMenu(); });
    //SDS button menu end
    ui->selectButton->setToolTip(tr("Select on map"));
    ui->autoSelButton->setPopupMode(QToolButton::MenuButtonPopup);
    menu = new QMenu(this);
    ui->autoSelButton->setMenu(menu);
    auto sb = new QSpinBox(this);
    sb->setRange(1, 30);
    sb->setWrapping(true);
    wact = new QWidgetAction(this);
    wact->setDefaultWidget(sb);
    menu->addAction(wact);
    //use object name to store selected sb value
    ui->autoSelButton->setObjectName(sb->text());
    ui->autoSelButton->setToolTip(tr("Select all within %1 km")
                                  .arg(sb->text()));
    connect(sb, &QSpinBox::textChanged, this,
            [this](const QString &txt)
            {
                ui->autoSelButton->setObjectName(txt);
                ui->autoSelButton->setToolTip(tr("Select all within %1 km")
                                              .arg(txt));
            });
    ui->addButton->setToolTip(tr("Add"));
    ui->removeButton->setToolTip(tr("Remove"));
    ui->tabWidget->setTabToolTip(ui->tabWidget->indexOf(ui->commsTab),
                                 tr("Involving assigned resources within "
                                    "incident period"));
    ui->callTable->setItemDelegateForColumn(QtTableUtils::COL_CALL_TIME,
                                            new DateTimeDelegate(this));
    ui->msgTable->setItemDelegateForColumn(QtTableUtils::COL_TIME,
                                           new DateTimeDelegate(this));
    connect(ui->callTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
            this, [this] { ui->callTable->resizeRowsToContents(); });
    connect(ui->msgTable->verticalHeader(), &QHeaderView::sectionDoubleClicked,
            this, [this] { ui->msgTable->resizeRowsToContents(); });
}

Incident::~Incident()
{
    delete mActiveInc;
    delete mTrmMdl;
    delete mMobMdl;
    delete mTrmList;
    delete mMobList;
    delete mRscAssigned;
    //QTableView does not take ownership of item delegate and model
    delete ui->callTable->itemDelegateForColumn(QtTableUtils::COL_CALL_TIME);
    delete ui->msgTable->itemDelegateForColumn(QtTableUtils::COL_TIME);
    delete ui->callTable->model();
    delete ui->msgTable->model();
    delete ui;
}

void Incident::setTheme(bool init)
{
    if (!init)
    {
        setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
        const QString &ss(Style::getStyle(Style::OBJ_FRAME_TITLE));
        ui->incidentFrame->setStyleSheet(ss);
        ui->srcTrmFrame->setStyleSheet(ss);
        ui->srcMobFrame->setStyleSheet(ss);
        ui->assignFrame->setStyleSheet(ss);
        mActiveInc->setTheme();
    }
    if (mMobList != 0)
    {
        QString ss("background:" + Style::getStyle(Style::OBJ_BACKGROUND));
        mMobList->setStyleSheet(ss);
        if (mTrmList != 0)
            mTrmList->setStyleSheet(ss);
    }
}

void Incident::init(ServerSession *session)
{
    if (session == 0)
    {
        assert("Invalid param in Incident::init" == 0);
        return;
    }
    mSession = session;
    if (mMobList != 0)
        return; //already initialized
    if (ResourceData::isFullMode())
    {
        mTrmList = new DraggableListView(ResourceData::TYPE_SUBSCRIBER, this);
        mTrmList->setEnabled(false);
        mTrmList->setAcceptDrops(true);
        mTrmList->installEventFilter(this);
        ui->srcTrmLayout->addWidget(mTrmList);
        mTrmMdl = new QSortFilterProxyModel();
        mTrmMdl->setSourceModel(
                         ResourceData::getModel(ResourceData::TYPE_SUBSCRIBER));
        mTrmList->setModel(mTrmMdl);
    }
    else
    {
        ui->srcTrmFrame->hide();
        mRscAssigned->setType(ResourceData::TYPE_MOBILE);
    }
    mMobList = new DraggableListView(ResourceData::TYPE_MOBILE, this);
    mMobList->setAcceptDrops(true);
    mMobList->setEnabled(false);
    mMobList->installEventFilter(this);
    ui->srcMobLayout->addWidget(mMobList);
    mMobMdl = new QSortFilterProxyModel();
    mMobMdl->setSourceModel(ResourceData::getModel(ResourceData::TYPE_MOBILE));
    mMobList->setModel(mMobMdl);
    mRscAssigned->setModel(ResourceData::createModel(mRscAssigned->getType(),
                                                     mRscAssigned));
    setTheme(true);
}

void Incident::setGis(GisWindow *gis)
{
    mGis = gis;
    if (gis == 0)
        return;
    connect(mGis, &GisWindow::mainMapLoaded, this,
            [this]
            {
                for (auto &p : mActiveInc->getAllData())
                {
                    mGis->incidentUpdate(p);
                }
                mGis->incidentLock(getEditId());
                enableResourceSelect();
            });
    connect(mGis, &GisWindow::showIncident, this,
            [this](int id) { showData(id); });
    connect(mGis, SIGNAL(incCoordinates(double,double)),
            SLOT(showIncidentCoord(double,double)));
    connect(mGis, SIGNAL(resourceAssign(ResourceData::ListModel*)),
            SLOT(onResourceAssign(ResourceData::ListModel*)));
}

void Incident::reset()
{
    //must disable updateButton before startNew()
    ui->updateButton->setEnabled(false);
    startNew();
    ui->latEdit->clear();
    ui->lonEdit->clear();
    ui->address1Edit->clear();
    ui->address2Edit->clear();
    ui->callCardNumEdit->clear();
    ui->descriptionEdit->clear();
    ui->tabWidget->setCurrentIndex(0);
    IncidentButton::resetFilter();
    setEditable(false); //must click 'New' to start editing
}

void Incident::setUserName(const QString &userName)
{
    mUserName = userName;
    ui->createdByEdit->setText(mUserName);
}

void Incident::enableResourceSelect(bool enabled)
{
    if (enabled == mRscSelEnabled)
        return; //no change
    mRscSelEnabled = enabled;
    //enable the select buttons only if in edit mode
    if (enabled)
        enabled = ui->addButton->isEnabled();
    ui->selectButton->setEnabled(enabled);
    ui->autoSelButton->setEnabled(enabled);
}

int Incident::getEditId()
{
    if (ui->editButton->isChecked())
        return ui->idEdit->text().toInt();
    return 0;
}

void Incident::retrieveData(const QString &username)
{
    //filters below are dependent on IncidentButton, thus on DB as well
    Settings &cfg(Settings::instance());
    string val(cfg.get<string>(Props::FLD_CFG_INCFILTER_ADDRSTATE));
    if (!val.empty())
        IncidentButton::addFilter(IncidentButton::FIELD_ADDRSTATE,
                                  QString::fromStdString(val)
                                      .split(IncidentButton::FILTER_SEP));

    val = cfg.get<string>(Props::FLD_CFG_INCFILTER_CATEGORY);
    if (!val.empty())
        IncidentButton::addFilter(IncidentButton::FIELD_CATEGORY,
                                  QString::fromStdString(val)
                                      .split(IncidentButton::FILTER_SEP));
    QAbstractItemModel *mdl = ui->priorityCombo->model();
    IncidentButton::setFilterMaxSize(IncidentButton::FIELD_PRIORITY,
                                     mdl->rowCount());
    val = cfg.get<string>(Props::FLD_CFG_INCFILTER_PRIORITY);
    if (!val.empty())
    {
        IncidentButton::addFilter(IncidentButton::FIELD_PRIORITY,
                                  QString::fromStdString(val)
                                      .split(IncidentButton::FILTER_SEP));
    }
    else
    {
        //add all
        int i = mdl->rowCount() - 1;
        for (; i>=0; --i)
        {
            IncidentButton::updateFilter(IncidentButton::FIELD_PRIORITY,
                                         mdl->index(i, 0).data().toString());
        }
    }
    QStringList stateList = IncidentData::getStateList();
    IncidentButton::setFilterMaxSize(IncidentButton::FIELD_STATE,
                                     stateList.size());
    val = cfg.get<string>(Props::FLD_CFG_INCFILTER_STATE);
    if (!val.empty())
    {
        IncidentButton::addFilter(IncidentButton::FIELD_STATE,
                                  QString::fromStdString(val)
                                      .split(IncidentButton::FILTER_SEP));
    }
    else
    {
        //add all
        IncidentButton::addFilter(IncidentButton::FIELD_STATE, stateList);
    }
#ifndef NO_DB
    DbInt &db(DbInt::instance());
    DbInt::DataMapT categories;
    DbInt::DataMapT states;
    if (!db.getIncidentCategories(categories) || !db.getCountryStates(states))
    {
        QMessageBox::critical(this, tr("Incident: Data Error"),
                              tr("Database connection failed. Incident "
                                 "creation is not possible."));
        ui->newButton->setEnabled(false);
        ui->createButton->setEnabled(false);
        ui->newButton->setToolTip(tr("Database connection failed. Unable to "
                                     "create incident."));
        return;
    }
    IncidentData::setCategories(categories);
    IncidentData::setAddrStates(states);
    ui->newButton->setToolTip("");
    ui->stateCombo->clear();
    mStates.clear();
    QString state;
    //add all states to filter - only if none configured
    bool noFilter = !IncidentButton::hasFilter(IncidentButton::FIELD_ADDRSTATE);
    for (const auto &it : states)
    {
        state = QString::fromStdString(it.first);
        mStates[state] = it.second;
        ui->stateCombo->addItem(state);
        if (noFilter)
            IncidentButton::updateFilter(IncidentButton::FIELD_ADDRSTATE,
                                         state);
    }
    IncidentButton::setFilterMaxSize(IncidentButton::FIELD_ADDRSTATE,
                                     states.size());
    ui->stateCombo->model()->sort(0, Qt::AscendingOrder);
    ui->stateCombo->setCurrentIndex(0);
    ui->categoryCombo->clear();
    for (const auto &it : categories)
    {
        ui->categoryCombo->addItem(QString::fromStdString(it.first));
    }
#endif //!NO_DB
    ui->categoryCombo->model()->sort(0, Qt::AscendingOrder);
    ui->categoryCombo->setCurrentIndex(0);
    mdl = ui->categoryCombo->model();
    IncidentButton::setFilterMaxSize(IncidentButton::FIELD_CATEGORY,
                                     mdl->rowCount());
    //add all categories to filter - only if none configured
    if (!IncidentButton::hasFilter(IncidentButton::FIELD_CATEGORY))
    {
        int i = mdl->rowCount() - 1;
        for (; i>=0; --i)
        {
            IncidentButton::updateFilter(IncidentButton::FIELD_CATEGORY,
                                         mdl->index(i, 0).data().toString());
        }
    }
    //add filter menu - must be done before loadData()
    mActiveInc->setFilterMenu();
#ifndef NO_DB
    //load open incidents
    loadData();
    int id = getEditId();
    //check for unreleased lock from previous session, and release it if the
    //incident is not being edited now
    int idDb = db.getLockedIncident(username.toStdString());
    if (idDb > 0 && idDb != id && mSession->incidentLock(idDb, false) <= 0)
        LOGGER_ERROR(mLogger, LOGPREFIX << "retrieveData: "
                     << "Failed to send unlock request to server for "
                     << "Incident " << idDb << ".");
    if (id != 0)
    {
        //was disconnected from server while editing
        mSession->incidentLock(id, true);
    }
    else if (ui->idEdit->text().isEmpty())
#endif //!NO_DB
    {
        ui->newButton->setEnabled(true);
        ui->createButton->setEnabled(false);
        ui->editButton->setChecked(false);
        ui->editButton->setText(tr("Edit"));
        setCurrentDateTime();
    }
}

void Incident::loadData(int id)
{
    auto *res = (id > 0)?
                DbInt::instance().getIncidentHistory(id):
                DbInt::instance().getIncidentsByExclStatus(
                                                    IncidentData::STATE_CLOSED);
    if (res == 0 || res->getNumRows() == 0)
    {
        LOGGER_WARNING(mLogger, LOGPREFIX << "loadData: No result for ID "
                       << id);
        delete res;
        return;
    }
    IncidentData *data = 0;
    int dispId = ui->idEdit->text().toInt();
    int numRows = res->getNumRows();
    int i = 0;
    int creator;
    int updater;
    bool doCheck = SubsData::isMultiCluster();
    for (; i<numRows; ++i)
    {
        //check for either creator or updater in common branches if defined
        if (doCheck &&
            (!res->getFieldValue(DbInt::FIELD_CREATED_BY, creator, i) ||
             !SubsData::isValidClient(MsgSp::Value::SUBS_TYPE_DISPATCHER,
                                      creator)) &&
            (!res->getFieldValue(DbInt::FIELD_MODIFIED_BY, updater, i) ||
             updater == creator ||
             !SubsData::isValidClient(MsgSp::Value::SUBS_TYPE_DISPATCHER,
                                      updater)))
        {
            continue; //data not applicable
        }
        data = new IncidentData(res, i);
        if (!data->isValid())
        {
            LOGGER_WARNING(mLogger, LOGPREFIX
                           << "loadData: Invalid data for Incident ID "
                           << res->getFieldStr(DbInt::FIELD_ID, i));
            delete data;
            continue;
        }
        if (data->getId() == dispId)
            mData = data;
        updateCase(data);
    }
    delete res;
    if (data != 0 && id > 0 && id == dispId)
        showData(data); //updated incident is on display
}

void Incident::showData(IncidentButton *btn)
{
    if (btn == 0)
    {
        assert("Invalid param in Incident::showData(btn)" == 0);
        return;
    }
    if (ui->editButton->isChecked())
    {
        QToolTip::showText(QCursor::pos(),
                           tr("%1Cannot switch incidents while editing")
                               .arg("<FONT COLOR='red'>"),
                           btn, btn->rect());
        return;
    }
    if (showData(btn->getData()))
        btn->release(); //a dialog prevented IncidentButton mouseReleaseEvent
}

bool Incident::showData(IncidentData *data)
{
    if (data == 0)
    {
        assert("Invalid param in Incident::showData(data)" == 0);
        return false;
    }
    int editId = getEditId();
    if (editId != 0)
    {
        QMessageBox::information(this, tr("Show Incident Details"),
                                 tr("Unable to show incident %1 details "
                                    "because incident %2 is being edited.")
                                     .arg(data->getId()).arg(editId));
        return true;
    }
    bool retVal = false;
    if (ui->createButton->isEnabled())
    {
        if (QMessageBox::question(this,
                                  tr("Warning: View Incident %1")
                                      .arg(data->getId()),
                                  tr("You have unsaved new Incident data.\n"
                                     "Discard?")) !=
            QMessageBox::Yes)
            return true;
        retVal = true;
        mUpdatedFields.clear();
    }
    mData = 0; //prevent enabling of update button while changing field values
    closeCase(0);
    if (mGis != 0)
        mGis->modeReset();
    ui->createButton->setEnabled(false);
    ui->updateButton->setEnabled(false);
    double lat;
    double lon;
    if (data->getLocation(lat, lon))
    {
        ui->latEdit->setText(QString::fromStdString(GisPoint::getCoord(lat)));
        ui->lonEdit->setText(QString::fromStdString(GisPoint::getCoord(lon)));
        ui->locButton->setEnabled(true);
    }
    else
    {
        ui->latEdit->clear();
        ui->lonEdit->clear();
        ui->locButton->setEnabled(false);
    }
    ui->address1Edit->setText(data->getAddress1());
    ui->address2Edit->setText(data->getAddress2());
    ui->stateCombo->setCurrentText(data->getAddrState());
    ui->idEdit->setText(QString::number(data->getId()));
    ui->frameIdLabel->setText(ui->idEdit->text());
    ui->callCardNumEdit->setText(data->getCallCardNum());
    ui->priorityCombo->setCurrentIndex(data->getPriority());
    ui->categoryCombo->setCurrentText(data->getCategory());
    ui->createdByEdit->setText(data->getCreatedBy());
    ui->updatedByEdit->setText(data->getUpdatedBy());
    ui->lastUpdateEdit
      ->setText(QtUtils::getTimestamp(data->getLastUpdateDateTime()));
    ui->descriptionEdit->setText(data->getDescription());
    auto *mdl = ResourceData::model(mRscAssigned);
    mdl->clear();
    QStringList rscs(data->getResources());
    if (!rscs.isEmpty())
    {
        ResourceData::IdsT ids;
        for (auto &s: rscs)
        {
            ids.insert(s.toInt());
        }
        ResourceData::addIds(mdl, ids);
    }
    enableDisableSendNow();
    setCurrentDateTime();
    setStatusUiStates(data, true);
    setEditable(false);
    if (data->getState() == IncidentData::STATE_CLOSED)
    {
        ui->editButton->setEnabled(false);
    }
    else
    {
        ui->editButton->setEnabled(!data->isLocked());
        mData = data;
    }
    //update comms tab
    QtTableUtils::clearTable(ui->callTable, ui->callFrame);
    QtTableUtils::clearTable(ui->msgTable, ui->msgFrame);
    if (!rscs.isEmpty())
    {
        //find all comms involving assigned resources
        string start(ui->callDate->text().toStdString());
        start.append(" ").append(ui->callTime->text().toStdString());
        string end(ui->closeDate->text().toStdString());
        end.append(" ").append(ui->closeTime->text().toStdString());
        set<int> gssis;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        //first pass for individual comms, collecting gssis
        ui->callFrame
          ->setVisible(QtTableUtils::getCallData(rscs, start, end, gssis,
                                                 ui->callTable));
        ui->msgFrame
          ->setVisible(QtTableUtils::getMsgData(rscs, start, end, gssis,
                                                ui->msgTable));
        if (!gssis.empty())
        {
            //second pass for group comms
            rscs.clear();
            ui->callFrame
              ->setVisible(QtTableUtils::getCallData(rscs, start, end, gssis,
                                                     ui->callTable));
            ui->msgFrame
              ->setVisible(QtTableUtils::getMsgData(rscs, start, end, gssis,
                                                    ui->msgTable));
        }
        QApplication::restoreOverrideCursor();
    }
    ui->printButton->setEnabled(true);
    return retVal;
}

bool Incident::showData(int id)
{
    if (getEditId() == id)
        return true; //already shown
    auto *d = mActiveInc->getData(id);
    if (d != 0)
    {
        showData(d);
        return true;
    }
    QMessageBox::information(this, tr("Show Incident Details"),
                             tr("Unable to show incident %1 details "
                                "because it may have just been closed.\n"
                                "Search again for that incident.")
                                 .arg(id));
    return false;
}

void Incident::setLockHolder(int id, int lockHolder, bool unlocked)
{
    if (mData != 0 && mData->getId() == id)
        ui->editButton->setEnabled(unlocked);
    mActiveInc->setLockHolder(id, (unlocked)? 0: lockHolder);
}

void Incident::editLock(int id, bool doLock)
{
    mActiveInc->setLockHolder(id, (doLock)? mUserName.toInt(): 0);
    if (mGis != 0)
        mGis->incidentLock((doLock)? id: 0);
    //unlock request may have been sent in retrieveData() - in that case,
    //the response gets here while mData is 0
    if (mData == 0 || mData->getId() != id)
        return;
    if (doLock)
    {
        ui->editButton->setText(tr("Stop Editing"));
        ui->updateButton->setEnabled(!mUpdatedFields.empty());
    }
    else
    {
        ui->editButton->setText(tr("Edit"));
        ui->updateButton->setEnabled(false);
    }
    setEditable(doLock);
    ui->editButton->setEnabled(true);
}

void Incident::editLockRevert(bool enableEdit)
{
    ui->editButton->setChecked(!ui->editButton->isChecked());
    ui->editButton->setEnabled(enableEdit);
}

void Incident::handleLogout()
{
    if (ui->editButton->isChecked())
        ui->editButton->click();
    mActiveInc->clear();
}

void Incident::onResourceAssign(ResourceData::ListModel *mdl)
{
    if (mdl == 0)
    {
        QMessageBox::information(this, tr("Incident: Resource Assignment"),
                                 tr("No resource within %1 km")
                                     .arg(ui->autoSelButton->objectName()));
        return;
    }
    if (mdl->rowCount() == 0)
    {
        assert("Invalid param in Incident::onResourceAssign" == 0);
        delete mdl;
        return;
    }
    auto *rscMdl = ResourceData::model(mRscAssigned);
    ResourceData::IdsT ids;
    int id;
    for (auto r=mdl->rowCount()-1; r>=0; --r)
    {
        id = ResourceData::getItemId(mdl->item(r));
        if (ResourceData::addId(rscMdl, id))
            ids.insert(id);
    }
    delete mdl;
    if (!ids.empty())
    {
        rscMdl->sort(0);
        handleResourceAssign(ids);
    }
}

void Incident::showIncidentCoord(double x, double y)
{
    if (ui->editButton->isChecked() || ui->createButton->isEnabled() ||
        startNew())
    {
        //use insert() instead of setText() to enable undo
        ui->latEdit->selectAll();
        ui->latEdit->insert(QString::fromStdString(GisPoint::getCoord(y)));
        ui->lonEdit->selectAll();
        ui->lonEdit->insert(QString::fromStdString(GisPoint::getCoord(x)));
        lookupAddress(y, x);
    }
}

void Incident::onNewComm(int type, const QString &from, const QString &to)
{
    switch (type)
    {
        case CmnTypes::COMMS_MSG_SDS:
        case CmnTypes::COMMS_MSG_STATUS:
            checkRelevantComm(type, from, to, ui->msgFrame, ui->msgTable);
            break;
        default:
            checkRelevantComm(type, from, to, ui->callFrame, ui->callTable);
            break;
    }
}

void Incident::onPlotData(const set<IncidentData *> &data, const set<int> &ids)
{
    if (mGis == 0)
    {
        QMessageBox::critical(this, tr("Plot Incidents"),
                              tr("Unable to plot because the map Window is not "
                                 "yet loaded."));
        return;
    }
    QStringList errIds;
    if (!data.empty())
    {
        for (auto &d: data)
        {
            if (d->hasLocation())
                mGis->incidentUpdate(d, true);
            else
                errIds << QString::number(d->getId());
        }
    }
    if (!ids.empty())
    {
        IncidentData *d;
        for (auto id : ids)
        {
            d = mActiveInc->getData(id);
            if (d != 0 && d->hasLocation())
                mGis->incidentUpdate(d, true);
            else
                errIds << QString::number(id);
        }
    }
    mGis->show();
    if (!errIds.isEmpty())
        QMessageBox::information(this, tr("Plot Incidents"),
                                 tr("Incidents without location: %1")
                                     .arg(errIds.join(", ")));
}

void Incident::onTextChanged(const QString &text)
{
    int field = QObject::sender()->property(PROP_ID).toInt();
    if (ui->createButton->isEnabled())
    {
        if (field == DbInt::FIELD_INC_LAT || field == DbInt::FIELD_INC_LON)
            ui->locButton->setEnabled(!ui->latEdit->text().isEmpty() &&
                                      !ui->lonEdit->text().isEmpty());
        return;
    }
    if (mData == 0)
        return;
    QString saved;
    switch (field)
    {
        case DbInt::FIELD_INC_ADDR1:
            saved = mData->getAddress1();
            break;
        case DbInt::FIELD_INC_ADDR2:
            saved = mData->getAddress2();
            break;
        case DbInt::FIELD_INC_CALLCARD_NO:
            saved = mData->getCallCardNum();
            break;
        case DbInt::FIELD_INC_CATEGORY:
            saved = mData->getCategory();
            break;
        case DbInt::FIELD_INC_LAT:
            saved = QString::number(mData->getLat());
            break;
        case DbInt::FIELD_INC_LON:
            saved = QString::number(mData->getLon());
            break;
        case DbInt::FIELD_INC_STATE:
            saved = mData->getAddrState();
            break;
        default:
            return; //do nothing
    }
    addRemoveField(field, (text.trimmed() != saved));
}

void Incident::onDateChanged(const QDate &date)
{
    if (mData == 0 || ui->createButton->isEnabled())
        return;
    int field = QObject::sender()->property(PROP_ID).toInt();
    QDate saved;
    switch (field)
    {
        case DbInt::FIELD_INC_RECVD_DATE:
            saved = mData->getDate(IncidentData::STATE_RECEIVED);
            break;
        case DbInt::FIELD_INC_DISP_DATE:
            saved = mData->getDate(IncidentData::STATE_DISPATCHED);
            break;
        case DbInt::FIELD_INC_ONSCENE_DATE:
            saved = mData->getDate(IncidentData::STATE_ON_SCENE);
            break;
        case DbInt::FIELD_INC_CLOSED_DATE:
            saved = mData->getDate(IncidentData::STATE_CLOSED);
            break;
        default:
            return; //do nothing
    }
    addRemoveField(field, (date != saved));
}

void Incident::onTimeChanged(const QTime &time)
{
    if (mData == 0 || ui->createButton->isEnabled())
        return;
    int field = QObject::sender()->property(PROP_ID).toInt();
    QTime saved;
    switch (field)
    {
        case DbInt::FIELD_INC_RECVD_DATE:
            saved = mData->getTime(IncidentData::STATE_RECEIVED);
            break;
        case DbInt::FIELD_INC_DISP_DATE:
            saved = mData->getTime(IncidentData::STATE_DISPATCHED);
            break;
        case DbInt::FIELD_INC_ONSCENE_DATE:
            saved = mData->getTime(IncidentData::STATE_ON_SCENE);
            break;
        case DbInt::FIELD_INC_CLOSED_DATE:
            saved = mData->getTime(IncidentData::STATE_CLOSED);
            break;
        default:
            return; //do nothing
    }
    addRemoveField(field, (time != saved));
}

bool Incident::eventFilter(QObject *obj, QEvent *event)
{
    //accept only drag-and-drop between mTrmList/mMobList and mRscAssigned
    if (event->type() == QEvent::DragEnter)
    {
        auto *e = static_cast<QDragEnterEvent *>(event);
        auto *src = e->source();
        if (obj != src &&
            ((obj == mRscAssigned && (src == mTrmList || src == mMobList)) ||
             (src == mRscAssigned && (obj == mTrmList || obj == mMobList))))
        {
            e->acceptProposedAction();
            return true;
        }
    }
    else if (event->type() == QEvent::Drop)
    {
        auto *e = static_cast<QDropEvent *>(event);
        if (obj == e->source())
            return false;
        //source already validated in DragEnter event
        if (obj == mRscAssigned)
        {
            ui->addButton->click();
            return true;
        }
        if (obj == mTrmList || obj == mMobList)
        {
            ui->removeButton->click();
            return true;
        }
    }
    return false;
}

bool Incident::startNew()
{
    mUpdatedFields.clear();
    mData = 0;
    ui->frameIdLabel->clear();
    ui->idEdit->clear();
    ui->createdByEdit->setText(mUserName);
    ui->updatedByEdit->clear();
    ui->lastUpdateEdit->clear();
    ui->editButton->setChecked(false);
    ui->editButton->setEnabled(false);
    ui->updateButton->setEnabled(false);
    ui->printButton->setEnabled(false);
    if (mRscAssigned->model() != 0)
        ResourceData::model(mRscAssigned)->clear();
    enableDisableSendNow();
    setCurrentDateTime();
    setEditable(true);
    ui->createButton->setEnabled(true);
    ui->callRadioBtn->click();
    //clear comms tab
    QtTableUtils::clearTable(ui->callTable, ui->callFrame);
    QtTableUtils::clearTable(ui->msgTable, ui->msgFrame);
    return true;
}

void Incident::setCurrentDateTime()
{
    QDate d(QDate::currentDate());
    ui->callDate->setDate(d);
    ui->dispatchDate->setDate(d);
    ui->sceneDate->setDate(d);
    ui->closeDate->setDate(d);
    ui->callDate->setMaximumDate(d);
    ui->dispatchDate->setMaximumDate(d);
    ui->sceneDate->setMaximumDate(d);
    ui->closeDate->setMaximumDate(d);
    d = d.addMonths(-6);
    ui->callDate->setMinimumDate(d);
    ui->dispatchDate->setMinimumDate(d);
    ui->sceneDate->setMinimumDate(d);
    ui->closeDate->setMinimumDate(d);
    QTime now(QTime::currentTime());
    ui->callTime->setTime(now);
    ui->dispatchTime->setTime(now);
    ui->sceneTime->setTime(now);
    ui->closeTime->setTime(now);
}

void Incident::lookupAddress(double lat, double lon)
{
    string addr;
    string state;
    if (!GisPoint::getAddress(lat, lon, addr, state))
        return;
    //successful getAddress() guarantees state, but not addr
    ui->stateCombo->setCurrentText(QString::fromStdString(state));
    if (addr.empty())
        return;
    QString address(QString::fromStdString(addr));
    QString curr(ui->address2Edit->text());
    if (curr.isEmpty())
    {
        ui->address2Edit->setText(address);
        return;
    }
    if (curr != address)
    {
        QMessageBox mb(QMessageBox::Question, tr("Update Incident Address"),
                       tr("Current address line 2 is '%1', but map "
                          "location shows '%2'.\nOverwrite or append?")
                           .arg(curr, address),
                       QMessageBox::Cancel, this);
        QPushButton *btn = mb.addButton(tr("Overwrite"),
                                        QMessageBox::AcceptRole);
        mb.addButton(tr("Append"), QMessageBox::AcceptRole);
        if (mb.exec() != QMessageBox::Cancel)
        {
            ui->address2Edit->selectAll();
            if (mb.clickedButton() == btn)
                ui->address2Edit->insert(address);
            else
                ui->address2Edit->insert(curr + ", " + address);
        }
    }
}

void Incident::setData()
{
    if (!ui->latEdit->text().isEmpty() && !ui->lonEdit->text().isEmpty())
    {
        //already validated
        mData->setLocation(ui->latEdit->text().toDouble(),
                           ui->lonEdit->text().toDouble());
    }
    mData->setAddress(ui->address1Edit->text().trimmed(),
                      ui->address2Edit->text().trimmed(),
                      ui->stateCombo->currentText());
    mData->setCallCardNum(ui->callCardNumEdit->text());
    mData->setPriority(ui->priorityCombo->currentIndex());
    mData->setCategory(ui->categoryCombo->currentText());
    mData->setCreatedBy(ui->createdByEdit->text());
    mData->setUpdatedBy(ui->updatedByEdit->text());
    mData->setLastUpdateDateTime(ui->lastUpdateEdit->text());
    mData->setDescription(ui->descriptionEdit->toPlainText().trimmed());
    mData->setState(IncidentData::STATE_RECEIVED, ui->callDate->date(),
                    ui->callTime->time());
    if (!ui->callRadioBtn->isChecked())
    {
        mData->setState(IncidentData::STATE_DISPATCHED,
                        ui->dispatchDate->date(), ui->dispatchTime->time());
        if (!ui->dispatchRadioBtn->isChecked())
        {
            mData->setState(IncidentData::STATE_ON_SCENE,
                            ui->sceneDate->date(), ui->sceneTime->time());
            if (!ui->sceneRadioBtn->isChecked())
                mData->setState(IncidentData::STATE_CLOSED,
                                ui->closeDate->date(), ui->closeTime->time());
        }
    }
    QStringList list;
    auto *mdl = ResourceData::model(mRscAssigned);
    for (auto r=mdl->rowCount()-1; r>=0; --r)
    {
        list.append(QString::number(ResourceData::getItemId(mdl->item(r))));
    }
    mData->setResources(list);
}

void Incident::updateCase(IncidentData *data)
{
    mActiveInc->updateCase(data);
    if (mGis != 0)
        mGis->incidentUpdate(data);
}

void Incident::closeCase(int id)
{
    mActiveInc->closeCase(id);
    if (id != 0 && getEditId() == id)
        editLock(id, false);
    if (mGis != 0)
        mGis->incidentClose(id);
}

void Incident::setStatusFields(bool enableDisp,
                               bool enableOnScene,
                               bool enableClosed,
                               int  status)
{
    ui->callDate->setEnabled(true);
    ui->callTime->setEnabled(true);
    ui->dispatchDate->setEnabled(enableDisp);
    ui->dispatchTime->setEnabled(enableDisp);
    ui->sceneDate->setEnabled(enableOnScene);
    ui->sceneTime->setEnabled(enableOnScene);
    ui->closeDate->setEnabled(enableClosed);
    ui->closeTime->setEnabled(enableClosed);
    ui->closeRadioBtn->setEnabled(enableOnScene &&
                                  !ui->createButton->isEnabled());
    if (mData != 0 && !ui->createButton->isEnabled())
        addRemoveField(DbInt::FIELD_INC_STATUS, mData->getState() != status);
}

void Incident::addRemoveField(int field, bool changed)
{
    assert(!ui->createButton->isEnabled());
    if (changed)
    {
        mUpdatedFields.insert(field);
        ui->updateButton->setEnabled(true);
    }
    else
    {
        mUpdatedFields.erase(field);
        if (mUpdatedFields.empty())
            ui->updateButton->setEnabled(false);
    }
}

string Incident::updateStatusDateTime(DbInt::RecordT &data)
{
    int    status;
    string name;
    if (ui->callRadioBtn->isChecked())
    {
        status = IncidentData::STATE_RECEIVED;
        name = ui->callRadioBtn->text().toStdString();
    }
    else if (ui->dispatchRadioBtn->isChecked())
    {
        status = IncidentData::STATE_DISPATCHED;
        name = ui->dispatchRadioBtn->text().toStdString();
    }
    else if (ui->sceneRadioBtn->isChecked())
    {
        status = IncidentData::STATE_ON_SCENE;
        name = ui->sceneRadioBtn->text().toStdString();
    }
    else
    {
        status = IncidentData::STATE_CLOSED;
        name = ui->closeRadioBtn->text().toStdString();
    }

    data[DbInt::FIELD_INC_STATUS] = Utils::toString(status);
    switch (status)
    {
        case IncidentData::STATE_CLOSED:
            data[DbInt::FIELD_INC_CLOSED_DATE] = ui->closeDate->text()
                                                     .toStdString() + " " +
                                                 ui->closeTime->text()
                                                     .toStdString();
            //fallthrough
        case IncidentData::STATE_ON_SCENE:
            data[DbInt::FIELD_INC_ONSCENE_DATE] = ui->sceneDate->text()
                                                      .toStdString() + " " +
                                                  ui->sceneTime->text()
                                                      .toStdString();
            //fallthrough
        case IncidentData::STATE_DISPATCHED:
            data[DbInt::FIELD_INC_DISP_DATE] = ui->dispatchDate->text()
                                                   .toStdString() + " " +
                                               ui->dispatchTime->text()
                                                   .toStdString();
            //fallthrough
        default:
            data[DbInt::FIELD_INC_RECVD_DATE] = ui->callDate->text()
                                                    .toStdString() + " " +
                                                ui->callTime->text()
                                                    .toStdString();
            break;
    }
    name.pop_back(); //remove colon
    return name;
}

void Incident::getResources(set<string> &resources)
{
    if (ui->createButton->isEnabled() ||
        mUpdatedFields.count(DbInt::FIELD_IR_ISSI) != 0)
    {
        auto *mdl = ResourceData::model(mRscAssigned);
        for (auto r=mdl->rowCount()-1; r>=0; --r)
        {
            resources.insert(Utils::toString(
                                        ResourceData::getItemId(mdl->item(r))));
        }
    }
}

void Incident::handleResourceAssign(const ResourceData::IdsT &ids)
{
    if (!ids.empty())
    {
        int id = (mData == 0)? 0: mData->getId(); //incident ID
        emit assignedResources(mRscAssigned->getType(), ids, id);
        //notify resources if data already saved, i.e. with ID, and auto-send
        //enabled
        if (id != 0 &&
            ui->sdsButton->menu()
              ->findChild<QCheckBox *>(QString(), Qt::FindDirectChildrenOnly)
              ->isChecked())
        {
            QString s;
            QStringList l(getSdsMsg(s));
            emit sendSds(mRscAssigned->getType(), ids, l, s);
        }
    }
    enableDisableSendNow();
    if (mData != 0 && !ui->createButton->isEnabled())
    {
        //user updates the resources
        mUpdatedFields.insert(DbInt::FIELD_IR_ISSI);
        ui->updateButton->setEnabled(true);
    }
}

bool Incident::validate()
{
    QString text(ui->descriptionEdit->toPlainText().trimmed());
    ui->descriptionEdit->setText(text);
    if (text.isEmpty())
    {
        QMessageBox::critical(this, tr("Incident: Missing Description"),
                              tr("Please add a description."));
        return false;
    }
    if (!validateCoords())
        return false;
    if (ui->dispatchRadioBtn->isChecked())
        return validateStatusDateTime(IncidentData::STATE_DISPATCHED);
    if (ui->sceneRadioBtn->isChecked())
        return validateStatusDateTime(IncidentData::STATE_ON_SCENE);
    if (ui->closeRadioBtn->isChecked())
        return validateStatusDateTime(IncidentData::STATE_CLOSED);
    return true;
}

bool Incident::validateCoords(QPointF *pt)
{
    QString coords(ui->latEdit->text().trimmed());
    QString lon(ui->lonEdit->text().trimmed());
    if (coords.isEmpty() && lon.isEmpty())
    {
        //clear input boxes in case they contain white space
        ui->latEdit->clear();
        ui->lonEdit->clear();
        return true;
    }
    if (coords.isEmpty() || lon.isEmpty())
    {
        QMessageBox::critical(this, tr("Incident: Invalid Coordinates"),
                              tr("Latitude and longitude must both be "
                                 "either specified or empty."));
        return false;
    }
    coords.append(",").append(lon);
    QPointF point;
    if (GisPoint::checkCoords(this, tr("Incident: Invalid Coordinates"),
                              coords, true, point) !=
        GisPoint::COORD_VALID)
    {
        return false;
    }
    if (pt != 0)
        *pt = point;
    //put in basic format
    ui->latEdit->selectAll();
    ui->latEdit->insert(QString::fromStdString(GisPoint::getLat(point)));
    ui->lonEdit->selectAll();
    ui->lonEdit->insert(QString::fromStdString(GisPoint::getLon(point)));
    return true;
}

bool Incident::validateStatusDateTime(int status)
{
    bool          invalidDate = false;
    QRadioButton *sel;
    QRadioButton *ref;
    switch (status)
    {
        case IncidentData::STATE_CLOSED:
            sel = ui->closeRadioBtn;
            ref = ui->sceneRadioBtn;
            if (ui->closeDate->date() < ui->sceneDate->date())
            {
                invalidDate = true;
                break;
            }
            if (ui->closeDate->date() == ui->sceneDate->date() &&
                ui->closeTime->time() <= ui->sceneTime->time())
                break;
            //fallthrough
        case IncidentData::STATE_ON_SCENE:
            sel = ui->sceneRadioBtn;
            ref = ui->dispatchRadioBtn;
            if (ui->sceneDate->date() < ui->dispatchDate->date())
            {
                invalidDate = true;
                break;
            }
            if (ui->sceneDate->date() == ui->dispatchDate->date() &&
                ui->sceneTime->time() <= ui->dispatchTime->time())
                break;
            //fallthrough
        case IncidentData::STATE_DISPATCHED:
            sel = ui->dispatchRadioBtn;
            ref = ui->callRadioBtn;
            if (ui->dispatchDate->date() < ui->callDate->date())
            {
                invalidDate = true;
                break;
            }
            if (ui->dispatchDate->date() == ui->callDate->date() &&
                ui->dispatchTime->time() <= ui->callTime->time())
                break;
            //fallthrough
        default:
            return true;
    }
    if (invalidDate)
        QMessageBox::critical(this, tr("Incident: Invalid Date"),
                              tr("'%1' date must not be earlier than '%2' "
                                 "date.").arg(sel->text(), ref->text()));
    else
        QMessageBox::critical(this, tr("Incident: Invalid Time"),
                              tr("'%1' time must be later than '%2' time.")
                                  .arg(sel->text(), ref->text()));
    return false;
}

void Incident::setStatusUiStates(IncidentData *data, bool doSetData)
{
    assert(data != 0 || !doSetData);
    int state = (data != 0)? data->getState(): IncidentData::STATE_RECEIVED;
    switch (state)
    {
        case IncidentData::STATE_CLOSED:
            ui->closeDate->setEnabled(true);
            ui->closeTime->setEnabled(true);
            if (doSetData)
            {
                ui->closeRadioBtn->setChecked(true);
                ui->closeDate->setDate(data->getDate(
                                                IncidentData::STATE_CLOSED));
                ui->closeTime->setTime(data->getTime(
                                                IncidentData::STATE_CLOSED));
            }
            //fallthrough
        case IncidentData::STATE_ON_SCENE:
            ui->sceneDate->setEnabled(true);
            ui->sceneTime->setEnabled(true);
            ui->closeRadioBtn->setEnabled(true);
            if (doSetData)
            {
                if (state == IncidentData::STATE_ON_SCENE)
                    ui->sceneRadioBtn->setChecked(true);
                ui->sceneDate->setDate(data->getDate(
                                              IncidentData::STATE_ON_SCENE));
                ui->sceneTime->setTime(data->getTime(
                                              IncidentData::STATE_ON_SCENE));
            }
            //fallthrough
        case IncidentData::STATE_DISPATCHED:
            ui->dispatchDate->setEnabled(true);
            ui->dispatchTime->setEnabled(true);
            if (doSetData)
            {
                if (state == IncidentData::STATE_DISPATCHED)
                    ui->dispatchRadioBtn->setChecked(true);
                ui->dispatchDate->setDate(data->getDate(
                                            IncidentData::STATE_DISPATCHED));
                ui->dispatchTime->setTime(data->getTime(
                                            IncidentData::STATE_DISPATCHED));
            }
            //fallthrough
        case IncidentData::STATE_RECEIVED:
        default:
            ui->callDate->setEnabled(true);
            ui->callTime->setEnabled(true);
            if (doSetData)
            {
                if (state == IncidentData::STATE_RECEIVED)
                    ui->callRadioBtn->setChecked(true);
                ui->callDate->setDate(data->getDate(
                                              IncidentData::STATE_RECEIVED));
                ui->callTime->setTime(data->getTime(
                                              IncidentData::STATE_RECEIVED));
            }
            break;
    }
    ui->callRadioBtn->setEnabled(true);
    ui->dispatchRadioBtn->setEnabled(true);
    ui->sceneRadioBtn->setEnabled(true);
}

void Incident::setEditable(bool editable)
{
    if (editable)
    {
        setStatusUiStates(mData, false);
    }
    else
    {
        ui->callRadioBtn->setEnabled(false);
        ui->dispatchRadioBtn->setEnabled(false);
        ui->sceneRadioBtn->setEnabled(false);
        ui->closeRadioBtn->setEnabled(false);
    }
    ui->latEdit->setReadOnly(!editable);
    ui->lonEdit->setReadOnly(!editable);
    ui->address1Edit->setReadOnly(!editable);
    ui->address2Edit->setReadOnly(!editable);
    ui->stateCombo->setEnabled(editable);
    ui->callCardNumEdit->setReadOnly(!editable);
    ui->priorityCombo->setEnabled(editable);
    ui->categoryCombo->setEnabled(editable);
    ui->descriptionEdit->setReadOnly(!editable);
    ui->callDate->setReadOnly(!editable);
    ui->callTime->setReadOnly(!editable);
    ui->dispatchDate->setReadOnly(!editable);
    ui->dispatchTime->setReadOnly(!editable);
    ui->sceneDate->setReadOnly(!editable);
    ui->sceneTime->setReadOnly(!editable);
    ui->closeDate->setReadOnly(!editable);
    ui->closeTime->setReadOnly(!editable);
    ui->nowButton->setEnabled(editable);
    ui->newButton->setEnabled(!editable &&
                              ui->newButton->toolTip().isEmpty());
    ui->createButton->setEnabled(false);
    ui->selectButton->setEnabled(editable && mRscSelEnabled);
    ui->autoSelButton->setEnabled(editable && mRscSelEnabled);
    ui->addButton->setEnabled(editable);
    ui->removeButton->setEnabled(editable);
    if (mTrmList != 0)
        mTrmList->setEnabled(editable);
    if (mMobList != 0)
        mMobList->setEnabled(editable);
}

void Incident::doPrint(int printType)
{
    QString title(windowTitle());
    title.append(" ").append(ui->idEdit->text());
    Document doc(printType, title, "", Document::TYPE_INCIDENT);
    QString str(ui->address2Edit->text());
    if (!str.isEmpty())
        str.prepend("\n");
    str.prepend(ui->address1Edit->text());
    doc.addRow(ui->callCardLabel->text(), ui->callCardNumEdit->text(), false,
               ui->priorityLabel->text(), ui->priorityCombo->currentText())
       .addRow(ui->categoryLabel->text(), ui->categoryCombo->currentText(),
               false, ui->createdLabel->text(), ui->createdByEdit->text())
       .addRow(ui->latLabel->text(), ui->latEdit->text(), false,
               ui->updateLabel->text(), ui->updatedByEdit->text())
       .addRow(ui->longLabel->text(), ui->lonEdit->text(), false,
               ui->lastUpdateLabel->text(), ui->lastUpdateEdit->text())
       .addRow(ui->addressLabel->text(), str, true)
       .addRow(ui->stateLabel->text(), ui->stateCombo->currentText());
    QDateTime start(ui->callDate->date(), ui->callTime->time());
    str = ui->callDate->text();
    str.append(" ").append(ui->callTime->text());
    if (!ui->closeRadioBtn->isChecked())
        str.append(" - ").append(tr("still open")).append(" ")
           .append(formatDuration(start.secsTo(QDateTime::currentDateTime())));
    doc.addRow(ui->callRadioBtn->text(), str);
    if (!ui->callRadioBtn->isChecked())
    {
        QDateTime dt(ui->dispatchDate->date(), ui->dispatchTime->time());
        str = ui->dispatchDate->text();
        str.append(" ").append(ui->dispatchTime->text())
           .append(formatDuration(start.secsTo(dt)));
        doc.addRow(ui->dispatchRadioBtn->text(), str);
        if (!ui->dispatchRadioBtn->isChecked())
        {
            dt = QDateTime(ui->sceneDate->date(), ui->sceneTime->time());
            str = ui->sceneDate->text();
            str.append(" ").append(ui->sceneTime->text())
               .append(formatDuration(start.secsTo(dt)));
            doc.addRow(ui->sceneRadioBtn->text(), str);
            if (!ui->sceneRadioBtn->isChecked())
            {
                dt = QDateTime(ui->closeDate->date(), ui->closeTime->time());
                str = ui->closeDate->text();
                str.append(" ").append(ui->closeTime->text())
                   .append(formatDuration(start.secsTo(dt)));
                doc.addRow(ui->closeRadioBtn->text(), str);
            }
        }
    }
    QStringList rsc;
    auto *mdl = ResourceData::model(mRscAssigned);
    for (auto r=mdl->rowCount()-1; r>=0; --r)
    {
        rsc << mdl->item(r)->text();
    }
    if (!rsc.isEmpty())
        doc.addRow(tr("Resources") + ":", rsc.join("\n"), true);
    doc.addRow(ui->descriptionLabel->text(), ui->descriptionEdit->toPlainText(),
               true);
    doc.endTable();
    //check the print menu checkbox for comms tab
    if (ui->printButton->menu()->findChild<QCheckBox *>()->isChecked())
    {
        //note:
        //-check frame instead of table because table is never hidden but frame
        // is
        //-!frame->isHidden() is not equal to frame->isVisible() here,
        // because isVisible() is false when the comms tab is not selected even
        // though the frame is not hidden
        if (!ui->callFrame->isHidden())
            doc.addTable(ui->callTable, tr("Calls"),
                         QtTableUtils::COL_CALL_TIME);
        if (!ui->msgFrame->isHidden())
            doc.addTable(ui->msgTable, tr("Messages"), QtTableUtils::COL_TIME,
                         QtTableUtils::COL_TYPE);
    }
    doc.print(this, title.remove(" "));
}

QString Incident::formatDuration(int t)
{
    QString s;
    t = t/60; //minutes
    if (t == 0)
        return s; //nothing if under 1 minute
    int v = t % 60;
    if (v == 1)
        s.append("1 ").append(tr("minute"));
    else if (v != 0)
        s.append(QString(tr("%1 minutes")).arg(v));
    if (t >= 60) //at least 1 hour
    {
        if (v != 0)
            s.prepend(" ");
        t = t/60; //hours
        v = t % 24;
        if (v == 1)
            s.prepend(tr("hour")).prepend("1 ");
        else if (v != 0)
            s.prepend(QString(tr("%1 hours")).arg(v));
        if (t >= 24) //at least 1 day
        {
            if (v != 0)
                s.prepend(" ");
            if (t >= 48)
                s.prepend(QString(tr("%1 days")).arg(t/24));
            else
                s.prepend(tr("day")).prepend("1 ");
        }
    }
    return s.prepend(" (").append(")");
}

void Incident::enableDisableSendNow()
{
    bool hasAssigned = !ResourceData::isEmpty(mRscAssigned);
    ui->sdsButton->menu()
      ->findChild<QAction *>("auto", Qt::FindDirectChildrenOnly)
      ->setEnabled(!ui->idEdit->text().isEmpty() && hasAssigned);
    //custom message can be sent before creation
    ui->sdsButton->menu()
      ->findChild<QAction *>("custom", Qt::FindDirectChildrenOnly)
      ->setEnabled(hasAssigned);
}

QStringList Incident::getSdsMsg(QString &txt)
{
    QStringList l;
    QString prefix(ui->idEdit->text());
    if (prefix.isEmpty())
        return l; //no message because not yet created
    prefix.prepend("-").prepend(windowTitle());
    QString addr;
    QString latLon;
    QString desc;
    for (auto &chk : ui->sdsButton->menu()->findChildren<QCheckBox *>())
    {
        if (!chk->isChecked())
            continue;
        switch (chk->objectName().toInt())
        {
            case DbInt::FIELD_INC_ADDR1:
            {
                addr.append(ui->address1Edit->text().trimmed());
                QString s(ui->address2Edit->text().trimmed());
                if (!s.isEmpty())
                {
                    if (!addr.isEmpty() && !addr.endsWith(","))
                        addr.append(", ");
                    addr.append(s);
                }
                if (!addr.isEmpty() && !addr.endsWith(","))
                    addr.append(", ");
                addr.append(ui->stateCombo->currentText()).append(".")
                    .prepend(" ");
                break;
            }
            case DbInt::FIELD_INC_DESC:
            {
                desc.append(ui->descriptionEdit->toPlainText().simplified())
                    .prepend(" ");
                break;
            }
            case DbInt::FIELD_INC_LAT:
            {
                latLon.append(ui->latEdit->text());
                if (!latLon.isEmpty())
                    latLon.prepend(" (").append(",")
                          .append(ui->lonEdit->text()).append(")");
                break;
            }
            default:
            {
                break; //do nothing - should never occur
            }
        }
    }
    addr.prepend("]").prepend(ui->categoryCombo->currentText()).prepend(" [");
    l << addr;
    if (!latLon.isEmpty())
        l << latLon;
    if (!desc.isEmpty())
        l << desc;
    if (ResourceData::hasMobileStat())
    {
        txt = prefix; //prepare MMS text
        txt.append(l.join(""));
    }
    Sds::prepMsgs(prefix, l);
    if (l.size() == 1)
        txt.clear(); //single SDS - no need for MMS
    return l;
}

void Incident::notifyResources(bool autoText)
{
    ResourceData::IdsT ids;
    if (ResourceData::model(mRscAssigned)->getIds(ids))
    {
        QString s;
        QStringList l;
        if (autoText)
            l = getSdsMsg(s);
        emit sendSds(mRscAssigned->getType(), ids, l, s);
    }
}

void Incident::checkRelevantComm(int            commType,
                                 const QString &from,
                                 const QString &to,
                                 QFrame        *frame,
                                 QTableView    *tv)
{
    assert(frame != 0 && tv != 0);
    //not relevant if:
    //-showing a closed incident,
    //-no assigned resource
    if ((ui->closeRadioBtn->isChecked() && !ui->editButton->isEnabled()) ||
        ResourceData::isEmpty(mRscAssigned))
        return;
    int tblType;
    switch (commType)
    {
        case CmnTypes::COMMS_MSG_SDS:
        case CmnTypes::COMMS_MSG_STATUS:
            tblType = QtTableUtils::TBLTYPE_MSG;
            break;
        default:
            tblType = QtTableUtils::TBLTYPE_CALL;
            break;
    }
    QSet<int> rscTypes; //acceptable resource types
    rscTypes << ResourceData::TYPE_UNKNOWN;
    int dst = mRscAssigned->getType(); //tmp use of dst
    if (dst == ResourceData::TYPE_SUBS_OR_MOBILE)
        rscTypes << ResourceData::TYPE_SUBSCRIBER << ResourceData::TYPE_MOBILE;
    else
        rscTypes << dst;
    int toType = ResourceData::getType(to);
    if (toType == ResourceData::TYPE_DISPATCHER &&
        tblType != QtTableUtils::TBLTYPE_CALL)
        dst = mServerIssi;
    else
        dst = ResourceData::getId(to);
    int src = (ResourceData::isClient(from))? 0: ResourceData::getId(from);
    //match src/dst against resources
    if (((src != 0 && ResourceData::getItem(mRscAssigned, src) != 0) ||
         (rscTypes.contains(toType) &&
          ResourceData::getItem(mRscAssigned, dst) != 0)) ||
        //find group name in current tables
        (toType == ResourceData::TYPE_GROUP &&
         (QtTableUtils::find(ui->callTable, QtTableUtils::COL_CALL_TO, to) ||
          QtTableUtils::find(ui->msgTable, QtTableUtils::COL_TO, to))))
    {
        if (src == 0)
        {
            //from dispatcher
            src = ResourceData::getId(from);
            if (tblType == QtTableUtils::TBLTYPE_MSG &&
                src == mUserName.toInt())
            {
                //outgoing SDS or Status Message - delay data retrieval to
                //allow time for database update by server, which depends on
                //MON-SDS/MON-STATUS from network, which usually comes later
                //than SDS-RPT/STATUS-RPT
                QTimer::singleShot(300, this,
                                   [tblType, commType, src, dst, tv, frame]
                                   {
                                       if (QtTableUtils::getLastData(
                                               tblType, commType, src, dst, tv))
                                           frame->show();
                                   });
                return;
            }
        } //if (src == 0)
        if (QtTableUtils::getLastData(tblType, commType, src, dst,  tv))
            frame->show();
    }
}
