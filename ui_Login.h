/********************************************************************************
** Form generated from reading UI file 'Login.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QVBoxLayout *verticalLayout1;
    QHBoxLayout *horizontalLayout1;
    QLabel *titleLabel;
    QLabel *logoLabel;
    QHBoxLayout *horizontalLayout2;
    QLabel *orgLogo;
    QHBoxLayout *horizontalLayout3;
    QToolButton *settingsButton;
    QGridLayout *inpLayout;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QToolButton *eyeButton;
    QToolButton *loginButton;
    QHBoxLayout *horizontalLayout4;
    QSpacerItem *verticalSpacer1;
    QSpacerItem *horizontalSpacer1;
    QLabel *connectLabel;
    QPushButton *cancelButton;
    QPushButton *oskButton;
    QSpacerItem *horizontalSpacer2;
    QHBoxLayout *horizontalLayout5;
    QSpacerItem *horizontalSpacer3;
    QVBoxLayout *verticalLayout4;
    QLabel *helpLabel;
    QLabel *phoneLabel;

    void setupUi(QDialog *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName(QString::fromUtf8("Login"));
        Login->resize(672, 379);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Login->sizePolicy().hasHeightForWidth());
        Login->setSizePolicy(sizePolicy);
        Login->setMaximumSize(QSize(672, 379));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_main.png"), QSize(), QIcon::Normal, QIcon::Off);
        Login->setWindowIcon(icon);
        Login->setModal(false);
        verticalLayout1 = new QVBoxLayout(Login);
        verticalLayout1->setSpacing(0);
        verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
        verticalLayout1->setContentsMargins(5, 5, 5, 5);
        horizontalLayout1 = new QHBoxLayout();
        horizontalLayout1->setSpacing(0);
        horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
        horizontalLayout1->setContentsMargins(90, 5, 5, 5);
        titleLabel = new QLabel(Login);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setFamily(QString::fromUtf8("Sapura"));
        font.setPointSize(13);
        titleLabel->setFont(font);
        titleLabel->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        titleLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout1->addWidget(titleLabel);

        logoLabel = new QLabel(Login);
        logoLabel->setObjectName(QString::fromUtf8("logoLabel"));
        logoLabel->setMaximumSize(QSize(85, 85));
        logoLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_main.png")));
        logoLabel->setScaledContents(true);
        logoLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout1->addWidget(logoLabel);


        verticalLayout1->addLayout(horizontalLayout1);

        horizontalLayout2 = new QHBoxLayout();
        horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
        horizontalLayout2->setContentsMargins(-1, -1, -1, 10);
        orgLogo = new QLabel(Login);
        orgLogo->setObjectName(QString::fromUtf8("orgLogo"));
        orgLogo->setMaximumSize(QSize(350, 100));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Sapura"));
        orgLogo->setFont(font1);
        orgLogo->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_agency.png")));
        orgLogo->setAlignment(Qt::AlignCenter);

        horizontalLayout2->addWidget(orgLogo);


        verticalLayout1->addLayout(horizontalLayout2);

        horizontalLayout3 = new QHBoxLayout();
        horizontalLayout3->setSpacing(5);
        horizontalLayout3->setObjectName(QString::fromUtf8("horizontalLayout3"));
        horizontalLayout3->setContentsMargins(5, 10, 5, -1);
        settingsButton = new QToolButton(Login);
        settingsButton->setObjectName(QString::fromUtf8("settingsButton"));
        settingsButton->setMinimumSize(QSize(80, 80));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        settingsButton->setIcon(icon1);
        settingsButton->setIconSize(QSize(40, 40));
        settingsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        settingsButton->setAutoRaise(true);

        horizontalLayout3->addWidget(settingsButton);

        inpLayout = new QGridLayout();
        inpLayout->setObjectName(QString::fromUtf8("inpLayout"));
        inpLayout->setVerticalSpacing(5);
        inpLayout->setContentsMargins(0, 5, 0, 5);
        usernameEdit = new QLineEdit(Login);
        usernameEdit->setObjectName(QString::fromUtf8("usernameEdit"));
        usernameEdit->setMinimumSize(QSize(0, 30));
        usernameEdit->setFont(font);
        usernameEdit->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255); border: 1px solid gray; border-radius: 6px"));

        inpLayout->addWidget(usernameEdit, 0, 0, 1, 2);

        passwordEdit = new QLineEdit(Login);
        passwordEdit->setObjectName(QString::fromUtf8("passwordEdit"));
        passwordEdit->setMinimumSize(QSize(0, 30));
        passwordEdit->setFont(font);
        passwordEdit->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255); border: 1px solid gray; border-radius: 6px"));

        inpLayout->addWidget(passwordEdit, 1, 0, 1, 1);

        eyeButton = new QToolButton(Login);
        eyeButton->setObjectName(QString::fromUtf8("eyeButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(eyeButton->sizePolicy().hasHeightForWidth());
        eyeButton->setSizePolicy(sizePolicy1);
        eyeButton->setMinimumSize(QSize(30, 30));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_txt_hide.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_monitoring2.png"), QSize(), QIcon::Normal, QIcon::On);
        eyeButton->setIcon(icon2);
        eyeButton->setCheckable(true);
        eyeButton->setAutoRaise(true);

        inpLayout->addWidget(eyeButton, 1, 1, 1, 1);


        horizontalLayout3->addLayout(inpLayout);

        loginButton = new QToolButton(Login);
        loginButton->setObjectName(QString::fromUtf8("loginButton"));
        loginButton->setEnabled(false);
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(loginButton->sizePolicy().hasHeightForWidth());
        loginButton->setSizePolicy(sizePolicy2);
        loginButton->setMinimumSize(QSize(70, 70));
        loginButton->setAutoFillBackground(false);
        loginButton->setAutoRaise(true);

        horizontalLayout3->addWidget(loginButton);


        verticalLayout1->addLayout(horizontalLayout3);

        horizontalLayout4 = new QHBoxLayout();
        horizontalLayout4->setSpacing(30);
        horizontalLayout4->setObjectName(QString::fromUtf8("horizontalLayout4"));
        verticalSpacer1 = new QSpacerItem(10, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout4->addItem(verticalSpacer1);

        horizontalSpacer1 = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout4->addItem(horizontalSpacer1);

        connectLabel = new QLabel(Login);
        connectLabel->setObjectName(QString::fromUtf8("connectLabel"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Sapura"));
        font2.setPointSize(11);
        connectLabel->setFont(font2);
        connectLabel->setStyleSheet(QString::fromUtf8("color: rgb(196, 154, 108);"));

        horizontalLayout4->addWidget(connectLabel);

        cancelButton = new QPushButton(Login);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setMaximumSize(QSize(80, 16777215));
        cancelButton->setAutoDefault(false);

        horizontalLayout4->addWidget(cancelButton);

        oskButton = new QPushButton(Login);
        oskButton->setObjectName(QString::fromUtf8("oskButton"));
        sizePolicy1.setHeightForWidth(oskButton->sizePolicy().hasHeightForWidth());
        oskButton->setSizePolicy(sizePolicy1);
        oskButton->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_keyboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        oskButton->setIcon(icon3);
        oskButton->setIconSize(QSize(40, 40));
        oskButton->setAutoDefault(false);
        oskButton->setFlat(true);

        horizontalLayout4->addWidget(oskButton);

        horizontalSpacer2 = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout4->addItem(horizontalSpacer2);


        verticalLayout1->addLayout(horizontalLayout4);

        horizontalLayout5 = new QHBoxLayout();
        horizontalLayout5->setObjectName(QString::fromUtf8("horizontalLayout5"));
        horizontalLayout5->setContentsMargins(-1, -1, 5, -1);
        horizontalSpacer3 = new QSpacerItem(40, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout5->addItem(horizontalSpacer3);

        verticalLayout4 = new QVBoxLayout();
        verticalLayout4->setSpacing(5);
        verticalLayout4->setObjectName(QString::fromUtf8("verticalLayout4"));
        helpLabel = new QLabel(Login);
        helpLabel->setObjectName(QString::fromUtf8("helpLabel"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Sapura"));
        font3.setPointSize(10);
        helpLabel->setFont(font3);
        helpLabel->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        helpLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout4->addWidget(helpLabel);

        phoneLabel = new QLabel(Login);
        phoneLabel->setObjectName(QString::fromUtf8("phoneLabel"));
        phoneLabel->setFont(font3);
        phoneLabel->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        phoneLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout4->addWidget(phoneLabel);


        horizontalLayout5->addLayout(verticalLayout4);


        verticalLayout1->addLayout(horizontalLayout5);


        retranslateUi(Login);

        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QDialog *Login)
    {
        Login->setWindowTitle(QCoreApplication::translate("Login", "Login", nullptr));
        settingsButton->setText(QCoreApplication::translate("Login", "Settings", nullptr));
        usernameEdit->setPlaceholderText(QCoreApplication::translate("Login", "ID", nullptr));
        passwordEdit->setPlaceholderText(QCoreApplication::translate("Login", "Password", nullptr));
        loginButton->setText(QCoreApplication::translate("Login", "OK", nullptr));
        cancelButton->setText(QCoreApplication::translate("Login", "Cancel", nullptr));
#if QT_CONFIG(tooltip)
        oskButton->setToolTip(QCoreApplication::translate("Login", "On-screen keyboard", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
