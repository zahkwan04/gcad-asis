/**
 * Resource data manager implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2020-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ResourceData.cpp 1885 2024-11-28 08:32:01Z hazim.rujhan $
 * @author Rosnin Mustaffa
 * @author Mohd Rozaimi
 */
#include <QSet>

#include "QtUtils.h"
#include "Utils.h"
#include "ResourceData.h"

using namespace std;

Logger                 *ResourceData::sLogger(0);
bool                    ResourceData::sFull(true);
bool                    ResourceData::sMobileStat(true);
int                     ResourceData::sGrpDspOpt     = DSP_OPT_NAME_SSI;
int                     ResourceData::sSubsDspOpt    = DSP_OPT_SSI_NAME;
int                     ResourceData::sMapSubsDspOpt = DSP_OPT_SSI_NAME;
ResourceData::TypeMapT  ResourceData::sDataMap;
set<ResourceData::ListModel *> ResourceData::sModels;

static const string  LOGPREFIX("ResourceData:: ");
static const QString PREFIX_DISPATCHER("D:");
static const QString SEP_GRP(": ");  //separator between group name and GSSI
static const QString SEP_ISSI(" :"); //separator between ISSI and name

void ResourceData::init(Logger *logger)
{
    if (logger == 0)
    {
        assert("Bad param in ResourceData::init" == 0);
        return;
    }
    sLogger = logger;
    //create master models
    QSet<int> types;
    types << TYPE_SUBSCRIBER << TYPE_GROUP << TYPE_DGNA_IND
#ifdef DGNA_OF_GRPS
          << TYPE_DGNA_GRP
#endif
          << TYPE_DISPATCHER << TYPE_MOBILE;
    if (sMobileStat)
        types << TYPE_MOBILE_ONLINE;
    for (auto t : qAsConst(types))
    {
        sDataMap[t].mdl = new ListModel(t);
    }
}

void ResourceData::setMobileStat(bool enable)
{
    sMobileStat = enable;
    if (enable)
    {
        if (sDataMap.count(TYPE_MOBILE_ONLINE) == 0)
            sDataMap[TYPE_MOBILE_ONLINE].mdl = new ListModel(TYPE_MOBILE_ONLINE);
    }
    else if (sDataMap.count(TYPE_MOBILE_ONLINE) != 0)
    {
        delete sDataMap[TYPE_MOBILE_ONLINE].mdl;
        sDataMap.erase(TYPE_MOBILE_ONLINE);
    }
}

QString ResourceData::getTypeTxt(int type)
{
    switch (type)
    {
#ifdef DGNA_OF_GRPS
        case ResourceData::TYPE_DGNA_GRP:
            return QObject::tr("DGNA-Grp");
        case ResourceData::TYPE_DGNA_IND:
            return QObject::tr("DGNA-Ind");
#else
        case ResourceData::TYPE_DGNA_IND:
            return QObject::tr("DGNA");
#endif
        case ResourceData::TYPE_DISPATCHER:
            return QObject::tr("Dispatcher");
        case ResourceData::TYPE_GROUP:
            return QObject::tr("Group");
        case ResourceData::TYPE_GROUP_OR_DGNA:
            return QObject::tr("Group").append(" & ")
                   .append(QObject::tr("DGNA"));
        case ResourceData::TYPE_MOBILE:
        case ResourceData::TYPE_MOBILE_ONLINE:
            return QObject::tr("Mobile");
        case ResourceData::TYPE_SUBS_OR_MOBILE:
            return QObject::tr("Subscriber").append(" & ")
                   .append(QObject::tr("Mobile"));
        case ResourceData::TYPE_SUBSCRIBER:
            return QObject::tr("Subscriber");
        case ResourceData::TYPE_UNKNOWN:
        default:
            return "";
    }
}

void ResourceData::cleanup(bool doDelete)
{
    for (auto &it : sDataMap)
    {
        if (doDelete)
            delete it.second.mdl;
        else
            it.second.reset();
    }
}

