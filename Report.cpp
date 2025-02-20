/**
 * The Report UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Report.cpp 1885 2024-11-28 08:32:01Z hazim.rujhan $
 * @author Zulzaidi Atan
 */
#include <assert.h>
#include <QAbstractItemModel>
#include <QAction>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QRegExp>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTableView>
#include <QToolButton>
#include <QWidgetAction>

#include "AudioPlayer.h"
#include "DbInt.h"
#include "Document.h"
#include "QtTableUtils.h"
#include "QtUtils.h"
#include "ResourceData.h"
#include "StatusCodes.h"
#include "Style.h"
#include "ui_Report.h"
#include "Report.h"

using namespace std;

static const int MAX_DAYS  = 62;
static const int MAX_ITEMS = 10; //completer maximum visible items

#define ADDTYPE(tp) \
    do \
    { \
        ui->typeCombo->addItem(\
                       QtTableUtils::getCallType(QtTableUtils::CALLTYPE_##tp), \
                       QString::fromStdString(QtTableUtils::CALLTYPE_##tp)); \
    } \
    while (0)

#ifdef INCIDENT
#define ADDSTAT(st) \
    do \
    { \
        ui->statusCombo->addItem( \
                    QtTableUtils::getIncidentStatus(IncidentData::STATE_##st), \
                    IncidentData::STATE_##st); \
    } \
    while (0)
#endif

Report::Report(AudioPlayer *audioPlayer, Logger *logger, QWidget *parent) :
QWidget(parent), ui(new Ui::Report), mAudioPlayer(audioPlayer),
mLogger(logger), mDateTimeDelegate(0), mFilterType(FILTERTYPE_CALL)
{
    if (audioPlayer == 0 || logger == 0)
    {
        assert("Bad param in Report::Report" == 0);
        return;
    }
    ui->setupUi(this);
    setTheme();
    QDate d(QDate::currentDate());
    ui->startDate->setDate(d);
    ui->endDate->setDate(d);
    QTime now(QTime::currentTime());
    ui->startTime->setTime(now);
    ui->endTime->setTime(now);
    connect(ui->startDate, &QDateEdit::dateChanged, this,
            [this](const QDate &date)
            {
                //end cannot be earlier than start
                if (ui->endDate->date() < date)
                {
                    ui->endDate->blockSignals(true); //prevent dateChanged()
                    ui->endDate->setDate(date);
                    ui->endDate->blockSignals(false);
                }
            });
    connect(ui->endDate, &QDateEdit::dateChanged, this,
            [this](const QDate &date)
            {
                //start cannot be later than end
                if (ui->startDate->date() > date)
                {
                    ui->startDate->blockSignals(true);
                    ui->startDate->setDate(date);
                    ui->startDate->blockSignals(false);
                }
            });
    showFilter(FILTERTYPE_CALL);
    ui->fromCombo->setValidator(new QIntValidator(this));
    ui->idCombo->setValidator(new QIntValidator(0, 9999999, this));
    mCompleter = new QCompleter(this);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setFilterMode(Qt::MatchContains);
    mCompleter->setMaxVisibleItems(MAX_ITEMS);
    ui->toCombo->setCompleter(mCompleter);
    ui->typeCombo->addItem(QtTableUtils::getCallType(""), "");
    ADDTYPE(AMBIENCE);
    ADDTYPE(BROADCAST);
    ADDTYPE(GROUP);
    ADDTYPE(INCOMING);
    ADDTYPE(OUTGOING);
    ADDTYPE(MOBILE);

#ifdef INCIDENT
    ui->priorityCombo->addItem(tr("All"));
    ui->priorityCombo->addItems(IncidentData::getPriorityList());
    ADDSTAT(OPEN);
    ADDSTAT(RECEIVED);
    ADDSTAT(DISPATCHED);
    ADDSTAT(ON_SCENE);
    ADDSTAT(CLOSED);
#else
    delete ui->incidentRadioBtn;
#endif
    ui->andOrCombo->addItem(tr("AND"), true);
    ui->andOrCombo->addItem(tr("OR"), false);
    ui->descCombo->setToolTip(tr("Space-separated"));
    ui->resourceCombo->setToolTip(tr("Comma-separated"));
    connect(ui->callRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_CALL); });
    connect(ui->sdsRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_SDS); });
    connect(ui->stsRadioBtn, &QRadioButton::clicked, this,
            [this]()
            {
                ui->stsCombo->clear();
                ui->stsCombo->addItem("");
                StatusCodes::DataMapT dataMap;
                if (StatusCodes::getData(dataMap))
                {
                    for (auto &it : dataMap)
                    {
                        ui->stsCombo->addItem("\"" +
                                QtUtils::fromHexUnicode(it.second.text) + "\"");
                    }
                }
                showFilter(FILTERTYPE_STS);
            });
    connect(ui->mmsRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_MMS); });
    connect(ui->msgRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_MSG); });
    connect(ui->locRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_LOC); });
