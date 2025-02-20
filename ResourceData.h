/**
 * Resource data manager.
 *  -Stores master models and resource names for every resource type.
 *  -Controls the resource display text formats.
 *  -Controls the terminal label format on map.
 * All modules that display interactive resources, i.e. those that may be used
 * to perform operations, must use this module so that a resource's ID can
 * always be retrieved regardless of its display text.
 * In particular, a list view's model must be created using
 * ResourceData::createModel().
 *
 * Primary data type:
 *  - Custom ListModel for all data models, used in a QListView.
 *    Every model item shows the display text and stores the resource ID.
 *    Each model has a type from eType.
 *  - A QTableWidgetItem that holds a resource also stores the ID and type in
 *    its user data.
 * The resource ID is set and read through accessors.
 * Utility functions defined here for data handling convenience.
 *
 * Copyright (C) Sapura Secured Technologies, 2020-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: ResourceData.h 1884 2024-11-25 09:10:00Z hazim.rujhan $
 * @author Rosnin Mustaffa
 * @author Mohd Rozaimi
 */
#ifndef RESOURCEDATA_H
#define RESOURCEDATA_H

#include <map>
#include <set>
#include <assert.h>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QStringList>
#include <QTableWidgetItem>

#include "CmnTypes.h"
#include "Logger.h"
#include "SubsData.h"

class ResourceData
{
public:
    typedef std::set<int> IdsT; //resource IDs - ISSI/GSSI/Dispatcher ID

    //data model definition
    class ListModel : public QStandardItemModel
    {
    public:
        /**
         * Constructor.
         * Do not instantiate a non-master model directly - use createModel()
         * instead.
         *
         * @param[in] type   The resource type - eType.
         * @param[in] parent The parent object, if any.
         */
        ListModel(int type, QObject *parent = 0):
            QStandardItemModel(parent), mType(type) {}

        virtual ~ListModel() { sModels.erase(this); }

        void setType(int type) { mType = type; }

        int type() { return mType; }

        bool empty() { return (rowCount() == 0); }

        /**
         * Adds an ID with a name.
         * This is used to fill up a master model.
         *
         * @param[in] id     The ID.
         * @param[in] name   The name.
         * @param[in] doSort true to sort after adding.
         */
        void addIdWithName(int id, const QString &name, bool doSort = false)
        {
            //caller must ensure id not yet in the model
            auto *itm = new QStandardItem(getDspTxt(mType, id, name));
            setItemId(itm, id);
            appendRow(itm);
            if (doSort)
                sort(0);
        }

        /**
         * Adds an ID with a given display text.
         * This is used to copy a resource from a master model.
         *
         * @param[in] id     The ID.
         * @param[in] txt    The display text.
         * @param[in] doSort true to sort after adding.
         * @return true if ID added or display text modified for an existing ID.
         */
        bool addId(int id, const QString &txt, bool doSort = false)
        {
            auto *itm = getItem(id);
            if (itm == 0)
            {
                itm = new QStandardItem(txt);
                setItemId(itm, id);
                appendRow(itm);
            }
            else if (itm->text() == txt)
            {
                return false;
            }
            else
            {
                itm->setText(txt);
            }
            if (doSort)
                sort(0);
            return true;
        }

        /**
         * Removes an ID.
         *
         * @param[in] id The ID.
         * @return true if ID found and removed.
         */
        bool removeId(int id)
        {
            int row;
            if (getItem(id, &row) == 0)
                return false;
            removeRow(row);
            return true;
        }

        /**
         * Removes IDs.
         *
         * @param[in] ids The IDs.
         * @return true if at least one ID found and removed.
         */
        bool removeIds(const IdsT &ids)
        {
            bool found = false;
            int row;
            for (auto id : ids)
            {
                if (getItem(id, &row) != 0)
                {
                    removeRow(row);
                    found = true;
                }
            }
            return found;
        }

