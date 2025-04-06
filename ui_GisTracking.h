/********************************************************************************
** Form generated from reading UI file 'GisTracking.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISTRACKING_H
#define UI_GISTRACKING_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GisTracking
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *trackTitleFrame;
    QHBoxLayout *horizontalLayout_10;
    QLabel *trackIconLbl;
    QLabel *trackLbl;
    QToolButton *floatButton;
    QToolButton *toggleBtn;
    QFrame *trackFrame;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *trackGLayout;
    QLabel *incidentLbl;
    QGridLayout *gridLayout;
    QComboBox *incidentCb;
    QPushButton *rscButton;
    QLabel *subscriberLbl;
    QComboBox *subscriberCb;
    QLabel *startDateLbl;
    QDateEdit *startDate;
    QTimeEdit *startTime;
    QLabel *endDateLbl;
    QDateEdit *endDate;
    QTimeEdit *endTime;
    QHBoxLayout *trackHLayout;
    QPushButton *trailingButton;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *submitButton;

    void setupUi(QWidget *GisTracking)
    {
        if (GisTracking->objectName().isEmpty())
            GisTracking->setObjectName(QString::fromUtf8("GisTracking"));
        GisTracking->resize(425, 188);
        verticalLayout = new QVBoxLayout(GisTracking);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        trackTitleFrame = new QFrame(GisTracking);
        trackTitleFrame->setObjectName(QString::fromUtf8("trackTitleFrame"));
        trackTitleFrame->setMaximumSize(QSize(16777215, 37));
        trackTitleFrame->setFrameShape(QFrame::StyledPanel);
        trackTitleFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_10 = new QHBoxLayout(trackTitleFrame);
        horizontalLayout_10->setSpacing(5);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(5, 5, 5, 5);
        trackIconLbl = new QLabel(trackTitleFrame);
        trackIconLbl->setObjectName(QString::fromUtf8("trackIconLbl"));
        trackIconLbl->setMaximumSize(QSize(30, 30));
        trackIconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_trail.png")));
        trackIconLbl->setScaledContents(true);

        horizontalLayout_10->addWidget(trackIconLbl);

        trackLbl = new QLabel(trackTitleFrame);
        trackLbl->setObjectName(QString::fromUtf8("trackLbl"));

        horizontalLayout_10->addWidget(trackLbl);

        floatButton = new QToolButton(trackTitleFrame);
        floatButton->setObjectName(QString::fromUtf8("floatButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_down.png"), QSize(), QIcon::Normal, QIcon::On);
        floatButton->setIcon(icon);
        floatButton->setIconSize(QSize(25, 25));
        floatButton->setCheckable(true);

        horizontalLayout_10->addWidget(floatButton);

        toggleBtn = new QToolButton(trackTitleFrame);
        toggleBtn->setObjectName(QString::fromUtf8("toggleBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::On);
        toggleBtn->setIcon(icon1);
        toggleBtn->setCheckable(true);
        toggleBtn->setChecked(true);
        toggleBtn->setAutoRaise(true);

        horizontalLayout_10->addWidget(toggleBtn);


        verticalLayout->addWidget(trackTitleFrame);

        trackFrame = new QFrame(GisTracking);
        trackFrame->setObjectName(QString::fromUtf8("trackFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(trackFrame->sizePolicy().hasHeightForWidth());
        trackFrame->setSizePolicy(sizePolicy);
        trackFrame->setMaximumSize(QSize(16777215, 16777215));
        trackFrame->setFrameShape(QFrame::NoFrame);
        trackFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(trackFrame);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        trackGLayout = new QGridLayout();
        trackGLayout->setSpacing(5);
        trackGLayout->setObjectName(QString::fromUtf8("trackGLayout"));
        incidentLbl = new QLabel(trackFrame);
        incidentLbl->setObjectName(QString::fromUtf8("incidentLbl"));

        trackGLayout->addWidget(incidentLbl, 0, 0, 1, 1);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        incidentCb = new QComboBox(trackFrame);
        incidentCb->setObjectName(QString::fromUtf8("incidentCb"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(incidentCb->sizePolicy().hasHeightForWidth());
        incidentCb->setSizePolicy(sizePolicy1);
        incidentCb->setEditable(true);
        incidentCb->setInsertPolicy(QComboBox::NoInsert);

        gridLayout->addWidget(incidentCb, 0, 0, 1, 1);

        rscButton = new QPushButton(trackFrame);
        rscButton->setObjectName(QString::fromUtf8("rscButton"));

        gridLayout->addWidget(rscButton, 0, 1, 1, 1);


        trackGLayout->addLayout(gridLayout, 0, 1, 1, 2);

        subscriberLbl = new QLabel(trackFrame);
        subscriberLbl->setObjectName(QString::fromUtf8("subscriberLbl"));
        subscriberLbl->setMaximumSize(QSize(110, 16777215));

        trackGLayout->addWidget(subscriberLbl, 1, 0, 1, 1);

        subscriberCb = new QComboBox(trackFrame);
        subscriberCb->setObjectName(QString::fromUtf8("subscriberCb"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(subscriberCb->sizePolicy().hasHeightForWidth());
        subscriberCb->setSizePolicy(sizePolicy2);
        subscriberCb->setEditable(true);
        subscriberCb->setInsertPolicy(QComboBox::NoInsert);

        trackGLayout->addWidget(subscriberCb, 1, 1, 1, 2);

        startDateLbl = new QLabel(trackFrame);
        startDateLbl->setObjectName(QString::fromUtf8("startDateLbl"));
        startDateLbl->setWordWrap(true);

        trackGLayout->addWidget(startDateLbl, 2, 0, 1, 1);

        startDate = new QDateEdit(trackFrame);
        startDate->setObjectName(QString::fromUtf8("startDate"));
        startDate->setAlignment(Qt::AlignCenter);
        startDate->setMaximumDate(QDate(7999, 2, 28));
        startDate->setCurrentSection(QDateTimeEdit::DaySection);
        startDate->setCalendarPopup(true);

        trackGLayout->addWidget(startDate, 2, 1, 1, 1);

        startTime = new QTimeEdit(trackFrame);
        startTime->setObjectName(QString::fromUtf8("startTime"));
        startTime->setAlignment(Qt::AlignCenter);
        startTime->setCalendarPopup(false);

        trackGLayout->addWidget(startTime, 2, 2, 1, 1);

        endDateLbl = new QLabel(trackFrame);
        endDateLbl->setObjectName(QString::fromUtf8("endDateLbl"));
        endDateLbl->setWordWrap(true);

        trackGLayout->addWidget(endDateLbl, 3, 0, 1, 1);

        endDate = new QDateEdit(trackFrame);
        endDate->setObjectName(QString::fromUtf8("endDate"));
        endDate->setAlignment(Qt::AlignCenter);
        endDate->setDateTime(QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0)));
        endDate->setCalendarPopup(true);

        trackGLayout->addWidget(endDate, 3, 1, 1, 1);

        endTime = new QTimeEdit(trackFrame);
        endTime->setObjectName(QString::fromUtf8("endTime"));
        endTime->setAlignment(Qt::AlignCenter);
        endTime->setCalendarPopup(false);

        trackGLayout->addWidget(endTime, 3, 2, 1, 1);


        verticalLayout_2->addLayout(trackGLayout);

        trackHLayout = new QHBoxLayout();
        trackHLayout->setObjectName(QString::fromUtf8("trackHLayout"));
        trailingButton = new QPushButton(trackFrame);
        trailingButton->setObjectName(QString::fromUtf8("trailingButton"));
        trailingButton->setStyleSheet(QString::fromUtf8(""));

        trackHLayout->addWidget(trailingButton);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        trackHLayout->addItem(horizontalSpacer_5);

        submitButton = new QPushButton(trackFrame);
        submitButton->setObjectName(QString::fromUtf8("submitButton"));

        trackHLayout->addWidget(submitButton);


        verticalLayout_2->addLayout(trackHLayout);


        verticalLayout->addWidget(trackFrame);


        retranslateUi(GisTracking);

        QMetaObject::connectSlotsByName(GisTracking);
    } // setupUi

    void retranslateUi(QWidget *GisTracking)
    {
        trackIconLbl->setText(QString());
        trackLbl->setText(QCoreApplication::translate("GisTracking", "Tracking/Trailing", nullptr));
#if QT_CONFIG(tooltip)
        floatButton->setToolTip(QCoreApplication::translate("GisTracking", "Float", nullptr));
#endif // QT_CONFIG(tooltip)
        incidentLbl->setText(QCoreApplication::translate("GisTracking", "Incident ID:", nullptr));
        rscButton->setText(QCoreApplication::translate("GisTracking", "Get Resources", nullptr));
        subscriberLbl->setText(QCoreApplication::translate("GisTracking", "Subscribers:", nullptr));
        startDateLbl->setText(QCoreApplication::translate("GisTracking", "Start:", nullptr));
        startDate->setDisplayFormat(QCoreApplication::translate("GisTracking", "d/M/yyyy", nullptr));
        startTime->setDisplayFormat(QCoreApplication::translate("GisTracking", "hh:mm", nullptr));
        endDateLbl->setText(QCoreApplication::translate("GisTracking", "End:", nullptr));
        endDate->setDisplayFormat(QCoreApplication::translate("GisTracking", "d/M/yyyy", nullptr));
        endTime->setDisplayFormat(QCoreApplication::translate("GisTracking", "hh:mm", nullptr));
        trailingButton->setText(QCoreApplication::translate("GisTracking", "Live Trailing", nullptr));
        submitButton->setText(QCoreApplication::translate("GisTracking", "Submit", nullptr));
        (void)GisTracking;
    } // retranslateUi

};

namespace Ui {
    class GisTracking: public Ui_GisTracking {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISTRACKING_H