#ifdef INCIDENT
    connect(ui->incidentRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_INCIDENT); });
    connect(ui->idRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_INCIDENT); });
    connect(ui->detailsRadioBtn, &QRadioButton::clicked, this,
            [this]() { showFilter(FILTERTYPE_INCIDENT); });
    connect(ui->plotCheck, &QCheckBox::clicked, this,
            [this](bool checked) { onPlotCheck(checked); });
#endif
    connect(ui->toTypeCombo, qOverload<int>(&QComboBox::currentIndexChanged),
            this,
            [this](int idx)
            {
                //set completer
                int type = ui->toTypeCombo->itemData(idx).toInt();
                if (type >= 0)
                    mCompleter->setModel(ResourceData::getModel(type));
                else
                    mCompleter->setModel(0);
            });
    connect(ui->typeCombo,
            qOverload<const QString &>(&QComboBox::currentIndexChanged), this,
            [this](const QString &value)
            {
                ui->toFrame->setEnabled(value != tr("Broadcast"));
            });
    connect(ui->displayButton, &QPushButton::clicked, this,
            [this]() { onDisplay(); });
    connect(ui->resultTable, &QTableView::doubleClicked, this,
            [this](const QModelIndex &idx)
            {
                switch (mFilterType)
                {
                    case FILTERTYPE_CALL:
                    {
                        QtTableUtils::AudioData data;
                        if (QtTableUtils::getCallAudioData(ui->resultTable,
                                                           idx.row(), data))
                            mAudioPlayer->setDetails(data);
                        break;
                    }
#ifdef INCIDENT
                    case FILTERTYPE_INCIDENT:
                    {
                        auto *itm = qobject_cast<QStandardItemModel *>(
                                             ui->resultTable->model())
                                               ->item(idx.row(),
                                                      QtTableUtils::COL_INC_ID);
                        auto *d = itm->data(Qt::UserRole)
                                  .value<IncidentData *>();
                        if (d != 0)
                            emit showIncidentByData(d);
                        else
                            emit showIncidentById(itm->text().toInt());
                        break;
                    }
#endif
                    default:
                    {
                        ui->resultTable->resizeRowsToContents();
                        break;
                    }
                }
            });
    //add print menu
    QMenu *menu = new QMenu(this);
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
    mDateTimeDelegate = new DateTimeDelegate(this);
    //invoke displayButton click when Enter pressed in certain comboboxes
    connect(ui->msgCombo->lineEdit(), &QLineEdit::returnPressed, this,
            [this] { onDisplay(); });
    connect(ui->stsCombo->lineEdit(), &QLineEdit::returnPressed, this,
            [this] { onDisplay(); });
    connect(ui->descCombo->lineEdit(), &QLineEdit::returnPressed, this,
            [this] { onDisplay(); });
    connect(ui->resultTable->verticalHeader(),
            &QHeaderView::sectionDoubleClicked, this,
            [this] { ui->resultTable->resizeRowsToContents(); });
}

Report::~Report()
{
    cleanup();
    delete mAudioPlayer;
    delete mCompleter;
    delete mDateTimeDelegate;
    delete ui;
}

void Report::setTheme()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    ui->resultTitleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    mAudioPlayer->setTheme();
}