        /**
         * Checks whether an ID exists.
         *
         * @param[in] id The ID.
         * @return true if exists.
         */
        bool hasId(int id) const
        {
            return (getItem(id) != 0);
        }

        /**
         * Gets all IDs.
         *
         * @param[out] ids The IDs.
         * @return true if at least one ID present.
         */
        bool getIds(IdsT &ids) const
        {
            for (auto r=rowCount()-1; r>=0; --r)
            {
                ids.insert(getItemId(item(r)));
            }
            return !ids.empty();
        }

        /**
         * Gets all IDs.
         *
         * @param[in] checked true to select only IDs in checked state.
         * @return The ID list.
         */
        QStringList getIds(bool checked) const
        {
            QStringList l;
            QStandardItem *itm;
            for (auto r=rowCount()-1; r>=0; --r)
            {
                itm = item(r, 0);
                if (!checked || itm->checkState() == Qt::Checked)
                    l << QString::number(getItemId(itm));
            }
            return l;
        }

        /**
         * Adds a resource by copying data from a source item.
         *
         * @param[in] srcItm The source item.
         */
        void addItem(const QStandardItem *srcItm)
        {
            auto *itm = new QStandardItem(srcItm->text());
            setItemId(itm, getItemId(srcItm));
            appendRow(itm);
        }

        /**
         * Gets a resource item with an ID.
         *
         * @param[in]  id  The ID.
         * @param[out] row The model row where the ID is.
         * @return The resource item, or 0 if not found.
         */
        QStandardItem *getItem(int id, int *row = 0) const
        {
            QStandardItem *itm;
            for (auto r=rowCount()-1; r>=0; --r)
            {
                itm = item(r);
                if (getItemId(itm) == id)
                {
                    if (row != 0)
                        *row = r;
                    return itm;
                }
            }
            return 0;
        }

        /**
         * Gets the resource text for an ID.
         *
         * @param[in]  id  The ID.
         * @return The text - empty if not found.
         */
        QString getItemText(int id)
        {
            auto *itm = getItem(id);
            if (itm == 0)
                return "";
            return itm->text();
        }

        /**
         * Refreshes all display texts based on a source model.
         *
         * @param[in] srcMdl The source model.
         */
        void refresh(const ListModel *srcMdl)
        {
            assert(srcMdl != 0);
            QStandardItem *itm;
            QStandardItem *srcItm;
            for (auto r=rowCount()-1; r>=0; --r)
            {
                itm = item(r);
                srcItm = srcMdl->getItem(getItemId(itm));
                if (srcItm != 0)
                    itm->setText(srcItm->text());
                else if (mType != TYPE_GROUP_OR_DGNA)
                    removeRow(r); //id no longer exists
            }
            sort(0);
        }

    private:
        int mType; //eType
    };

    enum eType
    {
        TYPE_DGNA_GRP      = CmnTypes::IDTYPE_DGNA_GRP,
        TYPE_DGNA_IND      = CmnTypes::IDTYPE_DGNA_IND,
        TYPE_DISPATCHER    = CmnTypes::IDTYPE_DISPATCHER,
        TYPE_GROUP         = CmnTypes::IDTYPE_GROUP,
        TYPE_MOBILE        = CmnTypes::IDTYPE_MOBILE,
        TYPE_MOBILE_ONLINE = CmnTypes::IDTYPE_MOBILE_ONLINE,
        TYPE_SUBSCRIBER    = CmnTypes::IDTYPE_SUBSCRIBER,
        TYPE_UNKNOWN       = CmnTypes::IDTYPE_UNKNOWN,
        //values below are combination types - MUST be above TYPE_UNKNOWN
        TYPE_GROUP_OR_DGNA, //possible members of DGNA_GRP
        TYPE_SUBS_OR_MOBILE //monitoring, Incident rsc and GIS tracking/trailing
    };

