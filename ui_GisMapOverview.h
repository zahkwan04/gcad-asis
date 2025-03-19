/********************************************************************************
** Form generated from reading UI file 'GisMapOverview.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISMAPOVERVIEW_H
#define UI_GISMAPOVERVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GisMapOverview
{
public:
    QVBoxLayout *mainLayout;
    QFrame *titleFrame;
    QHBoxLayout *titleLayout;
    QLabel *iconLabel;
    QLabel *titleLabel;
    QToolButton *hideTbtn;
    QFrame *overviewFrame;
    QVBoxLayout *frameLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidget;
    QVBoxLayout *scrollLayout;
    QVBoxLayout *mapLayout;

    void setupUi(QWidget *GisMapOverview)
    {
        if (GisMapOverview->objectName().isEmpty())
            GisMapOverview->setObjectName(QString::fromUtf8("GisMapOverview"));
        GisMapOverview->setMaximumSize(QSize(16777215, 300));
        mainLayout = new QVBoxLayout(GisMapOverview);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(0, 0, 0, 0);
        titleFrame = new QFrame(GisMapOverview);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(titleFrame->sizePolicy().hasHeightForWidth());
        titleFrame->setSizePolicy(sizePolicy);
        titleFrame->setMaximumSize(QSize(16777215, 37));
        titleFrame->setFrameShape(QFrame::StyledPanel);
        titleFrame->setFrameShadow(QFrame::Raised);
        titleLayout = new QHBoxLayout(titleFrame);
        titleLayout->setSpacing(5);
        titleLayout->setObjectName(QString::fromUtf8("titleLayout"));
        titleLayout->setContentsMargins(5, 5, 5, 5);
        iconLabel = new QLabel(titleFrame);
        iconLabel->setObjectName(QString::fromUtf8("iconLabel"));
        iconLabel->setMaximumSize(QSize(30, 30));
        iconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_map.png")));
        iconLabel->setScaledContents(true);

        titleLayout->addWidget(iconLabel);

        titleLabel = new QLabel(titleFrame);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));

        titleLayout->addWidget(titleLabel);

        hideTbtn = new QToolButton(titleFrame);
        hideTbtn->setObjectName(QString::fromUtf8("hideTbtn"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::On);
        hideTbtn->setIcon(icon);
        hideTbtn->setCheckable(true);
        hideTbtn->setAutoRaise(true);

        titleLayout->addWidget(hideTbtn);


        mainLayout->addWidget(titleFrame);

        overviewFrame = new QFrame(GisMapOverview);
        overviewFrame->setObjectName(QString::fromUtf8("overviewFrame"));
        overviewFrame->setFrameShape(QFrame::StyledPanel);
        overviewFrame->setFrameShadow(QFrame::Raised);
        frameLayout = new QVBoxLayout(overviewFrame);
        frameLayout->setSpacing(0);
        frameLayout->setObjectName(QString::fromUtf8("frameLayout"));
        frameLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(overviewFrame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        scrollArea->setWidgetResizable(false);
        scrollAreaWidget = new QWidget();
        scrollAreaWidget->setObjectName(QString::fromUtf8("scrollAreaWidget"));
        scrollAreaWidget->setGeometry(QRect(0, 0, 570, 240));
        sizePolicy1.setHeightForWidth(scrollAreaWidget->sizePolicy().hasHeightForWidth());
        scrollAreaWidget->setSizePolicy(sizePolicy1);
        scrollLayout = new QVBoxLayout(scrollAreaWidget);
        scrollLayout->setSpacing(0);
        scrollLayout->setObjectName(QString::fromUtf8("scrollLayout"));
        scrollLayout->setContentsMargins(0, 0, 0, 0);
        mapLayout = new QVBoxLayout();
        mapLayout->setSpacing(0);
        mapLayout->setObjectName(QString::fromUtf8("mapLayout"));

        scrollLayout->addLayout(mapLayout);

        scrollArea->setWidget(scrollAreaWidget);

        frameLayout->addWidget(scrollArea);


        mainLayout->addWidget(overviewFrame);


        retranslateUi(GisMapOverview);

        QMetaObject::connectSlotsByName(GisMapOverview);
    } // setupUi

    void retranslateUi(QWidget *GisMapOverview)
    {
        titleLabel->setText(QCoreApplication::translate("GisMapOverview", "Map Overview", nullptr));
        (void)GisMapOverview;
    } // retranslateUi

};

namespace Ui {
    class GisMapOverview: public Ui_GisMapOverview {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISMAPOVERVIEW_H
