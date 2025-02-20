/********************************************************************************
** Form generated from reading UI file 'RscCollector.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RSCCOLLECTOR_H
#define UI_RSCCOLLECTOR_H

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

class Ui_RscCollector
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *topLayout;
    QVBoxLayout *rscLayout;
    QFrame *searchLblFrame;
    QVBoxLayout *searchLblLayout;
    QLabel *searchLbl;
    QFrame *searchTxtFrame;
    QHBoxLayout *searchTxtLayout;
    QLineEdit *searchEdit;
    QFrame *rscFrame;
    QVBoxLayout *rscLayout2;
    QSpacerItem *rscSpacer;
    QFrame *midFrame;
    QVBoxLayout *midLayout;
    QPushButton *addBtn;
    QPushButton *rmvBtn;
    QPushButton *rmvAllBtn;
    QVBoxLayout *selLayout;
    QFrame *selFrame;
    QVBoxLayout *verticalLayout;
    QLabel *selLabel;
    QSpacerItem *selSpacer;
    QHBoxLayout *btmLayout;
    QSpacerItem *spacer1;
    QPushButton *okBtn;
    QSpacerItem *spacer2;
    QPushButton *cancelBtn;

    void setupUi(QDialog *RscCollector)
    {
        if (RscCollector->objectName().isEmpty())
            RscCollector->setObjectName(QString::fromUtf8("RscCollector"));
        RscCollector->resize(750, 551);
        gridLayout = new QGridLayout(RscCollector);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        topLayout = new QHBoxLayout();
        topLayout->setSpacing(0);
        topLayout->setObjectName(QString::fromUtf8("topLayout"));
        topLayout->setContentsMargins(-1, -1, -1, 0);
        rscLayout = new QVBoxLayout();
        rscLayout->setSpacing(0);
        rscLayout->setObjectName(QString::fromUtf8("rscLayout"));
        rscLayout->setContentsMargins(0, -1, 0, -1);
        searchLblFrame = new QFrame(RscCollector);
        searchLblFrame->setObjectName(QString::fromUtf8("searchLblFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchLblFrame->sizePolicy().hasHeightForWidth());
        searchLblFrame->setSizePolicy(sizePolicy);
        searchLblFrame->setMinimumSize(QSize(0, 0));
        searchLblFrame->setMaximumSize(QSize(16777215, 16777215));
        searchLblFrame->setFrameShape(QFrame::StyledPanel);
        searchLblFrame->setFrameShadow(QFrame::Raised);
        searchLblLayout = new QVBoxLayout(searchLblFrame);
        searchLblLayout->setSpacing(0);
        searchLblLayout->setObjectName(QString::fromUtf8("searchLblLayout"));
        searchLblLayout->setContentsMargins(5, 0, 0, 0);
        searchLbl = new QLabel(searchLblFrame);
        searchLbl->setObjectName(QString::fromUtf8("searchLbl"));

        searchLblLayout->addWidget(searchLbl);


        rscLayout->addWidget(searchLblFrame);

        searchTxtFrame = new QFrame(RscCollector);
        searchTxtFrame->setObjectName(QString::fromUtf8("searchTxtFrame"));
        sizePolicy.setHeightForWidth(searchTxtFrame->sizePolicy().hasHeightForWidth());
        searchTxtFrame->setSizePolicy(sizePolicy);
        searchTxtFrame->setFrameShape(QFrame::StyledPanel);
        searchTxtFrame->setFrameShadow(QFrame::Raised);
        searchTxtLayout = new QHBoxLayout(searchTxtFrame);
        searchTxtLayout->setSpacing(0);
        searchTxtLayout->setObjectName(QString::fromUtf8("searchTxtLayout"));
        searchTxtLayout->setContentsMargins(0, 0, 0, 0);
        searchEdit = new QLineEdit(searchTxtFrame);
        searchEdit->setObjectName(QString::fromUtf8("searchEdit"));

        searchTxtLayout->addWidget(searchEdit);


        rscLayout->addWidget(searchTxtFrame);

        rscFrame = new QFrame(RscCollector);
        rscFrame->setObjectName(QString::fromUtf8("rscFrame"));
        sizePolicy.setHeightForWidth(rscFrame->sizePolicy().hasHeightForWidth());
        rscFrame->setSizePolicy(sizePolicy);
        rscFrame->setFrameShape(QFrame::StyledPanel);
        rscFrame->setFrameShadow(QFrame::Raised);
        rscLayout2 = new QVBoxLayout(rscFrame);
        rscLayout2->setSpacing(0);
        rscLayout2->setObjectName(QString::fromUtf8("rscLayout2"));
        rscLayout2->setContentsMargins(5, 0, 0, 0);

        rscLayout->addWidget(rscFrame);

        rscSpacer = new QSpacerItem(300, 0, QSizePolicy::Maximum, QSizePolicy::Minimum);

        rscLayout->addItem(rscSpacer);


        topLayout->addLayout(rscLayout);

        midFrame = new QFrame(RscCollector);
        midFrame->setObjectName(QString::fromUtf8("midFrame"));
        midFrame->setMinimumSize(QSize(150, 0));
        midFrame->setFrameShape(QFrame::StyledPanel);
        midFrame->setFrameShadow(QFrame::Plain);
        midLayout = new QVBoxLayout(midFrame);
        midLayout->setSpacing(0);
        midLayout->setObjectName(QString::fromUtf8("midLayout"));
        midLayout->setContentsMargins(5, 0, 5, 0);
        addBtn = new QPushButton(midFrame);
        addBtn->setObjectName(QString::fromUtf8("addBtn"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addBtn->sizePolicy().hasHeightForWidth());
        addBtn->setSizePolicy(sizePolicy1);
        addBtn->setMinimumSize(QSize(0, 0));
        addBtn->setMaximumSize(QSize(16777215, 70));

        midLayout->addWidget(addBtn);

        rmvBtn = new QPushButton(midFrame);
        rmvBtn->setObjectName(QString::fromUtf8("rmvBtn"));
        rmvBtn->setMaximumSize(QSize(16777215, 70));

        midLayout->addWidget(rmvBtn);

        rmvAllBtn = new QPushButton(midFrame);
        rmvAllBtn->setObjectName(QString::fromUtf8("rmvAllBtn"));
        rmvAllBtn->setMaximumSize(QSize(16777215, 70));

        midLayout->addWidget(rmvAllBtn);


        topLayout->addWidget(midFrame);

        selLayout = new QVBoxLayout();
        selLayout->setSpacing(0);
        selLayout->setObjectName(QString::fromUtf8("selLayout"));
        selLayout->setContentsMargins(0, -1, 0, -1);
        selFrame = new QFrame(RscCollector);
        selFrame->setObjectName(QString::fromUtf8("selFrame"));
        sizePolicy.setHeightForWidth(selFrame->sizePolicy().hasHeightForWidth());
        selFrame->setSizePolicy(sizePolicy);
        selFrame->setFrameShape(QFrame::StyledPanel);
        selFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(selFrame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 0, 0, 0);
        selLabel = new QLabel(selFrame);
        selLabel->setObjectName(QString::fromUtf8("selLabel"));

        verticalLayout->addWidget(selLabel);


        selLayout->addWidget(selFrame);

        selSpacer = new QSpacerItem(300, 0, QSizePolicy::Maximum, QSizePolicy::Minimum);

        selLayout->addItem(selSpacer);


        topLayout->addLayout(selLayout);


        gridLayout->addLayout(topLayout, 0, 0, 1, 1);

        btmLayout = new QHBoxLayout();
        btmLayout->setSpacing(0);
        btmLayout->setObjectName(QString::fromUtf8("btmLayout"));
        btmLayout->setContentsMargins(-1, 5, 5, 5);
        spacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        btmLayout->addItem(spacer1);

        okBtn = new QPushButton(RscCollector);
        okBtn->setObjectName(QString::fromUtf8("okBtn"));
        okBtn->setMinimumSize(QSize(90, 40));

        btmLayout->addWidget(okBtn);

        spacer2 = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        btmLayout->addItem(spacer2);

        cancelBtn = new QPushButton(RscCollector);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));
        cancelBtn->setMinimumSize(QSize(90, 40));

        btmLayout->addWidget(cancelBtn);


        gridLayout->addLayout(btmLayout, 1, 0, 1, 1);


        retranslateUi(RscCollector);

        QMetaObject::connectSlotsByName(RscCollector);
    } // setupUi

    void retranslateUi(QDialog *RscCollector)
    {
        RscCollector->setWindowTitle(QCoreApplication::translate("RscCollector", "Resource Selector: ", nullptr));
        searchLbl->setText(QCoreApplication::translate("RscCollector", "Search", nullptr));
        addBtn->setText(QCoreApplication::translate("RscCollector", "Add >>", nullptr));
        rmvBtn->setText(QCoreApplication::translate("RscCollector", "<< Remove", nullptr));
        rmvAllBtn->setText(QCoreApplication::translate("RscCollector", "<< Remove All", nullptr));
        selLabel->setText(QCoreApplication::translate("RscCollector", "Selection", nullptr));
        okBtn->setText(QCoreApplication::translate("RscCollector", "OK", nullptr));
        cancelBtn->setText(QCoreApplication::translate("RscCollector", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RscCollector: public Ui_RscCollector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RSCCOLLECTOR_H