QStringList ResourceData::dspOptLabels(int type)
{
    return QStringList()
           << QObject::tr("SSI")
           << QObject::tr("Name")
           << QString(QObject::tr("Name"))
                      .append((type == TYPE_GROUP)? SEP_GRP: SEP_ISSI)
                      .append(QObject::tr("SSI"))
           << QString(QObject::tr("SSI"))
                      .append((type == TYPE_GROUP)? SEP_GRP: SEP_ISSI)
                      .append(QObject::tr("Name"));
}

void ResourceData::reload(int type)
{
    string s;
    if (sFull && (type < 0 || type == TYPE_SUBSCRIBER))
    {
        s = SubsData::getIssis();
        if (!s.empty())
            loadModel(TYPE_SUBSCRIBER, QString::fromStdString(s));
        if (type >= 0)
            return;
    }
    if (type < 0 || type == TYPE_MOBILE)
    {
        s = SubsData::getIssis(true);
        if (!s.empty())
            loadModel(TYPE_MOBILE, QString::fromStdString(s));
        if (sMobileStat)
        {
            s = SubsData::getClientData(false);
            if (!s.empty())
                loadModel(TYPE_MOBILE_ONLINE, QString::fromStdString(s));
        }
        if (type >= 0)
            return;
    }
    if (type >= 0)
    {
        SubsData::Ssi2DescMapT grps;
        if (SubsData::getGroups(CmnTypes::toMsgSpSubsType(type), grps))
            loadModel(type, grps);
        return;
    }
    SubsData::Ssi2DescMapT statGrps;
    if (sFull)
    {
        SubsData::Ssi2DescMapT diGrps;
        SubsData::Ssi2DescMapT dgGrps;
        SubsData::getGroups(statGrps, diGrps, dgGrps);
        LOGGER_DEBUG(sLogger, LOGPREFIX << "reload: Subscribers:"
                     << sDataMap[TYPE_SUBSCRIBER].mdl->rowCount()
                     << ", Groups:" << statGrps.size() << ", DGNA-Ind:"
                     << diGrps.size() << ", DGNA-Grp:" << dgGrps.size());
        if (!statGrps.empty())
            loadModel(TYPE_GROUP, statGrps);
        if (!diGrps.empty())
            loadModel(TYPE_DGNA_IND, diGrps);
#ifdef DGNA_OF_GRPS
        if (!dgGrps.empty())
            loadModel(TYPE_DGNA_GRP, dgGrps);
#endif
    }
    else if (SubsData::getGroups(CmnTypes::toMsgSpSubsType(TYPE_GROUP),
                                 statGrps))
    {
        loadModel(TYPE_GROUP, statGrps);
    }
    s = SubsData::getClientData(true);
    if (!s.empty())
        loadModel(TYPE_DISPATCHER, QString::fromStdString(s));
}

bool ResourceData::addRemoveId(bool doAdd, int type, int id)
{
    auto *mdl = getModel(type);
    if (mdl == 0)
    {
        LOGGER_ERROR(sLogger, LOGPREFIX << "addRemoveId: Invalid type " << type);
        return false;
    }
    if (!doAdd)
    {
        sDataMap[type].idNames.erase(id);
        return mdl->removeId(id);
    }
    if (mdl->hasId(id))
        return false;
    string nm;
    switch (type)
    {
        case TYPE_DGNA_GRP:
        case TYPE_DGNA_IND:
        case TYPE_GROUP:
            nm = SubsData::getGrpName(id);
            break;
        case TYPE_MOBILE:
        case TYPE_MOBILE_ONLINE:
        case TYPE_SUBSCRIBER:
            nm = SubsData::getIssiName(id);
            break;
        default:
            break; //do nothing
    }
    QString name((nm.empty())? "": QtUtils::fromHexUnicode(nm).trimmed());
    sDataMap[type].idNames[id] = name;
    mdl->addIdWithName(id, name, true);
    return true;
}

