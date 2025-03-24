/**
 * UI Communications Register implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2020-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: CommsRegister.cpp 1915 2025-03-21 07:08:45Z rosnin $
 * @author Zulzaidi Atan
 */
#include <string>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QGroupBox>
#include <QHeaderView>
#include <QImageReader>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "CmnTypes.h"
#include "DateTimeDelegate.h"
#include "Document.h"
#include "QtUtils.h"
#include "ResourceData.h"
#include "StatusCodes.h"
#include "Style.h"
#include "ui_CommsRegister.h"
#include "CommsRegister.h"

using namespace std;

//call columns
enum eColCall
{
    COL_CALL_DIR,
    COL_CALL_TIME,
    COL_CALL_DURATION,
    COL_CALL_FROM,
    COL_CALL_TO,
    COL_CALL_CALLTYPE,
    COL_CALL_COUNT
};

//message columns
enum eColMsg
{
    COL_MSG_TYPE,
    COL_MSG_DIR,
    COL_MSG_TIME,
    COL_MSG_FROM,
    COL_MSG_TO,
    COL_MSG_DELSTAT,
    COL_MSG_MESSAGE,
    COL_MSG_COUNT
};

//user roles for QTableWidgetItem data
enum eUserRole
{
    //first 2 must match ResourceData::createTableItem() usage
    USERROLE_CALLPARTY = Qt::UserRole,     //COL_MSG_FROM/TO
    USERROLE_CPTYPE    = Qt::UserRole + 1, //COL_MSG_FROM/TO
    USERROLE_MSGREF    = Qt::UserRole + 2, //COL_MSG_FROM/TO
    USERROLE_MSGID     = Qt::UserRole,     //COL_MSG_DELSTAT
    USERROLE_DELSTAT   = Qt::UserRole + 1, //COL_MSG_DELSTAT
    USERROLE_MMSKEY    = Qt::UserRole,     //COL_MSG_MESSAGE
    USERROLE_MSGDLGTBL = Qt::UserRole,     //COL_CALL_CALLTYPE
    USERROLE_MSGDLG    = Qt::UserRole + 1  //COL_MSG_MESSAGE, COL_CALL_CALLTYPE
};

//delivery status
enum eDelStat
{
    DELSTAT_SENDING,
    DELSTAT_SENT,
    DELSTAT_PART,   //partial - has file failure
    DELSTAT_FAIL,
    //for files
    DELSTAT_UPLOADING,
    DELSTAT_UPLOAD_FAILED,
    DELSTAT_UPLOADED,
    DELSTAT_TX_DOWNLOAD_FAILED,   //tx - at recipient
    DELSTAT_TX_DOWNLOADED,
    DELSTAT_DOWNLOADING,          //rx
    DELSTAT_DOWNLOAD_FAILED,
    DELSTAT_DOWNLOAD_FAILED_PERM, //permanent failure - file no longer on server
    DELSTAT_DOWNLOADED,           //temp dir - clean up on exit
    DELSTAT_DOWNLOAD_SAVED,       //temp dir & saved by user - clean up on exit
    DELSTAT_DOWNLOAD_SAVED_USR    //user dir, no need to save or clean up
};

static const string LOGPREFIX("CommsRegister:: ");

map<int, QIcon> CommsRegister::mMmsIconMap;

