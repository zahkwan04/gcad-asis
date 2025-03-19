/********************************************************************************
** Form generated from reading UI file 'Resources.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESOURCES_H
#define UI_RESOURCES_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Resources
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *searchFrame;
    QVBoxLayout *searchVLayout;
    QHBoxLayout *searchInpHLayout;
    QComboBox *searchCombo;
    QToolButton *searchButton;
    QScrollArea *resScroll;
    QWidget *resScrollWidget;
    QHBoxLayout *resScrollLayout;
    QLabel *resLabel;
    QLabel *searchItem;
    QSpacerItem *resSpacer1;
    QHBoxLayout *resultLayout;
    QSpacerItem *resSpacer2;
    QFrame *rscFrame;
    QVBoxLayout *rscVLayout;
    QFrame *viewFrame;
    QHBoxLayout *viewHLayout;
    QRadioButton *listView;
    QRadioButton *buttonsView;
    QPushButton *viewAllButton;
    QPushButton *grpAttButton;
    QSpacerItem *vewSpacer2;
    QTabWidget *tabWidget;
    QWidget *subsTab;
    QVBoxLayout *subsVLayout;
    QScrollArea *subsScroll;
    QWidget *talkgroupTab;
    QVBoxLayout *talkgroupVLayout;
    QScrollArea *talkgroupScroll;
    QWidget *dgnaIndTab;
    QVBoxLayout *dgnaIndVLayout;
    QScrollArea *dgnaIndScroll;
    QWidget *dgnaGrpTab;
    QVBoxLayout *dgnaGrpVLayout;
    QScrollArea *dgnaGrpScroll;
    QWidget *mobileTab;
    QVBoxLayout *mobileVLayout;
    QScrollArea *mobileScroll;
    QWidget *dispatcherTab;
    QVBoxLayout *dispatcherVLayout;
    QScrollArea *dispScroll;
    QWidget *phonebookTab;
    QVBoxLayout *phonebookVLayout;
    QScrollArea *phonebookScroll;
    QButtonGroup *viewGroup;

    void setupUi(QWidget *Resources)
    {
        if (Resources->objectName().isEmpty())
            Resources->setObjectName(QString::fromUtf8("Resources"));
        Resources->resize(562, 731);
        verticalLayout = new QVBoxLayout(Resources);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        searchFrame = new QFrame(Resources);
        searchFrame->setObjectName(QString::fromUtf8("searchFrame"));
        searchFrame->setFrameShape(QFrame::StyledPanel);
        searchFrame->setFrameShadow(QFrame::Raised);
        searchVLayout = new QVBoxLayout(searchFrame);
        searchVLayout->setSpacing(0);
        searchVLayout->setObjectName(QString::fromUtf8("searchVLayout"));
        searchVLayout->setContentsMargins(5, 5, 5, 0);
        searchInpHLayout = new QHBoxLayout();
        searchInpHLayout->setSpacing(0);
        searchInpHLayout->setObjectName(QString::fromUtf8("searchInpHLayout"));
        searchInpHLayout->setContentsMargins(-1, 10, -1, -1);
        searchCombo = new QComboBox(searchFrame);
        searchCombo->setObjectName(QString::fromUtf8("searchCombo"));
        searchCombo->setMinimumSize(QSize(0, 30));
        searchCombo->setEditable(true);
        searchCombo->setMaxCount(20);
        searchCombo->setInsertPolicy(QComboBox::NoInsert);
        searchCombo->setDuplicatesEnabled(false);
        searchCombo->setFrame(true);

        searchInpHLayout->addWidget(searchCombo);

        searchButton = new QToolButton(searchFrame);
        searchButton->setObjectName(QString::fromUtf8("searchButton"));
        searchButton->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(searchButton->sizePolicy().hasHeightForWidth());
        searchButton->setSizePolicy(sizePolicy);
        searchButton->setMinimumSize(QSize(80, 30));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_search.png"), QSize(), QIcon::Normal, QIcon::On);
        searchButton->setIcon(icon);
        searchButton->setIconSize(QSize(30, 30));
        searchButton->setAutoRaise(true);

        searchInpHLayout->addWidget(searchButton);


        searchVLayout->addLayout(searchInpHLayout);

        resScroll = new QScrollArea(searchFrame);
        resScroll->setObjectName(QString::fromUtf8("resScroll"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(resScroll->sizePolicy().hasHeightForWidth());
        resScroll->setSizePolicy(sizePolicy1);
        resScroll->setMaximumSize(QSize(16777215, 80));
        resScroll->setFrameShape(QFrame::NoFrame);
        resScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        resScroll->setWidgetResizable(true);
        resScrollWidget = new QWidget();
        resScrollWidget->setObjectName(QString::fromUtf8("resScrollWidget"));
        resScrollLayout = new QHBoxLayout(resScrollWidget);
        resScrollLayout->setSpacing(5);
        resScrollLayout->setObjectName(QString::fromUtf8("resScrollLayout"));
        resScrollLayout->setContentsMargins(0, 0, 0, 0);
        resLabel = new QLabel(resScrollWidget);
        resLabel->setObjectName(QString::fromUtf8("resLabel"));

        resScrollLayout->addWidget(resLabel);

        searchItem = new QLabel(resScrollWidget);
        searchItem->setObjectName(QString::fromUtf8("searchItem"));

        resScrollLayout->addWidget(searchItem);

        resSpacer1 = new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);

        resScrollLayout->addItem(resSpacer1);

        resultLayout = new QHBoxLayout();
        resultLayout->setSpacing(10);
        resultLayout->setObjectName(QString::fromUtf8("resultLayout"));
        resultLayout->setContentsMargins(0, 0, 0, 0);

        resScrollLayout->addLayout(resultLayout);

        resSpacer2 = new QSpacerItem(20, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        resScrollLayout->addItem(resSpacer2);

        resScroll->setWidget(resScrollWidget);

        searchVLayout->addWidget(resScroll);


        verticalLayout->addWidget(searchFrame);

        rscFrame = new QFrame(Resources);
        rscFrame->setObjectName(QString::fromUtf8("rscFrame"));
        rscFrame->setFrameShape(QFrame::StyledPanel);
        rscFrame->setFrameShadow(QFrame::Raised);
        rscVLayout = new QVBoxLayout(rscFrame);
        rscVLayout->setSpacing(0);
        rscVLayout->setObjectName(QString::fromUtf8("rscVLayout"));
        rscVLayout->setContentsMargins(0, 0, 0, 0);
        viewFrame = new QFrame(rscFrame);
        viewFrame->setObjectName(QString::fromUtf8("viewFrame"));
        viewFrame->setFrameShape(QFrame::StyledPanel);
        viewFrame->setFrameShadow(QFrame::Raised);
        viewHLayout = new QHBoxLayout(viewFrame);
        viewHLayout->setSpacing(25);
        viewHLayout->setObjectName(QString::fromUtf8("viewHLayout"));
        viewHLayout->setSizeConstraint(QLayout::SetMinimumSize);
        viewHLayout->setContentsMargins(20, 10, 0, 10);
        listView = new QRadioButton(viewFrame);
        viewGroup = new QButtonGroup(Resources);
        viewGroup->setObjectName(QString::fromUtf8("viewGroup"));
        viewGroup->addButton(listView);
        listView->setObjectName(QString::fromUtf8("listView"));
        sizePolicy.setHeightForWidth(listView->sizePolicy().hasHeightForWidth());
        listView->setSizePolicy(sizePolicy);
        listView->setAutoFillBackground(false);

        viewHLayout->addWidget(listView);

        buttonsView = new QRadioButton(viewFrame);
        viewGroup->addButton(buttonsView);
        buttonsView->setObjectName(QString::fromUtf8("buttonsView"));
        sizePolicy.setHeightForWidth(buttonsView->sizePolicy().hasHeightForWidth());
        buttonsView->setSizePolicy(sizePolicy);

        viewHLayout->addWidget(buttonsView);

        viewAllButton = new QPushButton(viewFrame);
        viewAllButton->setObjectName(QString::fromUtf8("viewAllButton"));
        viewAllButton->setMinimumSize(QSize(90, 40));

        viewHLayout->addWidget(viewAllButton);

        grpAttButton = new QPushButton(viewFrame);
        grpAttButton->setObjectName(QString::fromUtf8("grpAttButton"));
        grpAttButton->setMinimumSize(QSize(220, 40));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_group_attach.png"), QSize(), QIcon::Normal, QIcon::On);
        grpAttButton->setIcon(icon1);
        grpAttButton->setIconSize(QSize(30, 30));
        grpAttButton->setContextMenuPolicy(Qt::CustomContextMenu);

        viewHLayout->addWidget(grpAttButton);

        vewSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        viewHLayout->addItem(vewSpacer2);


        rscVLayout->addWidget(viewFrame);

        tabWidget = new QTabWidget(rscFrame);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy2);
        tabWidget->setIconSize(QSize(28, 28));
        subsTab = new QWidget();
        subsTab->setObjectName(QString::fromUtf8("subsTab"));
        subsVLayout = new QVBoxLayout(subsTab);
        subsVLayout->setSpacing(0);
        subsVLayout->setObjectName(QString::fromUtf8("subsVLayout"));
        subsVLayout->setContentsMargins(10, 0, 0, 0);
        subsScroll = new QScrollArea(subsTab);
        subsScroll->setObjectName(QString::fromUtf8("subsScroll"));
        subsScroll->setFrameShape(QFrame::NoFrame);
        subsScroll->setWidgetResizable(true);

        subsVLayout->addWidget(subsScroll);

        tabWidget->addTab(subsTab, QString());
        talkgroupTab = new QWidget();
        talkgroupTab->setObjectName(QString::fromUtf8("talkgroupTab"));
        talkgroupVLayout = new QVBoxLayout(talkgroupTab);
        talkgroupVLayout->setSpacing(0);
        talkgroupVLayout->setObjectName(QString::fromUtf8("talkgroupVLayout"));
        talkgroupVLayout->setContentsMargins(0, 0, 0, 0);
        talkgroupScroll = new QScrollArea(talkgroupTab);
        talkgroupScroll->setObjectName(QString::fromUtf8("talkgroupScroll"));
        talkgroupScroll->setFrameShape(QFrame::NoFrame);
        talkgroupScroll->setWidgetResizable(true);

        talkgroupVLayout->addWidget(talkgroupScroll);

        tabWidget->addTab(talkgroupTab, QString());
        dgnaIndTab = new QWidget();
        dgnaIndTab->setObjectName(QString::fromUtf8("dgnaIndTab"));
        dgnaIndVLayout = new QVBoxLayout(dgnaIndTab);
        dgnaIndVLayout->setSpacing(0);
        dgnaIndVLayout->setObjectName(QString::fromUtf8("dgnaIndVLayout"));
        dgnaIndVLayout->setContentsMargins(0, 0, 0, 0);
        dgnaIndScroll = new QScrollArea(dgnaIndTab);
        dgnaIndScroll->setObjectName(QString::fromUtf8("dgnaIndScroll"));
        dgnaIndScroll->setFrameShape(QFrame::NoFrame);
        dgnaIndScroll->setWidgetResizable(true);

        dgnaIndVLayout->addWidget(dgnaIndScroll);

        tabWidget->addTab(dgnaIndTab, QString());
        dgnaGrpTab = new QWidget();
        dgnaGrpTab->setObjectName(QString::fromUtf8("dgnaGrpTab"));
        dgnaGrpVLayout = new QVBoxLayout(dgnaGrpTab);
        dgnaGrpVLayout->setSpacing(0);
        dgnaGrpVLayout->setObjectName(QString::fromUtf8("dgnaGrpVLayout"));
        dgnaGrpVLayout->setContentsMargins(0, 0, 0, 0);
        dgnaGrpScroll = new QScrollArea(dgnaGrpTab);
        dgnaGrpScroll->setObjectName(QString::fromUtf8("dgnaGrpScroll"));
        dgnaGrpScroll->setFrameShape(QFrame::NoFrame);
        dgnaGrpScroll->setWidgetResizable(true);

        dgnaGrpVLayout->addWidget(dgnaGrpScroll);

        tabWidget->addTab(dgnaGrpTab, QString());
        mobileTab = new QWidget();
        mobileTab->setObjectName(QString::fromUtf8("mobileTab"));
        mobileVLayout = new QVBoxLayout(mobileTab);
        mobileVLayout->setSpacing(0);
        mobileVLayout->setObjectName(QString::fromUtf8("mobileVLayout"));
        mobileVLayout->setContentsMargins(0, 0, 0, 0);
        mobileScroll = new QScrollArea(mobileTab);
        mobileScroll->setObjectName(QString::fromUtf8("mobileScroll"));
        mobileScroll->setFrameShape(QFrame::NoFrame);
        mobileScroll->setWidgetResizable(true);

        mobileVLayout->addWidget(mobileScroll);

        tabWidget->addTab(mobileTab, QString());
        dispatcherTab = new QWidget();
        dispatcherTab->setObjectName(QString::fromUtf8("dispatcherTab"));
        dispatcherVLayout = new QVBoxLayout(dispatcherTab);
        dispatcherVLayout->setSpacing(0);
        dispatcherVLayout->setObjectName(QString::fromUtf8("dispatcherVLayout"));
        dispatcherVLayout->setContentsMargins(0, 0, 0, 0);
        dispScroll = new QScrollArea(dispatcherTab);
        dispScroll->setObjectName(QString::fromUtf8("dispScroll"));
        dispScroll->setFrameShape(QFrame::NoFrame);
        dispScroll->setWidgetResizable(true);

        dispatcherVLayout->addWidget(dispScroll);

        tabWidget->addTab(dispatcherTab, QString());
        phonebookTab = new QWidget();
        phonebookTab->setObjectName(QString::fromUtf8("phonebookTab"));
        phonebookVLayout = new QVBoxLayout(phonebookTab);
        phonebookVLayout->setSpacing(0);
        phonebookVLayout->setObjectName(QString::fromUtf8("phonebookVLayout"));
        phonebookVLayout->setContentsMargins(0, 0, 0, 0);
        phonebookScroll = new QScrollArea(phonebookTab);
        phonebookScroll->setObjectName(QString::fromUtf8("phonebookScroll"));
        phonebookScroll->setFrameShape(QFrame::NoFrame);
        phonebookScroll->setWidgetResizable(true);

        phonebookVLayout->addWidget(phonebookScroll);

        tabWidget->addTab(phonebookTab, QString());

        rscVLayout->addWidget(tabWidget);


        verticalLayout->addWidget(rscFrame);


        retranslateUi(Resources);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Resources);
    } // setupUi

    void retranslateUi(QWidget *Resources)
    {
        Resources->setWindowTitle(QCoreApplication::translate("Resources", "Resources", nullptr));
        resLabel->setText(QCoreApplication::translate("Resources", "Search Results for:", nullptr));
        listView->setText(QCoreApplication::translate("Resources", "List", nullptr));
        buttonsView->setText(QCoreApplication::translate("Resources", "Buttons", nullptr));
        viewAllButton->setText(QCoreApplication::translate("Resources", "View All", nullptr));
#if QT_CONFIG(tooltip)
        grpAttButton->setToolTip(QCoreApplication::translate("Resources", "During this session only", nullptr));
#endif // QT_CONFIG(tooltip)
        grpAttButton->setText(QCoreApplication::translate("Resources", "Group Attach History", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(subsTab), QCoreApplication::translate("Resources", "Page", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(talkgroupTab), QCoreApplication::translate("Resources", "Page", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(dgnaIndTab), QCoreApplication::translate("Resources", "Page", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(dgnaGrpTab), QCoreApplication::translate("Resources", "Page", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(mobileTab), QCoreApplication::translate("Resources", "Page", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(dispatcherTab), QCoreApplication::translate("Resources", "Page", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(phonebookTab), QCoreApplication::translate("Resources", "Phonebook", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Resources: public Ui_Resources {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESOURCES_H