bool ResourceData::getName(int id, int &type, QString &name)
{
    if (type >= TYPE_UNKNOWN)
    {
        for (const auto &it : sDataMap)
        {
            if (it.second.idNames.count(id) != 0)
            {
                type = it.first;
                name = it.second.idNames.at(id);
                return true;
            }
        }
    }
    else if (sDataMap.count(type) != 0 && sDataMap[type].idNames.count(id) != 0)
    {
        name = sDataMap[type].idNames[id];
        return true;
    }
    return false;
}

QString ResourceData::getName(int id, int type, bool idIfEmpty)
{
    QString name;
    getName(id, type, name);
    if (idIfEmpty && name.isEmpty())
        return QString::number(id);
    return name;
}

QString ResourceData::getDspTxt(int id, int type)
{
    return getDspTxtAndType(id, type);
}

bool ResourceData::isClient(const QString &str)
{
    return (str.startsWith(PREFIX_DISPATCHER));
}

QString ResourceData::getClientDspTxt(int id)
{
    return QString::number(id).prepend(PREFIX_DISPATCHER);
}

void ResourceData::getClientDspTxt(QString &str)
{
    if (!isClient(str))
        str.prepend(PREFIX_DISPATCHER);
}

int ResourceData::getType(int id)
{
    for (const auto &it : sDataMap)
    {
        if (it.second.mdl->hasId(id))
            return it.first;
    }
    return TYPE_UNKNOWN;
}

int ResourceData::getType(const QString &str)
{
    if (str.contains(SEP_GRP))
        return TYPE_GROUP;
    if (isClient(str))
        return TYPE_DISPATCHER;
    int type = TYPE_UNKNOWN;
    QString s(str);
    matchName(false, s, type);
    if (type == TYPE_DGNA_GRP || type == TYPE_DGNA_IND)
        return TYPE_GROUP;
    return type;
}

int ResourceData::getId(const QString &txt)
{
    int id = 0;
    int type = TYPE_UNKNOWN;
    if (getId(txt, type, id))
        return id;
    QString s(txt);
    return matchName(false, s, type);
}

int ResourceData::matchName(bool update, QString &str, int &type)
{
    QSet<int> types; //possible types to check
    if (type != TYPE_UNKNOWN)
        types << type; //specific type
    int id = 0;
    bool ok = getId(str, type, id);
    if (types.isEmpty())
    {
        if (type == TYPE_GROUP_OR_DGNA)
        {
            types << TYPE_GROUP << TYPE_DGNA_IND;
#ifdef DGNA_OF_GRPS
            types << TYPE_DGNA_GRP;
#endif
        }
        else if (type != TYPE_UNKNOWN)
        {
            types << type;
        }
        else
        {
            //ID may be of any type
            types << TYPE_SUBSCRIBER << TYPE_GROUP << TYPE_DGNA_IND
#ifdef DGNA_OF_GRPS
                  << TYPE_DGNA_GRP
#endif
                  << TYPE_DISPATCHER << TYPE_MOBILE;
        }
    }
    if (ok)
    {
        //id determined
        for (auto t : qAsConst(types))
        {
            if (sDataMap[t].mdl->hasId(id))
            {
                if (type >= TYPE_UNKNOWN)
                    type = t;
                if (update)
                    str = getDspTxt(id, t);
                return id;
            }
        }
    }
    else
    {
        //match name
        for (auto t : qAsConst(types))
        {
            for (const auto &it : sDataMap[t].idNames)
            {
                if (it.second.compare(str, Qt::CaseInsensitive) == 0)
                {
                    if (type >= TYPE_UNKNOWN)
                        type = t;
                    if (update)
                        str = getDspTxt(it.first, t);
                    return it.first;
                }
            }
        }
    }
    return 0;
}

bool ResourceData::validate(int type, IdsT &ids, QStringList &invIds)
{
    //if TYPE_SUBS_OR_MOBILE, need to check both subs and mobile
    auto *mdl = getModel((type == TYPE_SUBS_OR_MOBILE)? TYPE_SUBSCRIBER: type);
    auto *mobMdl = (type == TYPE_SUBS_OR_MOBILE)? getModel(TYPE_MOBILE): 0;
    if (mdl == 0 && mobMdl == 0)
    {
        //all invalid
        invIds = QString::fromStdString(Utils::toString(ids, ",")).split(",");
        ids.clear();
        return false;
    }
    invIds.clear();
    IdsT valIds;
    for (auto i : ids)
    {
        if ((mdl != 0 && mdl->hasId(i)) || (mobMdl != 0 && mobMdl->hasId(i)))
            valIds.insert(i);
        else
            invIds << QString::number(i);
    }
    if (!invIds.empty())
        ids = valIds;
    return !ids.empty();
}