CommsRegister::CommsRegister(Logger *logger, int type, QWidget *parent) :
QWidget(parent), ui(new Ui::CommsRegister), mLogger(logger), mType(type),
mUnreadCount(0), mMmsKey(0)
{
    ui->setupUi(this);
    connect(ui->callButton, &QPushButton::toggled, this,
            [this](bool checked)
            {
                //show/hide call table
                ui->callFrame->setVisible(!checked);
                ui->callButton->setFlat(checked);
            });
    connect(ui->msgButton, &QPushButton::toggled, this,
            [this](bool checked)
            {
                //show/hide message table
                ui->msgFrame->setVisible(!checked);
                ui->msgButton->setFlat(checked);
            });
    connect(ui->msgTable, &QTableWidget::clicked, this,
            [this](const QModelIndex &idx)
            {
                //disable highlighting of the selected table row and update the
                //unread messages counter
                int row = idx.row();
                if (QtTableUtils::tableNormal(ui->msgTable, row) != 0)
                {
                    --mUnreadCount;
                    unreadCountChanged();
                }
                if (getCellData(row, COL_MSG_TYPE) == CmnTypes::COMMS_MSG_STATUS)
                    emit msgSelected(CmnTypes::COMMS_MSG_STATUS,
                              ui->msgTable->item(row, COL_MSG_MESSAGE)->text());
            });
    QTableWidget *tw;
    QMenu *menu;
    if (type == TYPE_MSG)
    {
        //move float and print buttons from titleFrame
        ui->floatButton->setParent(ui->msgTitleFrame);
        qobject_cast<QHBoxLayout *>(ui->msgTitleFrame->layout())
            ->insertWidget(1, ui->floatButton); //put next to title
        ui->printButton->setParent(ui->msgTitleFrame);
        ui->msgTitleFrame->layout()->addWidget(ui->printButton);
        delete ui->titleFrame;
        //remove splitter from scroll area and make msgFrame the scroll widget
        (void) ui->scrollArea->takeWidget();
        ui->scrollArea->setWidget(ui->msgFrame);
        delete ui->splitter1;
    }
    else
    {
        ui->callTable->setColumnCount(6);
        ui->callTable->setHorizontalHeaderLabels(QStringList()
                                    << tr("Dir") << tr("Time") << tr("Duration")
                                    << tr("From") << tr("To") << tr("Type"));
        tw = ui->callTable;
        //add hidden column for filter data - MUST be last column
        tw->setColumnCount(COL_CALL_COUNT + 1);
        tw->hideColumn(COL_CALL_COUNT);
        tw->setColumnWidth(COL_CALL_DIR, 50); //smaller than others
        tw->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tw, &QTableWidget::cellDoubleClicked, this,
                [this](int row, int col)
                {
                    //either emit signal for calling the ID in the selected cell
                    //if it is not the current user, or show PTT details, if any
                    auto *tw = qobject_cast<QTableWidget *>(sender());
                    QTableWidgetItem *itm;
                    if (col == COL_CALL_FROM || col == COL_CALL_TO)
                    {
                        itm = tw->item(row, col);
                        if (itm->text() != mUserName)
                        {
                            int id = ResourceData::getTableItemId(itm);
                            if (id != 0)
                            {
                                emit startAction(CmnTypes::ACTIONTYPE_CALL,
                                                 ResourceData::getType(id), id);
                                return;
                            }
                        }
                    }
                    //show PTT details
                    itm = tw->item(row, COL_CALL_CALLTYPE);
                    auto v(itm->data(USERROLE_MSGDLGTBL));
                    if (!v.canConvert<MessageDialog::TableDataT>())
                        return;
                    auto *md = itm->data(USERROLE_MSGDLG)
                                   .value<MessageDialog *>();
                    if (md != 0)
                    {
                        md->activateWindow();
                    }
                    else
                    {
                        auto data(v.value<MessageDialog::TableDataT>());
                        md = MessageDialog::showMessage(
                              tr("Group Call Details: %1")
                                  .arg(tw->item(row, COL_CALL_TO)->text()),
                              tr("%1\tDuration: %2\nFrom: %3\nPTT Sequence:")
                                  .arg(DateTimeDelegate::getDateTime(
                                           tw->item(row, COL_CALL_TIME)
                                             ->data(Qt::DisplayRole)),
                                       tw->item(row, COL_CALL_DURATION)->text(),
                                       tw->item(row, COL_CALL_FROM)->text()),
                              data,
                              QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_CALL),
                              true, this);
                        itm->setData(USERROLE_MSGDLG, QVariant::fromValue(md));
                        connect(md, &MessageDialog::finished, this,
                                [md, this](int)
                                {
                                    auto *tw2 = ui->callTable;
                                    QTableWidgetItem *it;
                                    for (auto r=tw2->rowCount()-1; r>=0; --r)
                                    {
                                        it = tw2->item(r, COL_CALL_CALLTYPE);
                                        if (it->data(USERROLE_MSGDLG)
                                                .value<MessageDialog *>() ==
                                            md)
                                        {
                                            //dlg automatically deleted on close
                                            it->setData(USERROLE_MSGDLG, 0);
                                            break;
                                        }
                                    }
                                });
                    }
                });
        connect(tw, &QTableWidget::customContextMenuRequested, this,
                [this](const QPoint &pos)
                {
                    QtTableUtils::tableContextMenu(ui->callTable, pos);
                    if (ui->callTable->rowCount() == 0)
                    {
                        QtTableUtils::updateFilterButtonText(
                                                   ui->callFilterButton, false);
                        if (mType == TYPE_CALL || ui->msgTable->rowCount() == 0)
                            ui->printButton->setEnabled(false);
                    }
                });
        connect(tw->verticalHeader(), &QHeaderView::sectionDoubleClicked, this,
                [this] { ui->callTable->resizeRowsToContents(); });
        tw->setItemDelegateForColumn(COL_CALL_TIME, new DateTimeDelegate(this));
        menu = new QMenu(this);
        //filter menu - call directions
        QtTableUtils::addDirFilter(ui->callTable, ui->callFilterButton,
                                   COL_CALL_DIR, &mCallHideDirs, menu, true);
        ui->callFilterButton->setMenu(menu);
    }

    if (type == TYPE_CALL)
    {
        //move float and print buttons from titleFrame
        ui->floatButton->setParent(ui->callTitleFrame);
        qobject_cast<QHBoxLayout *>(ui->callTitleFrame->layout())
            ->insertWidget(1, ui->floatButton); //put next to title
        ui->printButton->setParent(ui->callTitleFrame);
        ui->callTitleFrame->layout()->addWidget(ui->printButton);
        delete ui->titleFrame;
        //remove splitter from scroll area and make callFrame the scroll widget
        (void) ui->scrollArea->takeWidget();
        ui->scrollArea->setWidget(ui->callFrame);
        delete ui->splitter1;
    }
    else
    {
        ui->msgTable->setColumnCount(COL_MSG_COUNT);
        ui->msgTable->setHorizontalHeaderLabels(QStringList()
                          << tr("Type") << tr("Dir") << tr("Time") << tr("From")
                          << tr("To") << tr("Delivery") << tr("Message"));
        tw = ui->msgTable;
        //add hidden column for filter data - MUST be last column
        tw->setColumnCount(COL_MSG_COUNT + 1);
        tw->hideColumn(COL_MSG_COUNT);
        tw->setColumnWidth(COL_MSG_TYPE, 50); //some columns smaller than others
        tw->setColumnWidth(COL_MSG_DIR, 50);
        tw->setItemDelegateForColumn(COL_MSG_TIME, new DateTimeDelegate(this));
        tw->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(tw, &QTableWidget::cellDoubleClicked, this,
                [this](int row, int col)
                {
                    //either emit signal to send message to the ID in the
                    //selected cell if it is not the current user, or show msg
                    //details if applicable
                    auto *tw = qobject_cast<QTableWidget *>(sender());
                    auto *itm = tw->item(row, col);
                    int msgType = getCellData(row, COL_MSG_TYPE);
                    QString txt(tw->item(row, COL_MSG_MESSAGE)->text());
                    if ((col == COL_MSG_FROM || col == COL_MSG_TO) &&
                        itm->text() != mUserName)
                    {
                        switch (msgType)
                        {
                            case CmnTypes::COMMS_MSG_MMS:
                                msgType = CmnTypes::ACTIONTYPE_MMS; //reuse var
                                break;
                            case CmnTypes::COMMS_MSG_SDS:
                                msgType = CmnTypes::ACTIONTYPE_SDS;
                                break;
                            case CmnTypes::COMMS_MSG_STATUS:
                            default:
                                msgType = CmnTypes::ACTIONTYPE_STATUS;
                                break;
                        }
                        emit startAction(msgType,
                                         ResourceData::getTableItemType(itm),
                                         ResourceData::getTableItemId(itm), txt);
                        return;
                    }
                    emit msgSelected(msgType, txt);
                    if (msgType == CmnTypes::COMMS_MSG_STATUS)
                        return;
                    //show SDS/MMS details
                    itm = tw->item(row, COL_MSG_MESSAGE);
                    auto *md = itm->data(USERROLE_MSGDLG)
                                   .value<MessageDialog *>();
                    if (md != 0)
                    {
                        md->activateWindow();
                        return;
                    }
                    bool isMms = (msgType == CmnTypes::COMMS_MSG_MMS);
                    md = new MessageDialog((isMms)? tr("MMS"): tr("SDS"),
                                   tr("%1\nFrom: %2\nTo: %3\nLength: %4")
                                       .arg(DateTimeDelegate::getDateTime(
                                                tw->item(row, COL_MSG_TIME)
                                                  ->data(Qt::DisplayRole)),
                                            tw->item(row, COL_MSG_FROM)->text(),
                                            tw->item(row, COL_MSG_TO)->text())
                                       .arg(txt.size()),
                                   txt,
                                   QtUtils::getActionIcon((isMms)?
                                                      CmnTypes::ACTIONTYPE_MMS:
                                                      CmnTypes::ACTIONTYPE_SDS),
                                   true, this);
                    md->resize(500, 500);
                    itm->setData(USERROLE_MSGDLG, QVariant::fromValue(md));
                    if (isMms)
                    {
                        int key = tw->item(row, COL_MSG_MESSAGE)
                                    ->data(USERROLE_MMSKEY).toInt();
                        if (mMmsMap.count(key) != 0)
                            mmsShowAtt(md, key,
                                       (getCellData(row, COL_MSG_DIR) ==
                                        CmnTypes::COMMS_DIR_OUT));
                    }
                    md->show();
                    connect(md, &MessageDialog::finished, this,
                            [md, this](int)
                            {
                                //remove dialog box from the message table
                                auto *tw2 = ui->msgTable;
                                QTableWidgetItem *itm2;
                                for (auto r=tw2->rowCount()-1; r>=0; --r)
                                {
                                    itm2 = tw2->item(r, COL_MSG_MESSAGE);
                                    if (itm2->data(USERROLE_MSGDLG)
                                            .value<MessageDialog *>() ==
                                        md)
                                    {
                                        //dialog automatically deleted on close
                                        itm2->setData(USERROLE_MSGDLG, 0);
                                        break;
                                    }
                                }
                            });
                });
        connect(tw, &QTableWidget::customContextMenuRequested, this,
                [this](const QPoint &pos)
                {
                    int n = ui->msgTable->rowCount();
                    if (QtTableUtils::tableContextMenu(ui->msgTable, pos,
                                           mmsUnsavedAtt(false), &mUnreadCount))
                        unreadCountChanged();
                    if (ui->msgTable->rowCount() != n)
                    {
                        //at least 1 row removed
                        mmsCleanup(false);
                        if (ui->msgTable->rowCount() == 0)
                        {
                            QtTableUtils::updateFilterButtonText(
                                                    ui->msgFilterButton, false);
                            if (mType == TYPE_MSG ||
                                ui->callTable->rowCount() == 0)
                                ui->printButton->setEnabled(false);
                        }
                    }
                });
        connect(tw->verticalHeader(), &QHeaderView::sectionDoubleClicked, this,
                [this] { ui->msgTable->resizeRowsToContents(); });
        //filter menu - add message types
        menu = new QMenu(this);
        auto *vbox = new QVBoxLayout();
        vbox->addWidget(QtTableUtils::addFilterCheckBox(tw, ui->msgFilterButton,
                               COL_MSG_TYPE, CmnTypes::COMMS_MSG_SDS, tr("SDS"),
                               QtUtils::getCommsIcon(CmnTypes::COMMS_MSG_SDS),
                               &mMsgHideTypes, menu));
        vbox->addWidget(QtTableUtils::addFilterCheckBox(tw, ui->msgFilterButton,
                         COL_MSG_TYPE, CmnTypes::COMMS_MSG_STATUS, tr("Status"),
                         QtUtils::getCommsIcon(CmnTypes::COMMS_MSG_STATUS),
                         &mMsgHideTypes, menu));
        vbox->addWidget(QtTableUtils::addFilterCheckBox(tw, ui->msgFilterButton,
                               COL_MSG_TYPE, CmnTypes::COMMS_MSG_MMS, tr("MMS"),
                               QtUtils::getCommsIcon(CmnTypes::COMMS_MSG_MMS),
                               &mMsgHideTypes, menu));
        auto *gbox = new QGroupBox(tr("Types"), menu);
        gbox->setLayout(vbox);
        auto *wAct = new QWidgetAction(menu);
        wAct->setDefaultWidget(gbox);
        menu->addAction(wAct);
        //filter menu - add message directions
        QtTableUtils::addDirFilter(ui->msgTable, ui->msgFilterButton,
                                   COL_MSG_DIR, &mMsgHideDirs, menu);
        ui->msgFilterButton->setMenu(menu);
    }
    ui->floatButton->setIconSize(QSize(ui->printButton->height(),
                                       ui->printButton->height()));
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
                    //remove title bar help button and disable close button
                    dlg->setWindowFlags(dlg->windowFlags() &
                                        ~Qt::WindowContextHelpButtonHint &
                                        ~Qt::WindowCloseButtonHint);
                    switch (mType)
                    {
                        case TYPE_CALL:
                            dlg->setWindowTitle(tr("Call Register"));
                            break;
                        case TYPE_MSG:
                            dlg->setWindowTitle(tr("Message Register"));
                            break;
                        case TYPE_ALL:
                        default:
                            dlg->setWindowTitle(tr("Communications Register"));
                            break;
                    }
                    dlg->setLayout(layout);
                    dlg->setMinimumSize(200, 100);
                    if (dlgSize.isValid())
                        dlg->resize(dlgSize);
                    else
                        dlg->resize(width(), height());
                    dlg->show();
                    ui->floatButton->setToolTip(tr("Dock"));
                    if (mType == TYPE_CALL)
                        emit floatOrDock(0);
                }
                else
                {
                    auto *dlg = qobject_cast<QDialog *>(parentWidget());
                    if (dlg != 0)
                    {
                        //dock back into parent, which will delete dialog
                        dlgSize = dlg->size();
                        ui->floatButton->setToolTip(tr("Float"));
                        emit floatOrDock(dlg);
                    }
                }
            });
    //print menu
    menu = new QMenu(this);
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
}

