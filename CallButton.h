/**
 * The CallButton UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: CallButton.h 1880 2024-11-15 08:24:57Z hazim.rujhan $
 * @author Mazdiana Makmor
 */
#ifndef CALLBUTTON_H
#define CALLBUTTON_H

#include <QMouseEvent>
#include <QWidget>

#include "CallWindow.h"

namespace Ui {
class CallButton;
}

class CallButton : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] callWindow Call Window object.
     * @param[in] parent     Parent widget, if any.
     */
    explicit CallButton(CallWindow *callWindow, QWidget *parent = 0);

    ~CallButton();

    /**
     * Calls the corresponding CallWindow function and sets the calling party
     * label.
     *
     * @param[in] type ResourceData::TYPE_SUBSCRIBER or TYPE_DISPATCHER.
     * @param[in] id   The ID.
     */
    void setCallingParty(int type, int id);

    /**
     * Sets the called party label and calls the corresponding CallWindow
     * function.
     *
     * @param[in] name The name.
     */
    void setCalledParty(const QString &name);

    /**
     * Sets call status to connected.
     *
     * @param[in] txParty  The talking party ID, if any.
     * @param[in] priority The call priority, if any.
     */
    void setConnected(const QString &txParty,
                      int            priority = MsgSp::Value::UNDEFINED);

    /**
     * Changes the call ownership.
     *
     * @param[in] callingPartyName The calling party name.
     * @param[in] priority         New call priority, if any.
     */
    void changeOwnership(const QString &callingPartyName, int priority = 0);

    /**
     * Resets the call window and releases ownership.
     */
    void resetCallWindow();

    void releaseCallWindow() { mCallWindow = 0; }

    CallWindow *getCallWindow() { return mCallWindow; }

    /**
     * Checks whether this object is for a group call.
     *
     * @return true if group call.
     */
    bool isGrpCall() const { return mCallWindow->isGrpCall(); }

protected:
    /**
     * Displays the call window.
     *
     * @param[in] event The mouse press event.
     */
    void mousePressEvent(QMouseEvent *event);

private:
    Ui::CallButton *ui;
    int             mPriority;
    CallWindow     *mCallWindow;

    /**
     * Initializes the instance.
     */
    void init();

    /**
     * Sets call priority and changes appearance accordingly.
     *
     * @param[in] priority Call priority.
     */
    void setPriority(int priority);
};
#endif //CALLBUTTON_H