void Report::handleLogout()
{
    cleanup();
    ui->idRadioBtn->click();
    ui->callRadioBtn->click();
    ui->categoryCombo->clear();
    ui->stsCombo->clear();
    ui->statusCombo->setCurrentIndex(0);
    ui->typeCombo->setCurrentIndex(0);
    ui->priorityCombo->setCurrentIndex(0);
    ui->descCombo->lineEdit()->clear();
    ui->fromCombo->clear();
    ui->idCombo->clear();
    ui->msgCombo->lineEdit()->clear();
    ui->resourceCombo->clear();
    ui->toCombo->clear();
    ui->andOrCombo->setCurrentIndex(0);
    ui->toTypeCombo->setCurrentIndex(0);
    ui->locButton->setToolTip("");
    delete ui->locButton->menu();
    ui->resultTitleLabel->setText(tr("Results"));
    ui->resultTitleLabel->setToolTip("");
}

void Report::retrieveData()
{
    if (ui->toTypeCombo->count() == 0)
    {
        ui->toTypeCombo->addItem(tr("Subs/Disp"), -1);
        ui->toTypeCombo->addItem(ResourceData::getTypeTxt(
                                                      ResourceData::TYPE_GROUP),
                                 CmnTypes::IDTYPE_GROUP);
        if (ResourceData::isFullMode())
        {
            ui->toTypeCombo->addItem(ResourceData::getTypeTxt(
                                                   ResourceData::TYPE_DGNA_IND),
                                     CmnTypes::IDTYPE_DGNA_IND);
#ifdef DGNA_OF_GRPS
            ui->toTypeCombo->addItem(ResourceData::getTypeTxt(
                                                   ResourceData::TYPE_DGNA_GRP),
                                     CmnTypes::IDTYPE_DGNA_GRP);
#endif
        }
    }
    if (ui->locButton->menu() == 0)
    {
        DbInt::Int2StringMapT tt;
        if (DbInt::instance().getTerminalTypes(tt))
        {
            if (tt.size() == 1)
            {
                ui->locButton->setProperty("l",
                        QVariant::fromValue(
                            QStringList({QString::number(tt.begin()->first)})));
                //ensure always hidden - see showFilter()
                ui->locButton->setCheckable(true);
            }
            else
            {
                //tooltip will contain selected terminal type names
                ui->locButton->setToolTip("");
                //storage for set of selected terminal type values
                ui->locButton->setProperty("l",
                                           QVariant::fromValue(QStringList()));
                //put terminal types in a button menu of checkboxes
                auto *menu = new QMenu(tr("Types"));
                ui->locButton->setMenu(menu);
                connect(ui->locButton, &QToolButton::clicked, this,
                        [this] { ui->locButton->showMenu(); });
                auto *vbox = new QVBoxLayout();
                auto *chkAll = new QCheckBox(tr("All"), menu);
                vbox->addWidget(chkAll);
                QCheckBox *chk;
                for (auto &it : tt)
                {
                    chk = new QCheckBox(QString::fromStdString(it.second), menu);
                    chk->setProperty("v", it.first); //type value
                    connect(chk, &QCheckBox::toggled, this,
                            [chk, this](bool isChecked)
                            {
                                auto t(ui->locButton->toolTip()
                                          .split(",", Qt::SkipEmptyParts));
                                auto l(ui->locButton
                                         ->property("l").value<QStringList>());
                                if (isChecked)
                                {
                                    t.append(chk->text());
                                    t.sort();
                                    l.append(chk->property("v").toString());
                                }
                                else
                                {
                                    t.removeOne(chk->text());
                                    l.removeOne(chk->property("v").toString());
                                }
                                ui->locButton->setToolTip(t.join(","));
                                ui->locButton
                                  ->setProperty("l", QVariant::fromValue(l));
                            });
                    //uncheck chkAll if any other box is unchecked by clicking
                    connect(chk, &QCheckBox::clicked, this,
                            [chkAll](bool isChecked)
                            {
                                if (!isChecked)
                                    chkAll->setChecked(false);
                            });
                    //clicking chkAll goes to all checkboxes
                    connect(chkAll, SIGNAL(clicked(bool)),
                            chk, SLOT(setChecked(bool)));
                    vbox->addWidget(chk);
                }
                auto *gbox = new QGroupBox();
                gbox->setLayout(vbox);
                auto *wAct = new QWidgetAction(menu);
                wAct->setDefaultWidget(gbox);
                menu->addAction(wAct);
                ui->locButton->setEnabled(true);
            }
        }
        else
        {
            ui->locButton->setEnabled(false);
        }
    }
#ifdef INCIDENT
    ui->stateCombo->clear();
    ui->stateCombo->addItem(tr("All"), "");
    DbInt::DataMapT data;
    if (DbInt::instance().getCountryStates(data))
    {
        for (const auto &it : data)
        {
            ui->stateCombo->addItem(QString::fromStdString(it.first),
                                    QString::fromStdString(it.second));
        }
    }
    data.clear();
    if (DbInt::instance().getIncidentCategories(data))
    {
        ui->categoryCombo->clear();
        ui->categoryCombo->addItem(tr("All"), "");
        for (const auto &it : data)
        {
            ui->categoryCombo->addItem(QString::fromStdString(it.first));
        }
    }
#endif //INCIDENT
}