CommsRegister::~CommsRegister()
{
    mmsCleanup(true);
    if (mType != TYPE_MSG)
        delete ui->callTable->itemDelegateForColumn(COL_CALL_TIME);
    if (mType != TYPE_CALL)
        delete ui->msgTable->itemDelegateForColumn(COL_MSG_TIME);
    delete ui;
}

void CommsRegister::setUser(const QString &name, int id)
{
    mUserName = name;
    mUserId = (id == 0 && !name.isEmpty())? ResourceData::getId(name): id;
    if (name.isEmpty() && ui->floatButton->isChecked())
        ui->floatButton->click(); //force dock on logout
}

void CommsRegister::setTheme()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    if (mType == TYPE_ALL)
        ui->titleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    if (mType != TYPE_MSG)
        ui->callTitleFrame
          ->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    if (mType != TYPE_CALL)
        ui->msgTitleFrame
          ->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
}

void CommsRegister::addCall(MsgSp *msg)
{
    if (msg == 0)
    {
        assert("Bad param in CommsRegister::addCall" == 0);
        return;
    }
    //time is: <time_t> <client formatted time>, use formatted
    QString st(QString::fromStdString(msg->getFieldString(MsgSp::Field::TIME)));
    //duration is: <seconds> <client formatted duration>, use formatted
    QString sd(QString::fromStdString(
                           msg->getFieldString(MsgSp::Field::TIME_IN_TRAFFIC)));
    MessageDialog::TableDataT d;
    onNewCall(CmnTypes::CALLTYPE_MOBILE, MsgSp::Value::CALL_PRIORITY_UNDEFINED,
              true, st.remove(0, st.indexOf(' ') + 1),
              sd.remove(0, sd.indexOf(' ') + 1),
              ResourceData::getDspTxt(
                                 msg->getFieldInt(MsgSp::Field::CALLING_PARTY)),
              ResourceData::getClientDspTxt(
                                  msg->getFieldInt(MsgSp::Field::CALLED_PARTY)),
              "", d);
}

QIcon CommsRegister::getIcon(const QString &filename)
{
    if (!QImageReader::imageFormat(filename).isEmpty())
        return QIcon(QPixmap(filename).scaled(20, 20, Qt::KeepAspectRatio));
    return QFileIconProvider().icon(QFileInfo(filename));
}

void CommsRegister::onNewCall(int                              type,
                              int                              priority,
                              bool                             isDuplex,
                              const QString                   &startTime,
                              const QString                   &duration,
                              const QString                   &callingParty,
                              const QString                   &calledParty,
                              const QString                   &failedCause,
                              const MessageDialog::TableDataT &pttData)
{
#ifdef INCIDENT
    bool doEmitNewComm = false;
#endif
    int dir = CmnTypes::COMMS_DIR_MON;
    QString name;
    switch (type)
    {
        case CmnTypes::CALLTYPE_BROADCAST_OUT:
            dir = CmnTypes::COMMS_DIR_OUT;
            //fallthrough
        case CmnTypes::CALLTYPE_BROADCAST_IN:
            name = tr("Broadcast");
            break;
        case CmnTypes::CALLTYPE_GROUP_OUT:
            dir = CmnTypes::COMMS_DIR_OUT;
            //fallthrough
        case CmnTypes::CALLTYPE_GROUP_IN:
            name = ResourceData::getTypeTxt(ResourceData::TYPE_GROUP);
#ifdef INCIDENT
            doEmitNewComm = true;
#endif
            break;
        case CmnTypes::CALLTYPE_IND_AMBIENCE:
            dir = CmnTypes::COMMS_DIR_OUT;
#ifdef INCIDENT
            doEmitNewComm = true;
#endif
            //fallthrough
        case CmnTypes::CALLTYPE_MON_AMBIENCE:
            name = tr("Ambience");
            break;
        case CmnTypes::CALLTYPE_IND_OUT:
            dir = CmnTypes::COMMS_DIR_OUT;
            name = getIndividualCallName(isDuplex);
#ifdef INCIDENT
            if (!duration.isEmpty())
                doEmitNewComm = true;
#endif
            break;
        case CmnTypes::CALLTYPE_DISPATCHER:
        case CmnTypes::CALLTYPE_MOBILE:
            if (callingParty.isEmpty())
                dir = CmnTypes::COMMS_DIR_OUT;
            else if (duration.isEmpty())
                dir = CmnTypes::COMMS_DIR_MISSED;
            else
                dir = CmnTypes::COMMS_DIR_IN;
            name = ResourceData::getTypeTxt(
                      (type == CmnTypes::CALLTYPE_DISPATCHER)?
                      ResourceData::TYPE_DISPATCHER: ResourceData::TYPE_MOBILE);
            break;
        case CmnTypes::CALLTYPE_IND_IN:
            if (duration.isEmpty())
            {
                dir = CmnTypes::COMMS_DIR_MISSED;
            }
            else
            {
                dir = CmnTypes::COMMS_DIR_IN;
#ifdef INCIDENT
                doEmitNewComm = true;
#endif
            }
            name = getIndividualCallName(isDuplex);
            break;
        case CmnTypes::CALLTYPE_MON_IND_DUPLEX:
            name = getIndividualCallName(true);
            break;
        case CmnTypes::CALLTYPE_MON_IND_PTT:
            name = getIndividualCallName(false);
            break;
        default:
            assert("Invalid type in CommsRegister::onNewCall" == 0);
            return;
    }
    auto *tw = ui->callTable;
    //temporarily disable sorting because otherwise modifying a cell may change
    //its row sort position
    tw->setSortingEnabled(false);
    tw->insertRow(0);
    if (mCallHideDirs.count(dir) != 0)
        QtTableUtils::updateFilteredRow(tw, 0, COL_CALL_DIR, true,
                                        ui->callFilterButton);
    auto *lbl = new QLabel();
    lbl->setObjectName(QString::number(dir));
    const auto &icon(QtUtils::getCommsIcon(dir));
    lbl->setPixmap(icon.pixmap(icon.actualSize(QSize(24, 24))));
    lbl->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lbl->setAttribute(Qt::WA_TranslucentBackground);
    ui->callTable->setCellWidget(0, COL_CALL_DIR, lbl);
    auto *itm = newItem();
    itm->setData(Qt::DisplayRole,
                 QDateTime::fromString(startTime, QtUtils::timestampFormat));
    tw->setItem(0, COL_CALL_TIME, itm);
    tw->setItem(0, COL_CALL_DURATION, newItem(duration));
    if (!failedCause.isEmpty())
        tw->item(0, COL_CALL_DURATION)->setToolTip(failedCause);
    const QString &cp((callingParty.isEmpty())? mUserName: callingParty);
    tw->setItem(0, COL_CALL_FROM,
                ResourceData::createTableItem(ResourceData::getId(cp),
                                              (dir == CmnTypes::COMMS_DIR_OUT)?
                                                  ResourceData::TYPE_DISPATCHER:
                                                  ResourceData::TYPE_UNKNOWN,
                                              cp));
    tw->setItem(0, COL_CALL_TO,
                ResourceData::createTableItem(ResourceData::getId(calledParty),
                                              ResourceData::TYPE_UNKNOWN,
                                              calledParty));
    if (priority >= MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_1)
    {
        name.append(" - ");
        if (priority == MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)
            name.append(tr("Emergency"));
        else
            name.append(tr("Preemptive"));
    }
    itm = newItem(name);
    if (priority == MsgSp::Value::CALL_PRIORITY_PREEMPTIVE_4_EMERGENCY)
        itm->setForeground(QBrush(Qt::red));
    if (!pttData.empty())
        itm->setData(USERROLE_MSGDLGTBL, QVariant::fromValue(pttData));
    tw->setItem(0, COL_CALL_CALLTYPE, itm);
    tw->setSortingEnabled(true);
    tw->resizeRowsToContents();
    if (dir == CmnTypes::COMMS_DIR_MISSED)
        emit missedCall();
#ifdef INCIDENT
    //notify Incident - type param can be anything other than
    //CmnTypes::COMMS_MSG_*
    if (doEmitNewComm)
        emit newComm(-1, callingParty, calledParty);
#endif
    ui->printButton->setEnabled(true);
}

bool CommsRegister::onNewMsg(MsgSp *msg, const QString &err)
{
    if (msg == 0)
    {
        assert("Bad param in CommsRegister::onNewMsg" == 0);
        return true;
    }
    LOGGER_DEBUG(mLogger, LOGPREFIX << "onNewMsg:\n" << *msg);
    int type = msg->getType();
    switch (type)
    {
        case MsgSp::Type::MMS_RPT:
            if (!msg->hasField(MsgSp::Field::CALLED_PARTY) ||
                msg->hasField(MsgSp::Field::CALLING_PARTY))
                return mmsCheckTx(msg, err); //server/recipient response
            mmsCheckRx(msg, err); //download finished notification
            return true;
        case MsgSp::Type::MMS_TRANSFER:
            if (!msg->isResultSuccessful() &&
                !msg->hasField(MsgSp::Field::CALLING_PARTY))
            {
                mmsCheckTx(msg, err); //upload failure notification
                return true;
            }
            if (msg->hasField(MsgSp::Field::FILE_PATH) && mmsCheckRx(msg, err))
                return true; //subsequent file in one send
            //else new incoming
            break;
        case MsgSp::Type::SDS_TRANSFER:
        case MsgSp::Type::STATUS:
            if (msg->isResultSuccessful())
                break;
            //fallthrough
        case MsgSp::Type::SDS_RPT:
        case MsgSp::Type::STATUS_RPT:
            setDeliveryStatus(msg);
            return true;
        default:
            break; //do nothing
    }
    auto *tw = ui->msgTable;
    switch (type)
    {
        case MsgSp::Type::MMS_TRANSFER:
            type = CmnTypes::COMMS_MSG_MMS; //type changes meaning now
            break;
        case MsgSp::Type::MON_SDS:
        case MsgSp::Type::SDS_TRANSFER:
            type = CmnTypes::COMMS_MSG_SDS;
            break;
        case MsgSp::Type::MON_STATUS:
        case MsgSp::Type::STATUS:
        default:
            type = CmnTypes::COMMS_MSG_STATUS;
            break;
    }
    //temporarily disable sorting because otherwise modifying a cell may change
    //its row sort position
    tw->setSortingEnabled(false);
    QTableWidgetItem *itm;
    int dir = msg->getFieldInt(MsgSp::Field::CALLED_PARTY); //temp use
    if (dir == MsgSp::Value::UNDEFINED || dir == mUserId)
    {
        itm = ResourceData::createTableItem(mUserId,
                                            ResourceData::TYPE_DISPATCHER,
                                            mUserName);
        dir = CmnTypes::COMMS_DIR_IN;
    }
    else
    {
        itm = ResourceData::createTableItem(dir,
                        CmnTypes::fromMsgSpIdentityType(
                            msg->getFieldInt(MsgSp::Field::CALLED_PARTY_TYPE)));
        dir = CmnTypes::COMMS_DIR_MON;
    }
    tw->insertRow(0);
    setCellData(COL_MSG_TYPE, type);
    setCellData(COL_MSG_DIR, dir);
    if (mMsgHideTypes.count(type) != 0)
        QtTableUtils::updateFilteredRow(tw, 0, COL_MSG_TYPE, true,
                                        ui->msgFilterButton);
    if (mMsgHideDirs.count(CmnTypes::COMMS_DIR_OUT) != 0)
        QtTableUtils::updateFilteredRow(tw, 0, COL_MSG_DIR, true,
                                        ui->msgFilterButton);
    tw->setItem(0, COL_MSG_TO, itm);
    itm = newItem();
    itm->setData(Qt::DisplayRole, QDateTime::currentDateTime());
    tw->setItem(0, COL_MSG_TIME, itm);
    itm = ResourceData::createTableItem(
                       msg->getFieldInt(MsgSp::Field::CALLING_PARTY),
                       CmnTypes::fromMsgSpIdentityType(
                           msg->getFieldInt(MsgSp::Field::CALLING_PARTY_TYPE)));
    tw->setItem(0, COL_MSG_FROM, itm);
    switch (type)
    {
        case CmnTypes::COMMS_MSG_MMS:
        {
            time_t ref = 0;
            msg->getFieldVal(MsgSp::Field::MSG_REF, ref);
            itm->setData(USERROLE_MSGREF, ref);
            itm = newItem(QString::fromStdString(msg->getUserText()));
            if (msg->hasField(MsgSp::Field::FILE_PATH))
            {
                itm->setIcon(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_ATT));
                QString url(QString::fromStdString(
                                 msg->getFieldString(MsgSp::Field::FILE_PATH)));
                itm->setData(USERROLE_MMSKEY, ++mMmsKey);
                QString fn(QtUtils::fromHexUnicode(
                                 msg->getFieldString(MsgSp::Field::FILE_LIST)));
                qint64 sz = 0;
                msg->getFieldVal(MsgSp::Field::FILE_SIZE, sz);
                int id = msg->getFieldInt(MsgSp::Field::MSG_ID);
                mMmsMap[mMmsKey][id] = MmsData(false,
                                               (msg->isResultSuccessful())?
                                                   DELSTAT_DOWNLOADING:
                                                   DELSTAT_DOWNLOAD_FAILED,
                                               fn, url, sz, new MsgSp(*msg));
#ifdef DEBUG
                mMmsMap[mMmsKey][id].addInfo("DBG: Msg-ID " +
                                             QString::number(id));
#endif
                if (!err.isEmpty())
                {
#ifdef DEBUG
                    mMmsMap[mMmsKey][id].addInfo("DBG: " + url);
#endif
                    mMmsMap[mMmsKey][id].addInfo(err);
                }
            }
            break;
        }
        case CmnTypes::COMMS_MSG_SDS:
        {
            string s;
            msg->getSdsText(s);
            itm = newItem(QString::fromStdString(s));
            break;
        }
        case CmnTypes::COMMS_MSG_STATUS:
        default:
        {
            dir = msg->getFieldInt(MsgSp::Field::STATUS_CODE); //reuse 'dir'
            QString s(QtUtils::fromHexUnicode(
                      QString::fromStdString(StatusCodes::getStatusText(dir))));
            itm = newItem((s.isEmpty())? QString::number(dir): s);
            break;
        }
    }
    tw->setItem(0, COL_MSG_MESSAGE, itm);
    tw->resizeRowsToContents();
    QtTableUtils::tableHighlight(tw, 0, true);
    tw->setSortingEnabled(true);
    ++mUnreadCount;
    unreadCountChanged();
#ifdef INCIDENT
    //notify Incident
    if (type != CmnTypes::COMMS_MSG_MMS)
        emit newComm(type, tw->item(0, COL_MSG_FROM)->text(),
                     tw->item(0, COL_MSG_TO)->text());
#endif
    ui->printButton->setEnabled(true);
    return true;
}

void CommsRegister::onOutMsg(int            msgType,
                             int            idType,
                             int            dstId,
                             int            msgId,
                             const QString &txt)
{
    auto *tw = ui->msgTable;
    tw->setSortingEnabled(false);
    tw->insertRow(0);
    setCellData(COL_MSG_TYPE, msgType);
    setCellData(COL_MSG_DIR, CmnTypes::COMMS_DIR_OUT);
    if (mMsgHideTypes.count(msgType) != 0)
        QtTableUtils::updateFilteredRow(tw, 0, COL_MSG_TYPE, true,
                                        ui->msgFilterButton);
    if (mMsgHideDirs.count(CmnTypes::COMMS_DIR_OUT) != 0)
        QtTableUtils::updateFilteredRow(tw, 0, COL_MSG_DIR, true,
                                        ui->msgFilterButton);
    auto *itm = newItem();
    itm->setData(Qt::DisplayRole, QDateTime::currentDateTime());
    tw->setItem(0, COL_MSG_TIME, itm);
    tw->setItem(0, COL_MSG_FROM,
                ResourceData::createTableItem(mUserId,
                                              ResourceData::TYPE_DISPATCHER,
                                              mUserName));
    if (idType == CmnTypes::IDTYPE_DISPATCHER)
        itm = ResourceData::createTableItem(dstId, idType);
    else
        itm = ResourceData::createTableItem(dstId, idType,
                                        ResourceData::getDspTxt(dstId, idType));
    tw->setItem(0, COL_MSG_TO, itm);
    itm = newItem(tr("Sending"));
    itm->setData(USERROLE_MSGID, msgId);
    tw->setItem(0, COL_MSG_DELSTAT, itm);
    tw->setItem(0, COL_MSG_MESSAGE, newItem(txt));
    tw->resizeRowsToContents();
    tw->setSortingEnabled(true);
    ui->printButton->setEnabled(true);
}

