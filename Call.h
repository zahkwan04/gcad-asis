/**
 * The Call UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Call.h 1806 2024-01-30 01:46:19Z hazim.rujhan $
 * @author Mazdiana Makmor
 */
#ifndef CALL_H
#define CALL_H

#include <QWidget>

#include "CommsRegister.h"
#include "ResourceSelector.h"

namespace Ui {
class Call;
}

class Call : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] logger      The app logger.
     * @param[in] rscSelector Resource Selector object.
     * @param[in] commsReg    Call register. Takes over ownership.
     * @param[in] parent      Parent widget, if any.
     */
    explicit Call(Logger           *logger,
                  ResourceSelector *rscSelector,
                  CommsRegister    *commsReg,
                  QWidget          *parent = 0);

    ~Call();

    /**
     * Loads ResourceSelector object.
     */
    void activate();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    CommsRegister *getCommsRegister() { return mCommsReg; }

    /**
     * Shows the Broadcast button if allowed.
     *
     * @param[in] allowed true to allow.
     */
    void setBroadcastPermission(bool allowed);

    /**
     * Opens a CallWindow.
     */
    void openCallWindow();

signals:
    void newBroadcast();
    void newCall(int rscType, int ssi);

public slots:
    /**
     * Enables the Call button if the recipient single-selection is valid.
     * Disables it otherwise.
     *
     * @param isValidSingle true if single-selection is valid.
     */
    void onSelectionChanged(bool isValidSingle, bool);

private:
    Ui::Call         *ui;
    CommsRegister    *mCommsReg;
    //following objects are owned by another module
    Logger           *mLogger;
    ResourceSelector *mResourceSelector;
};
#endif //CALL_H
