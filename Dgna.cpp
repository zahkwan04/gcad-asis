/**
 * UI DGNA implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Dgna.cpp 1812 2024-02-14 03:07:20Z rosnin $
 * @author Mazdiana Makmor
 */
#include <assert.h>
#include <QList>
#include <QMessageBox>
#include <QRegExp>

#include "CmnTypes.h"
#include "QtUtils.h"
#include "Style.h"
#include "SubsData.h"
#include "ui_Dgna.h"
#include "Dgna.h"

using namespace std;

static const string LOGPREFIX("Dgna:: ");

Dgna::Dgna(Logger *logger, QWidget *parent) :
QDialog(parent), ui(new Ui::Dgna), mType(ResourceData::TYPE_DGNA_IND),
mLogger(logger)
{
    ui->setupUi(this);
    connect(ui->doneButton, &QPushButton::clicked, this,
            [this] { close(); });
    connect(ui->addButton, &QPushButton::clicked, this,
            [this]
            {
                //assign selected SSI to current DGNA group
                QModelIndexList l(mResourceList->selectionModel()
                                  ->selectedIndexes());
                if (l.empty())
                    return;
                auto *mdl = static_cast<ResourceData::ListModel *>(
                                                     mSortModel->sourceModel());
                ResourceData::IdsT ssis;
                for (auto &idx : l)
                {
                    ssis.insert(ResourceData::getItemId(
                                mdl->item(mSortModel->mapToSource(idx).row())));
                }
                addMembers(mGssi, mType, ssis, false);
            });
    connect(ui->removeButton, &QPushButton::clicked, this,
            [this]
            {
                //deassign selected SSIs from DGNA group
                QModelIndexList l(mMemberList->selectionModel()
                                  ->selectedIndexes());
                if (l.empty())
                    return;
                auto *mdl = ResourceData::model(mMemberList);
                ResourceData::IdsT ssis;
                for (auto &idx : l)
                {
                    ssis.insert(ResourceData::getItemId(mdl->item(idx.row())));
                }
                removeMembers(mGssi, mType, ssis, false);
            });
    connect(ui->removeAllButton, &QPushButton::clicked, this,
            [this]
            {
                //deassign all SSIs from DGNA group
                mMemberList->selectAll();
                ui->removeButton->click();
            });
    connect(ui->searchEdit, &QLineEdit::textChanged, this,
            [this](const QString &txt)
            {
                //filter the resources list
                QRegExp re(txt, Qt::CaseInsensitive);
                if (re.isValid())
                    mSortModel->setFilterRegExp(re);
            });
    setTheme();
    //remove the help button from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    mSortModel = new QSortFilterProxyModel;
    mSortModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    mResourceList = new DraggableListView(ResourceData::TYPE_SUBSCRIBER);
    mResourceList->setDragDropMode(QAbstractItemView::DragDrop);
    mResourceList->setFont(ui->searchEdit->font());
    mResourceList->installEventFilter(this);
    ui->searchLayout->addWidget(mResourceList);

    mMemberList = new DraggableListView(ResourceData::TYPE_SUBSCRIBER);
    mMemberList->setDragDropMode(QAbstractItemView::DragDrop);
    mMemberList->setFont(ui->dgnaGrpEdit->font());
    mMemberList->installEventFilter(this);
    ui->addVLayout->addWidget(mMemberList);
    mMemberList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mMemberList, &QListView::customContextMenuRequested, this,
            [this](const QPoint &pos)
            {
                auto idx(mMemberList->indexAt(pos));
                if (!idx.isValid())
                    return;
                QList<int> actTypes;
                actTypes << CmnTypes::ACTIONTYPE_DELETE;
                if (mMemberList->selectionModel()->selectedIndexes().count() ==
                    1)
                    actTypes << CmnTypes::ACTIONTYPE_LOCATE;
                actTypes << CmnTypes::ACTIONTYPE_COPY;
                QMenu menu(this);
                QtUtils::addMenuActions(menu, actTypes);
                auto *act = menu.exec(mMemberList->mapToGlobal(pos));
                if (act == 0)
                    return;
                switch (act->data().toInt())
                {
                    case CmnTypes::ACTIONTYPE_COPY:
                        QtUtils::copyToClipboard(mMemberList);
                        break;
                    case CmnTypes::ACTIONTYPE_DELETE:
                        ui->removeButton->click();
                        break;
                    case CmnTypes::ACTIONTYPE_LOCATE:
                        emit locateResource(
                               ResourceData::getItemId(mMemberList, idx.row()));
                        break;
                    default:
                        break; //do nothing
                }
            });
}

Dgna::~Dgna()
{
    delete mSortModel;
    delete mResourceList;
    delete mMemberList;
    delete ui;
}

