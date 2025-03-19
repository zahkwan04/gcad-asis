/********************************************************************************
** Form generated from reading UI file 'IncidentButton.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INCIDENTBUTTON_H
#define UI_INCIDENTBUTTON_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_IncidentButton
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QLabel *incidentId;
    QFrame *incidentFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *iconLabel;
    QLabel *statusLabel;

    void setupUi(QWidget *IncidentButton)
    {
        if (IncidentButton->objectName().isEmpty())
            IncidentButton->setObjectName(QString::fromUtf8("IncidentButton"));
        IncidentButton->resize(150, 100);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(IncidentButton->sizePolicy().hasHeightForWidth());
        IncidentButton->setSizePolicy(sizePolicy);
        IncidentButton->setFocusPolicy(Qt::NoFocus);
        IncidentButton->setAutoFillBackground(false);
        verticalLayout = new QVBoxLayout(IncidentButton);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(IncidentButton);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::Panel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(5, 5, 5, 5);
        incidentId = new QLabel(frame);
        incidentId->setObjectName(QString::fromUtf8("incidentId"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(incidentId->sizePolicy().hasHeightForWidth());
        incidentId->setSizePolicy(sizePolicy1);
        incidentId->setMinimumSize(QSize(0, 25));
        incidentId->setMaximumSize(QSize(16777215, 25));
        incidentId->setFrameShadow(QFrame::Raised);
        incidentId->setAlignment(Qt::AlignCenter);
        incidentId->setWordWrap(true);

        verticalLayout_2->addWidget(incidentId);

        incidentFrame = new QFrame(frame);
        incidentFrame->setObjectName(QString::fromUtf8("incidentFrame"));
        horizontalLayout = new QHBoxLayout(incidentFrame);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        iconLabel = new QLabel(incidentFrame);
        iconLabel->setObjectName(QString::fromUtf8("iconLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(iconLabel->sizePolicy().hasHeightForWidth());
        iconLabel->setSizePolicy(sizePolicy2);
        iconLabel->setMinimumSize(QSize(25, 25));
        iconLabel->setMaximumSize(QSize(28, 28));
        iconLabel->setScaledContents(true);
        iconLabel->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(iconLabel);


        verticalLayout_2->addWidget(incidentFrame);

        statusLabel = new QLabel(frame);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        sizePolicy1.setHeightForWidth(statusLabel->sizePolicy().hasHeightForWidth());
        statusLabel->setSizePolicy(sizePolicy1);
        statusLabel->setMinimumSize(QSize(100, 25));
        statusLabel->setMaximumSize(QSize(150, 25));
        statusLabel->setAlignment(Qt::AlignCenter);
        statusLabel->setWordWrap(true);

        verticalLayout_2->addWidget(statusLabel);


        verticalLayout->addWidget(frame);


        retranslateUi(IncidentButton);

        QMetaObject::connectSlotsByName(IncidentButton);
    } // setupUi

    void retranslateUi(QWidget *IncidentButton)
    {
        IncidentButton->setWindowTitle(QCoreApplication::translate("IncidentButton", "Form", nullptr));
        iconLabel->setText(QString());
        statusLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class IncidentButton: public Ui_IncidentButton {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INCIDENTBUTTON_H
