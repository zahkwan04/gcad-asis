/********************************************************************************
** Form generated from reading UI file 'GisTrailingSelector.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISTRAILINGSELECTOR_H
#define UI_GISTRAILINGSELECTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GisTrailingSelector
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
    QFrame *middleFrame;
    QVBoxLayout *verticalLayout3;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *removeAllButton;
    QVBoxLayout *trlLayout;
    QFrame *trlFrame;
    QVBoxLayout *verticalLayout;
    QLabel *trlLabel;
    QHBoxLayout *bottomLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer2;

    void setupUi(QDialog *GisTrailingSelector)
    {
        if (GisTrailingSelector->objectName().isEmpty())
            GisTrailingSelector->setObjectName(QString::fromUtf8("GisTrailingSelector"));
        GisTrailingSelector->resize(600, 500);
        gridLayout = new QGridLayout(GisTrailingSelector);
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
        searchFrame = new QFrame(GisTrailingSelector);
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

        fieldFrame = new QFrame(GisTrailingSelector);
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

        rscFrame = new QFrame(GisTrailingSelector);
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


        horizontalLayout1->addLayout(rscLayout);

        middleFrame = new QFrame(GisTrailingSelector);
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

        trlLayout = new QVBoxLayout();
        trlLayout->setSpacing(0);
        trlLayout->setObjectName(QString::fromUtf8("trlLayout"));
        trlLayout->setContentsMargins(0, -1, 0, -1);
        trlFrame = new QFrame(GisTrailingSelector);
        trlFrame->setObjectName(QString::fromUtf8("trlFrame"));
        sizePolicy.setHeightForWidth(trlFrame->sizePolicy().hasHeightForWidth());
        trlFrame->setSizePolicy(sizePolicy);
        trlFrame->setFrameShape(QFrame::StyledPanel);
        trlFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(trlFrame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 0, 0, 0);
        trlLabel = new QLabel(trlFrame);
        trlLabel->setObjectName(QString::fromUtf8("trlLabel"));

        verticalLayout->addWidget(trlLabel);


        trlLayout->addWidget(trlFrame);


        horizontalLayout1->addLayout(trlLayout);


        gridLayout->addLayout(horizontalLayout1, 0, 0, 1, 1);

        bottomLayout = new QHBoxLayout();
        bottomLayout->setSpacing(0);
        bottomLayout->setObjectName(QString::fromUtf8("bottomLayout"));
        bottomLayout->setContentsMargins(-1, 5, 5, 5);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        bottomLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(GisTrailingSelector);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setMinimumSize(QSize(90, 40));

        bottomLayout->addWidget(okButton);

        horizontalSpacer2 = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        bottomLayout->addItem(horizontalSpacer2);


        gridLayout->addLayout(bottomLayout, 1, 0, 1, 1);


        retranslateUi(GisTrailingSelector);

        QMetaObject::connectSlotsByName(GisTrailingSelector);
    } // setupUi

    void retranslateUi(QDialog *GisTrailingSelector)
    {
        GisTrailingSelector->setWindowTitle(QCoreApplication::translate("GisTrailingSelector", "GIS Trailing Selector", nullptr));
        searchLabel->setText(QCoreApplication::translate("GisTrailingSelector", "Search", nullptr));
        addButton->setText(QCoreApplication::translate("GisTrailingSelector", "Add >>", nullptr));
        removeButton->setText(QCoreApplication::translate("GisTrailingSelector", "<< Remove", nullptr));
        removeAllButton->setText(QCoreApplication::translate("GisTrailingSelector", "<< Remove All", nullptr));
        trlLabel->setText(QCoreApplication::translate("GisTrailingSelector", "Trailing Enabled", nullptr));
        okButton->setText(QCoreApplication::translate("GisTrailingSelector", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GisTrailingSelector: public Ui_GisTrailingSelector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISTRAILINGSELECTOR_H
