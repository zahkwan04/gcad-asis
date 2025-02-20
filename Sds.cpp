/**
 * UI SDS, MMS and Status Message implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Sds.cpp 1862 2024-06-04 06:33:19Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#include <string>
#include <assert.h>
#include <QChar>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QRegularExpression>
#include <QTableWidgetItem>

#include "CmnTypes.h"
#include "QtUtils.h"
#include "Settings.h"
#include "StatusCodes.h"
#include "Style.h"
#include "Utils.h"
#include "ui_Sds.h"
#include "Sds.h"

using namespace std;

static const int    DEFAULT_STS_TYPE = -1;
static const string LOGPREFIX("Sds:: ");
static const QChar  SEPARATOR = '\n';
static const QRegularExpression QRE("[\n\t]+");

enum
{
    //Status Message table column IDs
    COL_STS_TYPE,
    COL_STS_CODE,
    COL_STS_TEXT,

    //recipient selection states
    REC_INVALID,
    REC_TXT,        //not all can receive MMS with attachment, i.e. incl radios
    REC_ATT,        //all can receive MMS with attachment, i.e. no radios

    //message input states
    MSG_EMPTY,
    MSG_STS,        //status message
    MSG_SDS,        //text length within SDS limit
    MSG_MMS,        //text length beyond SDS
    MSG_ATT         //has attachments
};

Sds::Sds(Logger           *logger,
         ResourceSelector *rscSelector,
         CommsRegister    *commsReg,
         MmsClient        *mmsClient,
         QWidget          *parent) :
QWidget(parent), ui(new Ui::Sds), mCommsReg(commsReg), mRecState(REC_INVALID),
mMsgState(MSG_EMPTY), mLogger(logger), mResourceSelector(rscSelector)
{
    ui->setupUi(this);
    ui->inpTabWidget->setTabIcon(ui->inpTabWidget->indexOf(ui->inpSdsTab),
                                QtUtils::getCommsIcon(CmnTypes::COMMS_MSG_SDS));
    ui->inpTabWidget->setTabIcon(ui->inpTabWidget->indexOf(ui->inpStsTab),
                             QtUtils::getCommsIcon(CmnTypes::COMMS_MSG_STATUS));
    connect(ui->inpTabWidget, &QTabWidget::currentChanged, this,
            [this]()
            {
                if (ui->inpTabWidget->currentWidget() == ui->inpSdsTab)
                    ui->msgEdit->setFocus();
                setSendEnabled();
            });
    //input SDS tab
    ui->attHistButton->setMenu(new QMenu(this));
    connect(ui->attHistButton, &QToolButton::clicked, this,
            [this] { ui->attHistButton->showMenu(); });
    ui->msgHistButton->setMenu(new QMenu(this));
    connect(ui->msgHistButton, &QToolButton::clicked, this,
            [this] { ui->msgHistButton->showMenu(); });
    //set template
    auto *menu = new QMenu(this);
    ui->tplButton->setMenu(menu);
    auto *submenu = menu->addMenu(tr("Current text"));
    string tpl(Settings::instance().get<string>(Props::FLD_CFG_SDSTEMPLATE));
    if (Utils::trim(tpl) > 0)
    {
        mTemplate = QString::fromStdString(tpl).split(SEPARATOR);
        mTemplate.removeDuplicates(); //just in case
        QAction *act;
        for (auto &s : mTemplate)
        {
            act = new QAction(s, menu);
            act->setObjectName(s); //for add/delete
            menu->addAction(act);
            connect(act, &QAction::triggered, this,
                    [act, this] { setMsgText(act->text()); });
        }
    }
    connect(QtUtils::addMenuAction(*submenu, CmnTypes::ACTIONTYPE_ADD),
            &QAction::triggered, this,
            [menu, this]
            {
                QString s(ui->msgEdit->toPlainText().trimmed());
                if (s.isEmpty())
                    return;
                s.replace(QRE, " ");
                //check # chars instead of bytes
                if (s.size() > SDSMAXLEN)
                {
                    QMessageBox::critical(this, tr("Template Error"),
                                          tr("Text exceeds maximum length %1")
                                              .arg(SDSMAXLEN));
                }
                else if (!mTemplate.contains(s))
                {
                    mTemplate << s;
                    mTemplate.sort();
                    auto *act = new QAction(s, menu);
                    act->setObjectName(s);
                    //insert at position as in mTemplate
                    int i = mTemplate.indexOf(s);
                    menu->insertAction((i == mTemplate.size() - 1)?
                                           0:
                                           menu->findChild<QAction *>(
                                                    mTemplate.at(i + 1),
                                                    Qt::FindDirectChildrenOnly),
                                       act);
                    connect(act, &QAction::triggered, this,
                            [act, this] { setMsgText(act->text()); });
                }
            });
    connect(QtUtils::addMenuAction(*submenu, CmnTypes::ACTIONTYPE_DELETE),
            &QAction::triggered, this,
            [this]
            {
                QString s(ui->msgEdit->toPlainText().trimmed());
                if (!s.isEmpty() && mTemplate.contains(s) &&
                    QMessageBox::question(this, tr("Delete Template"),
                                          tr("Delete '%1'?").arg(s)) ==
                        QMessageBox::Yes &&
                    mTemplate.removeOne(s))
                {
                    delete ui->tplButton->menu()
                             ->findChild<QAction *>(s,
                                                    Qt::FindDirectChildrenOnly);
                }
            });
    connect(ui->tplButton, &QToolButton::clicked, this,
            [this] { ui->tplButton->showMenu(); });
    connect(ui->stsButton, &QToolButton::clicked, this,
            [this] { ui->stsButton->showMenu(); });
    connect(ui->clearButton, &QToolButton::clicked, this,
            [this]
            {
                mMsgState = (ui->mmsFiles->count() != 0)? MSG_ATT: MSG_EMPTY;
                setMsgText("", true);
                ui->msgEdit->setFocus();
            });
    connect(ui->msgEdit, &QTextEdit::textChanged, this,
            [this] { onMsgEditTextChanged(); });
    ui->charLength->setNum(SDSMAXLEN);
    QSizePolicy sp(ui->charLength->sizePolicy());
    sp.setRetainSizeWhenHidden(true);
    ui->charLength->setSizePolicy(sp);
    //MMS
    connect(ui->mmsFileButton, &QToolButton::clicked, this,
            [this]
            {
                mmsAddFiles(QFileDialog::getOpenFileNames(this,
                                                          tr("Send Files")));
            });
    connect(ui->mmsClrButton, &QToolButton::clicked, this,
            [this]
            {
                ui->mmsFiles->clear();
                if (mMsgState != MSG_STS)
                {
                    mMsgState = MSG_EMPTY; //just to force check below
                    onMsgEditTextChanged();
                }
            });
    ui->mmsFiles->setAcceptDrops(true);
    ui->mmsFiles->installEventFilter(this);
    ui->mmsFiles->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mmsFiles, &QListWidget::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                auto idx(ui->mmsFiles->indexAt(pos));
                if (!idx.isValid())
                    return;
                auto sel(ui->mmsFiles->selectedItems());
                QList<int> actTypes;
                if (sel.size() < ui->mmsFiles->count())
                    actTypes << CmnTypes::ACTIONTYPE_SELECT_ALL;
                actTypes << CmnTypes::ACTIONTYPE_DELETE_SEL
                         << CmnTypes::ACTIONTYPE_COPY
                         << CmnTypes::ACTIONTYPE_COPY_PATH;
                QMenu mn(this);
                QtUtils::addMenuActions(mn, actTypes);
                auto *act = mn.exec(ui->mmsFiles->mapToGlobal(pos));
                if (act == 0)
                    return;
                switch (act->data().toInt())
                {
                    case CmnTypes::ACTIONTYPE_COPY:
                    {
                        QStringList l;
                        for (auto &itm : sel)
                        {
                            l << itm->text();
                        }
                        QtUtils::copyToClipboard(l.join("\n"));
                        break;
                    }
                    case CmnTypes::ACTIONTYPE_COPY_PATH:
                    {
                        QStringList l;
                        QString s;
                        for (auto &itm : sel)
                        {
                            s = QFileInfo(itm->text()).path() ;
                            if (!l.contains(s))
                                l << s;
                        }
                        QtUtils::copyToClipboard(l.join("\n"));
                        break;
                    }
                    case CmnTypes::ACTIONTYPE_DELETE_SEL:
                    {
                        for (auto &itm : sel)
                        {
                            delete itm;
                        }
                        if (mMsgState != MSG_STS && ui->mmsFiles->count() == 0)
                        {
                            mMsgState = MSG_EMPTY; //just to force check below
                            onMsgEditTextChanged();
                        }
                        break;
                    }
                    case CmnTypes::ACTIONTYPE_SELECT_ALL:
                    {
                        ui->mmsFiles->selectAll();
                        break;
                    }
                    default:
                    {
                        break; //do nothing
                    }
                }
            });
    //input Status tab
    mStsIconMap[DEFAULT_STS_TYPE] =
                               QIcon(":/Images/images/icon_status_default.png");
    ui->stsTable->sortByColumn(COL_STS_CODE, Qt::AscendingOrder);
    connect(ui->stsTable, &QTableWidget::clicked, this,
            [this] { setSendEnabled(); });

    ui->splitter1->insertWidget(1, mCommsReg);
    connect(mCommsReg, &CommsRegister::startAction, this,
            [this](int actType, int idType, int id, const QString &msg)
            {
                mResourceSelector->setSelectedId(idType, id);
                if (!msg.isEmpty())
                    setMsgText(msg);
                startAction(actType);
            });
    connect(mCommsReg, &CommsRegister::msgSelected, this,
            [this](int msgType, const QString &msg)
            {
                switch (msgType)
                {
                    case CmnTypes::COMMS_MSG_MMS:
                    case CmnTypes::COMMS_MSG_SDS:
                    {
                        ui->inpTabWidget->setCurrentWidget(ui->inpSdsTab);
                        if (!msg.isEmpty())
                            setMsgText(msg);
                        break;
                    }
                    case CmnTypes::COMMS_MSG_STATUS:
                    default:
                    {
                        ui->inpTabWidget->setCurrentWidget(ui->inpStsTab);
                        //scroll to the selected message row in the Status
                        //Message table
                        auto *tw = ui->stsTable;
                        for (int r=tw->rowCount()-1; r>=0; --r)
                        {
                            if (msg == tw->item(r, COL_STS_TEXT)->text())
                            {
                                tw->selectRow(r);
                                tw->scrollToItem(tw->item(r, COL_STS_TEXT),
                                              QAbstractItemView::PositionAtTop);
                                break;
                            }
                        }
                        break;
                    }
                }
            });
    connect(mCommsReg, &CommsRegister::floatOrDock, this,
            [this](QDialog *dlg)
            {
                assert(dlg != 0);
                //dock mCommsReg back into ui, delete its floating parent
                ui->splitter1->addWidget(mCommsReg);
                delete dlg; //splitter1 is now parent widget
            });
    setTheme();
    connect(ui->sendButton, &QPushButton::clicked, this,
            [this] { onSendButtonClick(); });
    mMms = new Mms(mLogger, mmsClient);
    connect(mMms, &Mms::dlFailed, this,
            [this](MsgSp *msg)
            {
                 mCommsReg->onNewMsg(msg); //MMS_RPT
                 QMessageBox::critical(this, tr("MMS Error"),
                                   tr("File '%1' download failed - retry later")
                                       .arg(mMms->getFileList(msg)));
                 delete msg;
            });
    onSelectionChanged(mResourceSelector->hasSelection(true), false);
}

Sds::~Sds()
{
    delete mMms;
    delete ui;
}

void Sds::activate()
{
    mResourceSelector->enableMultiSelect(true);
    ui->mainLayout->insertWidget(0, mResourceSelector);
    ui->msgEdit->setFocus();
}

void Sds::setTheme()
{
    setStyleSheet(Style::getStyle(Style::OBJ_COMMON));
    ui->charLength->setStyleSheet(Style::getStyle(Style::OBJ_LABEL_SMALL));
    mCommsReg->setTheme();
}

QString Sds::getTemplate()
{
    return mTemplate.join(SEPARATOR);
}

void Sds::loadData()
{
    StatusCodes::TypeMapT typeMap;
    if (StatusCodes::getTypes(typeMap))
    {
        //build type icon map
        for (auto &it : typeMap)
        {
            setStsIcon(it.first, it.second);
        }
    }
    refreshStsData();
}

void Sds::updateStsData(MsgSp *msg)
{
    if (msg == 0)
    {
        assert("Bad param in Sds::updateStsData" == 0);
        return;
    }
    //unsolicited update - STATUS_CODE_LIST and other related list fields
    //have exactly one item;
    //this is for either status type or status code
    int code = msg->getFieldInt(MsgSp::Field::STATUS_CODE_LIST);
    int type = msg->getFieldInt(MsgSp::Field::STATUS_TYPE_LIST);
    if (code != MsgSp::Value::UNDEFINED)
    {
        int i;
        auto *tw = ui->stsTable;
        if (type == MsgSp::Value::UNDEFINED)
        {
            //code only - remove status code
            setStsItem(true, code);
            for (i=tw->rowCount()-1; i>=0; --i)
            {
                if (tw->item(i, COL_STS_CODE)->text().toInt() == code)
                {
                    //table has ownership of widgets, so do not delete them
                    tw->removeRow(i);
                    break;
                }
            }
        }
        else if (msg->hasField(MsgSp::Field::STATUS_TEXT_LIST))
        {
            //code, type and text - add/modify status code
            QString s(QString::fromStdString(msg->getFieldString(
                                              MsgSp::Field::STATUS_TEXT_LIST)));
            setStsItem(true, code, type, s);
            tw->setSortingEnabled(false);
            int rows = tw->rowCount();
            for (i=0; i<rows; ++i)
            {
                if (tw->item(i, COL_STS_CODE)->text().toInt() == code)
                {
                    code = -1; //no need to set code in the row
                    break;
                }
            }
            if (i == rows)
                tw->insertRow(i);
            addStsTableData(i, type, code, s);
            tw->setSortingEnabled(true);
        }
    }
    else if (type != MsgSp::Value::UNDEFINED)
    {
        //type with or without description
        setStsIcon(type, msg->getFieldString(MsgSp::Field::DESC_LIST));
        refreshStsData();
    }
}

void Sds::sendMsg(int                       idType,
                  const ResourceData::IdsT &ids,
                  const QStringList        &msgs,
                  const QString            &txt)
{
    if (!mResourceSelector->setSelectedId(idType, ids, true, true))
        return;
    setStsMode(false);
    if (msgs.isEmpty() && txt.isEmpty())
    {
        ui->msgEdit->setFocus();
        return;
    }
    if (!txt.isEmpty())
    {
        if (idType == ResourceData::TYPE_MOBILE)
        {
            setMsgText(txt, false);
            onSendButtonClick(CmnTypes::COMMS_MSG_MMS);
            return;
        }
        //mixed - split mobiles and subs
        ResourceData::IdsT mobs;
        ResourceData::IdsT subs;
        for (auto i : ids)
        {
            if (ResourceData::getType(i) == ResourceData::TYPE_MOBILE)
                mobs.insert(i);
            else
                subs.insert(i);
        }
        if (!mobs.empty() &&
            mResourceSelector->setSelectedId(ResourceData::TYPE_MOBILE,
                                             mobs, true, true))
        {
            setMsgText(txt, false);
            onSendButtonClick(CmnTypes::COMMS_MSG_MMS);
        }
        if (subs.empty() ||
            !mResourceSelector->setSelectedId(ResourceData::TYPE_SUBSCRIBER,
                                              subs, true, true))
            return;
    }
    //send every message
    for (auto &m : msgs)
    {
        setMsgText(m, false);
        onSendButtonClick(CmnTypes::COMMS_MSG_SDS);
    }
}

void Sds::mmsRcv(MsgSp *msg)
{
    QString err(mMms->rcv(msg));
    if (mCommsReg->onNewMsg(msg, err) && !err.isEmpty())
        QMessageBox::critical(this, tr("MMS Error"), err);
}

void Sds::startAction(int actType)
{
    switch (actType)
    {
        case CmnTypes::ACTIONTYPE_MMS:
        case CmnTypes::ACTIONTYPE_SDS:
            setStsMode(false);
            ui->inpTabWidget->setCurrentWidget(ui->inpSdsTab);
            ui->msgEdit->setFocus();
            break;
        case CmnTypes::ACTIONTYPE_STATUS:
        default:
            ui->inpTabWidget->setCurrentWidget(ui->inpStsTab);
            break;
    }
}

void Sds::prepMsgs(const QString &prefix, QStringList &msgs)
{
    if (msgs.empty())
    {
        assert("Bad param in Sds::prepMsgs" == 0);
        return;
    }
    //prefix is expected to be very short and not cause size overrun
    auto it = msgs.begin();
    QString s(prefix);
    s.append(*it);
    QStringList l;
    for (++it; it!=msgs.end(); ++it)
    {
        if (getSize(s) + getSize(*it) <= SDSMAXLEN)
        {
            s.append(*it);
        }
        else
        {
            //s forms complete msg, put current item in next msg
            l << s;
            s = prefix;
            s.append(*it);
        }
    }
    s.truncate(SDSMAXLEN);
    while (getSize(s) > SDSMAXLEN)
    {
        s.chop(1);
    }
    l << s;
    msgs = l;
}

void Sds::onSelectionChanged(bool isValidSingle, bool isValidMulti)
{
    if (!isValidSingle && !isValidMulti)
    {
        mRecState = REC_INVALID;
    }
    else
    {
        mRecState = REC_ATT;
        ResourceSelector::SelectionsT sel;
        mResourceSelector->getSelection(sel);
        for (auto &it : sel)
        {
            if (it.second.type == CmnTypes::IDTYPE_SUBSCRIBER)
            {
                mRecState = REC_TXT;
                break;
            }
        }
    }
    setSendEnabled();
}

bool Sds::eventFilter(QObject *, QEvent *event)
{
    //accept only file drop
    if (event->type() == QEvent::DragEnter)
    {
        auto *e = static_cast<QDragEnterEvent *>(event);
        if (e->mimeData()->hasUrls())
        {
            e->acceptProposedAction();
            return true;
        }
    }
    else if (event->type() == QEvent::Drop)
    {
        QString f;
        QFileInfo fi;
        QStringList files;
        auto *e = static_cast<QDropEvent *>(event);
        for (auto &url : e->mimeData()->urls())
        {
            f = url.toLocalFile();
            fi.setFile(f);
            if (fi.isFile())
                files << f;
        }
        mmsAddFiles(files);
        return true;
    }
    return false;
}

void Sds::refreshStsData()
{
    StatusCodes::DataMapT dataMap;
    if (StatusCodes::getData(dataMap))
    {
        auto *tw = ui->stsTable;
        tw->setSortingEnabled(false);
        tw->setRowCount(0);
        mStsDataMap.clear();
        QString s;
        for (auto &it : dataMap)
        {
            tw->insertRow(0);
            s = QString::fromStdString(it.second.text);
            addStsTableData(0, it.second.type, it.first, s);
            setStsItem(false, it.first, it.second.type, s);
        }
        tw->setSortingEnabled(true);
        setStsItem(true, -1); //last call to update menu only
    }
}

void Sds::addStsTableData(int row, int type, int code, const QString &text)
{
    const auto &icon(getStsIcon(type));
    auto *lbl = new QLabel();
    lbl->setPixmap(icon.pixmap(icon.actualSize(QSize(24, 24))));
    lbl->setAlignment(Qt::AlignCenter);
    lbl->setAttribute(Qt::WA_TranslucentBackground);
    auto *tw = ui->stsTable;
    tw->setCellWidget(row, COL_STS_TYPE, lbl);
    if (code >= 0)
    {
        auto *item = new QTableWidgetItem(QString::number(code));
        item->setTextAlignment(Qt::AlignCenter);
        tw->setItem(row, COL_STS_CODE, item);
    }
    tw->setItem(row, COL_STS_TEXT,
                new QTableWidgetItem(QtUtils::fromHexUnicode(text)));
}

void Sds::setStsItem(bool doMenu, int code, int type, const QString &text)
{
    bool exists = (mStsDataMap.count(code) != 0);
    if (code >= 0)
    {
        if (type >= 0)
        {
            //add or modify
            mStsDataMap[code].type = type;
            mStsDataMap[code].text = QtUtils::fromHexUnicode(text);
            if (exists && getStsCode() == code) //currently selected - update
                setMsgText(mStsDataMap[code].text, false);
        }
        else if (exists)
        {
            mStsDataMap.erase(code);
            if (getStsCode() == code) //currently selected - clear
                setStsMode(false);
        }
        else
        {
            return; //deleting non-existent code - should not occur, do nothing
        }
    }
    if (!doMenu)
        return;
    auto *menu = ui->stsButton->menu();
    if (exists && menu != 0)
    {
        //modify or delete
        for (auto &act : menu->findChildren<QAction *>())
        {
            if (act->toolTip().toInt() == code)
            {
                if (type < 0)
                {
                    menu->removeAction(act);
                    delete act;
                    return;
                }
                act->setIcon(getStsIcon(type));
                if (act->text() == mStsDataMap[code].text)
                    return; //unchanged text - unchanged order
                //changed text - recreate menu
                act->setText(mStsDataMap[code].text);
                break;
            }
        }
    }
    if (menu != 0)
    {
        //add - clear and recreate menu to maintain order
        menu->clear(); //also deletes all actions
    }
    else if (code >= 0 && type < 0)
    {
        return; //delete but no menu - should not occur, do nothing
    }
    else
    {
        //add first item
        menu = new QMenu(this);
        menu->setToolTipsVisible(true);
        ui->stsButton->setMenu(menu);
    }
    //add menu actions with sorted text instead of code
    map<QString, int> txt2CodeMap;
    for (const auto &it : mStsDataMap)
    {
        txt2CodeMap[it.second.text] = it.first;
    }
    QAction *act;
    for (const auto &it : txt2CodeMap)
    {
        act = new QAction(getStsIcon(mStsDataMap[it.second].type), it.first,
                          menu);
        act->setToolTip(QString::number(it.second));
        connect(act, &QAction::triggered, this,
                [act, this]
                {
                    setStsMode(true);
                    setMsgText(act->text(), false);
                    ui->msgEdit->setToolTip(act->toolTip());
                });
        menu->addAction(act);
    }
}

void Sds::setStsIcon(int type, const string &desc)
{
    mStsIconMap.erase(type);
    if (desc.empty())
        return; //use default icon
    QString d(QtUtils::fromHexUnicode(desc));
    if (d.contains("alert", Qt::CaseInsensitive))
        mStsIconMap[type] = QIcon(":/Images/images/icon_status_alert.png");
    else if (d.contains("info", Qt::CaseInsensitive))
        mStsIconMap[type] = QIcon(":/Images/images/icon_status_info.png");
}

QIcon &Sds::getStsIcon(int type)
{
    if (mStsIconMap.count(type) == 0)
        type = DEFAULT_STS_TYPE;
    return mStsIconMap[type];
}

void Sds::setStsMode(bool on)
{
    if (on)
    {
        mMsgState = MSG_STS;
    }
    else
    {
        ui->msgEdit->setToolTip("");
        if (mMsgState == MSG_STS)
        {
            ui->msgEdit->clear();
            mMsgState = (ui->mmsFiles->count() != 0)? MSG_ATT: MSG_EMPTY;
        }
    }
    ui->msgEdit->setReadOnly(on);
}

int Sds::getStsCode()
{
    if (ui->msgEdit->isReadOnly())
        return ui->msgEdit->toolTip().toInt();
    return -1;
}

void Sds::setMsgText(const QString &txt, bool clearStsMode)
{
    if (clearStsMode)
        setStsMode(false);
    //do not use setText() or setPlainText() because they also clear undo/redo
    //history
    ui->msgEdit->selectAll();
    ui->msgEdit->insertPlainText(txt);
}

void Sds::onMsgEditTextChanged()
{
    int balance = (mMsgState >= MSG_MMS)? MMSMAXLEN: SDSMAXLEN;
    if (mMsgState != MSG_STS)
    {
        QString s(ui->msgEdit->toPlainText());
        if (mMsgState != MSG_ATT)
        {
            mMsgState = (s.isEmpty())?
                        MSG_EMPTY: (getSize(s) > SDSMAXLEN)? MSG_MMS: MSG_SDS;
            balance = (mMsgState >= MSG_MMS)? MMSMAXLEN: SDSMAXLEN;
        }
        if (!s.isEmpty())
        {
            //each str character may be multi-byte
            balance -= getSize(s);
            //check & truncate in MMS mode only
            if (mMsgState >= MSG_MMS && getSize(s) > MMSMAXLEN)
            {
                do
                {
                    if (s.size() > MMSMAXLEN)
                        s.truncate(MMSMAXLEN);
                    else
                        s.chop(1); //remove one by one
                }
                while (getSize(s) > MMSMAXLEN);
                ui->msgEdit->setText(s);
                balance = MMSMAXLEN - getSize(s);
                //put cursor at end
                auto c = ui->msgEdit->textCursor();
                c.movePosition(QTextCursor::End);
                ui->msgEdit->setTextCursor(c);
            }
        } //if (!s.isEmpty())
    } //if (mMsgState != MSG_STS)
    if (mMsgState != MSG_STS && balance <= SDSMAXLEN)
    {
        ui->charLength->setNum(balance);
        ui->charLength->show();
    }
    else
    {
        ui->charLength->hide();
    }
    setSendEnabled();
}

void Sds::mmsAddFiles(const QStringList &files)
{
    if (!files.isEmpty())
    {
        int n = ui->mmsFiles->count();
        for (auto &f : files)
        {
            //prevent duplicates
            if (ui->mmsFiles->findItems(f, Qt::MatchExactly).isEmpty())
                ui->mmsFiles->addItem(new QListWidgetItem(
                                                 CommsRegister::getIcon(f), f));
        }
        if (mMsgState != MSG_ATT && mMsgState != MSG_STS &&
            ui->mmsFiles->count() > n)
        {
            mMsgState = MSG_ATT;
            onMsgEditTextChanged();
        }
    }
}

void Sds::setSendEnabled()
{
    static const QRegularExpression QRE(
                           "^[^a-zA-Z0-9]* *" //absence of word before
                           "-?\\d{1,2}\\.\\d{3,},-?\\d{1,3}\\.\\d{3,}" //lat,lon
                           " *[^a-zA-Z0-9]*$"); //absence of word after
    bool enabled = (mRecState != REC_INVALID);
    QString s;
    if (enabled)
    {
        if (ui->inpTabWidget->currentWidget() == ui->inpStsTab)
        {
            s.append(tr("Send ")).append(tr("Status"));
            if (ui->stsTable->selectedItems().empty())
            {
                enabled = false;
                s.append(tr(" - none selected"));
            }
        }
        else if (mMsgState != MSG_STS &&
                 ui->msgEdit->toPlainText().contains(QRE))
        {
            enabled = false;
            s.append(tr("Text containing \"<lat>,<lon>\" must contain "
                        "alphanumeric label text - first word before, or "
                        "phrase after"));
        }
        else
        {
            switch (mMsgState)
            {
                case MSG_ATT:
                    s.append(tr("Send ")).append(tr("MMS"));
                    if (mRecState != REC_ATT)
                    {
                        enabled = false;
                        s.append(tr(" - invalid recipients"));
                    }
                    break;
                case MSG_MMS:
                    s.append(tr("Send ")).append(tr("MMS"));
                    if (mRecState != REC_ATT)
                        s.append(tr(" (old terminal models may not be able to "
                                    "receive)"));
                    break;
                case MSG_SDS:
                    if (mMsgState == MSG_EMPTY)
                        enabled = false;
                    else
                        s.append(tr("Send ")).append(tr("SDS"));
                    break;
                case MSG_STS:
                    s.append(tr("Send ")).append(tr("Status"));
                    break;
                case MSG_EMPTY:
                default:
                    enabled = false;
                    break;
            }
        }
    }
    ui->sendButton->setEnabled(enabled);
    ui->sendButton->setToolTip(s);
}

void Sds::onSendButtonClick(int type)
{
    if (!mSession->isLoggedIn())
        return;
    ResourceSelector::SelectionsT sel;
    if (!mResourceSelector->getSelection(sel))
        return;
    QStringList inv;
    auto sit = sel.begin();
    while (sit != sel.end())
    {
        if (sit->second.type == ResourceData::TYPE_MOBILE &&
            SubsData::getIssiType(sit->first) == SubsData::TERMINALTYPE_BWC)
        {
            inv << QString::number(sit->first);
            sit = sel.erase(sit);
        }
        else
        {
            ++sit;
        }
    }
    if (!inv.empty())
    {
        //show error only if invoked by sendButton
        if (type < 0)
            QMessageBox::critical(this, tr("Invalid recipient"),
                                  tr("Device cannot receive message - "
                                     "discarded %1").arg(inv.join(", ")));
        if (sel.empty())
            return; //no recipient
    }
    QString msg;
    string smsg;
    int stsCode = (type < 0)? getStsCode(): -1;
    if (ui->inpTabWidget->currentWidget() == ui->inpStsTab)
    {
        type = ui->stsTable->currentRow(); //temp use
        stsCode = ui->stsTable->item(type, COL_STS_CODE)->text().toInt();
        msg = ui->stsTable->item(type, COL_STS_TEXT)->text();
        type = CmnTypes::COMMS_MSG_STATUS;
    }
    else if (mMsgState >= MSG_MMS)
    {
        mmsSend(sel);
        return;
    }
    else if (stsCode >= 0)
    {
        //sendButton in sts mode
        type = CmnTypes::COMMS_MSG_STATUS;
        msg = ui->msgEdit->toPlainText();
    }
    else
    {
        //from either sendMsg() or sendButton while not in sts mode
        msg = ui->msgEdit->toPlainText().trimmed();
        if (msg.isEmpty())
            return; // nothing to send
        type = CmnTypes::COMMS_MSG_SDS;
        msg.replace(QRE, " ");
        smsg = msg.toStdString();
        updateHistMenu(ui->msgHistButton->menu(), msg, true);
    }
    int msgId;
    if (sel.size() > 1)
    {
        ServerSession::IntListT ids;
        ServerSession::IntListT idTypes;
        for (auto &it : sel)
        {
            ids.push_back(it.first);
            idTypes.push_back(CmnTypes::toMsgSpIdentityType(it.second.type));
        }
        if (type == CmnTypes::COMMS_MSG_SDS)
            msgId = mSession->sds(idTypes, ids, smsg);
        else
            msgId = mSession->status(idTypes, ids, stsCode);
    }
    else
    {
        auto it = sel.begin();  //single recipient
        if (type == CmnTypes::COMMS_MSG_SDS)
            msgId = mSession->sds(CmnTypes::toMsgSpIdentityType(it->second.type),
                                  it->first, smsg);
        else
            msgId = mSession->status(
                                 CmnTypes::toMsgSpIdentityType(it->second.type),
                                 it->first, stsCode);
    }
    if (msgId > 0)
    {
        for (auto &it : sel)
        {
            mCommsReg->onOutMsg(type, it.second.type, it.first, msgId, msg);
        }
    }
    else
    {
        QStringList fails;
        for (auto &it : sel)
        {
            fails << it.second.name;
        }
        QMessageBox::critical(this,
                              ((type == CmnTypes::COMMS_MSG_SDS)?
                               tr("SDS Error"): tr("Status Message Error")),
                              tr("Failed to send to ") + fails.join(", "));
    }
}

void Sds::mmsSend(const ResourceSelector::SelectionsT &sel)
{
    MsgSp m(MsgSp::Type::MMS_TRANSFER);
    QString str(ui->msgEdit->toPlainText().trimmed());
    if (!str.isEmpty())
    {
        m.addField(MsgSp::Field::USER_DATA, str.toStdString());
        //check # chars instead of bytes
        if (str.replace(QRE, " ").size() <= SDSMAXLEN)
            updateHistMenu(ui->msgHistButton->menu(), str, true);
        str.clear(); //for reuse
    }
    if (sel.size() > 1)
    {
        ServerSession::IntListT ids;
        ServerSession::IntListT types;
        for (auto &it : sel)
        {
            ids.push_back(it.first);
            types.push_back(CmnTypes::toMsgSpIdentityType(it.second.type));
        }
        m.addField(MsgSp::Field::CALLED_PARTY,
                   Utils::toString(ids, MsgSp::Value::LIST_DELIMITER));
        m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
                   Utils::toString(types, MsgSp::Value::LIST_DELIMITER));
    }
    else
    {
        auto it = sel.begin();  //single recipient
        m.addField(MsgSp::Field::CALLED_PARTY, it->first);
        m.addField(MsgSp::Field::CALLED_PARTY_TYPE,
                   CmnTypes::toMsgSpIdentityType(it->second.type));
    }
    auto *lw = ui->mmsFiles;
    int n = lw->count();
    if (n == 0)
    {
        str = mMms->send(&m);
        if (str.isEmpty())
            mCommsReg->onOutMms(&m); //add to table only if successful
    }
    else
    {
        QStringList l;
        QString fp;
        for (auto i=0; i<n; ++i)
        {
            fp = lw->item(i)->text();
            str = mMms->send(&m, fp); //FILE_LIST added
            if (!str.isEmpty())
                l << str;
            updateHistMenu(ui->attHistButton->menu(), fp, false);
            //add to table even if failed, to show failure on each file
            mCommsReg->onOutMms(&m, fp, str);
        }
        if (!l.isEmpty())
            str = l.join("\n");
    }
    if (!str.isEmpty())
        QMessageBox::critical(this, tr("MMS Error"), str);
}

void Sds::updateHistMenu(QMenu *menu, const QString &item, bool isMsg)
{
    assert(menu != 0 && !item.isEmpty());
    auto acts(menu->actions());
    for (auto &act : acts)
    {
        if (act->text() == item)
        {
            //existing - move up if not at top
            if (act != acts.first())
            {
                menu->removeAction(act);
                menu->insertAction(acts.first(), act);
            }
            return;
        }
    }
    if (acts.size() == 20)
        menu->removeAction(acts.last()); //remove oldest
    auto *act = (isMsg)? new QAction(item, menu):
                         new QAction(CommsRegister::getIcon(item), item, menu);
    //insert at top
    if (acts.empty())
        menu->addAction(act);
    else
        menu->insertAction(menu->actions().first(), act);
    if (isMsg)
        connect(act, &QAction::triggered, this,
                [act, this] { setMsgText(act->text()); });
    else
        connect(act, &QAction::triggered, this,
                [act, this]
                {
                    QString f(act->text());
                    if (QFile::exists(f))
                        mmsAddFiles(QStringList(f));
                });
}