    //options for resource display text and subscriber labels on map
    enum eDspOpt
    {
        DSP_OPT_SSI,
        DSP_OPT_NAME,
        DSP_OPT_NAME_SSI,
        DSP_OPT_SSI_NAME,
        DSP_OPT_MAX = DSP_OPT_SSI_NAME
    };

    /**
     * Initializes the module.
     *
     * @param[in] logger The app logger.
     */
    static void init(Logger *logger);

    static void setFullMode(bool full) { sFull = full; }

    static bool isFullMode() { return sFull; }

    /**
     * Sets whether or not mobile online status will be available.
     *
     * @param[in] enable true if available.
     */
    static void setMobileStat(bool enable);

    static bool hasMobileStat() { return sMobileStat; }

    static int onlineMobileType() { return (sMobileStat)? TYPE_MOBILE_ONLINE:
                                                          TYPE_MOBILE; }

    static int chkType(int type, int id)
    {
        return (type == ResourceData::TYPE_SUBS_OR_MOBILE)?
                ResourceData::getType(id): type;
    }

    /**
     * Gets the label text for a type.
     *
     * @param[in] type eType.
     * @return The label text, or empty string for TYPE_UNKNOWN and any other
     *         unknown values.
     */
    static QString getTypeTxt(int type);

    /**
     * Either clears or deletes master models.
     *
     * @param[in] doDelete true to delete - on application exit only.
     */
    static void cleanup(bool doDelete = false);

    /**
     * Gets labels for the display text options, i.e eDspOpt values.
     *
     * @param[in] type eType - TYPE_SUBSCRIBER or TYPE_GROUP.
     * @return The label list.
     */
    static QStringList dspOptLabels(int type);

    /**
     * Reloads data from SubsData.
     *
     * @param[in] type The resource type - eType. Omit to load all types.
     */
    static void reload(int type = -1);

    /**
     * Adds or removes a resource to/from the relevant existing model.
     *
     * @param[in] doAdd true to add, false to remove.
     * @param[in] type  The resource type - eType.
     * @param[in] id    The ID.
     * @return true if resource added/removed.
     */
    static bool addRemoveId(bool doAdd, int type, int id);

    /**
     * Gets the data model for a resource type.
     *
     * @param[in] type The resource type - eType.
     * @return The data model, or 0 if not found.
     */
    static ListModel *getModel(int type)
    {
        return (sDataMap.count(type) == 0)? 0: sDataMap[type].mdl;
    }

    /**
     * Checks whether a resource type has data.
     *
     * @param[in] type The resource type - eType.
     * @return true if it has data.
     */
    static bool hasData(int type)
    {
        return (sDataMap.count(type) != 0 &&
                sDataMap[type].mdl->rowCount() != 0);
    }

    /**
     * Checks whether a resource type contains an ID.
     *
     * @param[in] type The resource type - eType.
     * @param[in] id   The ID.
     * @return true if ID exists for the type.
     */
    static bool hasId(int type, int id)
    {
        if (type == TYPE_SUBS_OR_MOBILE)
            return (hasId(TYPE_SUBSCRIBER, id) || hasId(TYPE_MOBILE, id));
        return (sDataMap.count(type) != 0 &&
                sDataMap[type].idNames.count(id) != 0);
    }

    /**
     * Checks whether a resource type is for a group.
     *
     * @param[in] type The resource type - eType.
     * @return true for a group.
     */
    static bool isGrp(int type)
    {
        switch (type)
        {
            case TYPE_DGNA_GRP:
            case TYPE_DGNA_IND:
            case TYPE_GROUP:
                return true;
            default:
                break; //do nothing
        }
        return false;
    }

    /**
     * Gets the resource name for an ID.
     *
     * @param[in]     id   The ID.
     * @param[in,out] type The resource type - eType.
     *                     If IDTYPE_UNKNOWN, corrected if resource found.
     * @param[out]    name The name, set only if resource found.
     * @return bool if resource found.
     */
    static bool getName(int id, int &type, QString &name);

