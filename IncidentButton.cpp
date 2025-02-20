/**
 * The Incident Button UI implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: IncidentButton.cpp 1823 2024-03-11 09:03:42Z rosnin $
 * @author Mazdiana Makmor
 */
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <assert.h>

#include "QtUtils.h"
#include "Style.h"
#include "Utils.h"
#include "ui_IncidentButton.h"
#include "IncidentButton.h"

using namespace std;

static const QString ICON_FILE_PREFIX(":/Images/images/icon_inc_");
static const QString STYLE_LOCKED("QFrame#frame {border-width:1px;"
                                  "border-color:red;border-style:solid}");

IncidentButton::IntFilterT      IncidentButton::sIntFilter;
IncidentButton::StrFilterT      IncidentButton::sStrFilter;
IncidentButton::FilterMaxSizesT IncidentButton::sFilterMax;
map<string, QPixmap>            IncidentButton::sIconMap;

IncidentButton::IncidentButton(IncidentData *data, QWidget *parent) :
QWidget(parent), ui(new Ui::IncidentButton), mData(data)
{
    ui->setupUi(this);
    ui->incidentId->setNum(data->getId());
    updateData(data);
    setLockToolTip(data->getLockHolder());
}

IncidentButton::~IncidentButton()
{
    delete mData;
    delete ui;
}

void IncidentButton::release()
{
    setStyle();
}

void IncidentButton::updateData(IncidentData *data)
{
    if (data == 0 || (data != mData && data->getId() != mData->getId()))
    {
        assert("Bad param in IncidentButton::updateData" == 0);
        return;
    }
    if (data != mData)
    {
        delete mData;
        mData = data;
    }
    setStyle(true);
}

void IncidentButton::refreshIcon()
{
    string cat(mData->getBaseCat());
    ui->iconLabel->setPixmap(getPixmap(cat));
}

void IncidentButton::setLockHolder(int holder)
{
    mData->setLockHolder(holder);
    setStyle();
    setLockToolTip(holder);
}

void IncidentButton::setFilteredVisible()
{
    bool visible = true;
    for (const auto &it : sIntFilter)
    {
        switch (it.first)
        {
            case FIELD_PRIORITY:
                visible = isInFilter(it.first, mData->getPriority());
                break;
            case FIELD_STATE:
                visible = isInFilter(it.first, mData->getState());
                break;
            default:
                break; //do nothing
        }
        if (!visible)
            break;
    }
    if (visible)
    {
        for (const auto &it : sStrFilter)
        {
            switch (it.first)
            {
                case FIELD_ADDR2:
                    visible = isInFilter(it.first, mData->getAddress2());
                    break;
                case FIELD_ADDRSTATE:
                    visible = isInFilter(it.first, mData->getAddrState());
                    break;
                case FIELD_CATEGORY:
                    visible = isInFilter(it.first, mData->getCategory());
                    break;
                default:
                    break; //do nothing
            }
            if (!visible)
                break;
        }
    }
    setVisible(visible);
}

bool IncidentButton::setCustomIcons(const QStringList &cats,
                                    const QString     &dir,
                                    QString           &files)
{
    QDir d(dir, "icon_inc_*.png", QDir::Name, QDir::Files);
    if (!d.exists() || d.isEmpty())
        return false;
    QString cat;
    QPixmap p;
    files.clear();
    for (QFileInfo &f : d.entryInfoList())
    {
        //extract category from icon_inc_<cat>.png
        cat = f.fileName();
        cat.remove(0, 9).chop(4);
        if (cats.contains(cat) && p.load(f.absoluteFilePath()))
        {
            if (files.isEmpty())
            {
                files = dir; //first valid icon found
                sIconMap.clear();
            }
            files.append("\n").append(cat);
            //limit size - storing a big pixmap would be wasteful
            sIconMap[cat.toStdString()] = (p.height() <= 100)?
                                          p: p.scaledToHeight(100);
        }
    }
    return !files.isEmpty();
}

void IncidentButton::setDefaultIcons()
{
    sIconMap.clear();
}

void IncidentButton::setStyle(bool doSetLabel)
{
    string cat(mData->getBaseCat());
    switch (mData->getState())
    {
        case IncidentData::STATE_DISPATCHED:
            cat.append(Utils::toString(IncidentData::STATE_DISPATCHED));
            break;
        case IncidentData::STATE_ON_SCENE:
            cat.append(Utils::toString(IncidentData::STATE_ON_SCENE));
            break;
        case IncidentData::STATE_RECEIVED:
            cat.append(Utils::toString(IncidentData::STATE_RECEIVED));
            break;
        default:
            return;
    }
    ui->incidentId->setStyleSheet(Style::getStyle((mData->isHighPriority())?
                                                  Style::OBJ_LABEL_RED:
                                                  Style::OBJ_LABEL));
    if (doSetLabel)
        ui->statusLabel
          ->setText(IncidentData::getStateText(mData->getState()));
    ui->frame->setStyleSheet("QFrame {background:" +
                             Style::getStyle(Style::OBJ_BACKGROUND) + "}" +
                             ((mData->isLocked())? STYLE_LOCKED: ""));
    ui->iconLabel->setPixmap(getPixmap(cat));
}

bool IncidentButton::hasFilter(int field)
{
    if (sIntFilter.count(field) != 0)
        return !sIntFilter[field].empty();
    if (sStrFilter.count(field) != 0)
        return !sStrFilter[field].empty();
    return false;
}

bool IncidentButton::isInFilter(int field, int value)
{
    switch (field)
    {
        case FIELD_PRIORITY:
        case FIELD_STATE:
            if (sIntFilter.count(field) != 0)
                return (sIntFilter[field].count(value) != 0);
            break;
        default:
            break; //do nothing
    }
    return false;
}

