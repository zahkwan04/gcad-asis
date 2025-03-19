/********************************************************************************
** Form generated from reading UI file 'Dgna.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DGNA_H
#define UI_DGNA_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
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

class Ui_Dgna
{
public:
    QGridLayout *gridLayout_4;
    QHBoxLayout *bottomHLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *doneButton;
    QHBoxLayout *horizontalLayout_6;
    QVBoxLayout *searchLayout;
    QFrame *searchFrame;
    QVBoxLayout *verticalLayout_16;
    QLabel *searchLabel;
    QFrame *fieldFrame;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *searchEdit;
    QFrame *resourcesFrame;
    QVBoxLayout *verticalLayout_17;
    QLabel *categoryLabel;
    QSpacerItem *resourcesHSpacer;
    QSpacerItem *middleVSpacer;
    QFrame *middleFrame;
    QVBoxLayout *verticalLayout_21;
    QPushButton *addButton;
    QPushButton *removeButton;
    QPushButton *removeAllButton;
    QVBoxLayout *addVLayout;
    QFrame *dgnaGrpFrame;
    QVBoxLayout *verticalLayout_19;
    QLabel *dgnaGroupLabel;
    QFrame *dgnaFieldFrame;
    QHBoxLayout *horizontalLayout_8;
    QLineEdit *dgnaGrpEdit;
    QFrame *addFrame;
    QVBoxLayout *verticalLayout;
    QLabel *addedLabel;
    QSpacerItem *addHSpacer;

    void setupUi(QDialog *Dgna)
    {
        if (Dgna->objectName().isEmpty())
            Dgna->setObjectName(QString::fromUtf8("Dgna"));
        Dgna->resize(746, 551);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_main.png"), QSize(), QIcon::Normal, QIcon::Off);
        Dgna->setWindowIcon(icon);
        gridLayout_4 = new QGridLayout(Dgna);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        bottomHLayout = new QHBoxLayout();
        bottomHLayout->setSpacing(0);
        bottomHLayout->setObjectName(QString::fromUtf8("bottomHLayout"));
        bottomHLayout->setContentsMargins(-1, 5, 5, 5);
        horizontalSpacer = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        bottomHLayout->addItem(horizontalSpacer);

        doneButton = new QPushButton(Dgna);
        doneButton->setObjectName(QString::fromUtf8("doneButton"));
        doneButton->setMinimumSize(QSize(90, 40));

        bottomHLayout->addWidget(doneButton);


        gridLayout_4->addLayout(bottomHLayout, 1, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, -1, -1, 0);
        searchLayout = new QVBoxLayout();
        searchLayout->setSpacing(0);
        searchLayout->setObjectName(QString::fromUtf8("searchLayout"));
        searchLayout->setContentsMargins(0, -1, 0, -1);
        searchFrame = new QFrame(Dgna);
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
        verticalLayout_16 = new QVBoxLayout(searchFrame);
        verticalLayout_16->setSpacing(0);
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        verticalLayout_16->setContentsMargins(5, 0, 0, 0);
        searchLabel = new QLabel(searchFrame);
        searchLabel->setObjectName(QString::fromUtf8("searchLabel"));

        verticalLayout_16->addWidget(searchLabel);


        searchLayout->addWidget(searchFrame);

        fieldFrame = new QFrame(Dgna);
        fieldFrame->setObjectName(QString::fromUtf8("fieldFrame"));
        sizePolicy.setHeightForWidth(fieldFrame->sizePolicy().hasHeightForWidth());
        fieldFrame->setSizePolicy(sizePolicy);
        fieldFrame->setFrameShape(QFrame::StyledPanel);
        fieldFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_7 = new QHBoxLayout(fieldFrame);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        searchEdit = new QLineEdit(fieldFrame);
        searchEdit->setObjectName(QString::fromUtf8("searchEdit"));

        horizontalLayout_7->addWidget(searchEdit);


        searchLayout->addWidget(fieldFrame);

        resourcesFrame = new QFrame(Dgna);
        resourcesFrame->setObjectName(QString::fromUtf8("resourcesFrame"));
        sizePolicy.setHeightForWidth(resourcesFrame->sizePolicy().hasHeightForWidth());
        resourcesFrame->setSizePolicy(sizePolicy);
        resourcesFrame->setFrameShape(QFrame::StyledPanel);
        resourcesFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_17 = new QVBoxLayout(resourcesFrame);
        verticalLayout_17->setSpacing(0);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        verticalLayout_17->setContentsMargins(5, 0, 0, 0);
        categoryLabel = new QLabel(resourcesFrame);
        categoryLabel->setObjectName(QString::fromUtf8("categoryLabel"));

        verticalLayout_17->addWidget(categoryLabel);


        searchLayout->addWidget(resourcesFrame);

        resourcesHSpacer = new QSpacerItem(300, 0, QSizePolicy::Maximum, QSizePolicy::Minimum);

        searchLayout->addItem(resourcesHSpacer);


        horizontalLayout_6->addLayout(searchLayout);

        middleVSpacer = new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout_6->addItem(middleVSpacer);

        middleFrame = new QFrame(Dgna);
        middleFrame->setObjectName(QString::fromUtf8("middleFrame"));
        middleFrame->setMinimumSize(QSize(90, 0));
        middleFrame->setFrameShape(QFrame::StyledPanel);
        middleFrame->setFrameShadow(QFrame::Plain);
        verticalLayout_21 = new QVBoxLayout(middleFrame);
        verticalLayout_21->setSpacing(0);
        verticalLayout_21->setObjectName(QString::fromUtf8("verticalLayout_21"));
        verticalLayout_21->setContentsMargins(5, 0, 5, 0);
        addButton = new QPushButton(middleFrame);
        addButton->setObjectName(QString::fromUtf8("addButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addButton->sizePolicy().hasHeightForWidth());
        addButton->setSizePolicy(sizePolicy1);
        addButton->setMinimumSize(QSize(0, 0));
        addButton->setMaximumSize(QSize(16777215, 70));

        verticalLayout_21->addWidget(addButton);

        removeButton = new QPushButton(middleFrame);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));
        removeButton->setMaximumSize(QSize(16777215, 70));

        verticalLayout_21->addWidget(removeButton);

        removeAllButton = new QPushButton(middleFrame);
        removeAllButton->setObjectName(QString::fromUtf8("removeAllButton"));
        removeAllButton->setMaximumSize(QSize(16777215, 70));

        verticalLayout_21->addWidget(removeAllButton);


        horizontalLayout_6->addWidget(middleFrame);

        addVLayout = new QVBoxLayout();
        addVLayout->setSpacing(0);
        addVLayout->setObjectName(QString::fromUtf8("addVLayout"));
        addVLayout->setContentsMargins(0, -1, 0, -1);
        dgnaGrpFrame = new QFrame(Dgna);
        dgnaGrpFrame->setObjectName(QString::fromUtf8("dgnaGrpFrame"));
        sizePolicy.setHeightForWidth(dgnaGrpFrame->sizePolicy().hasHeightForWidth());
        dgnaGrpFrame->setSizePolicy(sizePolicy);
        dgnaGrpFrame->setFrameShape(QFrame::StyledPanel);
        dgnaGrpFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_19 = new QVBoxLayout(dgnaGrpFrame);
        verticalLayout_19->setSpacing(0);
        verticalLayout_19->setObjectName(QString::fromUtf8("verticalLayout_19"));
        verticalLayout_19->setContentsMargins(5, 0, 0, 0);
        dgnaGroupLabel = new QLabel(dgnaGrpFrame);
        dgnaGroupLabel->setObjectName(QString::fromUtf8("dgnaGroupLabel"));

        verticalLayout_19->addWidget(dgnaGroupLabel);


        addVLayout->addWidget(dgnaGrpFrame);

        dgnaFieldFrame = new QFrame(Dgna);
        dgnaFieldFrame->setObjectName(QString::fromUtf8("dgnaFieldFrame"));
        sizePolicy.setHeightForWidth(dgnaFieldFrame->sizePolicy().hasHeightForWidth());
        dgnaFieldFrame->setSizePolicy(sizePolicy);
        dgnaFieldFrame->setFrameShape(QFrame::StyledPanel);
        dgnaFieldFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_8 = new QHBoxLayout(dgnaFieldFrame);
        horizontalLayout_8->setSpacing(0);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        dgnaGrpEdit = new QLineEdit(dgnaFieldFrame);
        dgnaGrpEdit->setObjectName(QString::fromUtf8("dgnaGrpEdit"));
        dgnaGrpEdit->setReadOnly(true);

        horizontalLayout_8->addWidget(dgnaGrpEdit);


        addVLayout->addWidget(dgnaFieldFrame);

        addFrame = new QFrame(Dgna);
        addFrame->setObjectName(QString::fromUtf8("addFrame"));
        sizePolicy.setHeightForWidth(addFrame->sizePolicy().hasHeightForWidth());
        addFrame->setSizePolicy(sizePolicy);
        addFrame->setFrameShape(QFrame::StyledPanel);
        addFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(addFrame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(5, 0, 0, 0);
        addedLabel = new QLabel(addFrame);
        addedLabel->setObjectName(QString::fromUtf8("addedLabel"));

        verticalLayout->addWidget(addedLabel);


        addVLayout->addWidget(addFrame);

        addHSpacer = new QSpacerItem(300, 0, QSizePolicy::Maximum, QSizePolicy::Minimum);

        addVLayout->addItem(addHSpacer);


        horizontalLayout_6->addLayout(addVLayout);


        gridLayout_4->addLayout(horizontalLayout_6, 0, 0, 1, 1);


        retranslateUi(Dgna);

        QMetaObject::connectSlotsByName(Dgna);
    } // setupUi

    void retranslateUi(QDialog *Dgna)
    {
        Dgna->setWindowTitle(QCoreApplication::translate("Dgna", "DGNA Assignment", nullptr));
        doneButton->setText(QCoreApplication::translate("Dgna", "OK", nullptr));
        searchLabel->setText(QCoreApplication::translate("Dgna", "Search", nullptr));
        categoryLabel->setText(QString());
        addButton->setText(QCoreApplication::translate("Dgna", "Add >>", nullptr));
        removeButton->setText(QCoreApplication::translate("Dgna", "<< Remove", nullptr));
        removeAllButton->setText(QCoreApplication::translate("Dgna", "<< Remove All", nullptr));
        dgnaGroupLabel->setText(QCoreApplication::translate("Dgna", "DGNA Group", nullptr));
        addedLabel->setText(QCoreApplication::translate("Dgna", "Added Members", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dgna: public Ui_Dgna {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DGNA_H
