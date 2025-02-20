/**
 * UI Login implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Login.cpp 1805 2024-01-29 09:07:59Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#include <assert.h>
#include <QIntValidator>

#include "Settings.h"
#include "Style.h"
#include "Version.h"
#include "ui_Login.h"
#include "Login.h"

using namespace std;

Login::Login(const QString &userName,
             SettingsUi    *settingsUi,
             bool           hasOsk,
             QWidget       *parent) :
QDialog(parent), ui(new Ui::Login), mSettingsUi(settingsUi)
{
    assert(settingsUi != 0);
    ui->setupUi(this);
    //remove the help button from the title bar
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    //set labels content
    ui->titleLabel->setText(Version::NWK_NAME);
    string s(Settings::instance().get<string>(Props::FLD_CFG_HELPDESKNUM));
    if (!s.empty())
    {
        ui->helpLabel->setText(tr("Need Help?"));
        ui->phoneLabel
          ->setText("<div><img style=\"vertical-align:middle\" "
                    "src=:/Images/images/icon_help_desk.png width=\"30\"; "
                    "height=\"30\">" + QString::fromStdString(s) + "</div>");
    }
    QString ss("background:");
    ss.append(Style::getStyle(Style::OBJ_BACKGROUND_DARK));
    setStyleSheet(ss);
    ss = Style::getStyle(Style::OBJ_TOOLBUTTON);
    ui->settingsButton->setStyleSheet(ss);
    ui->eyeButton->setStyleSheet(ss);
    ui->loginButton->setStyleSheet(ss);
    ui->connectLabel->setText(tr("Connecting to server..."));
    ui->connectLabel->hide();
    ui->cancelButton->hide();
    ui->cancelButton->setStyleSheet(Style::getStyle(Style::OBJ_PUSHBUTTON));
    ui->oskButton->setStyleSheet(Style::getStyle(Style::OBJ_PUSHBUTTON_SIMPLE));
    ui->oskButton->setVisible(hasOsk);
    if (hasOsk)
        connect(ui->oskButton, &QPushButton::clicked, this,
                [this] { emit showOsk(); });
    ui->usernameEdit->setValidator(new QIntValidator(this));
    connect(ui->usernameEdit, &QLineEdit::textChanged, this,
            [this] { enableDisableLogin(); });
    connect(ui->passwordEdit, &QLineEdit::textChanged, this,
            [this] { enableDisableLogin(); });
    connect(ui->passwordEdit, &QLineEdit::returnPressed, this,
            [this] { ui->loginButton->click(); });
    if (userName.isEmpty())
    {
        ui->usernameEdit->setFocus();
    }
    else
    {
        ui->usernameEdit->setText(userName);
        ui->passwordEdit->setFocus();
    }
    connect(ui->settingsButton, &QToolButton::clicked, this,
            [this]
            {
                //show Settings dialog
                mSettingsUi->raise();
                mSettingsUi->show();
            });
    connect(ui->eyeButton, &QToolButton::clicked, this,
            [this](bool checked)
            {
                ui->passwordEdit->setEchoMode((checked)?
                                        QLineEdit::Password: QLineEdit::Normal);
            });
    ui->eyeButton->click(); //start in password mode
    connect(ui->loginButton, &QToolButton::clicked, this,
            [this]
            {
                //disable user interaction and emit signal
                ui->settingsButton->setEnabled(false);
                ui->usernameEdit->setEnabled(false);
                ui->passwordEdit->setEnabled(false);
                ui->loginButton->setEnabled(false);
                ui->cancelButton->show();
                ui->connectLabel->show();
                ui->oskButton->hide();
                emit doLogin(ui->usernameEdit->text(), ui->passwordEdit->text());
            });
    connect(ui->cancelButton, &QPushButton::clicked, this,
            [this] { emit cancelLogin(); });
}

Login::~Login()
{
   delete ui;
}

void Login::enable()
{
    ui->settingsButton->setEnabled(true);
    ui->usernameEdit->setEnabled(true);
    ui->passwordEdit->setEnabled(true);
    ui->cancelButton->hide();
    ui->connectLabel->hide();
    if (ui->oskButton->isEnabled())
        ui->oskButton->show();
    enableDisableLogin();
}

void Login::onLoginSuccess(bool, const string &helpNum)
{
    ui->connectLabel->setText(tr("Connected. Loading data from server..."));
    if (!helpNum.empty())
    {
        ui->helpLabel->setText(tr("Need Help?"));
        ui->phoneLabel
          ->setText("<div><img style=\"vertical-align:middle\" "
                    "src=:/Images/images/icon_help_desk.png width=\"30\"; "
                    "height=\"30\">" + QString::fromStdString(helpNum) +
                    "</div>");
    }
}

void Login::enableDisableLogin()
{
    ui->loginButton->setEnabled(!ui->usernameEdit->text().isEmpty() &&
                                !ui->passwordEdit->text().isEmpty());
}

void Login::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit lastWindowClosed();
}
