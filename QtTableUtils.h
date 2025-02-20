/**
 * Collection of Qt utility functions to handle data in tables and database.
 *
 * Copyright (C) Sapura Secured Technologies, 2019-2023. All Rights Reserved.
 *
 * @file
 * @version $Id: QtTableUtils.h 1799 2023-12-22 09:21:23Z hazim.rujhan $
 * @author Zulzaidi Atan
 */
#ifndef QTTABLEUTILS_H
#define QTTABLEUTILS_H

#include <set>
#include <QCheckBox>
#include <QIcon>
#include <QMenu>
#include <QPoint>
#include <QPushButton>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTableWidget>

#include "DbInt.h"

namespace QtTableUtils
{
    typedef std::set<int> IntSetT;

    //table types
    enum eTblType
    {
        TBLTYPE_CALL,
        TBLTYPE_INCIDENT,
        TBLTYPE_LOC,
        TBLTYPE_MMS,
        TBLTYPE_MSG,        //SDS, Status, MMS
        TBLTYPE_PTT,
        TBLTYPE_SDS,
        TBLTYPE_STS
    };

    //table column IDs for SDS and Status Message
    enum eCol
    {
        COL_TYPE,
        COL_TIME,
        COL_FROM,
        COL_TO,
        COL_MSG,
        COL_COUNT
    };

    //table column IDs for Call
    enum eColCall
    {
        COL_CALL_TIME,
        COL_CALL_DURATION,
        COL_CALL_FROM,
        COL_CALL_TO,
        COL_CALL_TYPE,
        //the next 3 are for audio playback only
        COL_CALL_KEY,
        COL_CALL_AUDIO_PATH,
        COL_CALL_GSSI,
        COL_CALL_COUNT
    };

    //table column IDs for Incident
    enum eColInc
    {
        COL_INC_ID,
        COL_INC_STATE,
        COL_INC_PRIORITY,
        COL_INC_CATEGORY,
        COL_INC_DESCRIPTION,
        COL_INC_STATUS,
        COL_INC_RESOURCES,
        COL_INC_COUNT
    };

    //table column IDs for Location
    enum eColLoc
    {
        COL_LOC_TYPE,
        COL_LOC_ISSI,
        COL_LOC_TIME,
        COL_LOC_LOC
    };

    //table column IDs for PTT
    enum eColPtt
    {
        COL_PTT_TIME,
        COL_PTT_SECONDS,
        COL_PTT_TX
    };

    //call audio data
    struct AudioData
    {
        QString path;
        QString callKey;
        QString from;
        QString to;
        QString callType;
        QString time;
        QString gssi;
    };

    //query results with more than 50000 entries take too long (>1 minute) to
    //display, causing perceived UI freeze - cap it
    static const int MAX_RES = 50000;

    //call types used in database
    extern const std::string CALLTYPE_AMBIENCE;
    extern const std::string CALLTYPE_BROADCAST;
    extern const std::string CALLTYPE_GROUP;
    extern const std::string CALLTYPE_INCOMING;
    extern const std::string CALLTYPE_MOBILE;
    extern const std::string CALLTYPE_OUTGOING;

    /**
     * Displays the context menu for a table widget.
     *
     * @param[in]     tw             The table widget.
     * @param[in]     pos            The mouse position.
     * @param[in]     unsaved        true if selected rows contain unsaved
     *                               message attachment.
     * @param[in,out] numHighlighted The number of highlighted rows, which
     *                               may be modified on return.
     * @return true if numHighlighted is modified.
     */
    bool tableContextMenu(QTableWidget *tw,
                          const QPoint &pos,
                          bool          unsaved = false,
                          int          *numHighlighted = 0);

    /**
     * Enables or disables highlighting of a table row.
     * Highlighting means setting bold and italic font.
     *
     * @param[in] tw     The table widget.
     * @param[in] row    The 0-based row number.
     * @param[in] enable true to enable.
     * @return 1 if the row highlighting has been changed, or 0 otherwise.
     */
    int tableHighlight(QTableWidget *tw, int row, bool enable);

