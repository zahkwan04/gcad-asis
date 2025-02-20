/**
 * Custom input QDialog implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: InputDialog.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Mohd Rozaimi
 * @author Zulzaidi Atan
 */
#include <QCheckBox>
#include <QComboBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QTableView>
#include <QVBoxLayout>
#include <assert.h>

#include "QtUtils.h"
#include "InputDialog.h"

using namespace std;

static const int PASSWORD_MIN_SIZE = 4;

InputDialog::InputDialog(int type, QWidget *parent) :
QDialog(parent), mType(type), mItemMdl(0)
{
    init();
    switch (type)
    {
        case TYPE_CHANGE_PASSWORD:
        {
            setWindowTitle(tr("Change Password"));
            auto *layout = new QFormLayout(this);
            auto *le = new QLineEdit(this);
            le->setEchoMode(QLineEdit::Password);
            layout->addRow(QString(tr("Current Password: ")), le);
            mFields << le;
            le = new QLineEdit(this);
            le->setEchoMode(QLineEdit::Password);
            layout->addRow(QString(tr("New Password: ")), le);
            mFields << le;
            le = new QLineEdit(this);
            le->setEchoMode(QLineEdit::Password);
            layout->addRow(QString(tr("Confirm New Password: ")), le);
            mFields << le;
            layout->addRow(getBtnBox());
            setMinimumSize(500, 160);
            break;
        }
        default:
        {
            assert("Invalid type in InputDialog::InputDialog" == 0);
            break;
        }
    }
}

InputDialog::InputDialog(ResourceData::ListModel *listMdl,
                         ResourceData::ListModel *membersMdl,
                         QWidget                 *parent) :
QDialog(parent), mType(TYPE_DGNA_ASSIGN), mItemMdl(0)
{
    //CAUTION! DO NOT modify listMdl
    if (membersMdl == 0)
    {
        assert("Bad param in InputDialog::InputDialog(DGNA_ASSIGN)" == 0);
        return;
    }
    init();
    setWindowTitle(tr("DGNA Assignment"));
    auto *cb = new QComboBox(this);
    cb->setModel(listMdl);
    mFields << cb;
    auto *lv = new QListView(this);
    lv->setModel(membersMdl);
    lv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mFields << lv;
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(cb);
    layout->addWidget(lv);
    layout->addWidget(getBtnBox());
}

InputDialog::InputDialog(const QString      &key,
                         QStandardItemModel *itemMdl,
                         const QString      &center,
                         const QString      &label,
                         const string       &layer,
                         QWidget            *parent) :