bool ResourceData::addId(ListModel *mdl, int id, bool doSort)
{
    assert(mdl != 0);
    QStandardItem *itm;
    int type = mdl->type();
    if (type == TYPE_GROUP_OR_DGNA)
    {
        itm = getModel(TYPE_GROUP)->getItem(id);
        if (itm == 0)
        {
            itm = getModel(TYPE_DGNA_IND)->getItem(id);
#ifdef DGNA_OF_GROUPS
            if (itm == 0)
                itm = getModel(TYPE_DGNA_GRP)->getItem(id);
#endif
        }
    }
    else if (type == TYPE_SUBS_OR_MOBILE)
    {
        itm = getModel(TYPE_SUBSCRIBER)->getItem(id);
        if (itm == 0)
            itm = getModel(TYPE_MOBILE)->getItem(id);
    }
    else
    {
        itm = getModel(type)->getItem(id);
    }
    if (itm == 0)
        return false;
    return mdl->addId(id, itm->text(), doSort);
}

void ResourceData::addIds(ListModel *mdl, const IdsT &ids)
{
    assert(mdl != 0);
    for (auto i : ids)
    {
        addId(mdl, i, false);
    }
    mdl->sort(0);
}

QStandardItem *ResourceData::addItem(ListModel *mdl, int id)
{
    assert(mdl != 0);
    if (addId(mdl, id, true))
        return mdl->getItem(id);
    return 0;
}

ResourceData::ListModel *ResourceData::filter(int type, const QString &re)
{
    auto *mdl = createModel(type);
    auto *srcMdl = getModel(type);
    if (srcMdl != 0)
    {
        for (auto &p : srcMdl->findItems(re, Qt::MatchRegExp))
        {
            mdl->addItem(p);
        }
    }
    return mdl;
}

QStandardItem *ResourceData::getItem(const QListView *list, int id)
{
    assert(list != 0);
    return model(list)->getItem(id);
}

QTableWidgetItem *ResourceData::createTableItem(int            id,
                                                int            type,
                                                const QString &txt)
{
    QString s(txt);
    if (type >= TYPE_UNKNOWN)
    {
        id = matchName(true, s, type);
    }
    else if (s.isEmpty())
    {
        if (type == TYPE_DISPATCHER)
            s = getClientDspTxt(id);
        else
            s = getDspTxtAndType(id, type);
    }
    auto *itm = new QTableWidgetItem(s);
    itm->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    itm->setData(Qt::UserRole, id);
    itm->setData(Qt::UserRole + 1, type);
    return itm;
}

int ResourceData::getTableItemId(QTableWidgetItem *itm)
{
    assert(itm != 0);
    return itm->data(Qt::UserRole).toInt();
}

int ResourceData::getTableItemType(QTableWidgetItem *itm)
{
    assert(itm != 0);
    return itm->data(Qt::UserRole + 1).toInt();
}

void ResourceData::updateTableItem(QTableWidgetItem *itm, int type)
{
    assert(itm != 0);
    int itmType = getTableItemType(itm);
    if (itmType == type ||
        (type == TYPE_GROUP &&
         (itmType == TYPE_DGNA_IND
#ifdef DGNA_OF_GROUPS
          || itmType == TYPE_DGNA_GRP
#endif
          )))
        itm->setText(getDspTxt(getTableItemId(itm), itmType));
}

int ResourceData::getItemId(const QListView *list, int row)
{
    assert(list != 0);
    auto *itm = model(list)->item(row);
    if (itm != 0)
        return getItemId(itm);
    return 0;
}