    /**
     * Disables highlighting of one table row or all rows.
     *
     * @param[in] tw  The table widget.
     * @param[in] row The 0-based row number, or -1 for all rows.
     * @return The number of changed rows.
     */
    int tableNormal(QTableWidget *tw, int row = -1);

    /**
     * Removes selected rows from a table widget.
     *
     * @param[in] tw      The table widget.
     * @param[in] unsaved true if the rows contain unsaved message attachment.
     * @return The number of highlighted rows removed.
     */
    int tableRemoveRows(QTableWidget *tw, bool unsaved = false);

    /**
     * Adds a call/message direction filter into a button menu.
     * Adds independently selectable checkboxes for all possible directions
     * using addFilterCheckBox().
     *
     * @param[in]     tw        The table to filter.
     * @param[in]     btn       The button containing the menu.
     * @param[in]     dirCol    The table column containing the direction.
     * @param[in,out] hideDirs  Hidden direction types based on menu selections.
     * @param[in,out] menu      The menu.
     * @param[in]     hasMissed true to include 'Missed' item.
     */
    void addDirFilter(QTableWidget *tw,
                      QPushButton  *btn,
                      int           dirCol,
                      IntSetT      *hideDirs,
                      QMenu        *menu,
                      bool          hasMissed = false);

    /**
     * Creates and adds a checkbox into a filter menu.
     * Sets the checkbox click handler to:
     * -update rows affected by the option with updateFilteredRow(),
     * -add unchecked option into the given value set, and remove checked one,
     * -call updateFilterButtonText().
     *
     * @param[in]     tw      The table to filter. The last column must be
     *                        reserved for filter data.
     * @param[in]     btn     The button containing the menu.
     * @param[in]     col     The table column containing the relevant value.
     * @param[in]     value   The checkbox stored value.
     * @param[in]     text    The checkbox text.
     * @param[in]     icon    The checkbox icon.
     * @param[in,out] hideSet Hidden row values based on menu selections.
     * @param[in,out] menu    The menu.
     * @return The checkbox.
     */
    QCheckBox *addFilterCheckBox(QTableWidget  *tw,
                                 QPushButton   *btn,
                                 int            col,
                                 int            value,
                                 const QString &text,
                                 const QIcon   &icon,
                                 IntSetT       *hideSet,
                                 QMenu         *menu);

    /**
     * Adds/removes a column index to/from the list of columns that cause a
     * table row to be hidden. The list is stored in the last column.
     * If adding, hides the row. Otherwise if the list is now empty, shows the
     * row.
     * Optionally updates a button text with updateFilterButtonText() when
     * adding an index.
     *
     * @param[in] tw    The table.
     * @param[in] row   Row index.
     * @param[in] col   Column index.
     * @param[in] doAdd true to add.
     * @param[in] btn   Button to update, if any.
     * @return true if hidden row is now shown for first time.
     */
    bool updateFilteredRow(QTableWidget *tw,
                           int           row,
                           int           col,
                           bool          doAdd,
                           QPushButton  *btn = 0);

    /**
     * Updates a filter button text depending on whether:
     *  -filter is active (has unchecked options):
     *    -appends a '*' to the text,
     *  -table has hidden row:
     *    -appends another '*' to the text.
     * If filter active state is unknown by caller, determines it from the
     * current button text.
     *
     * @param[in] btn         The button.
     * @param[in] hidden      true if table has hidden row.
     * @param[in] activeKnown true if caller knows the active state.
     * @param[in] active      true if filter is active.
     */
    void updateFilterButtonText(QPushButton *btn,
                                bool         hidden,
                                bool         activeKnown = false,
                                bool         active      = true);

    /**
     * Deletes a table's model.
     * If a parent widget is given, hides it. Otherwise hides the table.
     *
     * @param[out] tv     The table.
     * @param[out] parent The parent widget, if any.
     */
    void clearTable(QTableView *tv, QWidget *parent = 0);

    /**
     * Gets a set of table column headers.
     *
     * @param[in] type     The table type. See eTblType.
     * @param[in] showType true to show the table type in a certain column
     *                     header. Mainly for Incident module.
     * @return The headers. Empty for unknown type.
     */
    QStringList getHeaders(int type, bool showType = false);