#ifdef INCIDENT
void Report::onPlotCheck(bool plot)
{
    if (plot)
    {
        auto *mdl = qobject_cast<QStandardItemModel *>(ui->resultTable->model());
        //plotCheck is enabled only if table has model, so no need to check
        QStandardItem *item;
        IncidentData *d;
        set<IncidentData *> data;
        set<int> ids;
        int i = mdl->rowCount() - 1;
        for (; i>=0; --i)
        {
            item = mdl->item(i, QtTableUtils::COL_INC_ID);
            d = item->data(Qt::UserRole).value<IncidentData *>();
            if (d != 0)
                data.insert(d);
            else
                ids.insert(item->text().toInt());
        }
        emit plotIncidents(data, ids);
    }
    else
    {
        emit clearIncidents();
    }
}
#endif //INCIDENT

void Report::onDisplay()
{
    int from = 0;
    int to = 0;
    if (!validateFields(from, to))
        return;
    cleanup();
    if (ui->callRadioBtn->isChecked())
        mFilterType = FILTERTYPE_CALL;
    else if (ui->sdsRadioBtn->isChecked())
        mFilterType = FILTERTYPE_SDS;
    else if (ui->stsRadioBtn->isChecked())
        mFilterType = FILTERTYPE_STS;
    else if (ui->mmsRadioBtn->isChecked())
        mFilterType = FILTERTYPE_MMS;
    else if (ui->msgRadioBtn->isChecked())
        mFilterType = FILTERTYPE_MSG;
    else if (ui->locRadioBtn->isChecked())
        mFilterType = FILTERTYPE_LOC;
#ifdef INCIDENT
    else
        mFilterType = FILTERTYPE_INCIDENT;

    ui->plotCheck->setEnabled(false);
    onPlotCheck(false);
#endif
    setTitleAndToolTip();
    QTableView *tv = ui->resultTable;
    bool success = false;
    int  res = 0;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString txt;
    switch (mFilterType)
    {
        case FILTERTYPE_CALL:
        {
            tv->setItemDelegateForColumn(QtTableUtils::COL_CALL_TIME,
                                         mDateTimeDelegate);
            success = QtTableUtils::getCallData(from, to,
                                     ui->andOrCombo->currentData().toBool(),
                                     ui->startDate->text().toStdString() + " " +
                                         ui->startTime->text().toStdString(),
                                     ui->endDate->text().toStdString() + " " +
                                         ui->endTime->text().toStdString(),
                                     ui->typeCombo->currentData().toString()
                                                                 .toStdString(),
                                     tv);
            break;
        }
#ifdef INCIDENT
        case FILTERTYPE_INCIDENT:
        {
            //no datetime column, so no DateTimeDelegate
            tv->setItemDelegateForColumn(QtTableUtils::COL_TIME, 0);
            if (ui->idCombo->isVisible())
            {
                txt = ui->idCombo->currentText();
                success = QtTableUtils::getIncidentData(txt.toInt(), tv);
                //put txt at top of recent search list
                QtUtils::addToComboBox(txt, ui->idCombo);
            }
            else
            {
                string pri;
                if (ui->priorityCombo->currentText() != tr("All"))
                    pri = ui->priorityCombo->currentText().toStdString();
                string cat;
                if (ui->categoryCombo->currentText() != tr("All"))
                    cat = ui->categoryCombo->currentText().toStdString();
                txt = ui->descCombo->currentText().trimmed();
                QString rsc(ui->resourceCombo->currentText().trimmed());
                success = QtTableUtils::getIncidentData(
                                    ui->statusCombo->currentData().toInt(),
                                    ui->startDate->text().toStdString() + " " +
                                        ui->startTime->text().toStdString(),
                                    ui->endDate->text().toStdString() + " " +
                                        ui->endTime->text().toStdString(),
                                    ui->stateCombo->currentData().toString()
                                                                 .toStdString(),
                                    pri, cat, txt.toStdString(),
                                    rsc.toStdString(), tv);
                if (!txt.isEmpty())
                    QtUtils::addToComboBox(txt, ui->descCombo);
                if (!rsc.isEmpty())
                    QtUtils::addToComboBox(rsc, ui->resourceCombo);
            }
            if (success && tv->model() != 0)
            {
                ui->plotCheck->setEnabled(true);
                if (ui->plotCheck->isChecked())
                    onPlotCheck(true);
            }
            break;
        }
#endif //INCIDENT
        case FILTERTYPE_LOC:
        {
            tv->setItemDelegateForColumn(QtTableUtils::COL_LOC_TIME,
                                         mDateTimeDelegate);
            QString types;
            txt = ui->locRscCombo->currentText().trimmed();
            if (txt.isEmpty())
            {
                //get type selection
                types = ui->locButton
                          ->property("l").value<QStringList>().join(" ");
            }
            else
            {
                //split using comma and space as separator
                auto l(txt.split(QRegExp("[,\\s]+"), Qt::SkipEmptyParts));
                l.removeDuplicates();
                txt.clear();
                foreach (QString s, l)
                {
                    if (s.toInt() > 0)
                        txt.append(s).append(" ");
                }
                if (txt.isEmpty())
                {
                    to = -1; //just to indicate error
                }
                else
                {
                    txt.chop(1); //remove trailing space
                    QtUtils::addToComboBox(txt, ui->locRscCombo);
                    //need to do this because addToComboBox() does not do it if
                    //txt is already at the top of the list
                    ui->locRscCombo->setCurrentIndex(0);
                    setTitleAndToolTip();
                }
            }
            if (txt.isEmpty() && types.isEmpty())
            {
                success = true; //prevent error dialog
            }
            else
            {
                res = QtTableUtils::getLocData(txt.toStdString(),
                                     types.toStdString(),
                                     ui->startDate->text().toStdString() + " " +
                                         ui->startTime->text().toStdString(),
                                     ui->endDate->text().toStdString() + " " +
                                         ui->endTime->text().toStdString(),
                                     tv);
                if (res >= 0)
                {
                    success = true;
                    if (ui->locButton->isCheckable())
                        tv->hideColumn(QtTableUtils::COL_TYPE);
                }
            }
            break;
        }
        case FILTERTYPE_MMS:
        {
            tv->setItemDelegateForColumn(QtTableUtils::COL_TIME,
                                         mDateTimeDelegate);
            txt = ui->msgCombo->currentText().trimmed();
            success = QtTableUtils::getMsgData(from, to,
                                     CmnTypes::COMMS_MSG_MMS,
                                     ui->andOrCombo->currentData().toBool(),
                                     ui->startDate->text().toStdString() + " " +
                                         ui->startTime->text().toStdString(),
                                     ui->endDate->text().toStdString() + " " +
                                         ui->endTime->text().toStdString(),
                                     txt.toStdString(), tv);
            break;
        }
        case FILTERTYPE_MSG:
        {
            tv->setItemDelegateForColumn(QtTableUtils::COL_TIME,
                                         mDateTimeDelegate);
            success = QtTableUtils::getMsgData(from, to, -1,
                                     ui->andOrCombo->currentData().toBool(),
                                     ui->startDate->text().toStdString() + " " +
                                         ui->startTime->text().toStdString(),
                                     ui->endDate->text().toStdString() + " " +
                                         ui->endTime->text().toStdString(),
                                     "", tv);
            break;
        }
        case FILTERTYPE_SDS:
        {
            tv->setItemDelegateForColumn(QtTableUtils::COL_TIME,
                                         mDateTimeDelegate);
            txt = ui->msgCombo->currentText().trimmed();
            success = QtTableUtils::getMsgData(from, to,
                                     CmnTypes::COMMS_MSG_SDS,
                                     ui->andOrCombo->currentData().toBool(),
                                     ui->startDate->text().toStdString() + " " +
                                         ui->startTime->text().toStdString(),
                                     ui->endDate->text().toStdString() + " " +
                                         ui->endTime->text().toStdString(),
                                     txt.toStdString(), tv);
            if (!txt.isEmpty())
                QtUtils::addToComboBox(txt, ui->msgCombo);
            break;
        }
        case FILTERTYPE_STS:
        default:
        {
            tv->setItemDelegateForColumn(QtTableUtils::COL_TIME,
                                         mDateTimeDelegate);
            success = QtTableUtils::getMsgData(from, to,
                            CmnTypes::COMMS_MSG_STATUS,
                            ui->andOrCombo->currentData().toBool(),
                            ui->startDate->text().toStdString() + " " +
                                ui->startTime->text().toStdString(),
                            ui->endDate->text().toStdString() + " " +
                                ui->endTime->text().toStdString(),
                            ui->stsCombo->currentText().trimmed().toStdString(),
                            tv);
            break;
        }
    }
    QApplication::restoreOverrideCursor();
    if (!success)
    {
        QMessageBox::critical(this, tr("Report Error"),
                              tr("Failed to display report because of database "
                                 "link error."));
        return;
    }
    if (mFilterType == FILTERTYPE_LOC)
    {
        if (to < 0)
            QMessageBox::critical(this, tr("Report: Invalid Field"),
                                  tr("'%1' contains no valid number.")
                                      .arg(ui->locRscLabel->text()));
        else if (res != 0)
            QMessageBox::critical(this, tr("Report: Too Many Records"),
                                  tr("%1 records found - may take too long to "
                                     "display.\n"
                                     "Please refine search parameters to limit "
                                     "to %2.")
                                      .arg(res).arg(QtTableUtils::MAX_RES));
    }
    auto *mdl = tv->model();
    if (mdl != 0)
        to = mdl->rowCount(); //reuse 'to'
    else
        to = 0;
    ui->resultTitleLabel->setText(ui->resultTitleLabel->text() + " (" +
                                  QString::number(to) + ")");
    ui->printButton->setEnabled(to != 0);
}

