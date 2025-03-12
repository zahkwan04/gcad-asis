/********************************************************************************
** Form generated from reading UI file 'CommsRegister.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMSREGISTER_H
#define UI_COMMSREGISTER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommsRegister
{
public:
    QVBoxLayout *verticalLayout1;
    QFrame *mainFrame;
    QVBoxLayout *mainLayout;
    QFrame *titleFrame;
    QHBoxLayout *horizontalLayout1;
    QToolButton *floatButton;
    QSpacerItem *titleFrameSpacer;
    QPushButton *callButton;
    QPushButton *msgButton;
    QPushButton *printButton;
    QScrollArea *scrollArea;
    QSplitter *splitter1;
    QFrame *callFrame;
    QVBoxLayout *verticalLayout2;
    QFrame *callTitleFrame;
    QHBoxLayout *horizontalLayout2;
    QLabel *callTitleLabel;
    QSpacerItem *callTitleFrameSpacer;
    QPushButton *callFilterButton;
    QTableWidget *callTable;
    QFrame *msgFrame;
    QVBoxLayout *verticalLayout3;
    QFrame *msgTitleFrame;
    QHBoxLayout *horizontalLayout3;
    QLabel *msgTitleLabel;
    QSpacerItem *msgTitleFrameSpacer;
    QPushButton *msgFilterButton;
    QTableWidget *msgTable;

    void setupUi(QWidget *CommsRegister)
    {
        if (CommsRegister->objectName().isEmpty())
            CommsRegister->setObjectName(QString::fromUtf8("CommsRegister"));
        CommsRegister->resize(562, 610);
        verticalLayout1 = new QVBoxLayout(CommsRegister);
        verticalLayout1->setSpacing(5);
        verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
        verticalLayout1->setContentsMargins(0, 0, 0, 0);
        mainFrame = new QFrame(CommsRegister);
        mainFrame->setObjectName(QString::fromUtf8("mainFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainFrame->sizePolicy().hasHeightForWidth());
        mainFrame->setSizePolicy(sizePolicy);
        mainFrame->setFrameShape(QFrame::StyledPanel);
        mainFrame->setLineWidth(1);
        mainLayout = new QVBoxLayout(mainFrame);
        mainLayout->setSpacing(1);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(2, 2, 2, 0);
        titleFrame = new QFrame(mainFrame);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        titleFrame->setFrameShape(QFrame::NoFrame);
        titleFrame->setFrameShadow(QFrame::Raised);
        titleFrame->setLineWidth(0);
        horizontalLayout1 = new QHBoxLayout(titleFrame);
        horizontalLayout1->setSpacing(5);
        horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
        horizontalLayout1->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout1->setContentsMargins(5, 5, 5, 5);
        floatButton = new QToolButton(titleFrame);
        floatButton->setObjectName(QString::fromUtf8("floatButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_down.png"), QSize(), QIcon::Normal, QIcon::On);
        floatButton->setIcon(icon);
        floatButton->setIconSize(QSize(25, 25));
        floatButton->setCheckable(true);

        horizontalLayout1->addWidget(floatButton);

        titleFrameSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout1->addItem(titleFrameSpacer);

        callButton = new QPushButton(titleFrame);
        callButton->setObjectName(QString::fromUtf8("callButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_call.png"), QSize(), QIcon::Normal, QIcon::Off);
        callButton->setIcon(icon1);
        callButton->setIconSize(QSize(25, 25));
        callButton->setCheckable(true);

        horizontalLayout1->addWidget(callButton);

        msgButton = new QPushButton(titleFrame);
        msgButton->setObjectName(QString::fromUtf8("msgButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_sds.png"), QSize(), QIcon::Normal, QIcon::Off);
        msgButton->setIcon(icon2);
        msgButton->setIconSize(QSize(25, 25));
        msgButton->setCheckable(true);

        horizontalLayout1->addWidget(msgButton);

        printButton = new QPushButton(titleFrame);
        printButton->setObjectName(QString::fromUtf8("printButton"));
        printButton->setEnabled(false);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_print.png"), QSize(), QIcon::Normal, QIcon::Off);
        printButton->setIcon(icon3);
        printButton->setIconSize(QSize(25, 25));

        horizontalLayout1->addWidget(printButton);


        mainLayout->addWidget(titleFrame);

        scrollArea = new QScrollArea(mainFrame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        splitter1 = new QSplitter();
        splitter1->setObjectName(QString::fromUtf8("splitter1"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(splitter1->sizePolicy().hasHeightForWidth());
        splitter1->setSizePolicy(sizePolicy1);
        splitter1->setLineWidth(0);
        splitter1->setOrientation(Qt::Vertical);
        splitter1->setHandleWidth(2);
        splitter1->setChildrenCollapsible(false);
        callFrame = new QFrame(splitter1);
        callFrame->setObjectName(QString::fromUtf8("callFrame"));
        verticalLayout2 = new QVBoxLayout(callFrame);
        verticalLayout2->setSpacing(0);
        verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
        verticalLayout2->setContentsMargins(0, 0, 0, 0);
        callTitleFrame = new QFrame(callFrame);
        callTitleFrame->setObjectName(QString::fromUtf8("callTitleFrame"));
        callTitleFrame->setFrameShape(QFrame::NoFrame);
        callTitleFrame->setFrameShadow(QFrame::Raised);
        callTitleFrame->setLineWidth(0);
        horizontalLayout2 = new QHBoxLayout(callTitleFrame);
        horizontalLayout2->setSpacing(5);
        horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
        horizontalLayout2->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout2->setContentsMargins(5, 5, 5, 5);
        callTitleLabel = new QLabel(callTitleFrame);
        callTitleLabel->setObjectName(QString::fromUtf8("callTitleLabel"));

        horizontalLayout2->addWidget(callTitleLabel);

        callTitleFrameSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout2->addItem(callTitleFrameSpacer);

        callFilterButton = new QPushButton(callTitleFrame);
        callFilterButton->setObjectName(QString::fromUtf8("callFilterButton"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_filter.png"), QSize(), QIcon::Normal, QIcon::Off);
        callFilterButton->setIcon(icon4);
        callFilterButton->setIconSize(QSize(25, 25));

        horizontalLayout2->addWidget(callFilterButton);


        verticalLayout2->addWidget(callTitleFrame);

        callTable = new QTableWidget(callFrame);
        callTable->setObjectName(QString::fromUtf8("callTable"));
        callTable->setFrameShape(QFrame::NoFrame);
        callTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        callTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        callTable->setAlternatingRowColors(true);
        callTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
        callTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        callTable->setTextElideMode(Qt::ElideRight);
        callTable->setWordWrap(true);
        callTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        callTable->horizontalHeader()->setStretchLastSection(true);
        callTable->verticalHeader()->setVisible(true);

        verticalLayout2->addWidget(callTable);

        splitter1->addWidget(callFrame);
        msgFrame = new QFrame(splitter1);
        msgFrame->setObjectName(QString::fromUtf8("msgFrame"));
        verticalLayout3 = new QVBoxLayout(msgFrame);
        verticalLayout3->setSpacing(0);
        verticalLayout3->setObjectName(QString::fromUtf8("verticalLayout3"));
        verticalLayout3->setContentsMargins(0, 0, 0, 0);
        msgTitleFrame = new QFrame(msgFrame);
        msgTitleFrame->setObjectName(QString::fromUtf8("msgTitleFrame"));
        msgTitleFrame->setFrameShape(QFrame::NoFrame);
        msgTitleFrame->setFrameShadow(QFrame::Raised);
        msgTitleFrame->setLineWidth(0);
        horizontalLayout3 = new QHBoxLayout(msgTitleFrame);
        horizontalLayout3->setSpacing(5);
        horizontalLayout3->setObjectName(QString::fromUtf8("horizontalLayout3"));
        horizontalLayout3->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout3->setContentsMargins(5, 5, 5, 5);
        msgTitleLabel = new QLabel(msgTitleFrame);
        msgTitleLabel->setObjectName(QString::fromUtf8("msgTitleLabel"));

        horizontalLayout3->addWidget(msgTitleLabel);

        msgTitleFrameSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout3->addItem(msgTitleFrameSpacer);

        msgFilterButton = new QPushButton(msgTitleFrame);
        msgFilterButton->setObjectName(QString::fromUtf8("msgFilterButton"));
        msgFilterButton->setIcon(icon4);
        msgFilterButton->setIconSize(QSize(25, 25));

        horizontalLayout3->addWidget(msgFilterButton);


        verticalLayout3->addWidget(msgTitleFrame);

        msgTable = new QTableWidget(msgFrame);
        msgTable->setObjectName(QString::fromUtf8("msgTable"));
        msgTable->setFrameShape(QFrame::NoFrame);
        msgTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        msgTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        msgTable->setAlternatingRowColors(true);
        msgTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
        msgTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        msgTable->setTextElideMode(Qt::ElideRight);
        msgTable->setWordWrap(true);
        msgTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(false));
        msgTable->horizontalHeader()->setStretchLastSection(true);
        msgTable->verticalHeader()->setVisible(true);

        verticalLayout3->addWidget(msgTable);

        splitter1->addWidget(msgFrame);
        scrollArea->setWidget(splitter1);

        mainLayout->addWidget(scrollArea);


        verticalLayout1->addWidget(mainFrame);


        retranslateUi(CommsRegister);

        QMetaObject::connectSlotsByName(CommsRegister);
    } // setupUi

    void retranslateUi(QWidget *CommsRegister)
    {
        CommsRegister->setWindowTitle(QCoreApplication::translate("CommsRegister", "Communications History", nullptr));
#if QT_CONFIG(tooltip)
        printButton->setToolTip(QCoreApplication::translate("CommsRegister", "Print", nullptr));
#endif // QT_CONFIG(tooltip)
        callTitleLabel->setText(QCoreApplication::translate("CommsRegister", "Calls", nullptr));
#if QT_CONFIG(tooltip)
        callFilterButton->setToolTip(QCoreApplication::translate("CommsRegister", "Filter", nullptr));
#endif // QT_CONFIG(tooltip)
        msgTitleLabel->setText(QCoreApplication::translate("CommsRegister", "Messages", nullptr));
#if QT_CONFIG(tooltip)
        msgFilterButton->setToolTip(QCoreApplication::translate("CommsRegister", "Filter", nullptr));
#endif // QT_CONFIG(tooltip)
        msgFilterButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CommsRegister: public Ui_CommsRegister {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMSREGISTER_H