    /**
     * Converts a call type from database into a string for display, which is
     * subject to translation.
     *
     * @param[in] type   The call type from database.
     * @param[in] duplex The simplex/duplex indicator from database.
     *                   "0" for simplex, other values ignored.
     * @return The possibly translated type. Empty for unknown type.
     */
    QString getCallType(const std::string &type,
                        const std::string &duplex = "");

    /**
     * Gets call audio data from a table row.
     *
     * @param[in]  tv   The table.
     * @param[in]  row  The row.
     * @param[out] data The data container.
     * @return true if successful.
     */
    bool getCallAudioData(QTableView *tv, int row, AudioData &data);

    /**
     * Finds a string exact match in a table column.
     *
     * @param[in] tv  The table.
     * @param[in] col The table column.
     * @param[in] str The string.
     * @return true if found.
     */
    bool find(QTableView *tv, int col, const QString &str);

    /**
     * Gets call data in a specific period from database into a Report table.
     *
     * @param[in]  from      The calling party. 0 for any.
     * @param[in]  to        The called party. 0 for any.
     * @param[in]  doAnd     true to AND 'from' and 'to', false to OR.
     * @param[in]  startTime The start time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]  endTime   The end time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]  type      The call type, or empty string for all types.
     * @param[out] tv        The table.
     * @return true if successful (but tv may be empty). false indicates DB
     *         query failure.
     */
    bool getCallData(int                from,
                     int                to,
                     bool               doAnd,
                     const std::string &startTime,
                     const std::string &endTime,
                     const std::string &type,
                     QTableView        *tv);

    /**
     * Gets SDS or Status Message data involving specific resources in a
     * specific period from database into a Report table.
     *
     * @param[in]  from      The sender. 0 for any.
     * @param[in]  to        The recipient. 0 for any.
     * @param[in]  msgType   The message type - CmnType::eCommsType.
     *                       Used only for tblType == TBLTYPE_MSG.
     * @param[in]  doAnd     true to AND 'from' and 'to', false to OR.
     * @param[in]  startTime The start time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]  endTime   The end time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]  input     Either the search substring for SDS, or the
     *                       Status Message code.
     * @param[out] tv        The table.
     * @return true if successful (but tv may be empty). false indicates DB
     *         query failure.
     */
    bool getMsgData(int                from,
                    int                to,
                    int                msgType,
                    bool               doAnd,
                    const std::string &startTime,
                    const std::string &endTime,
                    const std::string &input,
                    QTableView        *tv);

    /**
     * Gets location data for specific resources or all resources of the given
     * types in a specific period from database into a Report table.
     *
     * @param[in]  issis     The space-separated ISSIs. Empty string to get by
     *                       type.
     * @param[in]  types     The space-separated type values.
     *                       See SubsData::eTerminalType. Used only if issis is
     *                       empty.
     * @param[in]  startTime The start time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]  endTime   The end time as "dd/MM/YYYY hh:mm:ss".
     * @param[out] tv        The table.
     * @return 0 if successful (but tv may be empty).
     *         The positive number of search results if exceeding MAX_RES.
     *         -1 for DB query failure.
     */
    int getLocData(const std::string &issis,
                   const std::string &types,
                   const std::string &startTime,
                   const std::string &endTime,
                   QTableView        *tv);

    /**
     * Sets up a table:
     *  -data model,
     *  -hidden columns where applicable,
     *  -model headers,
     *  -sorting,
     *  -tooltip on COL_CALL_DURATION for TBLTYPE_CALL only.
     *
     * @param[in]     type     The table type. See eTblType.
     * @param[in,out] mdl      The model.
     * @param[out]    tv       The table.
     * @param[in]     showType See getHeaders().
     */
    void setupTable(int                 type,
                    QStandardItemModel *mdl,
                    QTableView         *tv,
                    bool                showType = false);