void Report::showFilter(int type)
{
    bool loc = (type == FILTERTYPE_LOC);
    bool inc = (type == FILTERTYPE_INCIDENT);
    bool b = !loc && !inc;
    ui->fromLabel->setVisible(b);
    ui->fromCombo->setVisible(b);
    ui->andOrCombo->setVisible(b);
    ui->toLabel->setVisible(b);
    ui->toFrame->setVisible(b);
    ui->msgLabel->setVisible(type == FILTERTYPE_SDS || type == FILTERTYPE_MMS);
    ui->msgCombo->setVisible(type == FILTERTYPE_SDS || type == FILTERTYPE_MMS);
    ui->stsLabel->setVisible(type == FILTERTYPE_STS);
    ui->stsCombo->setVisible(type == FILTERTYPE_STS);
    ui->searchByLabel->setVisible(inc);
    ui->searchByBox->setVisible(inc);
    ui->plotCheck->setVisible(inc);
    if (inc)
        inc = ui->idRadioBtn->isChecked();
    ui->idLabel->setVisible(inc);
    ui->idCombo->setVisible(inc);
    ui->startDateLabel->setVisible(!inc);
    ui->startDate->setVisible(!inc);
    ui->startTime->setVisible(!inc);
    ui->endDateLabel->setVisible(!inc);
    ui->endDate->setVisible(!inc);
    ui->endTime->setVisible(!inc);
    b = (type == FILTERTYPE_CALL);
    ui->typeLabel->setVisible(b);
    ui->typeCombo->setVisible(b);
    ui->toFrame->setEnabled(!b ||
                            ui->typeCombo->currentText() != tr("Broadcast"));
    b = (type == FILTERTYPE_INCIDENT && ui->detailsRadioBtn->isChecked());
    ui->stateLabel->setVisible(b);
    ui->stateCombo->setVisible(b);
    ui->priorityLabel->setVisible(b);
    ui->priorityCombo->setVisible(b);
    ui->categoryLabel->setVisible(b);
    ui->categoryCombo->setVisible(b);
    ui->statusLabel->setVisible(b);
    ui->statusCombo->setVisible(b);
    ui->resourceLabel->setVisible(b);
    ui->resourceCombo->setVisible(b);
    ui->descLabel->setVisible(b);
    ui->descCombo->setVisible(b);
    ui->locButton->setVisible(loc && !ui->locButton->isCheckable());
    ui->locRscLabel->setVisible(loc);
    ui->locRscCombo->setVisible(loc);
}