void Dgna::setTheme()
{
    const QString &ss(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->searchFrame->setStyleSheet(ss);
    ui->resourcesFrame->setStyleSheet(ss);
    ui->dgnaGrpFrame->setStyleSheet(ss);
    ui->addFrame->setStyleSheet(ss);
}

void Dgna::addMembers(int gssi, QStringList &members)
{
    ResourceData::IdsT ids;
    for (auto &s : members)
    {
        ids.insert(s.toInt());
    }
    addMembers(gssi, ResourceData::TYPE_DGNA_IND, ids, false);
}

void Dgna::removeMembers(int gssi, QStringList &members)
{
    ResourceData::IdsT ids;
    if (!members.empty())
    {
        for (auto &s : members)
        {
            ids.insert(s.toInt());
        }
    }
    else if (!SubsData::getGrpMembers(gssi, ids))
    {
        return;
    }
    removeMembers(gssi, ResourceData::TYPE_DGNA_IND, ids, false);
}

void Dgna::dgnaResult(MsgSp *msg)
{
    if (msg == 0)
    {
        assert("Bad param in Dgna::dgnaResult" == 0);
        return;
    }
    int i = msg->getType();
    if (msg->isResultSuccessful() &&
        i != MsgSp::Type::MON_SSDGNA_ASSIGN &&
        i != MsgSp::Type::MON_SSDGNA_DEASSIGN)
        return;

    int ssi = msg->getFieldInt(MsgSp::Field::AFFECTED_USER);
    //this result may not be for the current displayed DGNA
    int gssi = msg->getFieldInt(MsgSp::Field::GSSI);
    QString ssiName(ResourceData::getDspTxt(ssi,
                                         (mType == ResourceData::TYPE_DGNA_IND)?
                                             ResourceData::TYPE_SUBSCRIBER:
                                             ResourceData::TYPE_GROUP_OR_DGNA));
    QString grpName(ResourceData::getDspTxt(gssi, mType));
    ResourceData::IdsT ssis;

    switch (i)
    {
        case MsgSp::Type::SSDGNA_ASSIGN:
            LOGGER_ERROR(mLogger, LOGPREFIX << "dgnaResult: "
                         << msg->getName() << ": Failed to assign "
                         << msg->getFieldString(MsgSp::Field::AFFECTED_USER)
                         << " to " << grpName.toStdString() << ", "
                         << msg->getFieldValueString(MsgSp::Field::RESULT));
            if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                MsgSp::Value::RESULT_SSI_ALREADY_ASSIGNED)
                break;
            QMessageBox::information(this, tr("DGNA Assign"),
                                     tr("Failed to add %1 to %2,\n%3")
                                         .arg(ssiName, grpName,
                                              QString::fromStdString(
                                                  msg->getFieldValueString(
                                                     MsgSp::Field::RESULT))));
            //fallthrough
        case MsgSp::Type::MON_SSDGNA_DEASSIGN:
            if (gssi == mGssi)
            {
                ssis.insert(ssi);
                removeMembers(gssi, mType, ssis, true);
            }
            break;
        case MsgSp::Type::SSDGNA_DEASSIGN:
            LOGGER_ERROR(mLogger, LOGPREFIX << "dgnaResult: "
                         << msg->getName() << ": Failed to deassign "
                         << msg->getFieldString(MsgSp::Field::AFFECTED_USER)
                         << " from " << grpName.toStdString() << ", "
                         << msg->getFieldValueString(MsgSp::Field::RESULT));
            if (msg->getFieldInt(MsgSp::Field::RESULT) ==
                MsgSp::Value::RESULT_GSSI_NOT_ASSIGNED)
                break;
            QMessageBox::information(this, tr("DGNA Deassign"),
                                     tr("Failed to remove %1 from %2,\n%3")
                                         .arg(ssiName, grpName,
                                              QString::fromStdString(
                                                  msg->getFieldValueString(
                                                     MsgSp::Field::RESULT))));
            //fallthrough
        case MsgSp::Type::MON_SSDGNA_ASSIGN:
            if (gssi == mGssi)
            {
                ssis.insert(ssi);
                addMembers(gssi, mType, ssis, true);
            }
            break;
        default:
            break; //do nothing
    }
}