void CommsRegister::onOutMms(MsgSp *msg, const QString &path, const QString &err)
{
    if (mmsCheckTx(msg, err, path))
        return; //subsequent file in one send - updated in existing row
    time_t ref = 0;
    msg->getFieldVal(MsgSp::Field::MSG_REF, ref);
    QString fn((path.isEmpty())? "":
                                 QtUtils::fromHexUnicode(
                                 msg->getFieldString(MsgSp::Field::FILE_LIST)));
    qint64 sz = 0;
    if (!fn.isEmpty())
        msg->getFieldVal(MsgSp::Field::FILE_SIZE, sz);
    bool ok = msg->isResultSuccessful() && err.isEmpty();
    int delstat = (ok)? (fn.isEmpty())? DELSTAT_SENDING: DELSTAT_UPLOADING:
                        (fn.isEmpty())? DELSTAT_FAIL: DELSTAT_UPLOAD_FAILED;
    vector<int> ids; //may have multiple recipients - add one row each
    vector<int> types;
    msg->getFieldVals(MsgSp::Field::CALLED_PARTY, ids);
    msg->getFieldVals(MsgSp::Field::CALLED_PARTY_TYPE, types);
    QDateTime dateTime(QDateTime::currentDateTime());
    int rowstat = (ok)? DELSTAT_SENDING: DELSTAT_FAIL;
    QString rowstatStr((ok)? tr("Sending"): tr("Failed"));
    QString userTxt(QString::fromStdString(msg->getUserText()));
    int idType = 0;
    QTableWidgetItem *itm;
    auto *tw = ui->msgTable;
    tw->setSortingEnabled(false);
    auto it = types.begin();
    for (auto id : ids)
    {
        tw->insertRow(0);
        setCellData(COL_MSG_TYPE, CmnTypes::COMMS_MSG_MMS);
        setCellData(COL_MSG_DIR, CmnTypes::COMMS_DIR_OUT);
        if (mMsgHideTypes.count(CmnTypes::COMMS_MSG_MMS) != 0)
            QtTableUtils::updateFilteredRow(tw, 0, COL_MSG_TYPE, true,
                                            ui->msgFilterButton);
        if (mMsgHideDirs.count(CmnTypes::COMMS_DIR_OUT) != 0)
            QtTableUtils::updateFilteredRow(tw, 0, COL_MSG_DIR, true,
                                            ui->msgFilterButton);
        itm = newItem();
        itm->setData(Qt::DisplayRole, dateTime);
        tw->setItem(0, COL_MSG_TIME, itm);
        tw->setItem(0, COL_MSG_FROM,
                    ResourceData::createTableItem(mUserId,
                                                  ResourceData::TYPE_DISPATCHER,
                                                  mUserName));
        idType = CmnTypes::fromMsgSpIdentityType(*it++);
        if (idType == CmnTypes::IDTYPE_DISPATCHER)
            itm = ResourceData::createTableItem(id, idType);
        else
            itm = ResourceData::createTableItem(id, idType,
                                           ResourceData::getDspTxt(id, idType));
        itm->setData(USERROLE_MSGREF, ref);
        tw->setItem(0, COL_MSG_TO, itm);
        itm = newItem(rowstatStr);
        itm->setData(USERROLE_DELSTAT, rowstat);
        tw->setItem(0, COL_MSG_DELSTAT, itm);
        itm = newItem(userTxt);
        if (!path.isEmpty())
        {
            itm->setIcon(QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_ATT));
            itm->setData(USERROLE_MMSKEY, ++mMmsKey);
            int id = msg->getFieldInt(MsgSp::Field::MSG_ID);
            mMmsMap[mMmsKey][id] = MmsData(true, delstat, fn, path, sz);
#ifdef DEBUG
            mMmsMap[mMmsKey][id].addInfo("DBG: Msg-ID " + QString::number(id));
#endif
            if (!err.isEmpty())
                mMmsMap[mMmsKey][id].addInfo(err);
        }
        tw->setItem(0, COL_MSG_MESSAGE, itm);
    }
    tw->resizeRowsToContents();
    tw->setSortingEnabled(true);
    ui->printButton->setEnabled(true);
}

void CommsRegister::onRscDspOptChanged(int type)
{
    if (mType != TYPE_MSG)
    {
        auto *tw = ui->callTable;
        for (auto r=tw->rowCount()-1; r>=0; --r)
        {
            ResourceData::updateTableItem(tw->item(r, COL_CALL_FROM), type);
            ResourceData::updateTableItem(tw->item(r, COL_CALL_TO), type);
        }
    }
    if (mType != TYPE_CALL)
    {
        auto *tw = ui->msgTable;
        for (auto r=tw->rowCount()-1; r>=0; --r)
        {
            ResourceData::updateTableItem(tw->item(r, COL_MSG_FROM), type);
            ResourceData::updateTableItem(tw->item(r, COL_MSG_TO), type);
        }
    }
}

QString CommsRegister::getIndividualCallName(bool isDuplex) const
{
    if (isDuplex)
        return tr("Individual Duplex");
    return tr("Individual PTT");
}

bool CommsRegister::mmsCheckRx(MsgSp *msg, const QString &err)
{
    assert(msg != 0);
    //MMS_TRANSFER:
    // -no result: new incoming - may be subsequent file in one send, path = url
    // -fail: download failed (could not start), path = url
    //MMS_RPT: download finished (success/failure), path = local/url
    time_t ref = 0;
    msg->getFieldVal(MsgSp::Field::MSG_REF, ref);
    bool isXfer = (msg->getType() == MsgSp::Type::MMS_TRANSFER);
    int cp = msg->getFieldInt((isXfer)?
                       MsgSp::Field::CALLING_PARTY: MsgSp::Field::CALLED_PARTY);
    QTableWidgetItem *itm;
    auto *tw = ui->msgTable;
    tw->setSortingEnabled(false);
    int row = 0;
    int rowCount = tw->rowCount();
    for (; row<rowCount; ++row)
    {
        //find incoming MMS
        if (getCellData(row, COL_MSG_TYPE) != CmnTypes::COMMS_MSG_MMS ||
            getCellData(row, COL_MSG_DIR) == CmnTypes::COMMS_DIR_OUT)
            continue;
        //match ref & call party
        itm = tw->item(row, COL_MSG_FROM);
        if (itm == 0 || itm->data(USERROLE_MSGREF).toLongLong() != ref ||
            itm->data(USERROLE_CALLPARTY).toInt() != cp)
            continue;
        //an MMS may be received more than once with different called parties,
        //e.g. 1 to dispatcher and 1 to grp, or to multiple monitored grps - so
        //must match that too
        itm = tw->item(row, COL_MSG_TO);
        if ((isXfer &&
             itm->data(USERROLE_CALLPARTY).toInt() !=
                 msg->getFieldInt(MsgSp::Field::CALLED_PARTY)) ||
            (!isXfer && msg->hasField(MsgSp::Field::GSSI) &&
             itm->data(USERROLE_CALLPARTY).toInt() !=
                 msg->getFieldInt(MsgSp::Field::GSSI)) ||
            (!isXfer && !msg->hasField(MsgSp::Field::GSSI) &&
             itm->data(USERROLE_CPTYPE).toInt() != CmnTypes::IDTYPE_DISPATCHER))
            continue;
        bool ok = msg->isResultSuccessful() && err.isEmpty();
        QString fp(QString::fromStdString(
                                 msg->getFieldString(MsgSp::Field::FILE_PATH)));
        int id = (msg->hasField(MsgSp::Field::MSG_ACK))?
                 msg->getFieldInt(MsgSp::Field::MSG_ACK):
                 msg->getFieldInt(MsgSp::Field::MSG_ID);
        auto &dt(mMmsMap[ui->msgTable->item(row, COL_MSG_MESSAGE)
                                     ->data(USERROLE_MMSKEY).toInt()][id]);
        if (isXfer)
        {
            //new attachment
            qint64 sz = 0;
            msg->getFieldVal(MsgSp::Field::FILE_SIZE, sz);
            dt = MmsData(false,
                         (ok)? DELSTAT_DOWNLOADING: DELSTAT_DOWNLOAD_FAILED,
                         QtUtils::fromHexUnicode(
                                  msg->getFieldString(MsgSp::Field::FILE_LIST)),
                         fp, sz, new MsgSp(*msg));
#ifdef DEBUG
            dt.addInfo("DBG: Msg-ID " + QString::number(id));
            if (!ok)
                dt.addInfo("DBG: " + fp);
#endif
        }
        //MMS_RPT - download finished
        else if (ok)
        {
            delete dt.msg;
            dt.msg = 0;
            if (dt.state == DELSTAT_DOWNLOAD_FAILED)
                dt.removeLastInfo(); //clear last error
            dt.state = (msg->hasField(MsgSp::Field::STORAGE))?
                       DELSTAT_DOWNLOAD_SAVED_USR: DELSTAT_DOWNLOADED;
            dt.path = fp; //change from url to local path
#ifdef DEBUG
            dt.addInfo("DBG: " + dt.path);
#endif
        }
        else if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                 MsgSp::Value::RESULT_MMSERR_DOWNLOAD_PERM)
        {
            dt.state = DELSTAT_DOWNLOAD_FAILED_PERM;
            delete dt.msg; //no longer needed
            dt.msg = 0;
            dt.addInfo(tr("File no longer available"));
        }
        else
        {
            if (dt.state == DELSTAT_DOWNLOAD_FAILED && !err.isEmpty())
                dt.removeLastInfo(); //overwrite last error
            dt.state = DELSTAT_DOWNLOAD_FAILED;
#ifdef DEBUG
            dt.addInfo("DBG: " + dt.path);
#endif
            dt.addInfo(err);
        }
        break;
    }
    ui->msgTable->setSortingEnabled(true);
    return (row < rowCount);
}

