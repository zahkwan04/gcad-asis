/********************************************************************************
** Form generated from reading UI file 'GisRouting.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISROUTING_H
#define UI_GISROUTING_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QColumnView>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GisRouting
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *routingFrame;
    QVBoxLayout *verticalLayout_8;
    QFrame *routeTitleFrame;
    QHBoxLayout *horizontalLayout_8;
    QLabel *routeIconLbl;
    QLabel *routeLbl;
    QToolButton *floatButton;
    QToolButton *toggleBtn;
    QFrame *routeFrame;
    QVBoxLayout *verticalLayout_11;
    QGridLayout *routeGLayout;
    QLineEdit *sourceEdit;
    QToolButton *srcBtn;
    QLabel *sourceLbl;
    QToolButton *dstBtn;
    QLineEdit *destinationEdit;
    QLabel *destinationLbl;
    QHBoxLayout *routeHLayout;
    QSpacerItem *routeHSpacer;
    QPushButton *clearBtn;
    QPushButton *routeBtn;
    QFrame *routeSuggestFrame;
    QVBoxLayout *verticalLayout_2;
    QLabel *routeSuggestLbl;
    QTableWidget *tableWidget;
    QFrame *routeDetailsFrame;
    QVBoxLayout *verticalLayout_3;
    QLabel *routeDetailsLbl;
    QColumnView *columnView;

    void setupUi(QWidget *GisRouting)
    {
        if (GisRouting->objectName().isEmpty())
            GisRouting->setObjectName(QString::fromUtf8("GisRouting"));
        GisRouting->resize(400, 468);
        verticalLayout = new QVBoxLayout(GisRouting);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        routingFrame = new QFrame(GisRouting);
        routingFrame->setObjectName(QString::fromUtf8("routingFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(routingFrame->sizePolicy().hasHeightForWidth());
        routingFrame->setSizePolicy(sizePolicy);
        routingFrame->setMaximumSize(QSize(16777215, 16777215));
        routingFrame->setFrameShape(QFrame::NoFrame);
        routingFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_8 = new QVBoxLayout(routingFrame);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        routeTitleFrame = new QFrame(routingFrame);
        routeTitleFrame->setObjectName(QString::fromUtf8("routeTitleFrame"));
        routeTitleFrame->setMaximumSize(QSize(16777215, 37));
        routeTitleFrame->setFrameShape(QFrame::StyledPanel);
        routeTitleFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_8 = new QHBoxLayout(routeTitleFrame);
        horizontalLayout_8->setSpacing(5);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(5, 5, 5, 5);
        routeIconLbl = new QLabel(routeTitleFrame);
        routeIconLbl->setObjectName(QString::fromUtf8("routeIconLbl"));
        routeIconLbl->setMaximumSize(QSize(30, 30));
        routeIconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_route.png")));
        routeIconLbl->setScaledContents(true);

        horizontalLayout_8->addWidget(routeIconLbl);

        routeLbl = new QLabel(routeTitleFrame);
        routeLbl->setObjectName(QString::fromUtf8("routeLbl"));

        horizontalLayout_8->addWidget(routeLbl);

        floatButton = new QToolButton(routeTitleFrame);
        floatButton->setObjectName(QString::fromUtf8("floatButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_down.png"), QSize(), QIcon::Normal, QIcon::On);
        floatButton->setIcon(icon);
        floatButton->setIconSize(QSize(25, 25));
        floatButton->setCheckable(true);

        horizontalLayout_8->addWidget(floatButton);

        toggleBtn = new QToolButton(routeTitleFrame);
        toggleBtn->setObjectName(QString::fromUtf8("toggleBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::On);
        toggleBtn->setIcon(icon1);
        toggleBtn->setCheckable(true);
        toggleBtn->setChecked(true);
        toggleBtn->setAutoRaise(true);

        horizontalLayout_8->addWidget(toggleBtn);


        verticalLayout_8->addWidget(routeTitleFrame);

        routeFrame = new QFrame(routingFrame);
        routeFrame->setObjectName(QString::fromUtf8("routeFrame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(routeFrame->sizePolicy().hasHeightForWidth());
        routeFrame->setSizePolicy(sizePolicy1);
        routeFrame->setMaximumSize(QSize(16777215, 16777215));
        routeFrame->setFrameShape(QFrame::NoFrame);
        routeFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_11 = new QVBoxLayout(routeFrame);
        verticalLayout_11->setSpacing(5);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(5, 5, 5, 5);
        routeGLayout = new QGridLayout();
        routeGLayout->setObjectName(QString::fromUtf8("routeGLayout"));
        routeGLayout->setContentsMargins(-1, 10, -1, -1);
        sourceEdit = new QLineEdit(routeFrame);
        sourceEdit->setObjectName(QString::fromUtf8("sourceEdit"));
        sourceEdit->setMinimumSize(QSize(0, 30));

        routeGLayout->addWidget(sourceEdit, 0, 1, 1, 1);

        srcBtn = new QToolButton(routeFrame);
        srcBtn->setObjectName(QString::fromUtf8("srcBtn"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_dropPin.png"), QSize(), QIcon::Normal, QIcon::Off);
        srcBtn->setIcon(icon2);
        srcBtn->setIconSize(QSize(30, 30));

        routeGLayout->addWidget(srcBtn, 0, 2, 1, 1);

        sourceLbl = new QLabel(routeFrame);
        sourceLbl->setObjectName(QString::fromUtf8("sourceLbl"));

        routeGLayout->addWidget(sourceLbl, 0, 0, 1, 1);

        dstBtn = new QToolButton(routeFrame);
        dstBtn->setObjectName(QString::fromUtf8("dstBtn"));
        dstBtn->setIcon(icon2);
        dstBtn->setIconSize(QSize(30, 30));

        routeGLayout->addWidget(dstBtn, 1, 2, 1, 1);

        destinationEdit = new QLineEdit(routeFrame);
        destinationEdit->setObjectName(QString::fromUtf8("destinationEdit"));
        destinationEdit->setMinimumSize(QSize(0, 30));

        routeGLayout->addWidget(destinationEdit, 1, 1, 1, 1);

        destinationLbl = new QLabel(routeFrame);
        destinationLbl->setObjectName(QString::fromUtf8("destinationLbl"));

        routeGLayout->addWidget(destinationLbl, 1, 0, 1, 1);


        verticalLayout_11->addLayout(routeGLayout);

        routeHLayout = new QHBoxLayout();
        routeHLayout->setObjectName(QString::fromUtf8("routeHLayout"));
        routeHLayout->setContentsMargins(-1, 10, 10, 10);
        routeHSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        routeHLayout->addItem(routeHSpacer);

        clearBtn = new QPushButton(routeFrame);
        clearBtn->setObjectName(QString::fromUtf8("clearBtn"));
        clearBtn->setMinimumSize(QSize(150, 40));

        routeHLayout->addWidget(clearBtn);

        routeBtn = new QPushButton(routeFrame);
        routeBtn->setObjectName(QString::fromUtf8("routeBtn"));
        routeBtn->setMinimumSize(QSize(150, 40));

        routeHLayout->addWidget(routeBtn);


        verticalLayout_11->addLayout(routeHLayout);

        routeSuggestFrame = new QFrame(routeFrame);
        routeSuggestFrame->setObjectName(QString::fromUtf8("routeSuggestFrame"));
        routeSuggestFrame->setFrameShape(QFrame::StyledPanel);
        routeSuggestFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(routeSuggestFrame);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        routeSuggestLbl = new QLabel(routeSuggestFrame);
        routeSuggestLbl->setObjectName(QString::fromUtf8("routeSuggestLbl"));

        verticalLayout_2->addWidget(routeSuggestLbl);


        verticalLayout_11->addWidget(routeSuggestFrame);

        tableWidget = new QTableWidget(routeFrame);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        verticalLayout_11->addWidget(tableWidget);

        routeDetailsFrame = new QFrame(routeFrame);
        routeDetailsFrame->setObjectName(QString::fromUtf8("routeDetailsFrame"));
        routeDetailsFrame->setFrameShape(QFrame::StyledPanel);
        routeDetailsFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(routeDetailsFrame);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        routeDetailsLbl = new QLabel(routeDetailsFrame);
        routeDetailsLbl->setObjectName(QString::fromUtf8("routeDetailsLbl"));

        verticalLayout_3->addWidget(routeDetailsLbl);


        verticalLayout_11->addWidget(routeDetailsFrame);

        columnView = new QColumnView(routeFrame);
        columnView->setObjectName(QString::fromUtf8("columnView"));

        verticalLayout_11->addWidget(columnView);


        verticalLayout_8->addWidget(routeFrame);


        verticalLayout->addWidget(routingFrame);


        retranslateUi(GisRouting);

        QMetaObject::connectSlotsByName(GisRouting);
    } // setupUi

    void retranslateUi(QWidget *GisRouting)
    {
        routeLbl->setText(QCoreApplication::translate("GisRouting", "Routing", nullptr));
#if QT_CONFIG(tooltip)
        floatButton->setToolTip(QCoreApplication::translate("GisRouting", "Float", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        srcBtn->setToolTip(QCoreApplication::translate("GisRouting", "Set source", nullptr));
#endif // QT_CONFIG(tooltip)
        sourceLbl->setText(QCoreApplication::translate("GisRouting", "Source:", nullptr));
#if QT_CONFIG(tooltip)
        dstBtn->setToolTip(QCoreApplication::translate("GisRouting", "Set destination", nullptr));
#endif // QT_CONFIG(tooltip)
        destinationLbl->setText(QCoreApplication::translate("GisRouting", "Destination:", nullptr));
        clearBtn->setText(QCoreApplication::translate("GisRouting", "Clear", nullptr));
        routeBtn->setText(QCoreApplication::translate("GisRouting", "Get Directions", nullptr));
        routeSuggestLbl->setText(QCoreApplication::translate("GisRouting", "Route Suggestion", nullptr));
        routeDetailsLbl->setText(QCoreApplication::translate("GisRouting", "Route Details", nullptr));
        (void)GisRouting;
    } // retranslateUi

};

namespace Ui {
    class GisRouting: public Ui_GisRouting {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISROUTING_H
