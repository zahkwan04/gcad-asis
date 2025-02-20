/**
 * Resource collector modal dialog for selection from available resources of a
 * particular type. When confirmed, emits a signal if the selection has changed.
 *
 * Copyright (C) Sapura Secured Technologies, 2024. All Rights Reserved.
 *
 * @file
 * @version $Id: RscCollector.h 1884 2024-11-25 09:10:00Z hazim.rujhan $
 * @author Hazim Mohd Rujhan
 */
#ifndef RSCCOLLECTOR_H
#define RSCCOLLECTOR_H

#include <string>
#include <QDialog>
#include <QSortFilterProxyModel>

#include "DraggableListView.h"
#include "Logger.h"
#include "ResourceData.h"

namespace Ui {
class RscCollector;
}

class RscCollector : public QDialog
{
    Q_OBJECT

public:
    typedef ResourceData::IdsT SsisT;

    /**
     * Constructor.
     *
     * @param[in] type   Resource type - ResourceData::TYPE_*.
     * @param[in] ssis   SSI collection. Updated directly when selection is
     *                   confirmed.
     * @param[in] parent Parent widget.
     */
    explicit RscCollector(int type, SsisT *ssis, QWidget *parent);

    ~RscCollector();

    static void init(Logger *logger) { sLogger = logger; }

signals:
    void selChanged();

protected:
    //override
    bool eventFilter(QObject *obj, QEvent *evt);

private:
    SsisT                 *mSsis;    //the collection - no ownership
    Ui::RscCollector      *ui;
    DraggableListView     *mRscList; //available resources
    DraggableListView     *mSelList; //selection
    QSortFilterProxyModel *mSortMdl;

    static Logger *sLogger;

    /**
     * Moves selected items between mRscList and mSelList, and sorts the
     * destination list.
     *
     * @param[in] add true to move from mRscList to mSelList, false for the
     *                other direction.
     */
    void moveItems(bool add);
};
#endif //RSCCOLLECTOR_H