string ResourceData::getMapSubsLbl(int issi)
{
    QString s;
    if (sMapSubsDspOpt != DSP_OPT_SSI)
    {
        s = getName(issi, TYPE_SUBSCRIBER);
        if (s.isEmpty())
            s = getName(issi, TYPE_MOBILE);
    }
    if (s.isEmpty())
        return Utils::toString(issi);
    switch (sMapSubsDspOpt)
    {
        case DSP_OPT_NAME_SSI:
            s.append(SEP_ISSI).append(QString::number(issi));
            break;
        case DSP_OPT_SSI_NAME:
            s.prepend(SEP_ISSI).prepend(QString::number(issi));
            break;
        default:
            break; //do nothing
    }
    return s.toStdString();
}

string ResourceData::toString(const IdsT &ids)
{
    return Utils::toString(ids, ",");
}

int ResourceData::setDspOpt(int val, int type)
{
    if (val < 0 || val > DSP_OPT_MAX)
        val = DSP_OPT_NAME_SSI;
    switch (type)
    {
        case TYPE_GROUP:
            sGrpDspOpt = val;
            refreshModel(type, true);
            refreshModel(TYPE_DGNA_IND, true);
#ifdef DGNA_OF_GRPS
            refreshModel(TYPE_DGNA_GRP, true);
#endif
            break;
        case TYPE_SUBSCRIBER:
            sSubsDspOpt = val;
            refreshModel(type, true);
            break;
        case TYPE_UNKNOWN:
            sMapSubsDspOpt = val;
            break;
        default:
            break; //do nothing
    }
    return val;
}

bool ResourceData::getId(const QString &txt, int &type, int &id)
{
    bool ok = false;
    int i = txt.toInt(&ok);
    if (!ok)
    {
        //txt not numeric - check for display text identifiers
        switch (type)
        {
            case TYPE_DGNA_GRP:
            case TYPE_DGNA_IND:
            case TYPE_GROUP:
            case TYPE_GROUP_OR_DGNA:
                if (txt.contains(SEP_GRP))
                {
                    i = txt.split(SEP_GRP).at(0).toInt(&ok);
                    if (!ok)
                        i = txt.split(SEP_GRP).at(1).toInt(&ok);
                }
                break;
            case TYPE_DISPATCHER:
                if (isClient(txt))
                    i = txt.split(PREFIX_DISPATCHER).at(1).toInt(&ok);
                break;
            case TYPE_MOBILE:
            case TYPE_SUBSCRIBER:
                if (txt.contains(SEP_ISSI))
                {
                    i = txt.split(SEP_ISSI).at(0).toInt(&ok);
                    if (!ok)
                        i = txt.split(SEP_ISSI).at(1).toInt(&ok);
                    if (sDataMap.count(TYPE_MOBILE) != 0 &&
                        sDataMap[TYPE_MOBILE].mdl->hasId(i))
                        type = TYPE_MOBILE;
                    else
                        type = TYPE_SUBSCRIBER;
                }
                break;
            default:
                //check all possible identifiers
                if (txt.contains(SEP_ISSI))
                {
                    i = txt.split(SEP_ISSI).at(0).toInt(&ok);
                    if (!ok)
                        i = txt.split(SEP_ISSI).at(1).toInt(&ok);
                    if (sDataMap.count(TYPE_MOBILE) != 0 &&
                        sDataMap[TYPE_MOBILE].mdl->hasId(i))
                        type = TYPE_MOBILE;
                    else
                        type = TYPE_SUBSCRIBER;
                }
                else if (txt.contains(SEP_GRP))
                {
                    type = TYPE_GROUP_OR_DGNA;
                    i = txt.split(SEP_GRP).at(0).toInt(&ok);
                    if (!ok)
                        i = txt.split(SEP_GRP).at(1).toInt(&ok);
                }
                else if (isClient(txt))
                {
                    type = TYPE_DISPATCHER;
                    i = txt.split(PREFIX_DISPATCHER).at(1).toInt(&ok);
                }
                break;
        }
    }
    if (ok)
        id = i;
    return ok;
}