void Dgna::setDgnaItems(int gssi, int type, ResourceData::ListModel *mdl)
{
    if (mdl == 0)
    {
        assert("Bad param in Dgna::setDgnaItems" == 0);
        return;
    }
    mGssi = gssi;
    mType = type;
    if (mType == ResourceData::TYPE_DGNA_IND)
    {
        ui->categoryLabel->setText(tr("Subscribers"));
        type = ResourceData::TYPE_SUBSCRIBER; //type is now for members
    }
    else
    {
        ui->categoryLabel->setText(tr("Groups"));
        type = ResourceData::TYPE_GROUP_OR_DGNA;
    }
    mSortModel->setSourceModel(mdl);
    mResourceList->setModel(mSortModel);
    ui->dgnaGrpEdit->setText(ResourceData::getDspTxt(gssi, mType));
    //set the cursor to the beginning so that the beginning remains visible when
    //truncated
    ui->dgnaGrpEdit->home(false);
    auto *membersMdl = ResourceData::model(mMemberList);
    if (membersMdl == 0)
    {
        membersMdl = ResourceData::createModel(type, mMemberList);
        mMemberList->setModel(membersMdl);
    }
    else
    {
        membersMdl->clear();
        membersMdl->setType(type);
    }
    //show existing members, if any
    SubsData::IdSetT members;
    if (SubsData::getGrpMembers(mGssi, members))
    {
        for (auto i : members)
        {
            ResourceData::addId(membersMdl, i);
        }
        membersMdl->sort(0);
    }
    ui->searchEdit->setFocus();
    raise();
    show();
    activateWindow();
}

bool Dgna::eventFilter(QObject *obj, QEvent *event)
{
    //accept only drag-and-drop between mResourceList and mMemberList
    if (event->type() == QEvent::DragEnter)
    {
        auto *e = static_cast<QDragEnterEvent *>(event);
        if (obj != e->source() &&
            (e->source() == mResourceList || e->source() == mMemberList))
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
        if (obj == mMemberList)
        {
            ui->addButton->click();
            return true;
        }
        if (obj == mResourceList)
        {
            ui->removeButton->click();
            return true;
        }
    }
    return false;
}

void Dgna::addMembers(int                 gssi,
                      int                 type,
                      ResourceData::IdsT &members,
                      bool                moveOnly)
{
    bool isDgnaInd = (type == ResourceData::TYPE_DGNA_IND);
    bool isCurrent = (gssi == mGssi);
    int  ssiType   = (isDgnaInd)? ResourceData::TYPE_SUBSCRIBER:
                                  ResourceData::TYPE_GROUP_OR_DGNA;
    auto *mdl = ResourceData::model(mMemberList); //may be 0 if !isCurrent
    ResourceData::IdsT ids;
    if (isCurrent)
        mdl->getIds(ids);
    QString fails;
    for (auto ssi : members)
    {
        if (isCurrent && ids.count(ssi) != 0)
            continue; //already in
        if (!moveOnly)
        {
            if (mSession->ssDgnaAssign(gssi, ssi, isDgnaInd, true) <= 0)
            {
                if (!fails.isEmpty())
                    fails.append(", ");
                fails.append(ResourceData::getDspTxt(ssi, ssiType));
            }
            else if (isCurrent)
            {
                ResourceData::addId(mdl, ssi);
            }
        }
        else if (isCurrent)
        {
            ResourceData::addId(mdl, ssi);
        }
    } //for (auto ssi : members)
    if (isCurrent)
        mdl->sort(0);
    if (!fails.isEmpty())
    {
        QString grp((isCurrent)? ui->dgnaGrpEdit->text():
                                 ResourceData::getDspTxt(gssi, type));
        LOGGER_ERROR(mLogger, LOGPREFIX << "addMembers: Failed to assign "
                     << fails.toStdString() << " to " << grp.toStdString());
        QMessageBox::information(this, tr("DGNA Assign"),
                                 tr("Failed to add %1 to %2").arg(fails, grp));
    }
}

void Dgna::removeMembers(int                 gssi,
                         int                 type,
                         ResourceData::IdsT &members,
                         bool                moveOnly)
{
    bool isDgnaInd = (type == ResourceData::TYPE_DGNA_IND);
    bool isCurrent = (gssi == mGssi);
    int  ssiType   = (isDgnaInd)? ResourceData::TYPE_SUBSCRIBER:
                                  ResourceData::TYPE_GROUP_OR_DGNA;
    auto *mdl = ResourceData::model(mMemberList);
    QString fails;
    for (auto ssi : members)
    {
        if (moveOnly || mSession->ssDgnaAssign(gssi, ssi, isDgnaInd, false) > 0)
        {
            if (isCurrent)
                mdl->removeId(ssi);
        }
        else
        {
            if (!fails.isEmpty())
                fails.append(", ");
            fails.append(ResourceData::getDspTxt(ssi, ssiType));
        }
    } //for (auto ssi : members)
    if (!fails.isEmpty())
    {
        QString grp((isCurrent)? ui->dgnaGrpEdit->text():
                                 ResourceData::getDspTxt(gssi, type));
        LOGGER_ERROR(mLogger, LOGPREFIX << "removeMembers: "
                     << "Failed to deassign " << fails.toStdString() << " from "
                     << grp.toStdString());
        QMessageBox::information(this, tr("DGNA Deassign"),
                                 tr("Failed to remove %1 from %2")
                                     .arg(fails, grp));
    }
}
