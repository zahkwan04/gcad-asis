/********************************************************************************
** Form generated from reading UI file 'Call.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALL_H
#define UI_CALL_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Call
{
public:
    QVBoxLayout *verticalLayout1;
    QFrame *inpFrame;
    QVBoxLayout *inpLayout;
    QHBoxLayout *callHLayout;
    QSpacerItem *callHSpacer1;
    QToolButton *broadcastButton;
    QToolButton *callButton;
    QSpacerItem *callHSpacer2;

    void setupUi(QWidget *Call)
    {
        if (Call->objectName().isEmpty())
            Call->setObjectName(QString::fromUtf8("Call"));
        Call->resize(562, 610);
        verticalLayout1 = new QVBoxLayout(Call);
        verticalLayout1->setSpacing(0);
        verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
        verticalLayout1->setContentsMargins(0, 0, 0, 0);
        inpFrame = new QFrame(Call);
        inpFrame->setObjectName(QString::fromUtf8("inpFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(inpFrame->sizePolicy().hasHeightForWidth());
        inpFrame->setSizePolicy(sizePolicy);
        inpFrame->setFrameShape(QFrame::NoFrame);
        inpFrame->setFrameShadow(QFrame::Raised);
        inpFrame->setLineWidth(0);
        inpLayout = new QVBoxLayout(inpFrame);
        inpLayout->setSpacing(5);
        inpLayout->setObjectName(QString::fromUtf8("inpLayout"));
        inpLayout->setContentsMargins(2, 2, 2, 5);
        callHLayout = new QHBoxLayout();
        callHLayout->setSpacing(5);
        callHLayout->setObjectName(QString::fromUtf8("callHLayout"));
        callHLayout->setContentsMargins(5, 5, 5, 5);
        callHSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        callHLayout->addItem(callHSpacer1);

        broadcastButton = new QToolButton(inpFrame);
        broadcastButton->setObjectName(QString::fromUtf8("broadcastButton"));
        broadcastButton->setMinimumSize(QSize(108, 76));
        broadcastButton->setMaximumSize(QSize(108, 76));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_broadcast.png"), QSize(), QIcon::Normal, QIcon::Off);
        broadcastButton->setIcon(icon);
        broadcastButton->setIconSize(QSize(40, 40));
        broadcastButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        callHLayout->addWidget(broadcastButton);

        callButton = new QToolButton(inpFrame);
        callButton->setObjectName(QString::fromUtf8("callButton"));
        callButton->setEnabled(false);
        callButton->setMinimumSize(QSize(108, 76));
        callButton->setMaximumSize(QSize(108, 76));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_call.png"), QSize(), QIcon::Normal, QIcon::Off);
        callButton->setIcon(icon1);
        callButton->setIconSize(QSize(40, 40));
        callButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        callHLayout->addWidget(callButton);

        callHSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        callHLayout->addItem(callHSpacer2);


        inpLayout->addLayout(callHLayout);


        verticalLayout1->addWidget(inpFrame);


        retranslateUi(Call);

        QMetaObject::connectSlotsByName(Call);
    } // setupUi

    void retranslateUi(QWidget *Call)
    {
        Call->setWindowTitle(QCoreApplication::translate("Call", "Call", nullptr));
        broadcastButton->setText(QCoreApplication::translate("Call", "Broadcast", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Call: public Ui_Call {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALL_H
