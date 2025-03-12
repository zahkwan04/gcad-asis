/********************************************************************************
** Form generated from reading UI file 'MainWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QFrame *mainFrame;
    QVBoxLayout *mainVLayout;
    QFrame *headerFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *logoLabel;
    QLabel *titleLabel;
    QSpacerItem *statusHSpacer;
    QLabel *idLabel;
    QToolButton *logoutButton;
    QFrame *upperFrame;
    QHBoxLayout *upperLayout;
    QSpacerItem *horizontalSpacer;
    QToolButton *mapButton;
    QLabel *helpDeskIconLabel;
    QLabel *helpDeskLabel;
    QHBoxLayout *mainHLayout;
    QScrollArea *btnsScroll;
    QWidget *btnsWidget;
    QVBoxLayout *verticalLayout_3;
    QToolButton *contactsButton;
    QToolButton *resourcesButton;
    QToolButton *callButton;
    QToolButton *sdsButton;
    QToolButton *incidentButton;
    QToolButton *reportButton;
    QToolButton *settingsButton;
    QToolButton *oskButton;
    QSplitter *splitter;
    QMdiArea *mdiArea;
    QSplitter *vSplitter;
    QFrame *callsOuterFrame;
    QVBoxLayout *verticalLayout_42;
    QFrame *callsTitleFrame;
    QHBoxLayout *horizontalLayout_7;
    QLabel *callsIconlabel;
    QLabel *callsLabel;
    QToolButton *callsButton;
    QFrame *indCallTitleFrame;
    QHBoxLayout *horizontalLayout_8;
    QLabel *indCallIconLabel;
    QLabel *indCallLabel;
    QToolButton *indCallButton;
    QScrollArea *indCallScrollArea;
    QWidget *indCallScrollWidget;
    QFrame *grpCallTitleFrame;
    QHBoxLayout *horizontalLayout_9;
    QLabel *grpCallIconLabel;
    QLabel *grpCallLabel;
    QToolButton *grpCallButton;
    QScrollArea *grpCallScrollArea;
    QWidget *grpCallScrollWidget;
    QFrame *monOuterFrame;
    QVBoxLayout *verticalLayout_43;
    QFrame *monTitleFrame;
    QHBoxLayout *horizontalLayout_10;
    QLabel *monIconLabel;
    QLabel *monLabel;
    QToolButton *monButton;
    QFrame *monFrame;
    QHBoxLayout *horizontalLayout_2;
    QSplitter *splitter2;
    QFrame *monGrpFrame;
    QVBoxLayout *verticalLayout_5;
    QFrame *monGrpLabelFrame;
    QVBoxLayout *verticalLayout_12;
    QHBoxLayout *horizontalLayout_4;
    QLabel *monGrpIconLabel;
    QLabel *monGrpLabel;
    QListView *monGrpList;
    QFrame *monIndFrame;
    QVBoxLayout *verticalLayout_9;
    QFrame *monIndLabelFrame;
    QVBoxLayout *verticalLayout_13;
    QHBoxLayout *horizontalLayout_6;
    QLabel *monIndIconLabel;
    QLabel *monIndLabel;
    QListView *monIndList;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setWindowModality(Qt::ApplicationModal);
        MainWindow->resize(1280, 900);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        MainWindow->setMaximumSize(QSize(16777215, 16777215));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_main.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        mainFrame = new QFrame(MainWindow);
        mainFrame->setObjectName(QString::fromUtf8("mainFrame"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(mainFrame->sizePolicy().hasHeightForWidth());
        mainFrame->setSizePolicy(sizePolicy1);
        mainVLayout = new QVBoxLayout(mainFrame);
        mainVLayout->setSpacing(0);
        mainVLayout->setContentsMargins(5, 5, 5, 5);
        mainVLayout->setObjectName(QString::fromUtf8("mainVLayout"));
        mainVLayout->setContentsMargins(0, 0, 0, 0);
        headerFrame = new QFrame(mainFrame);
        headerFrame->setObjectName(QString::fromUtf8("headerFrame"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(headerFrame->sizePolicy().hasHeightForWidth());
        headerFrame->setSizePolicy(sizePolicy2);
        headerFrame->setFrameShape(QFrame::NoFrame);
        headerFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(headerFrame);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(10, 5, 10, 5);
        logoLabel = new QLabel(headerFrame);
        logoLabel->setObjectName(QString::fromUtf8("logoLabel"));
        logoLabel->setMaximumSize(QSize(50, 50));
        logoLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_main.png")));
        logoLabel->setScaledContents(true);
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setMargin(0);
        logoLabel->setIndent(-1);

        horizontalLayout->addWidget(logoLabel);

        titleLabel = new QLabel(headerFrame);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));

        horizontalLayout->addWidget(titleLabel);

        statusHSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(statusHSpacer);

        idLabel = new QLabel(headerFrame);
        idLabel->setObjectName(QString::fromUtf8("idLabel"));
        idLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(idLabel);

        logoutButton = new QToolButton(headerFrame);
        logoutButton->setObjectName(QString::fromUtf8("logoutButton"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        logoutButton->setIcon(icon1);
        logoutButton->setIconSize(QSize(40, 40));
        logoutButton->setAutoRaise(true);

        horizontalLayout->addWidget(logoutButton);


        mainVLayout->addWidget(headerFrame);

        upperFrame = new QFrame(mainFrame);
        upperFrame->setObjectName(QString::fromUtf8("upperFrame"));
        sizePolicy2.setHeightForWidth(upperFrame->sizePolicy().hasHeightForWidth());
        upperFrame->setSizePolicy(sizePolicy2);
        upperFrame->setFrameShape(QFrame::NoFrame);
        upperFrame->setFrameShadow(QFrame::Raised);
        upperLayout = new QHBoxLayout(upperFrame);
        upperLayout->setSpacing(5);
        upperLayout->setContentsMargins(5, 5, 5, 5);
        upperLayout->setObjectName(QString::fromUtf8("upperLayout"));
        upperLayout->setContentsMargins(10, 5, 10, 5);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        upperLayout->addItem(horizontalSpacer);

        mapButton = new QToolButton(upperFrame);
        mapButton->setObjectName(QString::fromUtf8("mapButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_map.png"), QSize(), QIcon::Normal, QIcon::Off);
        mapButton->setIcon(icon2);
        mapButton->setIconSize(QSize(40, 40));
        mapButton->setAutoRaise(true);

        upperLayout->addWidget(mapButton);

        helpDeskIconLabel = new QLabel(upperFrame);
        helpDeskIconLabel->setObjectName(QString::fromUtf8("helpDeskIconLabel"));
        helpDeskIconLabel->setMaximumSize(QSize(40, 40));
        helpDeskIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_help_desk.png")));
        helpDeskIconLabel->setScaledContents(true);

        upperLayout->addWidget(helpDeskIconLabel);

        helpDeskLabel = new QLabel(upperFrame);
        helpDeskLabel->setObjectName(QString::fromUtf8("helpDeskLabel"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(helpDeskLabel->sizePolicy().hasHeightForWidth());
        helpDeskLabel->setSizePolicy(sizePolicy3);

        upperLayout->addWidget(helpDeskLabel);


        mainVLayout->addWidget(upperFrame);

        mainHLayout = new QHBoxLayout();
        mainHLayout->setSpacing(0);
        mainHLayout->setObjectName(QString::fromUtf8("mainHLayout"));
        btnsScroll = new QScrollArea(mainFrame);
        btnsScroll->setObjectName(QString::fromUtf8("btnsScroll"));
        sizePolicy3.setHeightForWidth(btnsScroll->sizePolicy().hasHeightForWidth());
        btnsScroll->setSizePolicy(sizePolicy3);
        btnsScroll->setMinimumSize(QSize(180, 80));
        btnsScroll->setFrameShape(QFrame::NoFrame);
        btnsScroll->setFrameShadow(QFrame::Raised);
        btnsScroll->setLineWidth(0);
        btnsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        btnsScroll->setWidgetResizable(true);
        btnsWidget = new QWidget();
        btnsWidget->setObjectName(QString::fromUtf8("btnsWidget"));
        sizePolicy1.setHeightForWidth(btnsWidget->sizePolicy().hasHeightForWidth());
        btnsWidget->setSizePolicy(sizePolicy1);
        verticalLayout_3 = new QVBoxLayout(btnsWidget);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(5, 5, 5, 5);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(1, 0, 1, 0);
        contactsButton = new QToolButton(btnsWidget);
        contactsButton->setObjectName(QString::fromUtf8("contactsButton"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(contactsButton->sizePolicy().hasHeightForWidth());
        contactsButton->setSizePolicy(sizePolicy4);
        contactsButton->setMinimumSize(QSize(170, 80));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_contacts.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_contacts2.png"), QSize(), QIcon::Normal, QIcon::On);
        contactsButton->setIcon(icon3);
        contactsButton->setIconSize(QSize(80, 50));
        contactsButton->setCheckable(true);
        contactsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        contactsButton->setAutoRaise(true);

        verticalLayout_3->addWidget(contactsButton);

        resourcesButton = new QToolButton(btnsWidget);
        resourcesButton->setObjectName(QString::fromUtf8("resourcesButton"));
        sizePolicy4.setHeightForWidth(resourcesButton->sizePolicy().hasHeightForWidth());
        resourcesButton->setSizePolicy(sizePolicy4);
        resourcesButton->setMinimumSize(QSize(170, 80));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_resources.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_resources2.png"), QSize(), QIcon::Normal, QIcon::On);
        resourcesButton->setIcon(icon4);
        resourcesButton->setIconSize(QSize(80, 50));
        resourcesButton->setCheckable(true);
        resourcesButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        resourcesButton->setAutoRaise(true);

        verticalLayout_3->addWidget(resourcesButton);

        callButton = new QToolButton(btnsWidget);
        callButton->setObjectName(QString::fromUtf8("callButton"));
        sizePolicy4.setHeightForWidth(callButton->sizePolicy().hasHeightForWidth());
        callButton->setSizePolicy(sizePolicy4);
        callButton->setMinimumSize(QSize(170, 80));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_call.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_call2.png"), QSize(), QIcon::Normal, QIcon::On);
        callButton->setIcon(icon5);
        callButton->setIconSize(QSize(80, 50));
        callButton->setCheckable(true);
        callButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        callButton->setAutoRaise(true);

        verticalLayout_3->addWidget(callButton);

        sdsButton = new QToolButton(btnsWidget);
        sdsButton->setObjectName(QString::fromUtf8("sdsButton"));
        sizePolicy4.setHeightForWidth(sdsButton->sizePolicy().hasHeightForWidth());
        sdsButton->setSizePolicy(sizePolicy4);
        sdsButton->setMinimumSize(QSize(170, 80));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_sds.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_sds2.png"), QSize(), QIcon::Normal, QIcon::On);
        sdsButton->setIcon(icon6);
        sdsButton->setIconSize(QSize(80, 50));
        sdsButton->setCheckable(true);
        sdsButton->setPopupMode(QToolButton::DelayedPopup);
        sdsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        sdsButton->setAutoRaise(true);

        verticalLayout_3->addWidget(sdsButton);

        incidentButton = new QToolButton(btnsWidget);
        incidentButton->setObjectName(QString::fromUtf8("incidentButton"));
        sizePolicy4.setHeightForWidth(incidentButton->sizePolicy().hasHeightForWidth());
        incidentButton->setSizePolicy(sizePolicy4);
        incidentButton->setMinimumSize(QSize(170, 80));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_incident.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_incident2.png"), QSize(), QIcon::Normal, QIcon::On);
        incidentButton->setIcon(icon7);
        incidentButton->setIconSize(QSize(80, 50));
        incidentButton->setCheckable(true);
        incidentButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        incidentButton->setAutoRaise(true);

        verticalLayout_3->addWidget(incidentButton);

        reportButton = new QToolButton(btnsWidget);
        reportButton->setObjectName(QString::fromUtf8("reportButton"));
        sizePolicy4.setHeightForWidth(reportButton->sizePolicy().hasHeightForWidth());
        reportButton->setSizePolicy(sizePolicy4);
        reportButton->setMinimumSize(QSize(170, 80));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Images/images/icon_report.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon8.addFile(QString::fromUtf8(":/Images/images/icon_report2.png"), QSize(), QIcon::Normal, QIcon::On);
        reportButton->setIcon(icon8);
        reportButton->setIconSize(QSize(80, 50));
        reportButton->setCheckable(true);
        reportButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        reportButton->setAutoRaise(true);

        verticalLayout_3->addWidget(reportButton);

        settingsButton = new QToolButton(btnsWidget);
        settingsButton->setObjectName(QString::fromUtf8("settingsButton"));
        sizePolicy4.setHeightForWidth(settingsButton->sizePolicy().hasHeightForWidth());
        settingsButton->setSizePolicy(sizePolicy4);
        settingsButton->setMinimumSize(QSize(170, 80));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        settingsButton->setIcon(icon9);
        settingsButton->setIconSize(QSize(80, 50));
        settingsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        settingsButton->setAutoRaise(true);

        verticalLayout_3->addWidget(settingsButton);

        oskButton = new QToolButton(btnsWidget);
        oskButton->setObjectName(QString::fromUtf8("oskButton"));
        sizePolicy4.setHeightForWidth(oskButton->sizePolicy().hasHeightForWidth());
        oskButton->setSizePolicy(sizePolicy4);
        oskButton->setMinimumSize(QSize(170, 80));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/Images/images/icon_keyboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        oskButton->setIcon(icon10);
        oskButton->setIconSize(QSize(80, 50));
        oskButton->setAutoRaise(true);

        verticalLayout_3->addWidget(oskButton);

        btnsScroll->setWidget(btnsWidget);

        mainHLayout->addWidget(btnsScroll);

        splitter = new QSplitter(mainFrame);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        sizePolicy1.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy1);
        splitter->setOrientation(Qt::Horizontal);
        splitter->setHandleWidth(2);
        splitter->setChildrenCollapsible(false);
        mdiArea = new QMdiArea(splitter);
        mdiArea->setObjectName(QString::fromUtf8("mdiArea"));
        mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        splitter->addWidget(mdiArea);
        vSplitter = new QSplitter(splitter);
        vSplitter->setObjectName(QString::fromUtf8("vSplitter"));
        sizePolicy1.setHeightForWidth(vSplitter->sizePolicy().hasHeightForWidth());
        vSplitter->setSizePolicy(sizePolicy1);
        vSplitter->setOrientation(Qt::Vertical);
        vSplitter->setHandleWidth(2);
        vSplitter->setChildrenCollapsible(false);
        callsOuterFrame = new QFrame(vSplitter);
        callsOuterFrame->setObjectName(QString::fromUtf8("callsOuterFrame"));
        callsOuterFrame->setFrameShape(QFrame::StyledPanel);
        callsOuterFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout_42 = new QVBoxLayout(callsOuterFrame);
        verticalLayout_42->setSpacing(0);
        verticalLayout_42->setContentsMargins(5, 5, 5, 5);
        verticalLayout_42->setObjectName(QString::fromUtf8("verticalLayout_42"));
        verticalLayout_42->setContentsMargins(0, 0, 0, 0);
        callsTitleFrame = new QFrame(callsOuterFrame);
        callsTitleFrame->setObjectName(QString::fromUtf8("callsTitleFrame"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(callsTitleFrame->sizePolicy().hasHeightForWidth());
        callsTitleFrame->setSizePolicy(sizePolicy5);
        callsTitleFrame->setMinimumSize(QSize(0, 40));
        callsTitleFrame->setFrameShape(QFrame::NoFrame);
        callsTitleFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_7 = new QHBoxLayout(callsTitleFrame);
        horizontalLayout_7->setSpacing(5);
        horizontalLayout_7->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(5, 5, 5, 5);
        callsIconlabel = new QLabel(callsTitleFrame);
        callsIconlabel->setObjectName(QString::fromUtf8("callsIconlabel"));
        callsIconlabel->setMaximumSize(QSize(28, 28));
        callsIconlabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_call.png")));
        callsIconlabel->setScaledContents(true);

        horizontalLayout_7->addWidget(callsIconlabel);

        callsLabel = new QLabel(callsTitleFrame);
        callsLabel->setObjectName(QString::fromUtf8("callsLabel"));

        horizontalLayout_7->addWidget(callsLabel);

        callsButton = new QToolButton(callsTitleFrame);
        callsButton->setObjectName(QString::fromUtf8("callsButton"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon11.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::On);
        callsButton->setIcon(icon11);
        callsButton->setCheckable(true);

        horizontalLayout_7->addWidget(callsButton);


        verticalLayout_42->addWidget(callsTitleFrame);

        indCallTitleFrame = new QFrame(callsOuterFrame);
        indCallTitleFrame->setObjectName(QString::fromUtf8("indCallTitleFrame"));
        sizePolicy2.setHeightForWidth(indCallTitleFrame->sizePolicy().hasHeightForWidth());
        indCallTitleFrame->setSizePolicy(sizePolicy2);
        indCallTitleFrame->setFrameShape(QFrame::NoFrame);
        indCallTitleFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_8 = new QHBoxLayout(indCallTitleFrame);
        horizontalLayout_8->setSpacing(5);
        horizontalLayout_8->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(5, 5, 5, 5);
        indCallIconLabel = new QLabel(indCallTitleFrame);
        indCallIconLabel->setObjectName(QString::fromUtf8("indCallIconLabel"));
        indCallIconLabel->setMaximumSize(QSize(28, 28));
        indCallIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_mobile.png")));
        indCallIconLabel->setScaledContents(true);

        horizontalLayout_8->addWidget(indCallIconLabel);

        indCallLabel = new QLabel(indCallTitleFrame);
        indCallLabel->setObjectName(QString::fromUtf8("indCallLabel"));

        horizontalLayout_8->addWidget(indCallLabel);

        indCallButton = new QToolButton(indCallTitleFrame);
        indCallButton->setObjectName(QString::fromUtf8("indCallButton"));
        indCallButton->setIcon(icon11);
        indCallButton->setCheckable(true);

        horizontalLayout_8->addWidget(indCallButton);


        verticalLayout_42->addWidget(indCallTitleFrame);

        indCallScrollArea = new QScrollArea(callsOuterFrame);
        indCallScrollArea->setObjectName(QString::fromUtf8("indCallScrollArea"));
        indCallScrollArea->setFrameShape(QFrame::NoFrame);
        indCallScrollArea->setWidgetResizable(true);
        indCallScrollWidget = new QWidget();
        indCallScrollWidget->setObjectName(QString::fromUtf8("indCallScrollWidget"));
        indCallScrollArea->setWidget(indCallScrollWidget);

        verticalLayout_42->addWidget(indCallScrollArea);

        grpCallTitleFrame = new QFrame(callsOuterFrame);
        grpCallTitleFrame->setObjectName(QString::fromUtf8("grpCallTitleFrame"));
        sizePolicy2.setHeightForWidth(grpCallTitleFrame->sizePolicy().hasHeightForWidth());
        grpCallTitleFrame->setSizePolicy(sizePolicy2);
        grpCallTitleFrame->setFrameShape(QFrame::NoFrame);
        grpCallTitleFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_9 = new QHBoxLayout(grpCallTitleFrame);
        horizontalLayout_9->setSpacing(5);
        horizontalLayout_9->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(5, 5, 5, 5);
        grpCallIconLabel = new QLabel(grpCallTitleFrame);
        grpCallIconLabel->setObjectName(QString::fromUtf8("grpCallIconLabel"));
        grpCallIconLabel->setMaximumSize(QSize(28, 28));
        grpCallIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_group.png")));
        grpCallIconLabel->setScaledContents(true);

        horizontalLayout_9->addWidget(grpCallIconLabel);

        grpCallLabel = new QLabel(grpCallTitleFrame);
        grpCallLabel->setObjectName(QString::fromUtf8("grpCallLabel"));

        horizontalLayout_9->addWidget(grpCallLabel);

        grpCallButton = new QToolButton(grpCallTitleFrame);
        grpCallButton->setObjectName(QString::fromUtf8("grpCallButton"));
        grpCallButton->setIcon(icon11);
        grpCallButton->setCheckable(true);

        horizontalLayout_9->addWidget(grpCallButton);


        verticalLayout_42->addWidget(grpCallTitleFrame);

        grpCallScrollArea = new QScrollArea(callsOuterFrame);
        grpCallScrollArea->setObjectName(QString::fromUtf8("grpCallScrollArea"));
        grpCallScrollArea->setFrameShape(QFrame::NoFrame);
        grpCallScrollArea->setWidgetResizable(true);
        grpCallScrollWidget = new QWidget();
        grpCallScrollWidget->setObjectName(QString::fromUtf8("grpCallScrollWidget"));
        grpCallScrollArea->setWidget(grpCallScrollWidget);

        verticalLayout_42->addWidget(grpCallScrollArea);

        vSplitter->addWidget(callsOuterFrame);
        monOuterFrame = new QFrame(vSplitter);
        monOuterFrame->setObjectName(QString::fromUtf8("monOuterFrame"));
        monOuterFrame->setFrameShape(QFrame::NoFrame);
        monOuterFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_43 = new QVBoxLayout(monOuterFrame);
        verticalLayout_43->setSpacing(0);
        verticalLayout_43->setContentsMargins(5, 5, 5, 5);
        verticalLayout_43->setObjectName(QString::fromUtf8("verticalLayout_43"));
        verticalLayout_43->setContentsMargins(0, 0, 0, 0);
        monTitleFrame = new QFrame(monOuterFrame);
        monTitleFrame->setObjectName(QString::fromUtf8("monTitleFrame"));
        sizePolicy5.setHeightForWidth(monTitleFrame->sizePolicy().hasHeightForWidth());
        monTitleFrame->setSizePolicy(sizePolicy5);
        monTitleFrame->setMinimumSize(QSize(0, 40));
        monTitleFrame->setFrameShape(QFrame::NoFrame);
        monTitleFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_10 = new QHBoxLayout(monTitleFrame);
        horizontalLayout_10->setSpacing(5);
        horizontalLayout_10->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(5, 5, 5, 5);
        monIconLabel = new QLabel(monTitleFrame);
        monIconLabel->setObjectName(QString::fromUtf8("monIconLabel"));
        monIconLabel->setMaximumSize(QSize(28, 28));
        monIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_monitoring.png")));
        monIconLabel->setScaledContents(true);

        horizontalLayout_10->addWidget(monIconLabel);

        monLabel = new QLabel(monTitleFrame);
        monLabel->setObjectName(QString::fromUtf8("monLabel"));

        horizontalLayout_10->addWidget(monLabel);

        monButton = new QToolButton(monTitleFrame);
        monButton->setObjectName(QString::fromUtf8("monButton"));
        monButton->setIcon(icon11);
        monButton->setCheckable(true);

        horizontalLayout_10->addWidget(monButton);


        verticalLayout_43->addWidget(monTitleFrame);

        monFrame = new QFrame(monOuterFrame);
        monFrame->setObjectName(QString::fromUtf8("monFrame"));
        monFrame->setFrameShape(QFrame::NoFrame);
        monFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(monFrame);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        splitter2 = new QSplitter(monFrame);
        splitter2->setObjectName(QString::fromUtf8("splitter2"));
        sizePolicy1.setHeightForWidth(splitter2->sizePolicy().hasHeightForWidth());
        splitter2->setSizePolicy(sizePolicy1);
        splitter2->setOrientation(Qt::Horizontal);
        splitter2->setHandleWidth(2);
        splitter2->setChildrenCollapsible(false);
        monGrpFrame = new QFrame(splitter2);
        monGrpFrame->setObjectName(QString::fromUtf8("monGrpFrame"));
        monGrpFrame->setFrameShape(QFrame::NoFrame);
        monGrpFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(monGrpFrame);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setContentsMargins(5, 5, 5, 5);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        monGrpLabelFrame = new QFrame(monGrpFrame);
        monGrpLabelFrame->setObjectName(QString::fromUtf8("monGrpLabelFrame"));
        monGrpLabelFrame->setFrameShape(QFrame::NoFrame);
        monGrpLabelFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_12 = new QVBoxLayout(monGrpLabelFrame);
        verticalLayout_12->setSpacing(0);
        verticalLayout_12->setContentsMargins(5, 5, 5, 5);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        verticalLayout_12->setContentsMargins(5, 3, 5, 3);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(5);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        monGrpIconLabel = new QLabel(monGrpLabelFrame);
        monGrpIconLabel->setObjectName(QString::fromUtf8("monGrpIconLabel"));
        monGrpIconLabel->setMaximumSize(QSize(28, 28));
        monGrpIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_group.png")));
        monGrpIconLabel->setScaledContents(true);

        horizontalLayout_4->addWidget(monGrpIconLabel);

        monGrpLabel = new QLabel(monGrpLabelFrame);
        monGrpLabel->setObjectName(QString::fromUtf8("monGrpLabel"));

        horizontalLayout_4->addWidget(monGrpLabel);


        verticalLayout_12->addLayout(horizontalLayout_4);


        verticalLayout_5->addWidget(monGrpLabelFrame);

        monGrpList = new QListView(monGrpFrame);
        monGrpList->setObjectName(QString::fromUtf8("monGrpList"));
        monGrpList->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout_5->addWidget(monGrpList);

        splitter2->addWidget(monGrpFrame);
        monIndFrame = new QFrame(splitter2);
        monIndFrame->setObjectName(QString::fromUtf8("monIndFrame"));
        monIndFrame->setFrameShape(QFrame::NoFrame);
        monIndFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_9 = new QVBoxLayout(monIndFrame);
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setContentsMargins(5, 5, 5, 5);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        monIndLabelFrame = new QFrame(monIndFrame);
        monIndLabelFrame->setObjectName(QString::fromUtf8("monIndLabelFrame"));
        monIndLabelFrame->setFrameShape(QFrame::NoFrame);
        monIndLabelFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_13 = new QVBoxLayout(monIndLabelFrame);
        verticalLayout_13->setSpacing(0);
        verticalLayout_13->setContentsMargins(5, 5, 5, 5);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        verticalLayout_13->setContentsMargins(5, 3, 5, 3);
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(5);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        monIndIconLabel = new QLabel(monIndLabelFrame);
        monIndIconLabel->setObjectName(QString::fromUtf8("monIndIconLabel"));
        monIndIconLabel->setMaximumSize(QSize(28, 28));
        monIndIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_mobile.png")));
        monIndIconLabel->setScaledContents(true);

        horizontalLayout_6->addWidget(monIndIconLabel);

        monIndLabel = new QLabel(monIndLabelFrame);
        monIndLabel->setObjectName(QString::fromUtf8("monIndLabel"));

        horizontalLayout_6->addWidget(monIndLabel);


        verticalLayout_13->addLayout(horizontalLayout_6);


        verticalLayout_9->addWidget(monIndLabelFrame);

        monIndList = new QListView(monIndFrame);
        monIndList->setObjectName(QString::fromUtf8("monIndList"));
        monIndList->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout_9->addWidget(monIndList);

        splitter2->addWidget(monIndFrame);

        horizontalLayout_2->addWidget(splitter2);


        verticalLayout_43->addWidget(monFrame);

        vSplitter->addWidget(monOuterFrame);
        splitter->addWidget(vSplitter);

        mainHLayout->addWidget(splitter);


        mainVLayout->addLayout(mainHLayout);

        MainWindow->setCentralWidget(mainFrame);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
#if QT_CONFIG(tooltip)
        logoutButton->setToolTip(QCoreApplication::translate("MainWindow", "Logout", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        mapButton->setToolTip(QCoreApplication::translate("MainWindow", "GIS", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        helpDeskIconLabel->setToolTip(QCoreApplication::translate("MainWindow", "Help", nullptr));
#endif // QT_CONFIG(tooltip)
        contactsButton->setText(QCoreApplication::translate("MainWindow", "Quick Contacts", nullptr));
        resourcesButton->setText(QCoreApplication::translate("MainWindow", "Resources", nullptr));
        callButton->setText(QCoreApplication::translate("MainWindow", "Call", nullptr));
        sdsButton->setText(QCoreApplication::translate("MainWindow", "Message", nullptr));
        incidentButton->setText(QCoreApplication::translate("MainWindow", "Incident", nullptr));
        reportButton->setText(QCoreApplication::translate("MainWindow", "Report", nullptr));
        settingsButton->setText(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        callsLabel->setText(QCoreApplication::translate("MainWindow", "Calls", nullptr));
        indCallLabel->setText(QCoreApplication::translate("MainWindow", "Individual", nullptr));
        grpCallLabel->setText(QCoreApplication::translate("MainWindow", "Group", nullptr));
        monLabel->setText(QCoreApplication::translate("MainWindow", "Monitored", nullptr));
        monGrpLabel->setText(QCoreApplication::translate("MainWindow", "Groups", nullptr));
        monIndLabel->setText(QCoreApplication::translate("MainWindow", "Individuals", nullptr));
        (void)MainWindow;
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