QDialog(parent), mType(TYPE_SEARCH_RESULTS), mLayer(layer), mItemMdl(itemMdl)
{
    if (itemMdl == 0)
    {
        assert("Bad param in InputDialog::InputDialog(SEARCH_RESULTS)" == 0);
        return;
    }
    init();
    setAttribute(Qt::WA_DeleteOnClose);
    auto *tv = new QTableView(this);
    tv->setModel(itemMdl);
    tv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tv->setSelectionBehavior(QAbstractItemView::SelectRows);
    tv->setSelectionMode(QAbstractItemView::SingleSelection);
    tv->setAlternatingRowColors(true);
    tv->horizontalHeader()->setStretchLastSection(true);
    tv->verticalHeader()->setMinimumWidth(40);
    int hidColStart = COL_CATEGORY;
    QString loc(center);
    if (!loc.isEmpty() && !label.isEmpty())
        loc.prepend(" (").prepend(label).append(")");
    auto *layout = new QVBoxLayout(this);
    if (key.isEmpty())
    {
        hidColStart = COL_TERM_ITEM;
        setWindowTitle(tr("Resources near %1").arg(loc));
        layout->addWidget(new QLabel(QtUtils::getTimestamp(), this));
        tv->setMinimumSize(400, 100);
        tv->sortByColumn(COL_TERM_DISTANCE, Qt::AscendingOrder);
        tv->setSortingEnabled(true);
    }
    else
    {
        if (center.isEmpty())
        {
            setWindowTitle(tr("Search results for: %1").arg(key));
            tv->hideColumn(COL_DISTANCE);
        }
        else
        {
            setWindowTitle(tr("Search results near %1 for: %2").arg(loc, key));
            tv->sortByColumn(COL_DISTANCE, Qt::AscendingOrder);
            tv->setSortingEnabled(true);
        }
        tv->setMinimumSize(500, 300);
    }
    int i = itemMdl->columnCount() - 1;
    for (; i>=hidColStart; --i)
    {
        tv->hideColumn(i);
    }
    tv->resizeColumnsToContents();
    layout->addWidget(tv);
    layout->addWidget(getBtnBox(true));
    connect(tv, &QTableView::doubleClicked, this,
            [this](const QModelIndex &idx)
            {
                Props::ValueMapT prs;
                QString s(idx.model()->index(idx.row(), COL_NAME).data()
                             .toString());
                Props::set(prs, Props::FLD_LBL,
                           s.left(s.indexOf(',')).toStdString());
                Props::set(prs, Props::FLD_LAT,
                           idx.model()->index(idx.row(), COL_LAT).data()
                              .toDouble());
                Props::set(prs, Props::FLD_LON,
                           idx.model()->index(idx.row(), COL_LON).data()
                              .toDouble());
                emit showItem(prs,
                              idx.model()->index(idx.row(), COL_ID).data()
                                 .toString().toStdString(),
                              idx.model()->index(idx.row(), COL_LAYER).data()
                                 .toString().toStdString());
            });
}

InputDialog::InputDialog(const QString      &id,
                         const QString      &start,
                         const QString      &closed,
                         QStandardItemModel *mdl,
                         QWidget            *parent) :
QDialog(parent), mType(TYPE_TRACKING_INC_RESOURCES), mItemMdl(mdl)
{
    if (mdl == 0)
    {
        assert("Bad param in InputDialog::InputDialog(TRACKING_INC_RESOURCES)"
               == 0);
        return;
    }
    init();
    auto *lv = new QListView(this);
    mdl->sort(Qt::AscendingOrder);
    lv->setModel(mdl);
    lv->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(lv);
    setWindowTitle(tr("Tracking: ") + tr("Resources for Incident %1").arg(id));
    auto *chk = new QCheckBox(tr("Use Incident start time: %1").arg(start));
    chk->setChecked(true);
    mFields << chk;
    layout->addWidget(chk);
    if (!closed.isEmpty())
    {
        chk = new QCheckBox(tr("Use Incident closed time: %1").arg(closed));
        chk->setChecked(true);
        mFields << chk;
        layout->addWidget(chk);
    }
    layout->addWidget(getBtnBox());
}

InputDialog::InputDialog(const SubsData::BranchMapT &branches,
                         QSet<int>                   ids,
                         QWidget                    *parent) :
QDialog(parent), mType(TYPE_BRANCH_SELECT), mItemMdl(0)
{
    if (branches.empty())
    {
        assert("Bad param in InputDialog::InputDialog(BRANCH_SELECT)" == 0);
        return;
    }
    init();
    setWindowTitle(tr("Fleet Branch Selection"));
    mListWidget = new QListWidget(this);
    QListWidgetItem *item;
    bool selectAll = ids.contains(-1);
    for (auto &it : branches)
    {
        item = new QListWidgetItem(QtUtils::fromHexUnicode(it.second));
        item->setData(Qt::UserRole, it.first);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState((selectAll || ids.contains(it.first))?
                            Qt::Checked: Qt::Unchecked);
        mListWidget->addItem(item);
    }
    mListWidget->setToolTip(tr("Clear selection to follow server assignment"));
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(mListWidget);
    layout->addWidget(getBtnBox());
}

InputDialog::~InputDialog()
{
    delete mItemMdl;
}