bool CommsRegister::mmsCheckTx(MsgSp         *msg,
                               const QString &err,
                               const QString &path)
{
    //MMS_TRANSFER: update all rows with ref (i.e. all ids)
    // -no result: new outgoing - may be subsequent file in one send
    // -fail: upload failure
    //MMS_RPT:
    // -with CALLING_PARTY: recipient delivery/download success/failure (1 id)
    // -without CALLING_PARTY: server upload success/failure (all ids)
    assert(msg != 0);
    bool ok = msg->isResultSuccessful() && err.isEmpty();
    QString fn(QtUtils::fromHexUnicode(
                                 msg->getFieldString(MsgSp::Field::FILE_LIST)));
    int state;
    if (msg->getType() == MsgSp::Type::MMS_TRANSFER)
        state = (ok)? DELSTAT_UPLOADING: DELSTAT_UPLOAD_FAILED;
    else if (msg->hasField(MsgSp::Field::CALLING_PARTY))
        state = (ok)? DELSTAT_TX_DOWNLOADED: DELSTAT_TX_DOWNLOAD_FAILED;
    else
        state = (ok)? DELSTAT_UPLOADED: DELSTAT_UPLOAD_FAILED;
    int delstat = -1;
    int msgId = 0;
    qint64 sz = 0;
    if (fn.isEmpty())
    {
        delstat = (ok)? DELSTAT_SENT: DELSTAT_FAIL; //text-only MMS_RPT
    }
    else
    {
        msgId = (msg->hasField(MsgSp::Field::MSG_ACK))?
                msg->getFieldInt(MsgSp::Field::MSG_ACK):
                msg->getFieldInt(MsgSp::Field::MSG_ID);
        msg->getFieldVal(MsgSp::Field::FILE_SIZE, sz);
    }
    time_t ref = 0;
    msg->getFieldVal(MsgSp::Field::MSG_REF, ref);
    int cp = msg->getFieldInt((msg->hasField(MsgSp::Field::GSSI))?
                              MsgSp::Field::GSSI: MsgSp::Field::CALLING_PARTY);
    QTableWidgetItem *itm;
    bool found = false;
    auto *tw = ui->msgTable;
    tw->setSortingEnabled(false);
    int row = 0;
    int rowCount = tw->rowCount();
    for (; row<rowCount; ++row)
    {
        if (getCellData(row, COL_MSG_TYPE) != CmnTypes::COMMS_MSG_MMS ||
            getCellData(row, COL_MSG_DIR) != CmnTypes::COMMS_DIR_OUT)
            continue;
        //match ref & positive call party
        itm = tw->item(row, COL_MSG_TO);
        if (itm == 0 || itm->data(USERROLE_MSGREF).toLongLong() != ref ||
            (cp > 0 && itm->data(USERROLE_CALLPARTY).toInt() != cp))
            continue;
        found = true;
        if (msgId != 0)
        {
            auto &fmap = mMmsMap[tw->item(row, COL_MSG_MESSAGE)
                                   ->data(USERROLE_MMSKEY).toInt()];
            if (fmap.count(msgId) == 0 && msg->hasField(MsgSp::Field::MSG_ACK))
                continue; //invalid msgId for this row
            switch (state)
            {
                case DELSTAT_TX_DOWNLOAD_FAILED:
                    fmap[msgId].state = state;
                    fmap[msgId].addInfo(err);
                    if (!msg->hasMmsError())
                    {
                        //MMS_RPT with none-file error like ISSI_NOT_REACHABLE -
                        //if sending multiple files, show error dialog only once
                        //by returning false here for subsequent files
                        for (auto &it : fmap)
                        {
                            if (it.first != msgId && it.second.hasInfo(err))
                            {
                                found = false;
                                break;
                            }
                        }
                    }
                    break;
                case DELSTAT_TX_DOWNLOADED:
                    if (fmap[msgId].state == DELSTAT_TX_DOWNLOAD_FAILED)
                        fmap[msgId].removeLastInfo(); //clear error
                    //fallthrough
                case DELSTAT_UPLOADED:
                    fmap[msgId].state = state;
                    break;
                case DELSTAT_UPLOAD_FAILED:
                    if (msg->getType() == MsgSp::Type::MMS_TRANSFER)
                    {
                        //local failure - new entry
                        fmap[msgId] = MmsData(true, state, fn, path, sz);
#ifdef DEBUG
                        fmap[msgId].addInfo("DBG: Msg-ID " +
                                            QString::number(msgId));
#endif
                    }
                    else
                    {
                        fmap[msgId].state = state; //failure from server
                    }
                    fmap[msgId].addInfo(err);
                    break;
                case DELSTAT_UPLOADING:
                default:
                    fmap[msgId] = MmsData(true, state, fn, path, sz);
#ifdef DEBUG
                    fmap[msgId].addInfo("DBG: Msg-ID " + QString::number(msgId));
#endif
                    break;
            }
            //set status for table row
            int nDone = 0;
            int nFail = 0;
            for (auto &it : fmap)
            {
                switch (it.second.state)
                {
                    case DELSTAT_UPLOAD_FAILED:
                    case DELSTAT_TX_DOWNLOAD_FAILED:
                        nFail++;
                        break;
                    case DELSTAT_TX_DOWNLOADED:
                        nDone++;
                        break;
                    case DELSTAT_UPLOADING:
                    case DELSTAT_UPLOADED:
                    default:
                        break; //do nothing
                }
            }
            int nFiles = static_cast<int>(fmap.size());
            if (nDone == nFiles)
                delstat = DELSTAT_SENT; //all ok
            else if (nFail == nFiles)
                delstat = DELSTAT_FAIL; //all failed
            else if (nFail != 0)
                delstat = DELSTAT_PART; //some failed
            //else no row update
            else if (state == DELSTAT_TX_DOWNLOADED ||
                     state == DELSTAT_TX_DOWNLOAD_FAILED)
                break; //only 1 row (call party) affected
            else
                continue;
        }
        itm = tw->item(row, COL_MSG_DELSTAT);
        itm->setData(USERROLE_DELSTAT, delstat);
        switch (delstat)
        {
            case DELSTAT_FAIL:
                itm->setText(tr("Failed"));
                //show result in tooltip if not MMS error
                if (msgId == 0 || !msg->hasMmsError())
                    itm->setToolTip(QString::fromStdString(
                               msg->getFieldValueString(MsgSp::Field::RESULT)));
                break;
            case DELSTAT_PART:
                itm->setText(tr("Partial failure"));
                break;
            case DELSTAT_SENT:
                itm->setText(tr("Sent"));
                break;
            default:
                break; //do nothing
        }
        if (state == DELSTAT_TX_DOWNLOADED ||
            state == DELSTAT_TX_DOWNLOAD_FAILED)
            break; //only 1 row (call party) affected
    }
    ui->msgTable->setSortingEnabled(true);
    return found;
}

//treewidget column macros for easier maintenance
#define TC_ID 0 //msg ID
#define TC_ST 0 //status icon
#define TC_FN 1 //filename
#define TC_IM 1 //image icon
#define TC_FP 2 //file path