    /**
     * Gets the resource name for an ID using getName(int, int, QString).
     *
     * @param[in] id        The ID.
     * @param[in] type      The resource type - eType.
     * @param[in] idIfEmpty true to return the ID if name is empty.
     * @return The name. Empty string if ID not found or has no name, unless
     *         idIfEmpty is true.
     */
    static QString getName(int id, int type, bool idIfEmpty = false);

    /**
     * Gets a resource display text using getDspTxtAndType().
     *
     * @param[in] id   The ID.
     * @param[in] type The resource type - eType. Omit if unknown.
     * @return The resource string.
     */
    static QString getDspTxt(int id, int type = TYPE_UNKNOWN);

    /**
     * Checks whether a resource display text is for a dispatcher.
     *
     * @param[in] str The display text.
     * @return true for a dispatcher.
     */
    static bool isClient(const QString &str);

    /**
     * Gets a client display text.
     *
     * @param[in] id The ID.
     * @return The display text.
     */
    static QString getClientDspTxt(int id);

    /**
     * Gets a client display text.
     *
     * @param[in,out] str The ID on input. The display text on return.
     */
    static void getClientDspTxt(QString &str);

    /**
     * Gets the type of a resource ID.
     *
     * @param[in] id The ID.
     * @return eType.
     */
    static int getType(int id);

    /**
     * Gets the type of a resource string, mainly to differentiate between
     * individual and group resources.
     * Returns TYPE_GROUP for static and DGNA groups.
     *
     * @param[in] str The resource string.
     * @return eType.
     */
    static int getType(const QString &str);

    /**
     * Gets a resource ID from its display text. The determined ID need not be
     * present in the master data.
     *
     * @param[in] txt The display text.
     * @return The ID or 0 if not found.
     */
    static int getId(const QString &txt);

    /**
     * Matches a resource string against all data to find the ID.
     * The type is either specified to limit the search, or passed as
     * TYPE_UNKNOWN to determine from the data.
     *
     * @param[in]     update true to replace the given string with the correct
     *                       one.
     * @param[in]     str    The string to match.
     * @param[in,out] type   The resource type - eType. Updated on return if
     *                       passed UNKNOWN and match found.
     * @return The ID or 0 if not found.
     */
    static int matchName(bool update, QString &str, int &type);

    /**
     * Checks a list of IDs and removes those that are no longer valid.
     *
     * @param[in]     type   The resource type - eType.
     * @param[in,out] ids    The IDs, filtered on return.
     * @param[out]    invIds The invalid (filtered out) IDs.
     * @return true if the filtered ID list is not empty.
     */
    static bool validate(int type, IdsT &ids, QStringList &invIds);

    /**
     * Creates a data model for a resource type.
     *
     * @param[in] type   The resource type - eType.
     * @param[in] parent The owner object, if any.
     * @return The data model. Caller takes ownership.
     */
    static ListModel *createModel(int type = TYPE_UNKNOWN, QObject *parent = 0)
    {
        auto *mdl = new ListModel(type, parent);
        sModels.insert(mdl);
        return mdl;
    }

    /**
     * Adds a resource to a model.
     *
     * @param[in] mdl    The model.
     * @param[in] id     The ID.
     * @param[in] doSort true to sort the model after adding.
     * @return true if ID added or existing entry modified.
     */
    static bool addId(ListModel *mdl, int id, bool doSort = false);

    /**
     * Adds resources to a model and sorts the model.
     *
     * @param[in] mdl The model.
     * @param[in] ids The IDs.
     */
    static void addIds(ListModel *mdl, const IdsT &ids);

    /**
     * Creates and adds a model resource with an ID.
     *
     * @param[in,out] mdl The model.
     * @param[in]     id  The ID.
     * @return The created item.
     */
    static QStandardItem *addItem(ListModel *mdl, int id);

