/********************************************************************************
** Form generated from reading UI file 'ActiveIncidents.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACTIVEINCIDENTS_H
#define UI_ACTIVEINCIDENTS_H

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

class Ui_ActiveIncidents
{
public:
    QVBoxLayout *vLayout;
    QFrame *titleFrame;
    QHBoxLayout *titleHLayout;
    QLabel *titleIconLabel;
    QLabel *titleLabel;
    QToolButton *iconBtn;
    QToolButton *filterBtn;
    QToolButton *posBtn;
    QToolButton *toggleShowBtn;
    QFrame *scrollAreaFrame;
    QHBoxLayout *scrollAreaHLayout;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;

    void setupUi(QWidget *ActiveIncidents)
    {
        if (ActiveIncidents->objectName().isEmpty())
            ActiveIncidents->setObjectName(QString::fromUtf8("ActiveIncidents"));
        ActiveIncidents->setMinimumSize(QSize(0, 0));
        ActiveIncidents->setMaximumSize(QSize(16777215, 16777215));
        vLayout = new QVBoxLayout(ActiveIncidents);
        vLayout->setSpacing(0);
        vLayout->setObjectName(QString::fromUtf8("vLayout"));
        vLayout->setContentsMargins(2, 0, 2, 0);
        titleFrame = new QFrame(ActiveIncidents);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(titleFrame->sizePolicy().hasHeightForWidth());
        titleFrame->setSizePolicy(sizePolicy);
        titleFrame->setMinimumSize(QSize(0, 40));
        titleFrame->setFrameShape(QFrame::NoFrame);
        titleFrame->setFrameShadow(QFrame::Raised);
        titleHLayout = new QHBoxLayout(titleFrame);
        titleHLayout->setSpacing(5);
        titleHLayout->setObjectName(QString::fromUtf8("titleHLayout"));
        titleHLayout->setContentsMargins(2, 0, 2, 0);
        titleIconLabel = new QLabel(titleFrame);
        titleIconLabel->setObjectName(QString::fromUtf8("titleIconLabel"));
        titleIconLabel->setMaximumSize(QSize(28, 28));
        titleIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_incident.png")));
        titleIconLabel->setScaledContents(true);

        titleHLayout->addWidget(titleIconLabel);

        titleLabel = new QLabel(titleFrame);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));

        titleHLayout->addWidget(titleLabel);

        iconBtn = new QToolButton(titleFrame);
        iconBtn->setObjectName(QString::fromUtf8("iconBtn"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_icons.png"), QSize(), QIcon::Normal, QIcon::Off);
        iconBtn->setIcon(icon);
        iconBtn->setPopupMode(QToolButton::MenuButtonPopup);
        iconBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        iconBtn->setAutoRaise(true);

        titleHLayout->addWidget(iconBtn);

        filterBtn = new QToolButton(titleFrame);
        filterBtn->setObjectName(QString::fromUtf8("filterBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_filter.png"), QSize(), QIcon::Normal, QIcon::Off);
        filterBtn->setIcon(icon1);
        filterBtn->setPopupMode(QToolButton::MenuButtonPopup);
        filterBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        filterBtn->setAutoRaise(true);

        titleHLayout->addWidget(filterBtn);

        posBtn = new QToolButton(titleFrame);
        posBtn->setObjectName(QString::fromUtf8("posBtn"));
        posBtn->setPopupMode(QToolButton::MenuButtonPopup);
        posBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
        posBtn->setAutoRaise(true);

        titleHLayout->addWidget(posBtn);

        toggleShowBtn = new QToolButton(titleFrame);
        toggleShowBtn->setObjectName(QString::fromUtf8("toggleShowBtn"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::On);
        toggleShowBtn->setIcon(icon2);
        toggleShowBtn->setCheckable(true);

        titleHLayout->addWidget(toggleShowBtn);


        vLayout->addWidget(titleFrame);

        scrollAreaFrame = new QFrame(ActiveIncidents);
        scrollAreaFrame->setObjectName(QString::fromUtf8("scrollAreaFrame"));
        scrollAreaFrame->setFrameShape(QFrame::NoFrame);
        scrollAreaFrame->setFrameShadow(QFrame::Raised);
        scrollAreaHLayout = new QHBoxLayout(scrollAreaFrame);
        scrollAreaHLayout->setSpacing(0);
        scrollAreaHLayout->setObjectName(QString::fromUtf8("scrollAreaHLayout"));
        scrollAreaHLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(scrollAreaFrame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollWidget = new QWidget();
        scrollWidget->setObjectName(QString::fromUtf8("scrollWidget"));
        scrollArea->setWidget(scrollWidget);

        scrollAreaHLayout->addWidget(scrollArea);


        vLayout->addWidget(scrollAreaFrame);


        retranslateUi(ActiveIncidents);

        QMetaObject::connectSlotsByName(ActiveIncidents);
    } // setupUi

    void retranslateUi(QWidget *ActiveIncidents)
    {
#if QT_CONFIG(tooltip)
        iconBtn->setToolTip(QCoreApplication::translate("ActiveIncidents", "Icons", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        filterBtn->setToolTip(QCoreApplication::translate("ActiveIncidents", "Filter", nullptr));
#endif // QT_CONFIG(tooltip)
        posBtn->setText(QCoreApplication::translate("ActiveIncidents", "Position", nullptr));
        toggleShowBtn->setText(QString());
        (void)ActiveIncidents;
    } // retranslateUi

};

namespace Ui {
    class ActiveIncidents: public Ui_ActiveIncidents {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACTIVEINCIDENTS_H
