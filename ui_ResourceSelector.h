/********************************************************************************
** Form generated from reading UI file 'ResourceSelector.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESOURCESELECTOR_H
#define UI_RESOURCESELECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ResourceSelector
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *categoryScroll;
    QGroupBox *categoryBox;
    QHBoxLayout *categoryLayout;
    QRadioButton *subscriber;
    QRadioButton *talkGroup;
    QRadioButton *dgnaInd;
    QRadioButton *dgnaGrp;
    QRadioButton *mobile;
    QRadioButton *dispatcher;
    QGridLayout *gridLayout_2;
    QLabel *toLabel;
    QSpacerItem *horizontalSpacer;
    QLineEdit *nameEdit;
    QToolButton *viewButton;
    QFrame *recFrame;
    QGridLayout *gridLayout3;
    QPushButton *addButton;
    QLineEdit *recEdit;
    QToolButton *selRightButton;
    QToolButton *selLeftButton;
    QToolButton *clrButton;
    QButtonGroup *categoryGroup;

    void setupUi(QWidget *ResourceSelector)
    {
        if (ResourceSelector->objectName().isEmpty())
            ResourceSelector->setObjectName(QString::fromUtf8("ResourceSelector"));
        ResourceSelector->resize(660, 250);
        ResourceSelector->setMinimumSize(QSize(0, 0));
        ResourceSelector->setMaximumSize(QSize(16777215, 16777215));
        verticalLayout = new QVBoxLayout(ResourceSelector);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        categoryScroll = new QScrollArea(ResourceSelector);
        categoryScroll->setObjectName(QString::fromUtf8("categoryScroll"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(categoryScroll->sizePolicy().hasHeightForWidth());
        categoryScroll->setSizePolicy(sizePolicy);
        categoryScroll->setMaximumSize(QSize(16777215, 80));
        categoryScroll->setFrameShape(QFrame::NoFrame);
        categoryScroll->setWidgetResizable(true);
        categoryBox = new QGroupBox();
        categoryBox->setObjectName(QString::fromUtf8("categoryBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(categoryBox->sizePolicy().hasHeightForWidth());
        categoryBox->setSizePolicy(sizePolicy1);
        categoryBox->setFlat(false);
        categoryLayout = new QHBoxLayout(categoryBox);
        categoryLayout->setSpacing(5);
        categoryLayout->setObjectName(QString::fromUtf8("categoryLayout"));
        categoryLayout->setContentsMargins(5, 0, 5, 0);
        subscriber = new QRadioButton(categoryBox);
        categoryGroup = new QButtonGroup(ResourceSelector);
        categoryGroup->setObjectName(QString::fromUtf8("categoryGroup"));
        categoryGroup->addButton(subscriber);
        subscriber->setObjectName(QString::fromUtf8("subscriber"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(subscriber->sizePolicy().hasHeightForWidth());
        subscriber->setSizePolicy(sizePolicy2);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_mobile.png"), QSize(), QIcon::Normal, QIcon::Off);
        subscriber->setIcon(icon);
        subscriber->setIconSize(QSize(24, 24));

        categoryLayout->addWidget(subscriber);

        talkGroup = new QRadioButton(categoryBox);
        categoryGroup->addButton(talkGroup);
        talkGroup->setObjectName(QString::fromUtf8("talkGroup"));
        sizePolicy2.setHeightForWidth(talkGroup->sizePolicy().hasHeightForWidth());
        talkGroup->setSizePolicy(sizePolicy2);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_group.png"), QSize(), QIcon::Normal, QIcon::Off);
        talkGroup->setIcon(icon1);
        talkGroup->setIconSize(QSize(24, 24));

        categoryLayout->addWidget(talkGroup);

        dgnaInd = new QRadioButton(categoryBox);
        categoryGroup->addButton(dgnaInd);
        dgnaInd->setObjectName(QString::fromUtf8("dgnaInd"));
        sizePolicy2.setHeightForWidth(dgnaInd->sizePolicy().hasHeightForWidth());
        dgnaInd->setSizePolicy(sizePolicy2);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_dgna_ind.png"), QSize(), QIcon::Normal, QIcon::Off);
        dgnaInd->setIcon(icon2);
        dgnaInd->setIconSize(QSize(24, 24));

        categoryLayout->addWidget(dgnaInd);

        dgnaGrp = new QRadioButton(categoryBox);
        categoryGroup->addButton(dgnaGrp);
        dgnaGrp->setObjectName(QString::fromUtf8("dgnaGrp"));
        sizePolicy2.setHeightForWidth(dgnaGrp->sizePolicy().hasHeightForWidth());
        dgnaGrp->setSizePolicy(sizePolicy2);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_dgna_grp.png"), QSize(), QIcon::Normal, QIcon::Off);
        dgnaGrp->setIcon(icon3);
        dgnaGrp->setIconSize(QSize(24, 24));

        categoryLayout->addWidget(dgnaGrp);

        mobile = new QRadioButton(categoryBox);
        categoryGroup->addButton(mobile);
        mobile->setObjectName(QString::fromUtf8("mobile"));
        sizePolicy2.setHeightForWidth(mobile->sizePolicy().hasHeightForWidth());
        mobile->setSizePolicy(sizePolicy2);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_mobile_phone.png"), QSize(), QIcon::Normal, QIcon::Off);
        mobile->setIcon(icon4);
        mobile->setIconSize(QSize(24, 24));

        categoryLayout->addWidget(mobile);

        dispatcher = new QRadioButton(categoryBox);
        categoryGroup->addButton(dispatcher);
        dispatcher->setObjectName(QString::fromUtf8("dispatcher"));
        sizePolicy2.setHeightForWidth(dispatcher->sizePolicy().hasHeightForWidth());
        dispatcher->setSizePolicy(sizePolicy2);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_disp.png"), QSize(), QIcon::Normal, QIcon::Off);
        dispatcher->setIcon(icon5);
        dispatcher->setIconSize(QSize(24, 24));

        categoryLayout->addWidget(dispatcher);

        categoryScroll->setWidget(categoryBox);

        verticalLayout->addWidget(categoryScroll);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        toLabel = new QLabel(ResourceSelector);
        toLabel->setObjectName(QString::fromUtf8("toLabel"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(toLabel->sizePolicy().hasHeightForWidth());
        toLabel->setSizePolicy(sizePolicy3);
        toLabel->setMinimumSize(QSize(0, 0));

        gridLayout_2->addWidget(toLabel, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 0, 1, 1, 1);

        nameEdit = new QLineEdit(ResourceSelector);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));
        nameEdit->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(nameEdit, 0, 2, 1, 1);

        viewButton = new QToolButton(ResourceSelector);
        viewButton->setObjectName(QString::fromUtf8("viewButton"));
        viewButton->setMaximumSize(QSize(20, 30));
        viewButton->setArrowType(Qt::DownArrow);

        gridLayout_2->addWidget(viewButton, 0, 3, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        recFrame = new QFrame(ResourceSelector);
        recFrame->setObjectName(QString::fromUtf8("recFrame"));
        gridLayout3 = new QGridLayout(recFrame);
        gridLayout3->setSpacing(5);
        gridLayout3->setObjectName(QString::fromUtf8("gridLayout3"));
        gridLayout3->setSizeConstraint(QLayout::SetMinimumSize);
        gridLayout3->setContentsMargins(0, 5, 0, 0);
        addButton = new QPushButton(recFrame);
        addButton->setObjectName(QString::fromUtf8("addButton"));
        addButton->setEnabled(false);
        addButton->setMinimumSize(QSize(200, 40));

        gridLayout3->addWidget(addButton, 0, 0, 1, 1);

        recEdit = new QLineEdit(recFrame);
        recEdit->setObjectName(QString::fromUtf8("recEdit"));
        recEdit->setMinimumSize(QSize(0, 30));
        recEdit->setReadOnly(true);

        gridLayout3->addWidget(recEdit, 0, 1, 1, 1);

        selRightButton = new QToolButton(recFrame);
        selRightButton->setObjectName(QString::fromUtf8("selRightButton"));
        selRightButton->setMinimumSize(QSize(40, 40));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_to_right.png"), QSize(), QIcon::Normal, QIcon::Off);
        selRightButton->setIcon(icon6);
        selRightButton->setIconSize(QSize(30, 30));

        gridLayout3->addWidget(selRightButton, 0, 2, 1, 1);

        selLeftButton = new QToolButton(recFrame);
        selLeftButton->setObjectName(QString::fromUtf8("selLeftButton"));
        selLeftButton->setMinimumSize(QSize(40, 40));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_to_left.png"), QSize(), QIcon::Normal, QIcon::Off);
        selLeftButton->setIcon(icon7);
        selLeftButton->setIconSize(QSize(30, 30));

        gridLayout3->addWidget(selLeftButton, 0, 3, 1, 1);

        clrButton = new QToolButton(recFrame);
        clrButton->setObjectName(QString::fromUtf8("clrButton"));
        clrButton->setMinimumSize(QSize(40, 40));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Images/images/icon_clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        clrButton->setIcon(icon8);
        clrButton->setIconSize(QSize(30, 30));

        gridLayout3->addWidget(clrButton, 0, 4, 1, 1);


        verticalLayout->addWidget(recFrame);


        retranslateUi(ResourceSelector);

        QMetaObject::connectSlotsByName(ResourceSelector);
    } // setupUi

    void retranslateUi(QWidget *ResourceSelector)
    {
        ResourceSelector->setWindowTitle(QCoreApplication::translate("ResourceSelector", "Form", nullptr));
        toLabel->setText(QCoreApplication::translate("ResourceSelector", "To :", nullptr));
        viewButton->setText(QCoreApplication::translate("ResourceSelector", "...", nullptr));
        addButton->setText(QCoreApplication::translate("ResourceSelector", "Add To Recipients", nullptr));
#if QT_CONFIG(tooltip)
        selRightButton->setToolTip(QCoreApplication::translate("ResourceSelector", "Select next", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        selLeftButton->setToolTip(QCoreApplication::translate("ResourceSelector", "Select previous", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        clrButton->setToolTip(QCoreApplication::translate("ResourceSelector", "Clear", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class ResourceSelector: public Ui_ResourceSelector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESOURCESELECTOR_H
