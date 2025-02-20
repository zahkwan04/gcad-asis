/**
 * UI Login Module.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Login.h 1805 2024-01-29 09:07:59Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#ifndef LOGIN_H
#define LOGIN_H

#include <QCloseEvent>
#include <QDialog>
#include <QMessageBox>
#include <QString>
#include <QWidget>

#include "SettingsUi.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] userName   The user name, or empty string if none.
     * @param[in] settingsUi Settings object.
     * @param[in] hasOsk     true if the on-screen keyboard is available on this
     *                       platform.
     * @param[in] parent     Parent widget, if any.
     */
    explicit Login(const QString &userName,
                   SettingsUi    *settingsUi,
                   bool           hasOsk,
                   QWidget       *parent = 0);

    ~Login();

    /**
     * Enables user interaction.
     */
    void enable();

signals:
    void cancelLogin();
    void doLogin(const QString &userName, const QString &password);
    void showOsk();
    void lastWindowClosed();

public slots:
    /**
     * Sets the connectLabel text, and the Help Desk phone number, if any.
     *
     * @param[in] Unused signal parameter.
     * @param[in] helpNum The Help Desk phone number, or empty string if none.
     */
    void onLoginSuccess(bool, const std::string &helpNum);

private:
    Ui::Login  *ui;
    SettingsUi *mSettingsUi; //owned by another module

    /**
     * Enables or disables the Login button based on the user text fields
     * content.
     */
    void enableDisableLogin();

    /**
     * Handles close event from window close button.
     */
    void closeEvent(QCloseEvent *event);
};
#endif //LOGIN_H