    /**
     * Gets the resource ID from a model row.
     *
     * @param[in] mdl The model.
     * @param[in] row The row.
     * @return The ID or 0 if not found.
     */
    static int getItemId(const ListModel *mdl, int row)
    {
        assert(mdl != 0);
        return getItemId(mdl->item(row));
    }

    /**
     * Filters a resource model.
     *
     * @param[in] type The resource type - eType.
     * @param[in] re   The filter regular expression.
     * @return The filtered model, which may be empty but never null. Caller
     *         takes ownership.
     */
    static ListModel *filter(int type, const QString &re);

    /**
     * Gets the model of a list. Convenience function to avoid static_cast
     * everywhere.
     *
     * @param[in] list The list.
     * @return The model.
     */
    static ListModel *model(const QListView *list)
    {
        assert(list != 0);
        return static_cast<ListModel *>(list->model());
    }

    /**
     * Checks whether a list is empty.
     *
     * @param[in] list The list.
     * @return true if empty.
     */
    static bool isEmpty(const QListView *list)
    {
        assert(list != 0);
        auto *mdl = model(list);
        return (mdl == 0 || mdl->rowCount() == 0);
    }

    /**
     * Gets the resource ID from a list row.
     *
     * @param[in] list The list.
     * @param[in] row  The row.
     * @return The ID, or 0 if not found.
     */
    static int getItemId(const QListView *list, int row);

    /**
     * Gets the list item with a given ID.
     *
     * @param[in] list The list.
     * @param[in] id   The ID.
     * @return The item, or 0 if not found.
     */
    static QStandardItem *getItem(const QListView *list, int id);

    /**
     * Sets the ID of a list item.
     *
     * @param[in] itm The item.
     * @param[in] id  The ID.
     */
    static void setItemId(QStandardItem *itm, int id)
    {
        assert(itm != 0);
        itm->setData(id);
    }

    /**
     * Gets the ID of a list item.
     *
     * @param[in] itm The item.
     * @return The ID, or 0 if not set.
     */
    static int getItemId(const QStandardItem *itm)
    {
        assert(itm != 0);
        return itm->data().toInt();
    }

    /**
     * Creates a table item for a resource, with the ID and type in the item
     * user data.
     * If type is TYPE_UNKNOWN and text is non-empty, determines the ID and type
     * from the text.
     *
     * @param[in] id   The ID.
     * @param[in] type The resource type - eType.
     * @param[in] txt  The item text, if any.
     *                 Otherwise generated using id and type.
     * @return The ID, or 0 if not set.
     */
    static QTableWidgetItem *createTableItem(int            id,
                                             int            type,
                                             const QString &txt = "");

    /**
     * Gets the resource ID in a table item which was created by
     * createTableItem().
     *
     * @param[in] itm The table item.
     * @return The ID.
     */
    static int getTableItemId(QTableWidgetItem *itm);

    /**
     * Gets the resource type in a table item which was created by
     * createTableItem().
     *
     * @param[in] itm The table item.
     * @return The type.
     */
    static int getTableItemType(QTableWidgetItem *itm);

    /**
     * Refreshes a table item text for a given resource type.
     *
     * @param[in] itm  The table item.
     * @param[in] type TYPE_SUBSCRIBER or TYPE_GROUP (also for DGNA items).
     */
    static void updateTableItem(QTableWidgetItem *itm, int type);

    /**
     * Sets the group display text option.
     *
     * @param[in] val Option value from eDspOpt.
     * @return See setDspOpt().
     */
    static int setGrpDspOpt(int val)
    {
        return setDspOpt(val, TYPE_GROUP);
    }

    /**
     * Sets the subscriber display text option.
     *
     * @param[in] val Option value from eDspOpt.
     * @return See setDspOpt().
     */
    static int setSubsDspOpt(int val)
    {
        return setDspOpt(val, TYPE_SUBSCRIBER);
    }

