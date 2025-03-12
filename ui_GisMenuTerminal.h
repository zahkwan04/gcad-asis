/********************************************************************************
** Form generated from reading UI file 'GisMenuTerminal.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISMENUTERMINAL_H
#define UI_GISMENUTERMINAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GisMenuTerminal
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *formFrame;
    QGridLayout *gridLayout;
    QFrame *titleFrame;
    QHBoxLayout *horizontalLayout_2;
    QLabel *terminalIconLbl;
    QLabel *terminalLbl;
    QFormLayout *formLayout;
    QLabel *lastUpdLbl;
    QLabel *lastUpdVal;
    QLabel *lastValidUpdLbl;
    QLabel *lastValidUpdVal;
    QLabel *locationLbl;
    QLabel *locationVal;
    QLabel *lastActiveLbl;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *GisMenuTerminal)
    {
        if (GisMenuTerminal->objectName().isEmpty())
            GisMenuTerminal->setObjectName(QString::fromUtf8("GisMenuTerminal"));
        GisMenuTerminal->setWindowModality(Qt::WindowModal);
        GisMenuTerminal->resize(190, 118);
        GisMenuTerminal->setStyleSheet(QString::fromUtf8(""));
        GisMenuTerminal->setModal(true);
        verticalLayout = new QVBoxLayout(GisMenuTerminal);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        formFrame = new QFrame(GisMenuTerminal);
        formFrame->setObjectName(QString::fromUtf8("formFrame"));
        formFrame->setStyleSheet(QString::fromUtf8(""));
        formFrame->setFrameShape(QFrame::StyledPanel);
        formFrame->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(formFrame);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(0);
        gridLayout->setContentsMargins(5, 5, 5, 5);
        titleFrame = new QFrame(formFrame);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        titleFrame->setFrameShape(QFrame::StyledPanel);
        titleFrame->setFrameShadow(QFrame::Raised);
        titleFrame->setLineWidth(1);
        horizontalLayout_2 = new QHBoxLayout(titleFrame);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        terminalIconLbl = new QLabel(titleFrame);
        terminalIconLbl->setObjectName(QString::fromUtf8("terminalIconLbl"));
        terminalIconLbl->setMaximumSize(QSize(30, 30));
        terminalIconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_mobile.png")));
        terminalIconLbl->setScaledContents(true);

        horizontalLayout_2->addWidget(terminalIconLbl);

        terminalLbl = new QLabel(titleFrame);
        terminalLbl->setObjectName(QString::fromUtf8("terminalLbl"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(terminalLbl->sizePolicy().hasHeightForWidth());
        terminalLbl->setSizePolicy(sizePolicy);
        terminalLbl->setMaximumSize(QSize(16777215, 16777215));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        terminalLbl->setFont(font);
        terminalLbl->setContextMenuPolicy(Qt::ActionsContextMenu);
        terminalLbl->setLayoutDirection(Qt::LeftToRight);
        terminalLbl->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        terminalLbl->setWordWrap(true);

        horizontalLayout_2->addWidget(terminalLbl);


        gridLayout->addWidget(titleFrame, 0, 0, 1, 1);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setSizeConstraint(QLayout::SetFixedSize);
        formLayout->setHorizontalSpacing(5);
        formLayout->setVerticalSpacing(0);
        lastUpdLbl = new QLabel(formFrame);
        lastUpdLbl->setObjectName(QString::fromUtf8("lastUpdLbl"));
        QFont font1;
        font1.setPointSize(8);
        lastUpdLbl->setFont(font1);

        formLayout->setWidget(0, QFormLayout::LabelRole, lastUpdLbl);

        lastUpdVal = new QLabel(formFrame);
        lastUpdVal->setObjectName(QString::fromUtf8("lastUpdVal"));

        formLayout->setWidget(0, QFormLayout::FieldRole, lastUpdVal);

        lastValidUpdLbl = new QLabel(formFrame);
        lastValidUpdLbl->setObjectName(QString::fromUtf8("lastValidUpdLbl"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lastValidUpdLbl);

        lastValidUpdVal = new QLabel(formFrame);
        lastValidUpdVal->setObjectName(QString::fromUtf8("lastValidUpdVal"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lastValidUpdVal);

        locationLbl = new QLabel(formFrame);
        locationLbl->setObjectName(QString::fromUtf8("locationLbl"));

        formLayout->setWidget(2, QFormLayout::LabelRole, locationLbl);

        locationVal = new QLabel(formFrame);
        locationVal->setObjectName(QString::fromUtf8("locationVal"));

        formLayout->setWidget(2, QFormLayout::FieldRole, locationVal);

        lastActiveLbl = new QLabel(formFrame);
        lastActiveLbl->setObjectName(QString::fromUtf8("lastActiveLbl"));
        lastActiveLbl->setLayoutDirection(Qt::RightToLeft);
        lastActiveLbl->setAutoFillBackground(false);
        lastActiveLbl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        formLayout->setWidget(3, QFormLayout::SpanningRole, lastActiveLbl);


        gridLayout->addLayout(formLayout, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(formFrame);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        horizontalLayout->addWidget(okButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);


        verticalLayout->addWidget(formFrame);


        retranslateUi(GisMenuTerminal);

        QMetaObject::connectSlotsByName(GisMenuTerminal);
    } // setupUi

    void retranslateUi(QDialog *GisMenuTerminal)
    {
        GisMenuTerminal->setWindowTitle(QCoreApplication::translate("GisMenuTerminal", "Dialog", nullptr));
        terminalIconLbl->setText(QString());
        terminalLbl->setText(QString());
        lastUpdLbl->setText(QCoreApplication::translate("GisMenuTerminal", "Last Update:", nullptr));
        lastUpdVal->setText(QString());
        lastValidUpdLbl->setText(QCoreApplication::translate("GisMenuTerminal", "Last Valid Update:", nullptr));
        lastValidUpdVal->setText(QString());
        locationLbl->setText(QCoreApplication::translate("GisMenuTerminal", "Location:", nullptr));
        locationVal->setText(QString());
        lastActiveLbl->setText(QString());
        okButton->setText(QCoreApplication::translate("GisMenuTerminal", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GisMenuTerminal: public Ui_GisMenuTerminal {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISMENUTERMINAL_H
