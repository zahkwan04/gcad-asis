/********************************************************************************
** Form generated from reading UI file 'GpsMonitor.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GPSMONITOR_H
#define UI_GPSMONITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GpsMonitor
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout1;
    QVBoxLayout *rscLayout;
    QFrame *searchFrame;
    QVBoxLayout *verticalLayout1;
    QLabel *searchLabel;
    QFrame *fieldFrame;
    QHBoxLayout *horizontalLayout2;
    QLineEdit *searchEdit;
    QFrame *rscFrame;
    QVBoxLayout *verticalLayout2;
    QSpacerItem *rscSpacer;
    QFrame *middleFrame;
    QVBoxLayout *verticalLayout3;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *removeAllButton;
    QVBoxLayout *monLayout;
    QFrame *monFrame;
    QVBoxLayout *verticalLayout;
    QLabel *monLabel;
    QGroupBox *optGrp;
    QGridLayout *gridLayout1;
    QRadioButton *optAll;
    QRadioButton *optSel;
    QRadioButton *optMonGrps;
    QSpacerItem *monSpacer;
    QHBoxLayout *bottomLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer2;
    QPushButton *cancelButton;

    void setupUi(QDialog *GpsMonitor)
    {
        if (GpsMonitor->objectName().isEmpty())
            GpsMonitor->setObjectName(QString::fromUtf8("GpsMonitor"));
        GpsMonitor->resize(750, 551);
        gridLayout = new QGridLayout(GpsMonitor);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout1 = new QHBoxLayout();
        horizontalLayout1->setSpacing(0);
        horizontalLayout1->setObjectName(QString::fromUtf8("horizontalLayout1"));
        horizontalLayout1->setContentsMargins(-1, -1, -1, 0);
        rscLayout = new QVBoxLayout();
        rscLayout->setSpacing(0);
        rscLayout->setObjectName(QString::fromUtf8("rscLayout"));
        rscLayout->setContentsMargins(0, -1, 0, -1);
        searchFrame = new QFrame(GpsMonitor);
        searchFrame->setObjectName(QString::fromUtf8("searchFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchFrame->sizePolicy().hasHeightForWidth());
        searchFrame->setSizePolicy(sizePolicy);
        searchFrame->setMinimumSize(QSize(0, 0));
        searchFrame->setMaximumSize(QSize(16777215, 16777215));
        searchFrame->setFrameShape(QFrame::StyledPanel);
        searchFrame->setFrameShadow(QFrame::Raised);
        verticalLayout1 = new QVBoxLayout(searchFrame);
        verticalLayout1->setSpacing(0);
        verticalLayout1->setObjectName(QString::fromUtf8("verticalLayout1"));
        verticalLayout1->setContentsMargins(5, 0, 0, 0);
        searchLabel = new QLabel(searchFrame);
        searchLabel->setObjectName(QString::fromUtf8("searchLabel"));

        verticalLayout1->addWidget(searchLabel);


        rscLayout->addWidget(searchFrame);

        fieldFrame = new QFrame(GpsMonitor);
        fieldFrame->setObjectName(QString::fromUtf8("fieldFrame"));
        sizePolicy.setHeightForWidth(fieldFrame->sizePolicy().hasHeightForWidth());
        fieldFrame->setSizePolicy(sizePolicy);
        fieldFrame->setFrameShape(QFrame::StyledPanel);
        fieldFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout2 = new QHBoxLayout(fieldFrame);
        horizontalLayout2->setSpacing(0);
        horizontalLayout2->setObjectName(QString::fromUtf8("horizontalLayout2"));
        horizontalLayout2->setContentsMargins(0, 0, 0, 0);
        searchEdit = new QLineEdit(fieldFrame);
        searchEdit->setObjectName(QString::fromUtf8("searchEdit"));

        horizontalLayout2->addWidget(searchEdit);


        rscLayout->addWidget(fieldFrame);

        rscFrame = new QFrame(GpsMonitor);
        rscFrame->setObjectName(QString::fromUtf8("rscFrame"));
        sizePolicy.setHeightForWidth(rscFrame->sizePolicy().hasHeightForWidth());
        rscFrame->setSizePolicy(sizePolicy);
        rscFrame->setFrameShape(QFrame::StyledPanel);
        rscFrame->setFrameShadow(QFrame::Raised);
        verticalLayout2 = new QVBoxLayout(rscFrame);
        verticalLayout2->setSpacing(0);
        verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
        verticalLayout2->setContentsMargins(5, 0, 0, 0);

        rscLayout->addWidget(rscFrame);

        rscSpacer = new QSpacerItem(300, 0, QSizePolicy::Maximum, QSizePolicy::Minimum);

        rscLayout->addItem(rscSpacer);


        horizontalLayout1->addLayout(rscLayout);

        middleFrame = new QFrame(GpsMonitor);
        middleFrame->setObjectName(QString::fromUtf8("middleFrame"));
        middleFrame->setMinimumSize(QSize(150, 0));
        middleFrame->setFrameShape(QFrame::StyledPanel);
        middleFrame->setFrameShadow(QFrame::Plain);
        verticalLayout3 = new QVBoxLayout(middleFrame);
        verticalLayout3->setSpacing(0);
        verticalLayout3->setObjectName(QString::fromUtf8("verticalLayout3"));
        verticalLayout3->setContentsMargins(5, 0, 5, 0);
        addButton = new QPushButton(middleFrame);
        addButton->setObjectName(QString::fromUtf8("addButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addButton->sizePolicy().hasHeightForWidth());
        addButton->setSizePolicy(sizePolicy1);
        addButton->setMinimumSize(QSize(0, 0));
        addButton->setMaximumSize(QSize(16777215, 70));

        verticalLayout3->addWidget(addButton);

        removeButton = new QPushButton(middleFrame);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));
        removeButton->setMaximumSize(QSize(16777215, 70));

        verticalLayout3->addWidget(removeButton);

        removeAllButton = new QPushButton(middleFrame);
        removeAllButton->setObjectName(QString::fromUtf8("removeAllButton"));
        removeAllButton->setMaximumSize(QSize(16777215, 70));

        verticalLayout3->addWidget(removeAllButton);


        horizontalLayout1->addWidget(middleFrame);

        monLayout = new QVBoxLayout();
        monLayout->setSpacing(0);
        monLayout->setObjectName(QString::fromUtf8("monLayout"));
        monLayout->setContentsMargins(0, -1, 0, -1);
        monFrame = new QFrame(GpsMonitor);
        monFrame->setObjectName(QString::fromUtf8("monFrame"));
        sizePolicy.setHeightForWidth(monFrame->sizePolicy().hasHeightForWidth());
        monFrame->setSizePolicy(sizePolicy);
        monFrame->setFrameShape(QFrame::StyledPanel);
        monFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(monFrame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 0, 0, 0);
        monLabel = new QLabel(monFrame);
        monLabel->setObjectName(QString::fromUtf8("monLabel"));

        verticalLayout->addWidget(monLabel);


        monLayout->addWidget(monFrame);

        optGrp = new QGroupBox(GpsMonitor);
        optGrp->setObjectName(QString::fromUtf8("optGrp"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(optGrp->sizePolicy().hasHeightForWidth());
        optGrp->setSizePolicy(sizePolicy2);
        optGrp->setFlat(false);
        gridLayout1 = new QGridLayout(optGrp);
        gridLayout1->setSpacing(0);
        gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
        gridLayout1->setContentsMargins(5, 0, 5, 0);
        optAll = new QRadioButton(optGrp);
        optAll->setObjectName(QString::fromUtf8("optAll"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(optAll->sizePolicy().hasHeightForWidth());
        optAll->setSizePolicy(sizePolicy3);

        gridLayout1->addWidget(optAll, 0, 0, 1, 1);

        optSel = new QRadioButton(optGrp);
        optSel->setObjectName(QString::fromUtf8("optSel"));
        sizePolicy3.setHeightForWidth(optSel->sizePolicy().hasHeightForWidth());
        optSel->setSizePolicy(sizePolicy3);

        gridLayout1->addWidget(optSel, 0, 1, 1, 1);

        optMonGrps = new QRadioButton(optGrp);
        optMonGrps->setObjectName(QString::fromUtf8("optMonGrps"));
        sizePolicy3.setHeightForWidth(optMonGrps->sizePolicy().hasHeightForWidth());
        optMonGrps->setSizePolicy(sizePolicy3);

        gridLayout1->addWidget(optMonGrps, 0, 2, 1, 1);


        monLayout->addWidget(optGrp);

        monSpacer = new QSpacerItem(300, 0, QSizePolicy::Maximum, QSizePolicy::Minimum);

        monLayout->addItem(monSpacer);


        horizontalLayout1->addLayout(monLayout);


        gridLayout->addLayout(horizontalLayout1, 0, 0, 1, 1);

        bottomLayout = new QHBoxLayout();
        bottomLayout->setSpacing(0);
        bottomLayout->setObjectName(QString::fromUtf8("bottomLayout"));
        bottomLayout->setContentsMargins(-1, 5, 5, 5);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        bottomLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(GpsMonitor);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setMinimumSize(QSize(90, 40));

        bottomLayout->addWidget(okButton);

        horizontalSpacer2 = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        bottomLayout->addItem(horizontalSpacer2);

        cancelButton = new QPushButton(GpsMonitor);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setMinimumSize(QSize(90, 40));

        bottomLayout->addWidget(cancelButton);


        gridLayout->addLayout(bottomLayout, 1, 0, 1, 1);


        retranslateUi(GpsMonitor);

        QMetaObject::connectSlotsByName(GpsMonitor);
    } // setupUi

    void retranslateUi(QDialog *GpsMonitor)
    {
        GpsMonitor->setWindowTitle(QCoreApplication::translate("GpsMonitor", "GPS Monitoring", nullptr));
        searchLabel->setText(QCoreApplication::translate("GpsMonitor", "Search", nullptr));
        addButton->setText(QCoreApplication::translate("GpsMonitor", "Add >>", nullptr));
        removeButton->setText(QCoreApplication::translate("GpsMonitor", "<< Remove", nullptr));
        removeAllButton->setText(QCoreApplication::translate("GpsMonitor", "<< Remove All", nullptr));
        monLabel->setText(QCoreApplication::translate("GpsMonitor", "Monitored", nullptr));
        optGrp->setTitle(QString());
        optAll->setText(QCoreApplication::translate("GpsMonitor", "All", nullptr));
        optSel->setText(QCoreApplication::translate("GpsMonitor", "Selected", nullptr));
#if QT_CONFIG(tooltip)
        optMonGrps->setToolTip(QCoreApplication::translate("GpsMonitor", "Monitored group attached members", nullptr));
#endif // QT_CONFIG(tooltip)
        optMonGrps->setText(QCoreApplication::translate("GpsMonitor", "Monitored Groups", nullptr));
        okButton->setText(QCoreApplication::translate("GpsMonitor", "OK", nullptr));
        cancelButton->setText(QCoreApplication::translate("GpsMonitor", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GpsMonitor: public Ui_GpsMonitor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GPSMONITOR_H