bool IncidentButton::isInFilter(int field, const QString &value)
{
    switch (field)
    {
        case FIELD_ADDR2:
        {
            //positive if:
            //-filter value not set, or
            //-field value contains all filter values as case-insensitive
            // substrings
            if (hasFilter(field))
            {
                for (const auto &s : sStrFilter[field])
                {
                    if (!value.contains(s, Qt::CaseInsensitive))
                        return false;
                }
            }
            return true;
        }
        case FIELD_ADDRSTATE:
        case FIELD_CATEGORY:
        {
            if (sStrFilter.count(field) != 0)
                return (sStrFilter[field].count(value) != 0);
            break;
        }
        case FIELD_PRIORITY:
        {
            return isInFilter(field, IncidentData::getPriorityVal(value));
        }
        case FIELD_STATE:
        {
            return isInFilter(field, IncidentData::getStateVal(value));
        }
        default:
        {
            break; //do nothing
        }
    }
    return false;
}

void IncidentButton::updateFilter(int field, const QString &value, bool doAdd)
{
    int val;
    bool isNum;
    switch (field)
    {
        case FIELD_ADDR2:
            //value has space-separated tokens, each of which can be either a
            //word or a double-quoted phrase
            sStrFilter[field].clear();
            if (!value.isEmpty())
            {
                foreach (QString str, QtUtils::tokenize(value))
                {
                    sStrFilter[field].insert(str);
                }
            }
            break;
        case FIELD_ADDRSTATE:
        case FIELD_CATEGORY:
            if (doAdd)
                sStrFilter[field].insert(value);
            else if (sStrFilter.count(field) != 0)
                sStrFilter[field].erase(value);
            break;
        case FIELD_PRIORITY:
            val = value.toInt(&isNum);
            if (!isNum)
                val = IncidentData::getPriorityVal(value);
            if (doAdd)
                sIntFilter[field].insert(val);
            else if (sIntFilter.count(field) != 0)
                sIntFilter[field].erase(val);
            break;
        case FIELD_STATE:
            val = value.toInt(&isNum);
            if (!isNum)
                val = IncidentData::getStateVal(value);
            if (doAdd)
                sIntFilter[field].insert(val);
            else if (sIntFilter.count(field) != 0)
                sIntFilter[field].erase(val);
            break;
        default:
            break; //do nothing
    }
}

void IncidentButton::addFilter(int field, const QStringList &values)
{
    foreach (QString value, values)
    {
        updateFilter(field, value);
    }
}

string IncidentButton::getFilter(int field)
{
    if (!hasFilter(field))
        return "";
    QString filter;
    switch (field)
    {
        case FIELD_ADDR2:
        {
            QString s;
            for (const auto &it : sStrFilter[field])
            {
                s = it;
                //enclose a phrase in double quotes
                if (s.contains(" "))
                    s.prepend("\"").append("\"");
                filter.append(s).append(" ");
            }
            filter.chop(1); //remove last space
            break;
        }
        case FIELD_ADDRSTATE:
        case FIELD_CATEGORY:
        {
            if (sFilterMax.count(field) != 0 &&
                sStrFilter[field].size() == sFilterMax[field])
                return "";
            auto it = sStrFilter[field].begin();
            filter.append(*it);
            for (++it; it!=sStrFilter[field].end(); ++it)
            {
                filter.append(FILTER_SEP).append(*it);
            }
            break;
        }
        case FIELD_PRIORITY:
        case FIELD_STATE:
        {
            if (sFilterMax.count(field) != 0 &&
                sIntFilter[field].size() == sFilterMax[field])
                return "";
            return Utils::toString(sIntFilter[field], FILTER_SEP);
        }
        default:
        {
            break; //do nothing
        }
    }
    return filter.toStdString();
}

void IncidentButton::setFilterMaxSize(int field, int size)
{
    sFilterMax[field] = size;
}

void IncidentButton::resetFilter()
{
    sIntFilter.clear();
    sStrFilter.clear();
}

const QPixmap &IncidentButton::getPixmap(string &cat)
{
    //sample cat: accident/fire/rescue/robbery/default
    QString fname(ICON_FILE_PREFIX);
    while (sIconMap.count(cat) == 0) //not yet created - create now
    {
        fname.append(QString::fromStdString(cat)).append(".png");
        if (!QFile(fname).exists()) //icon not in resources
        {
            fname = ICON_FILE_PREFIX;
            cat = "default0";
            continue;
        }
        sIconMap[cat] = QPixmap(fname);
        break;
    }
    return sIconMap[cat];
}

void IncidentButton::enterEvent(QEvent *)
{
    ui->frame->setStyleSheet("QFrame {background:" +
                             Style::getThemeColor(Style::THEME_COLOR_IDX1) +
                             "}" + ((mData->isLocked())? STYLE_LOCKED: ""));
}

void IncidentButton::leaveEvent(QEvent *)
{
    setStyle();
}

void IncidentButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        ui->frame->setStyleSheet("QFrame {background:" +
                                 Style::getThemeColor(Style::THEME_COLOR_IDX2) +
                                 "}" + ((mData->isLocked())? STYLE_LOCKED: ""));
        emit showData();
    }
}

void IncidentButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        enterEvent(0); //revert to mouse hover color
}

void IncidentButton::setLockToolTip(int lockHolder)
{
    if (lockHolder != 0)
        setToolTip(tr("Being edited by Dispatcher %1").arg(lockHolder));
    else
        setToolTip("");
}