void CommsRegister::mmsShowAtt(MessageDialog *md, int key, bool outgoing)
{
    assert(md != 0);
    //QTreeWidget makes it easier to show and hide thumbnails, and add any other
    //info later
    auto *w = new QTreeWidget(md);
    w->setHeaderHidden(true);
    w->setColumnCount(2);
    w->setToolTip(tr("Attachments"));
    md->addWidget(w);
    QTreeWidgetItem *li;
    QTreeWidgetItem *li2;
    //show attachments
    //outgoing: size & local source path in tooltip
    //incoming: size (& local download path in DEBUG) & saved paths in tooltip
    for (const auto &it : mMmsMap[key])
    {
        li = new QTreeWidgetItem(w);
        li->setData(TC_ID, Qt::UserRole, it.first);
        li->setText(TC_FN, it.second.fname);
        li->setIcon(TC_FN, getIcon(it.second.path));
        li->setToolTip(TC_FN, it.second.info);
        switch (it.second.state)
        {
            case DELSTAT_DOWNLOAD_FAILED:
            case DELSTAT_DOWNLOAD_FAILED_PERM:
                li->setIcon(TC_ST,
                           QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_DELETE));
                li = 0; //no thumbnail
                break;
            case DELSTAT_DOWNLOAD_SAVED:
            case DELSTAT_DOWNLOAD_SAVED_USR:
                li->setIcon(TC_ST,
                            QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_SAVE));
                break;
            case DELSTAT_DOWNLOADING:
                li->setIcon(TC_ST, mmsStatIcon(it.second.state));
                li = 0;
                break;
            case DELSTAT_TX_DOWNLOAD_FAILED:
            case DELSTAT_UPLOAD_FAILED:
                li->setIcon(TC_ST,
                           QtUtils::getActionIcon(CmnTypes::ACTIONTYPE_DELETE));
                break;
            case DELSTAT_UPLOADED:
            case DELSTAT_UPLOADING:
                li->setIcon(TC_ST, mmsStatIcon(it.second.state));
                break;
            case DELSTAT_DOWNLOADED:
            case DELSTAT_TX_DOWNLOADED:
            default:
                break; //do nothing
        }
        if (li == 0)
            continue;
        if (!QImageReader::imageFormat(it.second.path).isEmpty())
        {
            //show scaled image
            li2 = new QTreeWidgetItem(li);
            li2->setData(TC_IM, Qt::DecorationRole,
                         QPixmap(it.second.path)
                             .scaled(300, 300, Qt::KeepAspectRatio));
            li2->setToolTip(TC_IM, it.second.info);
            //make unselectable because it would interfere with context actions
            li2->setFlags(li2->flags() & ~Qt::ItemIsSelectable);
            li = li2; //for possible use below
        }
        if (outgoing || it.second.state == DELSTAT_DOWNLOAD_SAVED_USR)
            li->setData(TC_FP, Qt::UserRole, it.second.path);
    }
    w->resizeColumnToContents(TC_ST);
    w->setSelectionMode(QTreeWidget::ExtendedSelection);
    w->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(w, &QTreeWidget::itemDoubleClicked, this,
            [](QTreeWidgetItem *wi)
            {
                if (wi->childCount() != 0)
                    return;
                auto url(QUrl::fromLocalFile(
                                     wi->data(TC_FP, Qt::UserRole).toString()));
                if (url.isValid())
                    QDesktopServices::openUrl(url);
            });
    if (outgoing)
    {
        connect(w, &QTreeWidget::customContextMenuRequested, this,
                [this, w, key](const QPoint &pos)
                {
                    if (!w->indexAt(pos).isValid())
                        return;
                    auto sel(w->selectedItems());
                    if (sel.isEmpty())
                        return;
                    QList<int> actTypes;
                    actTypes << CmnTypes::ACTIONTYPE_COPY
                             << CmnTypes::ACTIONTYPE_COPY_PATH;
                    QMenu mn(this);
                    QtUtils::addMenuActions(mn, actTypes);
                    auto *act = mn.exec(w->mapToGlobal(pos));
                    if (act == 0)
                        return;
                    QStringList l;
                    switch (act->data().toInt())
                    {
                        case CmnTypes::ACTIONTYPE_COPY:
                        {
                            for (auto &it : sel)
                            {
                                l << it->text(TC_FN);
                            }
                            break;
                        }
                        case CmnTypes::ACTIONTYPE_COPY_PATH:
                        default:
                        {
                            //get unique paths
                            auto &fmap(mMmsMap[key]);
                            QString s;
                            for (auto &it : sel)
                            {
                                s = QFileInfo(
                                     fmap[it->data(TC_ID, Qt::UserRole).toInt()]
                                     .path).path();
                                if (!l.contains(s))
                                    l << s;
                            }
                            break;
                        }
                    }
                    QtUtils::copyToClipboard(l.join("\n"));
                });
    } //if (outgoing)
    else
    {
        connect(w, &QTreeWidget::customContextMenuRequested, this,
                [this, w, key](const QPoint &pos)
                {
                    if (!w->indexAt(pos).isValid())
                        return;
                    auto sel(w->selectedItems());
                    if (sel.isEmpty())
                        return;
                    auto &fmap(mMmsMap[key]);
                    int numOk = 0;   //number of downloaded files
                    int numFail = 0; //number of non-permanent failed files
                    int st;
                    for (auto &it : sel)
                    {
                        st = fmap[it->data(TC_ID, Qt::UserRole).toInt()].state;
                        if (st >= DELSTAT_DOWNLOADED)
                            ++numOk;
                        else if (st == DELSTAT_DOWNLOAD_FAILED)
                            ++numFail;
                        if (numOk > 1 && numFail != 0)
                            break;
                    }
                    if (numOk == 1)
                    {
                        //no 'save all' if only 1 item, but also check
                        //unselected items for downloaded files
                        int i = w->topLevelItemCount();
                        if (i > sel.size())
                        {
                            for (--i; i>=0; --i)
                            {
                                if (!w->topLevelItem(i)->isSelected() &&
                                    fmap[w->topLevelItem(i)
                                          ->data(TC_ID, Qt::UserRole).toInt()]
                                        .state >= DELSTAT_DOWNLOADED &&
                                    ++numOk == 2)
                                {
                                    break;
                                }
                            }
                        }
                    }
                    QList<int> actTypes;
                    if (numFail != 0)
                        actTypes << CmnTypes::ACTIONTYPE_MMS_DOWNLOAD;
                    if (numOk != 0)
                    {
                        actTypes << CmnTypes::ACTIONTYPE_SAVE;
                        if (numOk > 1)
                            actTypes << CmnTypes::ACTIONTYPE_SAVE_ALL;
                    }
                    actTypes << CmnTypes::ACTIONTYPE_COPY;
                    QMenu mn(this);
                    QtUtils::addMenuActions(mn, actTypes);
                    auto *act = mn.exec(w->mapToGlobal(pos));
                    if (act == 0)
                        return;
                    int actType = act->data().toInt();
                    if (actType == CmnTypes::ACTIONTYPE_COPY)
                    {
                        QStringList l;
                        for (auto &it : sel)
                        {
                            l << it->text(TC_FN);
                        }
                        QtUtils::copyToClipboard(l.join("\n"));
                        return;
                    }
                    if (actType == CmnTypes::ACTIONTYPE_MMS_DOWNLOAD)
                    {
                        int id;
                        for (auto &it : sel)
                        {
                            id = it->data(TC_ID, Qt::UserRole).toInt();
                            if (fmap[id].msg != 0)
                            {
                                fmap[id].state = DELSTAT_DOWNLOADING;
                                emit mmsDownload(fmap[id].msg);
                            }
                        }
                        return;
                    }
                    if (actType == CmnTypes::ACTIONTYPE_SAVE_ALL)
                    {
                        w->selectAll();
                        sel = w->selectedItems();
                    }
                    QString fn;
                    map<int, QString> smap; //msgId => save path
                    if (sel.size() == 1)
                    {
                        fn = QFileDialog::getSaveFileName(w, tr("Save File"),
                                          mMmsSaveDir + sel.at(0)->text(TC_FN));
                        if (fn.isEmpty())
                            return;
                        mMmsSaveDir = QFileInfo(fn).path();
                        mMmsSaveDir.append("/");
                        if (QFile::exists(fn))
                            QFile::remove(fn); //dialog has confirmed to replace
                        smap[sel.at(0)->data(TC_ID, Qt::UserRole).toInt()] = fn;
                    }
                    else
                    {
                        //save multiple files
                        QString d(QFileDialog::getExistingDirectory(w,
                                             tr("Select Folder"), mMmsSaveDir));
                        if (d.isEmpty())
                            return;
                        mMmsSaveDir = d.append("/");
                        int id;
                        for (auto &it : sel)
                        {
                            id = it->data(TC_ID, Qt::UserRole).toInt();
                            if (fmap[id].state >= DELSTAT_DOWNLOADED)
                                smap[id].append(d).append(it->text(TC_FN));
                        }
                    }
                    auto it = smap.begin();
                    while (it != smap.end())
                    {
                        if (QFile::exists(it->second))
                        {
                            //rename - insert "-n" before extension
                            QFileInfo fi(it->second);
                            int n = 1;
                            do
                            {
                                it->second = fi.path();
                                it->second.append("/")
                                          .append(fi.completeBaseName())
                                          .append("-")
                                          .append(QString::number(n++))
                                          .append(".").append(fi.suffix());
                            }
                            while (QFileInfo::exists(it->second));
                        }
                        auto &dt(fmap[it->first]);
                        if (QFile::copy(dt.path, it->second))
                        {
                            LOGGER_VERBOSE(mLogger, "User " << mUserId
                                  << " saved MMS file "
                                  << QFileInfo(dt.path).fileName().toStdString()
                                  << " to " << it->second.toStdString());
                            //find item with matching msg ID to update
                            QTreeWidgetItemIterator wit(w);
                            while (*wit != 0)
                            {
                                if ((*wit)->data(TC_ID, Qt::UserRole).toInt() ==
                                    it->first)
                                {
                                    if (dt.state < DELSTAT_DOWNLOAD_SAVED)
                                    {
                                        dt.state = DELSTAT_DOWNLOAD_SAVED;
                                        (*wit)->setIcon(TC_ST,
                                                QtUtils::getActionIcon(
                                                    CmnTypes::ACTIONTYPE_SAVE));
                                    }
                                    //add saved location if new
                                    if (dt.addUniqueInfo(it->second))
                                        (*wit)->setToolTip(TC_FN, dt.info);
                                    break;
                                }
                                ++wit;
                            }
                            it = smap.erase(it);
                        }
                        else
                        {
                            LOGGER_ERROR(mLogger, "Failed to copy MMS file "
                                         << dt.path.toStdString() << " to "
                                         << it->second.toStdString());
                            ++it;
                        }
                    }
                    if (!smap.empty())
                    {
                        fn = tr("Failed to save:");
                        for (auto &it2 : smap)
                        {
                            fn.append("\n").append(it2.second);
                        }
                        QMessageBox::critical(this, tr("Save Error"), fn);
                    }
                });
    }
}