    //functions here onwards are used only locally
    /**
     * Fills up a table with call data from database query.
     *
     * @param[in]  res           The query result. Deleted on return.
     * @param[in]  allToGssi     true if records are for group calls.
     * @param[in]  doCheckBranch See fillCallRow().
     * @param[out] mdl           The table data model.
     */
    void fillCallData(DbInt::QResult     *res,
                      bool                allToGssi,
                      bool                doCheckBranch,
                      QStandardItemModel *mdl);

    /**
     * Fills up a table with messaging data from database query - SDS or
     * Status Message.
     *
     * @param[in]  res           The query result. Deleted on return.
     * @param[in]  msgType       See fillMsgRow().
     * @param[in]  doCheckBranch See fillMsgRow().
     * @param[out] mdl           The table data model.
     * @param[in]  showType      See fillMsgRow().
     */
    void fillMsgData(DbInt::QResult     *res,
                     int                 msgType,
                     bool                doCheckBranch,
                     QStandardItemModel *mdl,
                     bool                showType = false);

    /**
     * Fills up a table with location data from database query.
     *
     * @param[in]  res           The query result. Deleted on return.
     * @param[in]  doCheckBranch See fillLocRow().
     * @param[out] mdl           The table data model.
     */
    void fillLocData(DbInt::QResult     *res,
                     bool                doCheckBranch,
                     QStandardItemModel *mdl);

    //the following fill* functions are grouped here because they share a set
    //of macros
    /**
     * Adds table row 0 and fills it up with call data from database query.
     *
     * @param[in]  res           The query result.
     * @param[in]  dbRow         The 0-based db query result row.
     * @param[in]  allToGssi     true if records are for group messaging.
     * @param[in]  doCheckBranch true to do branch validation.
     * @param[out] mdl           The table data model.
     */
    void fillCallRow(DbInt::QResult     *res,
                     int                 dbRow,
                     bool                allToGssi,
                     bool                doCheckBranch,
                     QStandardItemModel *mdl);

    /**
     * Adds table row 0 and fills it up with messaging data from database
     * query - MMS, SDS or Status Message.
     *
     * @param[in]  res           The query result.
     * @param[in]  dbRow         The 0-based db query result row.
     * @param[in]  msgType       CmnTypes::COMMS_MSG_MMS/SDS/STATUS or -1 if not
     *                           known and needs to be checked from res.
     * @param[in]  doCheckBranch true to do branch validation.
     * @param[out] mdl           The table data model.
     * @param[in]  showType      true to show the message type in a column.
     */
    void fillMsgRow(DbInt::QResult     *res,
                    int                 dbRow,
                    int                 msgType,
                    bool                doCheckBranch,
                    QStandardItemModel *mdl,
                    bool                showType = false);

    /**
     * Adds table row 0 and fills it up with location data from database query.
     *
     * @param[in]  res           The query result.
     * @param[in]  dbRow         The 0-based db query result row.
     * @param[in]  doCheckBranch true to do branch validation.
     * @param[out] mdl           The table data model.
     */
    void fillLocRow(DbInt::QResult     *res,
                    int                 dbRow,
                    bool                doCheckBranch,
                    QStandardItemModel *mdl);

    /**
     * Checks whether the ID/ISSI/GSSI in the given field of a database query
     * result is valid for the current fleet branch configuration.
     * Should be called only if SubsData::isMultiCluster() returns true.
     *
     * @param[in] res   The query result.
     * @param[in] row   The query result row.
     * @param[in] idFld The field containing the ID.
     * @param[in] tpFld The field containing the ID type.
     * @return true if valid.
     */
    bool checkBranch(DbInt::QResult *res, int row, int idFld, int tpFld);

    /**
     * Gets the ISSI, group name or Dispatcher ID from the To field of a
     * database query result.
     *
     * @param[in]  res  The query result.
     * @param[in]  row  The query result row.
     * @param[out] gssi The GSSI, if applicable.
     * @return The group name or recipient ID. Empty if missing data in the
     *         result row.
     */
    QString getToField(DbInt::QResult *res, int row, int *gssi = 0);

#ifdef INCIDENT
    /**
     * Converts an incident status value from database into a string for
     * display, which is subject to translation.
     *
     * @param[in] status The status value from database.
     *                   See IncidentData::eState.
     * @return The possibly translated status. Empty for unknown value.
     */
    QString getIncidentStatus(int status);

