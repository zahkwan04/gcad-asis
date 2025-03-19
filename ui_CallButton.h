/********************************************************************************
** Form generated from reading UI file 'CallButton.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALLBUTTON_H
#define UI_CALLBUTTON_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CallButton
{
public:
    QVBoxLayout *callButtonVLayout;
    QFrame *callFrame;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *calledPartyHLayout;
    QLabel *calledLabel;
    QLabel *callingPartyLabel;
    QLabel *calledPartyLabel;

    void setupUi(QWidget *CallButton)
    {
        if (CallButton->objectName().isEmpty())
            CallButton->setObjectName(QString::fromUtf8("CallButton"));
        CallButton->resize(100, 65);
        CallButton->setMinimumSize(QSize(100, 55));
        CallButton->setMaximumSize(QSize(16777215, 16777215));
        callButtonVLayout = new QVBoxLayout(CallButton);
        callButtonVLayout->setObjectName(QString::fromUtf8("callButtonVLayout"));
        callButtonVLayout->setContentsMargins(0, 0, 0, 0);
        callFrame = new QFrame(CallButton);
        callFrame->setObjectName(QString::fromUtf8("callFrame"));
        callFrame->setFrameShape(QFrame::Panel);
        callFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(callFrame);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(3, 3, 3, 3);
        calledPartyHLayout = new QHBoxLayout();
        calledPartyHLayout->setObjectName(QString::fromUtf8("calledPartyHLayout"));
        calledLabel = new QLabel(callFrame);
        calledLabel->setObjectName(QString::fromUtf8("calledLabel"));
        calledLabel->setMinimumSize(QSize(30, 0));
        calledLabel->setMaximumSize(QSize(16777215, 16777215));

        calledPartyHLayout->addWidget(calledLabel);

        callingPartyLabel = new QLabel(callFrame);
        callingPartyLabel->setObjectName(QString::fromUtf8("callingPartyLabel"));
        callingPartyLabel->setAlignment(Qt::AlignCenter);

        calledPartyHLayout->addWidget(callingPartyLabel);


        verticalLayout_2->addLayout(calledPartyHLayout);

        calledPartyLabel = new QLabel(callFrame);
        calledPartyLabel->setObjectName(QString::fromUtf8("calledPartyLabel"));
        calledPartyLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(calledPartyLabel);


        callButtonVLayout->addWidget(callFrame);


        retranslateUi(CallButton);

        QMetaObject::connectSlotsByName(CallButton);
    } // setupUi

    void retranslateUi(QWidget *CallButton)
    {
        CallButton->setWindowTitle(QCoreApplication::translate("CallButton", "Form", nullptr));
        calledLabel->setText(QCoreApplication::translate("CallButton", "From", nullptr));
        callingPartyLabel->setText(QString());
        calledPartyLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CallButton: public Ui_CallButton {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALLBUTTON_H