bool Report::validateFields(int &from, int &to)
{
    if (ui->startDate->isVisible())
    {
        int dateRange = ui->startDate->date().daysTo(ui->endDate->date());
        if (dateRange > MAX_DAYS)
        {
            QMessageBox::critical(this, tr("Report: Invalid Date"),
                                  tr("Date range must not exceed %1 days.")
                                      .arg(MAX_DAYS));
            return false;
        }
        if (dateRange < 0)
        {
            QMessageBox::critical(this, tr("Report: Invalid Date"),
                                  tr("'%1' date must not be earlier than '%2' "
                                     "date.")
                                      .arg(ui->endDateLabel->text(),
                                           ui->startDateLabel->text()));
            return false;
        }
        if (dateRange == 0 && (ui->endTime->time() < ui->startTime->time()))
        {
            QMessageBox::critical(this, tr("Report: Invalid Time"),
                                  tr("'%1' time must not be earlier than '%2' "
                                     "time.")
                                      .arg(ui->endDateLabel->text(),
                                           ui->startDateLabel->text()));
            return false;
        }
    }
    if (ui->fromCombo->isVisible())
    {
        QString fromStr(ui->fromCombo->currentText());
        if (!fromStr.isEmpty())
        {
            from = fromStr.toInt();
            if (from <= 0)
            {
                QMessageBox::critical(this, tr("Report: Invalid Field"),
                                      tr("'%1' contains invalid number.")
                                          .arg(ui->fromLabel->text()));
                return false;
            }
            QtUtils::addToComboBox(fromStr, ui->fromCombo);
        }
    }
    if (ui->toFrame->isVisible() && ui->toCombo->isEnabled())
    {
        QString toStr(ui->toCombo->currentText().trimmed());
        if (!toStr.isEmpty())
        {
            to = ResourceData::getId(toStr);
            if (to <= 0)
            {
                QMessageBox::critical(this, tr("Report: Invalid Field"),
                                      tr("'%1' contains invalid number or "
                                         "Group name.")
                                          .arg(ui->toLabel->text()));
                return false;
            }
            QtUtils::addToComboBox(toStr, ui->toCombo);
        }
    }
    if (ui->idCombo->isVisible() && ui->idCombo->currentText().isEmpty())
    {
        QMessageBox::critical(this, tr("Report: Invalid Field"),
                              tr("Field '%1' must contain a value.")
                                  .arg(ui->idLabel->text()));
        return false;
    }
    return true;
}