const QIcon &CommsRegister::mmsStatIcon(int stat)
{
    if (mMmsIconMap.count(stat) == 0)
    {
        switch (stat)
        {
            case DELSTAT_DOWNLOADING:
                mMmsIconMap[stat] = QIcon(":/Images/images/icon_comms_in.png");
                break;
            case DELSTAT_UPLOADED:
                mMmsIconMap[stat] = QIcon(":/Images/images/icon_comms_dl.png");
                break;
            case DELSTAT_UPLOADING:
            default: //should not be passed any other value
                mMmsIconMap[stat] = QIcon(":/Images/images/icon_comms_out.png");
                break;
        }
    }
    return mMmsIconMap[stat];
}

bool CommsRegister::mmsUnsavedAtt(bool all)
{
    if (all)
    {
        for (const auto &it : mMmsMap)
        {
            for (const auto &it2 : it.second)
            {
                if (it2.second.state == DELSTAT_DOWNLOADED)
                    return true;
            }
        }
        return false;
    }
    int row;
    int key;
    auto *tw = ui->msgTable;
    for (auto &itm : tw->selectedItems())
    {
        row = itm->row();
        //check only once per row
        if (itm->column() == COL_MSG_MESSAGE &&
            getCellData(row, COL_MSG_DIR) == CmnTypes::COMMS_DIR_IN &&
            getCellData(row, COL_MSG_TYPE) == CmnTypes::COMMS_MSG_MMS)
        {
            key = tw->item(row, COL_MSG_MESSAGE)->data(USERROLE_MMSKEY).toInt();
            if (mMmsMap.count(key) != 0)
            {
                for (const auto &it : mMmsMap[key])
                {
                    if (it.second.state == DELSTAT_DOWNLOADED)
                        return true;
                }
            }
        }
    }
    return false;
}

void CommsRegister::mmsCleanup(bool all)
{
    set<int> keys;
    if (!all)
    {
        //collect keys from table
        auto *tw = ui->msgTable;
        int r = tw->rowCount() - 1;
        for (; r>=0; --r)
        {
            keys.insert(tw->item(r, COL_MSG_MESSAGE)
                          ->data(USERROLE_MMSKEY).toInt());
        }
    }
    auto it = mMmsMap.begin();
    while (it != mMmsMap.end())
    {
        if (!all && (keys.count(it->first) != 0))
        {
            ++it;
            continue;
        }
        //delete this entry
        for (auto &it2 : it->second)
        {
            if (it2.second.state >= DELSTAT_DOWNLOADED &&
                it2.second.state < DELSTAT_DOWNLOAD_SAVED_USR)
            {
                LOGGER_DEBUG(mLogger, LOGPREFIX << "mmsCleanup: "
                             "Deleting received file "
                             << it2.second.path.toStdString());
                QFile(it2.second.path).remove();
            }
            delete it2.second.msg; //non-zero only for unfinished download
        }
        it = mMmsMap.erase(it);
    }
}

void CommsRegister::setDeliveryStatus(MsgSp *msg)
{
    assert(msg != 0);
    string res;
    switch (msg->getType())
    {
        case MsgSp::Type::SDS_RPT:
            if (!msg->isSdsSuccessful())
                res = msg->getFieldValueString(MsgSp::Field::DELIVERY_STATUS);
            break;
        case MsgSp::Type::SDS_TRANSFER:
        case MsgSp::Type::STATUS:
            //received only in failure case
            res = msg->getFieldValueString(MsgSp::Field::RESULT);
            break;
        case MsgSp::Type::STATUS_RPT:
            if (!msg->isResultSuccessful())
                res = msg->getFieldValueString(MsgSp::Field::RESULT);
            break;
        default:
            assert("CommsRegister::setDeliveryStatus: Invalid msg type" == 0);
            return;
    }
    auto *tw = ui->msgTable;
    tw->setSortingEnabled(false);
    QTableWidgetItem *itm = 0;
    QTableWidgetItem *ackItm = 0; //last item matching ack - only if cp unknown
    bool found = false;
    int ack = msg->getFieldInt(MsgSp::Field::MSG_ACK);
    //cp may be UNDEFINED in unlikely error case on server
    int cp = msg->getFieldInt(MsgSp::Field::CALLED_PARTY);
    int ackCount = 0; //rows matching ack - only if cp unknown
    int row = -1;     //last row matching ack
    int rows = tw->rowCount();
    for (auto r=0; r<rows; ++r)
    {
        itm = tw->item(r, COL_MSG_DELSTAT);
        if (itm == 0 || itm->data(USERROLE_MSGID).toInt() != ack)
            continue;
        row = r;
        if (cp <= 0)
        {
            ++ackCount;
            ackItm = itm;
        }
        else if (tw->item(r, COL_MSG_TO)->data(USERROLE_CALLPARTY).toInt() == cp)
        {
            found = true;
            break;
        }
    }
    if (found || (ackItm != 0 && ackCount == 1))
    {
        //found==false here means cp unknown but exactly one match for ack -
        //consider matched
        if (!found)
            itm = ackItm;
        itm->setData(USERROLE_MSGID, 0); //just to prevent future match
        if (res.empty())
        {
            itm->setText(tr("Sent"));
#ifdef INCIDENT
            //notify Incident
            if (msg->getType() != MsgSp::Type::MMS_RPT)
                emit newComm((msg->getType() == MsgSp::Type::SDS_RPT)?
                                 CmnTypes::COMMS_MSG_SDS:
                                 CmnTypes::COMMS_MSG_STATUS,
                             tw->item(row, COL_MSG_FROM)->text(),
                             tw->item(row, COL_MSG_TO)->text());
#endif
        }
        else
        {
            if (cp <= 0)
                cp = tw->item(row, COL_MSG_TO)->data(USERROLE_CALLPARTY).toInt();
            itm->setText(tr("Failed"));
            itm->setForeground(QBrush(Qt::red));
            itm->setToolTip(QString::fromStdString(res));
        }
    }
    tw->setSortingEnabled(true);
    if (!res.empty())
    {
        row = msg->getType(); //reuse var
        LOGGER_ERROR(mLogger, LOGPREFIX << "setDeliveryStatus: "
                     << msg->getName() << ": "
                     << ((row == MsgSp::Type::SDS_RPT ||
                          row == MsgSp::Type::SDS_TRANSFER)?
                         "SDS": "Status Message")
                     << " failed to " << cp << ": "
                     << MsgSp::getFieldName(MsgSp::Field::MSG_ID) << "=" << ack
                     << ", " << res);
    }
}

void CommsRegister::setCellData(int col, int type)
{
    auto *lbl = new QLabel();
    lbl->setObjectName(QString::number(type));
    const auto &icon(QtUtils::getCommsIcon(type));
    lbl->setPixmap(icon.pixmap(icon.actualSize(QSize(24, 24))));
    lbl->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lbl->setAttribute(Qt::WA_TranslucentBackground);
    ui->msgTable->setCellWidget(0, col, lbl);
}

int CommsRegister::getCellData(int row, int col)
{
    auto *w = ui->msgTable->cellWidget(row, col);
    return (w == 0)? 0: w->objectName().toInt();
}

inline QTableWidgetItem *CommsRegister::newItem(const QString &txt)
{
    auto *itm = new QTableWidgetItem(txt);
    itm->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    return itm;
}

void CommsRegister::unreadCountChanged()
{
    if (mType == TYPE_MSG)
        emit unreadMessages(mUnreadCount); //counter shown elsewhere
    else if (mUnreadCount == 0)
        ui->msgButton->setText("");
    else
        ui->msgButton->setText(QString::number(mUnreadCount));
}

void CommsRegister::doPrint(int printType)
{
    QString title(windowTitle());
    Document doc(printType, title);
    if (mType != TYPE_MSG && ui->callTable->rowCount() != 0)
        doc.addTable(ui->callTable, ui->callTitleLabel->text(), COL_CALL_TIME,
                     -1, COL_CALL_DIR);
    if (mType != TYPE_CALL && ui->msgTable->rowCount() != 0)
        doc.addTable(ui->msgTable, ui->msgTitleLabel->text(), COL_MSG_TIME,
                     COL_MSG_TYPE, COL_MSG_DIR);
    doc.print(this, title.remove(" "));
}
