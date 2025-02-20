/********************************************************************************
** Form generated from reading UI file 'Contact.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTACT_H
#define UI_CONTACT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Contact
{
public:
    QVBoxLayout *vLayout;
    QFrame *mainFrame;
    QVBoxLayout *mainLayout;
    QHBoxLayout *layout1;
    QToolButton *userDefBtn;
    QCheckBox *selectCb;
    QLabel *iconLbl;
    QLabel *titleLbl;
    QLabel *grpStatLbl;
    QPushButton *msgBtn;
    QPushButton *callBtn;
    QPushButton *joinBtn;
    QPushButton *pttBtn;
    QToolButton *endBtn;
    QSpacerItem *spacer1;
    QToolButton *monBtn;
    QToolButton *menuBtn;
    QHBoxLayout *layout2;
    QPushButton *callPartyBtn;
    QLabel *timeIconLbl;
    QLabel *timeLbl;
    QLabel *txIconLbl;
    QLabel *txLbl;
    QSpacerItem *spacer2;

    void setupUi(QWidget *Contact)
    {
        if (Contact->objectName().isEmpty())
            Contact->setObjectName(QString::fromUtf8("Contact"));
        Contact->resize(500, 50);
        Contact->setMinimumSize(QSize(0, 80));
        Contact->setMaximumSize(QSize(16777215, 80));
        Contact->setAutoFillBackground(false);
        vLayout = new QVBoxLayout(Contact);
        vLayout->setSpacing(0);
        vLayout->setObjectName(QString::fromUtf8("vLayout"));
        vLayout->setContentsMargins(0, 0, 0, 0);
        mainFrame = new QFrame(Contact);
        mainFrame->setObjectName(QString::fromUtf8("mainFrame"));
        mainFrame->setFrameShape(QFrame::StyledPanel);
        mainFrame->setFrameShadow(QFrame::Raised);
        mainLayout = new QVBoxLayout(mainFrame);
        mainLayout->setSpacing(2);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(2, 2, 2, 2);
        layout1 = new QHBoxLayout();
        layout1->setSpacing(2);
        layout1->setObjectName(QString::fromUtf8("layout1"));
        layout1->setContentsMargins(0, 0, 0, 0);
        userDefBtn = new QToolButton(mainFrame);
        userDefBtn->setObjectName(QString::fromUtf8("userDefBtn"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_slider_off.png"), QSize(), QIcon::Selected, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_slider_on.png"), QSize(), QIcon::Selected, QIcon::On);
        userDefBtn->setIcon(icon);
        userDefBtn->setIconSize(QSize(30, 30));
        userDefBtn->setCheckable(true);

        layout1->addWidget(userDefBtn);

        selectCb = new QCheckBox(mainFrame);
        selectCb->setObjectName(QString::fromUtf8("selectCb"));
        selectCb->setMinimumSize(QSize(20, 20));

        layout1->addWidget(selectCb);

        iconLbl = new QLabel(mainFrame);
        iconLbl->setObjectName(QString::fromUtf8("iconLbl"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(iconLbl->sizePolicy().hasHeightForWidth());
        iconLbl->setSizePolicy(sizePolicy);
        iconLbl->setMinimumSize(QSize(40, 40));
        iconLbl->setMaximumSize(QSize(40, 40));
        iconLbl->setScaledContents(true);

        layout1->addWidget(iconLbl);

        titleLbl = new QLabel(mainFrame);
        titleLbl->setObjectName(QString::fromUtf8("titleLbl"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(titleLbl->sizePolicy().hasHeightForWidth());
        titleLbl->setSizePolicy(sizePolicy1);
        titleLbl->setMaximumSize(QSize(16777215, 40));
        titleLbl->setWordWrap(false);

        layout1->addWidget(titleLbl);

        grpStatLbl = new QLabel(mainFrame);
        grpStatLbl->setObjectName(QString::fromUtf8("grpStatLbl"));
        sizePolicy.setHeightForWidth(grpStatLbl->sizePolicy().hasHeightForWidth());
        grpStatLbl->setSizePolicy(sizePolicy);
        grpStatLbl->setMinimumSize(QSize(20, 20));
        grpStatLbl->setMaximumSize(QSize(20, 20));
        grpStatLbl->setScaledContents(true);
        grpStatLbl->setAlignment(Qt::AlignCenter);

        layout1->addWidget(grpStatLbl);

        msgBtn = new QPushButton(mainFrame);
        msgBtn->setObjectName(QString::fromUtf8("msgBtn"));
        sizePolicy.setHeightForWidth(msgBtn->sizePolicy().hasHeightForWidth());
        msgBtn->setSizePolicy(sizePolicy);
        msgBtn->setMaximumSize(QSize(40, 40));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_sds.png"), QSize(), QIcon::Normal, QIcon::Off);
        msgBtn->setIcon(icon1);
        msgBtn->setIconSize(QSize(30, 30));

        layout1->addWidget(msgBtn);

        callBtn = new QPushButton(mainFrame);
        callBtn->setObjectName(QString::fromUtf8("callBtn"));
        sizePolicy.setHeightForWidth(callBtn->sizePolicy().hasHeightForWidth());
        callBtn->setSizePolicy(sizePolicy);
        callBtn->setMaximumSize(QSize(40, 40));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_call.png"), QSize(), QIcon::Normal, QIcon::Off);
        callBtn->setIcon(icon2);
        callBtn->setIconSize(QSize(30, 30));

        layout1->addWidget(callBtn);

        joinBtn = new QPushButton(mainFrame);
        joinBtn->setObjectName(QString::fromUtf8("joinBtn"));
        sizePolicy.setHeightForWidth(joinBtn->sizePolicy().hasHeightForWidth());
        joinBtn->setSizePolicy(sizePolicy);
        joinBtn->setMinimumSize(QSize(30, 0));
        joinBtn->setMaximumSize(QSize(40, 40));
        joinBtn->setVisible(false);
        joinBtn->setIcon(icon2);
        joinBtn->setIconSize(QSize(30, 30));

        layout1->addWidget(joinBtn);

        pttBtn = new QPushButton(mainFrame);
        pttBtn->setObjectName(QString::fromUtf8("pttBtn"));
        sizePolicy.setHeightForWidth(pttBtn->sizePolicy().hasHeightForWidth());
        pttBtn->setSizePolicy(sizePolicy);
        pttBtn->setMinimumSize(QSize(30, 0));
        pttBtn->setMaximumSize(QSize(40, 40));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_ptt.png"), QSize(), QIcon::Normal, QIcon::Off);
        pttBtn->setIcon(icon3);
        pttBtn->setIconSize(QSize(30, 30));

        layout1->addWidget(pttBtn);

        endBtn = new QToolButton(mainFrame);
        endBtn->setObjectName(QString::fromUtf8("endBtn"));
        endBtn->setEnabled(false);
        sizePolicy.setHeightForWidth(endBtn->sizePolicy().hasHeightForWidth());
        endBtn->setSizePolicy(sizePolicy);
        endBtn->setMinimumSize(QSize(30, 0));
        endBtn->setMaximumSize(QSize(40, 40));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_endCall2.png"), QSize(), QIcon::Active, QIcon::On);
        endBtn->setIcon(icon4);
        endBtn->setIconSize(QSize(30, 30));

        layout1->addWidget(endBtn);

        spacer1 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

        layout1->addItem(spacer1);

        monBtn = new QToolButton(mainFrame);
        monBtn->setObjectName(QString::fromUtf8("monBtn"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_monitoring.png"), QSize(), QIcon::Selected, QIcon::Off);
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_monitoring2.png"), QSize(), QIcon::Selected, QIcon::On);
        monBtn->setIcon(icon5);
        monBtn->setIconSize(QSize(30, 30));
        monBtn->setMaximumSize(QSize(30, 30));
        monBtn->setCheckable(true);

        layout1->addWidget(monBtn);

        menuBtn = new QToolButton(mainFrame);
        menuBtn->setObjectName(QString::fromUtf8("menuBtn"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        menuBtn->setIcon(icon6);
        menuBtn->setIconSize(QSize(30, 30));
        menuBtn->setMaximumSize(QSize(30, 30));
        menuBtn->setPopupMode(QToolButton::InstantPopup);

        layout1->addWidget(menuBtn);


        mainLayout->addLayout(layout1);

        layout2 = new QHBoxLayout();
        layout2->setSpacing(2);
        layout2->setObjectName(QString::fromUtf8("layout2"));
        layout2->setContentsMargins(0, 0, 0, 0);
        callPartyBtn = new QPushButton(mainFrame);
        callPartyBtn->setObjectName(QString::fromUtf8("callPartyBtn"));
        sizePolicy1.setHeightForWidth(callPartyBtn->sizePolicy().hasHeightForWidth());
        callPartyBtn->setSizePolicy(sizePolicy1);
        callPartyBtn->setMaximumSize(QSize(16777215, 40));
        callPartyBtn->setVisible(false);

        layout2->addWidget(callPartyBtn);

        timeIconLbl = new QLabel(mainFrame);
        timeIconLbl->setObjectName(QString::fromUtf8("timeIconLbl"));
        timeIconLbl->setMaximumSize(QSize(30, 30));
        timeIconLbl->setScaledContents(true);
        timeIconLbl->setVisible(false);
        timeIconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_timer.png")));

        layout2->addWidget(timeIconLbl);

        timeLbl = new QLabel(mainFrame);
        timeLbl->setObjectName(QString::fromUtf8("timeLbl"));
        timeLbl->setMaximumSize(QSize(16777215, 30));
        timeLbl->setVisible(false);

        layout2->addWidget(timeLbl);

        txIconLbl = new QLabel(mainFrame);
        txIconLbl->setObjectName(QString::fromUtf8("txIconLbl"));
        txIconLbl->setMaximumSize(QSize(30, 30));
        txIconLbl->setScaledContents(true);
        txIconLbl->setVisible(false);

        layout2->addWidget(txIconLbl);

        txLbl = new QLabel(mainFrame);
        txLbl->setObjectName(QString::fromUtf8("txLbl"));
        txLbl->setMaximumSize(QSize(16777215, 30));
        txLbl->setVisible(false);

        layout2->addWidget(txLbl);

        spacer2 = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

        layout2->addItem(spacer2);


        mainLayout->addLayout(layout2);


        vLayout->addWidget(mainFrame);


        retranslateUi(Contact);

        QMetaObject::connectSlotsByName(Contact);
    } // setupUi

    void retranslateUi(QWidget *Contact)
    {
#if QT_CONFIG(tooltip)
        userDefBtn->setToolTip(QCoreApplication::translate("Contact", "Save as user defined", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        selectCb->setToolTip(QCoreApplication::translate("Contact", "Select", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        msgBtn->setToolTip(QCoreApplication::translate("Contact", "Message", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        callBtn->setToolTip(QCoreApplication::translate("Contact", "Call", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        joinBtn->setToolTip(QCoreApplication::translate("Contact", "Join", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pttBtn->setToolTip(QCoreApplication::translate("Contact", "PTT", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        endBtn->setToolTip(QCoreApplication::translate("Contact", "End", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        monBtn->setToolTip(QCoreApplication::translate("Contact", "Monitor", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        menuBtn->setToolTip(QCoreApplication::translate("Contact", "Menu", nullptr));
#endif // QT_CONFIG(tooltip)
        (void)Contact;
    } // retranslateUi

};

namespace Ui {
    class Contact: public Ui_Contact {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTACT_H