QString ResourceData::getDspTxt(int type, int id, const QString &name)
{
    if (type == TYPE_DISPATCHER)
        return QString::number(id);
    bool isIssi = (type == TYPE_SUBSCRIBER || type == TYPE_MOBILE);
    int opt = (isIssi)? sSubsDspOpt: sGrpDspOpt;
    if (name.isEmpty() || opt == DSP_OPT_SSI || type == TYPE_UNKNOWN)
        return QString::number(id);
    QString dspId(name);
    switch (opt)
    {
        case DSP_OPT_NAME_SSI:
            dspId.append((isIssi)? SEP_ISSI: SEP_GRP)
                 .append(QString::number(id));
            break;
        case DSP_OPT_SSI_NAME:
            dspId.prepend((isIssi)? SEP_ISSI: SEP_GRP)
                 .prepend(QString::number(id));
            break;
        default:
            break; //do nothing
    }
    return dspId;
}

QString ResourceData::getDspTxtAndType(int id, int &type)
{
    if (type == TYPE_DISPATCHER)
        return getClientDspTxt(id);
    //get from master model
    QStandardItem *itm;
    if (type < TYPE_UNKNOWN)
    {
        auto *mdl = getModel(type);
        if (mdl != 0)
        {
            itm = mdl->getItem(id);
            if (itm != 0)
                return itm->text();
        }
    }
    for (const auto &it : sDataMap)
    {
        itm = it.second.mdl->getItem(id);
        if (itm != 0)
        {
            type = it.first;
            return itm->text();
        }
    }
    type = TYPE_UNKNOWN;
    return QString::number(id);
}

void ResourceData::loadModel(int type, const SubsData::Ssi2DescMapT &data)
{
    assert(sDataMap.count(type) != 0);
    auto *mdl = sDataMap[type].reset();
    auto &idNames(sDataMap[type].idNames);
    for (const auto &it : data)
    {
        if (it.second.empty())
            idNames[it.first] = "";
        else
            idNames[it.first] = QtUtils::fromHexUnicode(it.second).trimmed();
        mdl->addIdWithName(it.first, idNames[it.first]);
    }
    mdl->sort(0);
    refreshModel(type, false);
}

void ResourceData::loadModel(int type, const QString &data)
{
    bool flag = (type == TYPE_SUBSCRIBER || type == TYPE_MOBILE ||
                 type == TYPE_MOBILE_ONLINE);
    assert(flag || type == TYPE_DISPATCHER);
    auto *mdl = sDataMap[type].reset();
    auto &idNames(sDataMap[type].idNames);
    string name;
    int id;
    bool ok;
    for (auto &s : data.split(','))
    {
        id = s.toInt(&ok);
        if (!ok)
            continue;
        if (flag)
        {
            name = SubsData::getIssiName(id);
            if (name.empty())
                idNames[id] = "";
            else
                idNames[id] = QtUtils::fromHexUnicode(name).trimmed();
        }
        else
        {
            idNames[id] = "";
        }
        mdl->addIdWithName(id, idNames[id]);
    }
    mdl->sort(0);
    refreshModel(type, false);
}

void ResourceData::refreshModel(int type, bool doMaster)
{
    auto *masterMdl = getModel(type);
    if (masterMdl == 0)
        return; //should not happen
    if (doMaster)
    {
        QStandardItem *itm;
        int id;
        auto &idNames(sDataMap[type].idNames);
        for (auto r=masterMdl->rowCount()-1; r>=0; --r)
        {
            itm = masterMdl->item(r);
            id = getItemId(itm);
            itm->setText(getDspTxt(type, id, idNames[id]));
        }
        masterMdl->sort(0);
    }
    set<int> types;
    types.insert(type);
    switch (type)
    {
        case TYPE_DGNA_GRP:
        case TYPE_DGNA_IND:
        case TYPE_GROUP:
            types.insert(TYPE_GROUP_OR_DGNA);
            break;
        default:
            break; //do nothing
    }
    for (auto &mdl : sModels)
    {
        if (types.count(mdl->type()) != 0)
            mdl->refresh(masterMdl);
    }
}