bool InputDialog::getPasswords(QString &oldP, QString &newP)
{
    assert(mType == TYPE_CHANGE_PASSWORD);
    oldP = qobject_cast<QLineEdit *>(mFields.at(0))->text();
    newP = qobject_cast<QLineEdit *>(mFields.at(1))->text();
    if (oldP.isEmpty() || newP.isEmpty())
    {
        QMessageBox::critical(this, tr("Change Password Error"),
                              tr("Password cannot be empty."));
        return false;
    }
    if (newP.trimmed().size() < newP.size() ||
        oldP.trimmed().size() < oldP.size())
    {
        QMessageBox::critical(this, tr("Change Password Error"),
                      tr("Password cannot contain leading or trailing space."));
        return false;
    }
    if (newP.size() < PASSWORD_MIN_SIZE)
    {
        QMessageBox::critical(this, tr("Change Password Error"),
                              tr("Password must have at least %1 characters.")
                                  .arg(PASSWORD_MIN_SIZE));
        return false;
    }
    if (newP != qobject_cast<QLineEdit *>(mFields.at(2))->text())
    {
        QMessageBox::critical(this, tr("Change Password Error"),
                              tr("New Passwords do not match."));
        return false;
    }
    return true;
}

int InputDialog::getDgnaGssi()
{
    assert(mType == TYPE_DGNA_ASSIGN);
    auto *cb = qobject_cast<QComboBox *>(mFields.at(0));
    return ResourceData::getItemId(
                            static_cast<ResourceData::ListModel *>(cb->model()),
                            cb->currentIndex());
}

QStringList InputDialog::getDgnaMembers()
{
    assert(mType == TYPE_DGNA_ASSIGN);
    return ResourceData::model(qobject_cast<QListView *>(mFields.at(1)))
           ->getIds(true);
}

QString InputDialog::getSelectedResources()
{
    assert(mType == TYPE_TRACKING_INC_RESOURCES);
    QStandardItem *item;
    QString list;
    //get all the checked resources
    int i = mItemMdl->rowCount() - 1;
    for (; i>=0; --i)
    {
        item = mItemMdl->item(i, 0);
        if (item->checkState() == Qt::Checked)
            list.append(item->data(Qt::DisplayRole).toString().append(" "));
    }
    return list;
}

bool InputDialog::getUseTimeFlag(bool start)
{
    assert(mType == TYPE_TRACKING_INC_RESOURCES);
    if (!start && mFields.size() < 2)
    {
        assert("Bad param in InputDialog::getUseTimeFlag" == 0);
        return false;
    }
    return qobject_cast<QCheckBox *>(mFields.at((start)? 0: 1))->isChecked();
}

QString InputDialog::getSelectedBranches()
{
    assert(mType == TYPE_BRANCH_SELECT);
    QString list;
    QListWidgetItem *item = 0;
    bool allSelected = true;
    int i = mListWidget->count() - 1;
    for (; i>=0; --i)
    {
        item = mListWidget->item(i);
        if (item->data(Qt::CheckStateRole) == Qt::Checked)
        {
            if (!list.isEmpty())
                list.append(',');
            list.append(item->data(Qt::UserRole).toString());
        }
        else if (allSelected)
        {
            allSelected = false;
        }
    }
    return ((allSelected)? "-": list);
}

void InputDialog::init()
{
    //remove the help and close buttons from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
}

QDialogButtonBox *InputDialog::getBtnBox(bool noCancel)
{
    auto *bb = new QDialogButtonBox(
                      (noCancel)? QDialogButtonBox::Ok:
                                  QDialogButtonBox::Ok|QDialogButtonBox::Cancel,
                      Qt::Horizontal, this);
    QObject::connect(bb, SIGNAL(accepted()), SLOT(accept()));
    QObject::connect(bb, SIGNAL(rejected()), SLOT(reject()));
    return bb;
}