    /**
     * Gets call data involving specific resources in a specific period from
     * database into an Incident table.
     * To be called in 2 passes:
     * -First pass:  With resource IDs - gets individual calls only, and
     *               collects unique GSSIs of group calls by the resources.
     * -Second pass: With empty resource list but non-empty collected GSSI
     *               list - gets group calls only.
     *
     * @param[in]     ids       The resource IDs. Empty in second pass.
     * @param[in]     startTime The start time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]     endTime   The end time as "dd/MM/YYYY hh:mm:ss".
     * @param[in,out] gssis     The GSSIs involved. Output in first pass,
     *                          input in second pass.
     * @param[out]    tv        The table.
     * @return true if successful and data available.
     */
    bool getCallData(const QStringList &ids,
                     const std::string &startTime,
                     const std::string &endTime,
                     IntSetT           &gssis,
                     QTableView        *tv);

    /**
     * Gets SDS and Status Message data involving specific resources in a
     * specific period from database into an Incident table.
     * To be called in 2 passes:
     * -First pass:  With resource IDs - gets individual messages only, and
     *               collects unique GSSIs of group messages by the resources.
     * -Second pass: With empty resource list but non-empty collected GSSI
     *               list - gets group messages only.
     *
     * @param[in]     ids       The resource IDs. Empty in second pass.
     * @param[in]     startTime The start time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]     endTime   The end time as "dd/MM/YYYY hh:mm:ss".
     * @param[in,out] gssis     The GSSIs involved. Output in first pass,
     *                          input in second pass.
     * @param[out]    tv        The table.
     * @return true if successful and data available.
     */
    bool getMsgData(const QStringList &ids,
                    const std::string &startTime,
                    const std::string &endTime,
                    IntSetT           &gssis,
                    QTableView        *tv);

    /**
     * Adds new call, SDS or Status Message data into an Incident table.
     *
     * @param[in]  tblType The table type - eTblType.
     * @param[in]  msgType The message type - CmnType::eCommsType.
     *                     Used only for tblType == TBLTYPE_MSG.
     * @param[in]  from    The sender.
     * @param[in]  to      The recipient.
     * @param[out] tv      The table.
     * @return true if this added the first row.
     */
    bool getLastData(int         tblType,
                     int         msgType,
                     int         from,
                     int         to,
                     QTableView *tv);

    /**
     * Gets incident data for a specific ID from database into a Report
     * table.
     *
     * @param[in]  id The incident ID.
     * @param[out] tv The table.
     * @return true if successful (but tv may be empty). false indicates DB
     *         query failure.
     */
    bool getIncidentData(int id, QTableView *tv);

    /**
     * Gets incident data in a specific period from database into a Report
     * table.
     *
     * @param[in]  status    The incident status. See IncidentData::eState.
     * @param[in]  startTime The start time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]  endTime   The end time as "dd/MM/YYYY hh:mm:ss".
     * @param[in]  state     The location state, or empty string for all.
     * @param[in]  priority  The priority, or empty string for all.
     * @param[in]  category  The category, or empty string for all.
     * @param[in]  desc      The space separated description keywords
     *                       (case-insensitive), or empty if not applicable.
     * @param[in]  resources The comma separated resource list, or empty if
     *                       not applicable.
     * @param[out] tv        The table.
     * @return true if successful (but tv may be empty). false indicates DB
     *         query failure.
     */
    bool getIncidentData(int                status,
                         const std::string &startTime,
                         const std::string &endTime,
                         const std::string &state,
                         const std::string &priority,
                         const std::string &category,
                         const std::string &desc,
                         const std::string &resources,
                         QTableView        *tv);

    /**
     * Fills up a table with incident data from database query.
     *
     * @param[in]  res The query result.
     * @param[out] tv  The table.
     */
    void fillIncidentData(DbInt::QResult *res, QTableView *tv);
#endif //INCIDENT
}
#endif //QTTABLEUTILS_H