    /**
     * Sets the subscriber map label option.
     *
     * @param[in] val Option value from eDspOpt.
     * @return See setDspOpt().
     */
    static int setMapSubsDspOpt(int val)
    {
        return setDspOpt(val, TYPE_UNKNOWN);
    }

    static int getMapSubsDspOpt() { return sMapSubsDspOpt; }

    /**
     * Gets the subscriber map label. This depends on sMapSubsDspOpt.
     *
     * @param[in] issi The ISSI.
     * @return The label text.
     */
    static std::string getMapSubsLbl(int issi);

    /**
     * Converts an ID list to string.
     *
     * @param[in] ids The IDs.
     * @return The comma-separated values.
     */
    static std::string toString(const IdsT &ids);

private:
    typedef std::map<int, QString> IdNameMapT;
    struct TypeData
    {
        ListModel  *mdl = 0; //master model
        IdNameMapT  idNames; //name of each ID

        //clear data and return the model
        ListModel *reset()
        {
            idNames.clear();
            mdl->clear();
            return mdl;
        }
    };
    typedef std::map<int, TypeData> TypeMapT;

    static Logger   *sLogger;
    //true normally, false in STM-nwk mode (fewer resource types)
    static bool      sFull;
    static bool      sMobileStat;    //true if mobile online status is available
    static int       sGrpDspOpt;     //eDspOpt
    static int       sSubsDspOpt;    //eDspOpt
    static int       sMapSubsDspOpt; //eDspOpt
    static TypeMapT  sDataMap;       //data for each eType
    //models to refresh upon display text option change - owned by others
    static std::set<ListModel *> sModels;

    /**
     * Sets the resource display text option.
     *
     * @param[in] val  Option value from eDspOpt.
     * @param[in] type TYPE_SUBSCRIBER/GROUP/UNKNOWN.
     * @return The actual stored option, in case the input is invalid.
     */
    static int setDspOpt(int val, int type);

    /**
     * Gets a resource ID from a display text. The determined ID need not be
     * present in the master data.
     *
     * @param[in]     txt  The display text.
     * @param[in,out] type The resource type - eType. If TYPE_UNKNOWN, corrected
     *                     on return if recognized. A group can only be set as
     *                     TYPE_GROUP_OR_DGNA.
     * @param[out]    id   The ID. Modified only if successful.
     * @return true if ID determined.
     */
    static bool getId(const QString &txt, int &type, int &id);

    /**
     * Gets a resource display text from its type, ID and name.
     * The display text is the ID itself in these cases:
     *  -Type is TYPE_DISPATCHER.
     *  -Name is empty.
     * Otherwise it depends on the type's display text option.
     *
     * @param[in] type The resource type - eType.
     * @param[in] id   The ID.
     * @param[in] name The name - may be empty.
     * @return The display text.
     */
    static QString getDspTxt(int type, int id, const QString &name);

    /**
     * Gets a resource display text and possibly its correct type, from the
     * master model.
     * If the resource has no name, the display text is the ID itself.
     *
     * @param[in]     id   The ID.
     * @param[in,out] type The resource type - eType.
     *                     On return, corrected if ID is found to be of another
     *                     type, or TYPE_UNKNOWN if ID not found.
     * @return The display text.
     */
    static QString getDspTxtAndType(int id, int &type);

    /**
     * Loads resource data into a model.
     *
     * @param[in] type The resource type - eType.
     * @param[in] data The SSI descriptions.
     */
    static void loadModel(int type, const SubsData::Ssi2DescMapT &data);

    /**
     * Loads subscriber or dispatcher data into a model.
     *
     * @param[in] type TYPE_SUBSCRIBER/DISPATCHER.
     * @param[in] data The comma-separated ID list.
     */
    static void loadModel(int type, const QString &data);

    /**
     * Refreshes the display texts in a model.
     *
     * @param[in] type     The resource type - eType.
     * @param[in] doMaster true to refresh the master model.
     */
    static void refreshModel(int type, bool doMaster);
};
#endif //RESOURCEDATA_H