#define APPENDTOOLTIP_IF(name) \
    do \
    { \
        s = name##Combo->currentText().trimmed(); \
        if (!s.isEmpty()) \
            toolTip.append("\n").append(name##Label->text()).append(" ") \
                   .append(s); \
    } \
    while (0)

#define APPENDTOOLTIP(name) \
    do \
    { \
        toolTip.append("\n").append(name##Label->text()).append(" ") \
               .append(name##Combo->currentText().trimmed()); \
    } \
    while (0)

void Report::setTitleAndToolTip()
{
    QString toolTip;
    QString s;
    if (ui->startDate->isVisible())
    {
        toolTip.append(ui->startDateLabel->text() + " " +
                       ui->startDate->text() + " " + ui->startTime->text() +
                       "\n" + ui->endDateLabel->text() + " " +
                       ui->endDate->text() + " " + ui->endTime->text());
        if (ui->fromCombo->isVisible())
        {
            APPENDTOOLTIP_IF(ui->from);
            if (ui->toFrame->isVisible() && ui->toCombo->isEnabled())
                APPENDTOOLTIP_IF(ui->to);
        }
    }
    QString title(tr("Results for "));
    switch (mFilterType)
    {
        case FILTERTYPE_CALL:
            title.append(ui->callRadioBtn->text());
            APPENDTOOLTIP(ui->type);
            break;
#ifdef INCIDENT
        case FILTERTYPE_INCIDENT:
            title.append(ui->incidentRadioBtn->text());
            if (ui->idCombo->isVisible())
            {
                toolTip.append(ui->idLabel->text()).append(" ")
                       .append(ui->idCombo->currentText());
            }
            else
            {
                APPENDTOOLTIP(ui->priority);
                APPENDTOOLTIP(ui->state);
                APPENDTOOLTIP(ui->category);
                APPENDTOOLTIP(ui->status);
                APPENDTOOLTIP_IF(ui->desc);
                APPENDTOOLTIP_IF(ui->resource);
            }
            break;
#endif
        case FILTERTYPE_LOC:
            title.append(ui->locRadioBtn->text());
            APPENDTOOLTIP_IF(ui->locRsc);
            if (s.isEmpty())
            {
                s = ui->locButton->toolTip();
                if (!s.isEmpty())
                    toolTip.append("\n").append(ui->locButton->text())
                           .append(": ").append(s);
            }
            break;
        case FILTERTYPE_MMS:
            title.append(ui->mmsRadioBtn->text());
            APPENDTOOLTIP_IF(ui->msg);
            break;
        case FILTERTYPE_MSG:
            title.append(ui->msgRadioBtn->text());
            break;
        case FILTERTYPE_SDS:
            title.append(ui->sdsRadioBtn->text());
            APPENDTOOLTIP_IF(ui->msg);
            break;
        case FILTERTYPE_STS:
            title.append(ui->stsRadioBtn->text());
            APPENDTOOLTIP_IF(ui->sts);
            break;
        default:
            break; //do nothing
    }
    ui->resultTitleLabel->setText(title);
    ui->resultTitleLabel->setToolTip(toolTip);
}

void Report::doPrint(int printType)
{
    QString title(windowTitle());
    title.append(" - ");
    int dtCol = -1;
    int typeCol = -1;
    switch (mFilterType)
    {
        case FILTERTYPE_CALL:
            title.append(ui->callRadioBtn->text());
            dtCol = QtTableUtils::COL_CALL_TIME;
            break;
#ifdef INCIDENT
        case FILTERTYPE_INCIDENT:
            title.append(ui->incidentRadioBtn->text());
            break;
#endif
        case FILTERTYPE_LOC:
            title.append(ui->locRadioBtn->text());
            dtCol = QtTableUtils::COL_LOC_TIME;
            break;
        case FILTERTYPE_MMS:
            title.append(ui->mmsRadioBtn->text());
            dtCol = QtTableUtils::COL_TIME;
            break;
        case FILTERTYPE_MSG:
            title.append(ui->msgRadioBtn->text());
            dtCol = QtTableUtils::COL_TIME;
            typeCol = QtTableUtils::COL_TYPE;
            break;
        case FILTERTYPE_SDS:
            title.append(ui->sdsRadioBtn->text());
            dtCol = QtTableUtils::COL_TIME;
            break;
        case FILTERTYPE_STS:
        default:
            title.append(ui->stsRadioBtn->text());
            dtCol = QtTableUtils::COL_TIME;
            break;
    }
    Document doc(printType, title, ui->resultTitleLabel->toolTip());
    doc.addTable(ui->resultTable,
                 (printType == Document::PRINTTYPE_EXCEL)? title: "",
                 dtCol, typeCol);
    doc.print(this, title.remove(" "));
}

void Report::cleanup()
{
    auto *mdl = qobject_cast<QStandardItemModel *>(ui->resultTable->model());
    if (mdl == 0)
        return;
#ifdef INCIDENT
    if (mFilterType == FILTERTYPE_INCIDENT)
    {
        int i = mdl->rowCount() - 1;
        for (; i>=0; --i)
        {
            delete mdl->item(i, QtTableUtils::COL_INC_ID)
                      ->data(Qt::UserRole).value<IncidentData *>();
        }
    }
#endif
    ui->resultTable->setModel(0);
    delete mdl;
}
