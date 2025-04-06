/********************************************************************************
** Form generated from reading UI file 'SettingsUi.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSUI_H
#define UI_SETTINGSUI_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsUi
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *settingsHLayout;
    QFrame *buttonsFrame;
    QVBoxLayout *verticalLayout_3;
    QToolButton *generalButton;
    QToolButton *resourceButton;
    QToolButton *serverButton;
    QToolButton *mapButton;
    QToolButton *mediaButton;
    QToolButton *aboutButton;
    QSpacerItem *buttonsHSpacer;
    QSpacerItem *buttonsVSpacer;
    QFrame *bodyFrame;
    QVBoxLayout *verticalLayout_2;
    QFrame *titleFrame;
    QVBoxLayout *verticalLayout_4;
    QLabel *categoryLabel;
    QFrame *stackedFrame;
    QVBoxLayout *verticalLayout_5;
    QStackedWidget *stackedWidget;
    QWidget *generalPage;
    QGridLayout *generalGLayout;
    QLabel *logLabel;
    QLineEdit *logEdit;
    QPushButton *logBrowseButton;
    QLabel *logLevelLabel;
    QComboBox *logLevelCombo;
    QPushButton *logLevelDefButton;
    QSpacerItem *logVSpacer;
    QLabel *dlLabel;
    QLineEdit *dlEdit;
    QToolButton *dlClrButton;
    QPushButton *dlBrowseButton;
    QSpacerItem *mmsVSpacer;
    QLabel *themeLabel;
    QComboBox *themeCombo;
    QSpacerItem *themeVSpacer;
    QCheckBox *grpCallAutoJoinCheck;
    QCheckBox *monRetainCheck;
    QSpacerItem *pttVSpacer;
    QLabel *pttLabel;
    QCheckBox *pttCtrlCheck;
    QCheckBox *pttAltCheck;
    QLineEdit *pttEdit;
    QSpacerItem *pttSpacer;
    QSpacerItem *monVSpacer;
    QPushButton *gpsMonButton;
    QPushButton *branchButton;
    QSpacerItem *gridVSpacer1;
    QWidget *resourcePage;
    QGridLayout *resourceGLayout;
    QLabel *grpDspLabel;
    QComboBox *grpDspCombo;
    QLabel *subsDspLabel;
    QComboBox *subsDspCombo;
    QLabel *termLblLabel;
    QComboBox *termLblCombo;
    QSpacerItem *gridVSpacer5;
    QWidget *connectionPage;
    QGridLayout *connectionGLayout;
    QLabel *serverIPLabel;
    QLineEdit *serverIpEdit;
    QLabel *portLabel;
    QLineEdit *serverPortEdit;
    QSpacerItem *pswdVSpacer;
    QPushButton *changePswdButton;
    QSpacerItem *gridVSpacer2;
    QWidget *mapPage;
    QGridLayout *mapGLayout;
    QLabel *mapLabel;
    QLineEdit *mapEdit;
    QPushButton *browseButton;
    QSpacerItem *mapVSpacer;
    QLabel *termTimeLabel;
    QLabel *termChangeLabel;
    QLineEdit *termChangeEdit;
    QLabel *termRemoveLabel;
    QLineEdit *termRemoveEdit;
    QCheckBox *ctrRscCheck;
    QCheckBox *seaMapCheck;
    QSpacerItem *gridVSpacer3;
    QWidget *mediaPage;
    QGridLayout *gridLayout;
    QFrame *mediaFrame;
    QGridLayout *gridLayout_2;
    QLabel *micLabel;
    QComboBox *micCombo;
    QProgressBar *audioInBar;
    QLabel *outVoiceLabel;
    QComboBox *speakerCombo;
    QPushButton *playButton;
    QSpacerItem *audioVSpacer;
    QLabel *camLabel;
    QComboBox *camCombo;
    QLabel *resLabel;
    QComboBox *resCombo;
    QLabel *videoLabel;
    QWidget *aboutPage;
    QGridLayout *accountGLayout;
    QLabel *agencyLabel;
    QLabel *agencyNameLabel;
    QLabel *titleLabel;
    QLabel *registrationLabel;
    QLabel *licenseInfoLabel;
    QSpacerItem *gridVSpacer4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *cancelButton;
    QSpacerItem *horizontalSpacer2;
    QPushButton *applyButton;

    void setupUi(QDialog *SettingsUi)
    {
        if (SettingsUi->objectName().isEmpty())
            SettingsUi->setObjectName(QString::fromUtf8("SettingsUi"));
        SettingsUi->setWindowModality(Qt::ApplicationModal);
        SettingsUi->resize(800, 600);
        SettingsUi->setMinimumSize(QSize(600, 500));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        SettingsUi->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(SettingsUi);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        settingsHLayout = new QHBoxLayout();
        settingsHLayout->setSpacing(0);
        settingsHLayout->setObjectName(QString::fromUtf8("settingsHLayout"));
        buttonsFrame = new QFrame(SettingsUi);
        buttonsFrame->setObjectName(QString::fromUtf8("buttonsFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(buttonsFrame->sizePolicy().hasHeightForWidth());
        buttonsFrame->setSizePolicy(sizePolicy);
        buttonsFrame->setMinimumSize(QSize(100, 0));
        buttonsFrame->setFrameShape(QFrame::StyledPanel);
        buttonsFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(buttonsFrame);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 20, 0, 20);
        generalButton = new QToolButton(buttonsFrame);
        generalButton->setObjectName(QString::fromUtf8("generalButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(generalButton->sizePolicy().hasHeightForWidth());
        generalButton->setSizePolicy(sizePolicy1);
        generalButton->setMinimumSize(QSize(80, 80));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_settings2.png"), QSize(), QIcon::Normal, QIcon::On);
        generalButton->setIcon(icon1);
        generalButton->setIconSize(QSize(40, 40));
        generalButton->setCheckable(true);
        generalButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        generalButton->setAutoRaise(true);

        verticalLayout_3->addWidget(generalButton);

        resourceButton = new QToolButton(buttonsFrame);
        resourceButton->setObjectName(QString::fromUtf8("resourceButton"));
        sizePolicy1.setHeightForWidth(resourceButton->sizePolicy().hasHeightForWidth());
        resourceButton->setSizePolicy(sizePolicy1);
        resourceButton->setMinimumSize(QSize(80, 80));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_resources.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_resources2.png"), QSize(), QIcon::Normal, QIcon::On);
        resourceButton->setIcon(icon2);
        resourceButton->setIconSize(QSize(40, 40));
        resourceButton->setCheckable(true);
        resourceButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        resourceButton->setAutoRaise(true);

        verticalLayout_3->addWidget(resourceButton);

        serverButton = new QToolButton(buttonsFrame);
        serverButton->setObjectName(QString::fromUtf8("serverButton"));
        sizePolicy1.setHeightForWidth(serverButton->sizePolicy().hasHeightForWidth());
        serverButton->setSizePolicy(sizePolicy1);
        serverButton->setMinimumSize(QSize(80, 80));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_server.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_server2.png"), QSize(), QIcon::Normal, QIcon::On);
        serverButton->setIcon(icon3);
        serverButton->setIconSize(QSize(40, 40));
        serverButton->setCheckable(true);
        serverButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        serverButton->setAutoRaise(true);

        verticalLayout_3->addWidget(serverButton);

        mapButton = new QToolButton(buttonsFrame);
        mapButton->setObjectName(QString::fromUtf8("mapButton"));
        sizePolicy1.setHeightForWidth(mapButton->sizePolicy().hasHeightForWidth());
        mapButton->setSizePolicy(sizePolicy1);
        mapButton->setMinimumSize(QSize(80, 80));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_map.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_map2.png"), QSize(), QIcon::Normal, QIcon::On);
        mapButton->setIcon(icon4);
        mapButton->setIconSize(QSize(40, 40));
        mapButton->setCheckable(true);
        mapButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        mapButton->setAutoRaise(true);

        verticalLayout_3->addWidget(mapButton);

        mediaButton = new QToolButton(buttonsFrame);
        mediaButton->setObjectName(QString::fromUtf8("mediaButton"));
        sizePolicy1.setHeightForWidth(mediaButton->sizePolicy().hasHeightForWidth());
        mediaButton->setSizePolicy(sizePolicy1);
        mediaButton->setMinimumSize(QSize(80, 80));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_speaker.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_speaker2.png"), QSize(), QIcon::Normal, QIcon::On);
        mediaButton->setIcon(icon5);
        mediaButton->setIconSize(QSize(40, 40));
        mediaButton->setCheckable(true);
        mediaButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        mediaButton->setAutoRaise(true);

        verticalLayout_3->addWidget(mediaButton);

        aboutButton = new QToolButton(buttonsFrame);
        aboutButton->setObjectName(QString::fromUtf8("aboutButton"));
        sizePolicy1.setHeightForWidth(aboutButton->sizePolicy().hasHeightForWidth());
        aboutButton->setSizePolicy(sizePolicy1);
        aboutButton->setMinimumSize(QSize(80, 80));
        aboutButton->setMaximumSize(QSize(16777215, 16777215));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_about.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_about2.png"), QSize(), QIcon::Normal, QIcon::On);
        aboutButton->setIcon(icon6);
        aboutButton->setIconSize(QSize(40, 40));
        aboutButton->setCheckable(true);
        aboutButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        aboutButton->setAutoRaise(true);

        verticalLayout_3->addWidget(aboutButton);

        buttonsHSpacer = new QSpacerItem(80, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        verticalLayout_3->addItem(buttonsHSpacer);

        buttonsVSpacer = new QSpacerItem(80, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(buttonsVSpacer);


        settingsHLayout->addWidget(buttonsFrame);

        bodyFrame = new QFrame(SettingsUi);
        bodyFrame->setObjectName(QString::fromUtf8("bodyFrame"));
        bodyFrame->setFrameShape(QFrame::StyledPanel);
        bodyFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(bodyFrame);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        titleFrame = new QFrame(bodyFrame);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        titleFrame->setMinimumSize(QSize(28, 38));
        titleFrame->setFrameShape(QFrame::StyledPanel);
        titleFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(titleFrame);
        verticalLayout_4->setSpacing(5);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(5, 5, 5, 5);
        categoryLabel = new QLabel(titleFrame);
        categoryLabel->setObjectName(QString::fromUtf8("categoryLabel"));

        verticalLayout_4->addWidget(categoryLabel);


        verticalLayout_2->addWidget(titleFrame);

        stackedFrame = new QFrame(bodyFrame);
        stackedFrame->setObjectName(QString::fromUtf8("stackedFrame"));
        stackedFrame->setFrameShape(QFrame::StyledPanel);
        stackedFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(stackedFrame);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        stackedWidget = new QStackedWidget(stackedFrame);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        generalPage = new QWidget();
        generalPage->setObjectName(QString::fromUtf8("generalPage"));
        generalGLayout = new QGridLayout(generalPage);
        generalGLayout->setObjectName(QString::fromUtf8("generalGLayout"));
        logLabel = new QLabel(generalPage);
        logLabel->setObjectName(QString::fromUtf8("logLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(logLabel->sizePolicy().hasHeightForWidth());
        logLabel->setSizePolicy(sizePolicy2);

        generalGLayout->addWidget(logLabel, 0, 0, 1, 1);

        logEdit = new QLineEdit(generalPage);
        logEdit->setObjectName(QString::fromUtf8("logEdit"));
        logEdit->setMinimumSize(QSize(0, 30));

        generalGLayout->addWidget(logEdit, 0, 1, 1, 5);

        logBrowseButton = new QPushButton(generalPage);
        logBrowseButton->setObjectName(QString::fromUtf8("logBrowseButton"));
        logBrowseButton->setMinimumSize(QSize(80, 30));

        generalGLayout->addWidget(logBrowseButton, 0, 6, 1, 1);

        logLevelLabel = new QLabel(generalPage);
        logLevelLabel->setObjectName(QString::fromUtf8("logLevelLabel"));
        sizePolicy2.setHeightForWidth(logLevelLabel->sizePolicy().hasHeightForWidth());
        logLevelLabel->setSizePolicy(sizePolicy2);

        generalGLayout->addWidget(logLevelLabel, 1, 0, 1, 1);

        logLevelCombo = new QComboBox(generalPage);
        logLevelCombo->setObjectName(QString::fromUtf8("logLevelCombo"));
        logLevelCombo->setEnabled(false);

        generalGLayout->addWidget(logLevelCombo, 1, 1, 1, 5);

        logLevelDefButton = new QPushButton(generalPage);
        logLevelDefButton->setObjectName(QString::fromUtf8("logLevelDefButton"));
        logLevelDefButton->setEnabled(false);
        logLevelDefButton->setMinimumSize(QSize(80, 30));

        generalGLayout->addWidget(logLevelDefButton, 1, 6, 1, 1);

        logVSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        generalGLayout->addItem(logVSpacer, 2, 0, 1, 1);

        dlLabel = new QLabel(generalPage);
        dlLabel->setObjectName(QString::fromUtf8("dlLabel"));
        sizePolicy2.setHeightForWidth(dlLabel->sizePolicy().hasHeightForWidth());
        dlLabel->setSizePolicy(sizePolicy2);

        generalGLayout->addWidget(dlLabel, 3, 0, 1, 1);

        dlEdit = new QLineEdit(generalPage);
        dlEdit->setObjectName(QString::fromUtf8("dlEdit"));
        dlEdit->setMinimumSize(QSize(0, 30));

        generalGLayout->addWidget(dlEdit, 3, 1, 1, 4);

        dlClrButton = new QToolButton(generalPage);
        dlClrButton->setObjectName(QString::fromUtf8("dlClrButton"));
        dlClrButton->setMinimumSize(QSize(30, 30));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        dlClrButton->setIcon(icon7);
        dlClrButton->setIconSize(QSize(30, 30));

        generalGLayout->addWidget(dlClrButton, 3, 5, 1, 1);

        dlBrowseButton = new QPushButton(generalPage);
        dlBrowseButton->setObjectName(QString::fromUtf8("dlBrowseButton"));
        dlBrowseButton->setMinimumSize(QSize(80, 30));

        generalGLayout->addWidget(dlBrowseButton, 3, 6, 1, 1);

        mmsVSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        generalGLayout->addItem(mmsVSpacer, 4, 0, 1, 1);

        themeLabel = new QLabel(generalPage);
        themeLabel->setObjectName(QString::fromUtf8("themeLabel"));
        sizePolicy2.setHeightForWidth(themeLabel->sizePolicy().hasHeightForWidth());
        themeLabel->setSizePolicy(sizePolicy2);

        generalGLayout->addWidget(themeLabel, 5, 0, 1, 1);

        themeCombo = new QComboBox(generalPage);
        themeCombo->setObjectName(QString::fromUtf8("themeCombo"));

        generalGLayout->addWidget(themeCombo, 5, 1, 1, 5);

        themeVSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        generalGLayout->addItem(themeVSpacer, 6, 0, 1, 1);

        grpCallAutoJoinCheck = new QCheckBox(generalPage);
        grpCallAutoJoinCheck->setObjectName(QString::fromUtf8("grpCallAutoJoinCheck"));
        grpCallAutoJoinCheck->setCheckable(true);

        generalGLayout->addWidget(grpCallAutoJoinCheck, 7, 0, 1, 7);

        monRetainCheck = new QCheckBox(generalPage);
        monRetainCheck->setObjectName(QString::fromUtf8("monRetainCheck"));
        monRetainCheck->setCheckable(true);

        generalGLayout->addWidget(monRetainCheck, 8, 0, 1, 7);

        pttVSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        generalGLayout->addItem(pttVSpacer, 9, 0, 1, 1);

        pttLabel = new QLabel(generalPage);
        pttLabel->setObjectName(QString::fromUtf8("pttLabel"));

        generalGLayout->addWidget(pttLabel, 10, 0, 1, 1);

        pttCtrlCheck = new QCheckBox(generalPage);
        pttCtrlCheck->setObjectName(QString::fromUtf8("pttCtrlCheck"));
        pttCtrlCheck->setCheckable(true);

        generalGLayout->addWidget(pttCtrlCheck, 10, 1, 1, 1);

        pttAltCheck = new QCheckBox(generalPage);
        pttAltCheck->setObjectName(QString::fromUtf8("pttAltCheck"));
        pttAltCheck->setCheckable(true);

        generalGLayout->addWidget(pttAltCheck, 10, 2, 1, 1);

        pttEdit = new QLineEdit(generalPage);
        pttEdit->setObjectName(QString::fromUtf8("pttEdit"));
        sizePolicy.setHeightForWidth(pttEdit->sizePolicy().hasHeightForWidth());
        pttEdit->setSizePolicy(sizePolicy);
        pttEdit->setMinimumSize(QSize(0, 30));
        pttEdit->setAlignment(Qt::AlignCenter);
        pttEdit->setMaximumWidth(30);
        pttEdit->setMaxLength(1);

        generalGLayout->addWidget(pttEdit, 10, 3, 1, 1);

        pttSpacer = new QSpacerItem(80, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        generalGLayout->addItem(pttSpacer, 10, 4, 1, 1);

        monVSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        generalGLayout->addItem(monVSpacer, 11, 0, 1, 1);

        gpsMonButton = new QPushButton(generalPage);
        gpsMonButton->setObjectName(QString::fromUtf8("gpsMonButton"));
        gpsMonButton->setMinimumSize(QSize(80, 30));
        gpsMonButton->setMaximumSize(QSize(240, 30));

        generalGLayout->addWidget(gpsMonButton, 12, 0, 1, 7);

        branchButton = new QPushButton(generalPage);
        branchButton->setObjectName(QString::fromUtf8("branchButton"));
        branchButton->setMinimumSize(QSize(80, 30));
        branchButton->setMaximumSize(QSize(240, 30));

        generalGLayout->addWidget(branchButton, 13, 0, 1, 7);

        gridVSpacer1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        generalGLayout->addItem(gridVSpacer1, 14, 0, 1, 1);

        stackedWidget->addWidget(generalPage);
        resourcePage = new QWidget();
        resourcePage->setObjectName(QString::fromUtf8("resourcePage"));
        resourceGLayout = new QGridLayout(resourcePage);
        resourceGLayout->setObjectName(QString::fromUtf8("resourceGLayout"));
        grpDspLabel = new QLabel(resourcePage);
        grpDspLabel->setObjectName(QString::fromUtf8("grpDspLabel"));
        sizePolicy.setHeightForWidth(grpDspLabel->sizePolicy().hasHeightForWidth());
        grpDspLabel->setSizePolicy(sizePolicy);
        grpDspLabel->setAlignment(Qt::AlignTop);

        resourceGLayout->addWidget(grpDspLabel, 0, 0, 1, 1);

        grpDspCombo = new QComboBox(resourcePage);
        grpDspCombo->setObjectName(QString::fromUtf8("grpDspCombo"));

        resourceGLayout->addWidget(grpDspCombo, 0, 1, 1, 1);

        subsDspLabel = new QLabel(resourcePage);
        subsDspLabel->setObjectName(QString::fromUtf8("subsDspLabel"));
        sizePolicy.setHeightForWidth(subsDspLabel->sizePolicy().hasHeightForWidth());
        subsDspLabel->setSizePolicy(sizePolicy);
        subsDspLabel->setAlignment(Qt::AlignTop);

        resourceGLayout->addWidget(subsDspLabel, 1, 0, 1, 1);

        subsDspCombo = new QComboBox(resourcePage);
        subsDspCombo->setObjectName(QString::fromUtf8("subsDspCombo"));

        resourceGLayout->addWidget(subsDspCombo, 1, 1, 1, 1);

        termLblLabel = new QLabel(resourcePage);
        termLblLabel->setObjectName(QString::fromUtf8("termLblLabel"));
        sizePolicy.setHeightForWidth(termLblLabel->sizePolicy().hasHeightForWidth());
        termLblLabel->setSizePolicy(sizePolicy);
        termLblLabel->setAlignment(Qt::AlignTop);

        resourceGLayout->addWidget(termLblLabel, 2, 0, 1, 1);

        termLblCombo = new QComboBox(resourcePage);
        termLblCombo->setObjectName(QString::fromUtf8("termLblCombo"));

        resourceGLayout->addWidget(termLblCombo, 2, 1, 1, 1);

        gridVSpacer5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        resourceGLayout->addItem(gridVSpacer5, 3, 0, 1, 1);

        stackedWidget->addWidget(resourcePage);
        connectionPage = new QWidget();
        connectionPage->setObjectName(QString::fromUtf8("connectionPage"));
        connectionGLayout = new QGridLayout(connectionPage);
        connectionGLayout->setObjectName(QString::fromUtf8("connectionGLayout"));
        serverIPLabel = new QLabel(connectionPage);
        serverIPLabel->setObjectName(QString::fromUtf8("serverIPLabel"));

        connectionGLayout->addWidget(serverIPLabel, 0, 0, 1, 1);

        serverIpEdit = new QLineEdit(connectionPage);
        serverIpEdit->setObjectName(QString::fromUtf8("serverIpEdit"));
        serverIpEdit->setEnabled(true);
        serverIpEdit->setMinimumSize(QSize(0, 30));

        connectionGLayout->addWidget(serverIpEdit, 0, 1, 1, 1);

        portLabel = new QLabel(connectionPage);
        portLabel->setObjectName(QString::fromUtf8("portLabel"));

        connectionGLayout->addWidget(portLabel, 1, 0, 1, 1);

        serverPortEdit = new QLineEdit(connectionPage);
        serverPortEdit->setObjectName(QString::fromUtf8("serverPortEdit"));
        serverPortEdit->setEnabled(true);
        serverPortEdit->setMinimumSize(QSize(0, 30));

        connectionGLayout->addWidget(serverPortEdit, 1, 1, 1, 1);

        pswdVSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        connectionGLayout->addItem(pswdVSpacer, 2, 0, 1, 1);

        changePswdButton = new QPushButton(connectionPage);
        changePswdButton->setObjectName(QString::fromUtf8("changePswdButton"));
        changePswdButton->setEnabled(false);
        changePswdButton->setMinimumSize(QSize(80, 30));
        changePswdButton->setMaximumSize(QSize(240, 30));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Images/images/icon_password.png"), QSize(), QIcon::Normal, QIcon::Off);
        changePswdButton->setIcon(icon8);

        connectionGLayout->addWidget(changePswdButton, 3, 1, 1, 1);

        gridVSpacer2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        connectionGLayout->addItem(gridVSpacer2, 4, 0, 1, 1);

        stackedWidget->addWidget(connectionPage);
        mapPage = new QWidget();
        mapPage->setObjectName(QString::fromUtf8("mapPage"));
        mapGLayout = new QGridLayout(mapPage);
        mapGLayout->setObjectName(QString::fromUtf8("mapGLayout"));
        mapLabel = new QLabel(mapPage);
        mapLabel->setObjectName(QString::fromUtf8("mapLabel"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(mapLabel->sizePolicy().hasHeightForWidth());
        mapLabel->setSizePolicy(sizePolicy3);

        mapGLayout->addWidget(mapLabel, 0, 0, 1, 1);

        mapEdit = new QLineEdit(mapPage);
        mapEdit->setObjectName(QString::fromUtf8("mapEdit"));
        mapEdit->setMinimumSize(QSize(0, 30));
        mapEdit->setReadOnly(true);

        mapGLayout->addWidget(mapEdit, 0, 1, 1, 2);

        browseButton = new QPushButton(mapPage);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));
        browseButton->setMinimumSize(QSize(80, 30));

        mapGLayout->addWidget(browseButton, 0, 3, 1, 1);

        mapVSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        mapGLayout->addItem(mapVSpacer, 1, 0, 1, 1);

        termTimeLabel = new QLabel(mapPage);
        termTimeLabel->setObjectName(QString::fromUtf8("termTimeLabel"));

        mapGLayout->addWidget(termTimeLabel, 2, 0, 1, 4);

        termChangeLabel = new QLabel(mapPage);
        termChangeLabel->setObjectName(QString::fromUtf8("termChangeLabel"));
        termChangeLabel->setIndent(20);

        mapGLayout->addWidget(termChangeLabel, 3, 0, 1, 2);

        termChangeEdit = new QLineEdit(mapPage);
        termChangeEdit->setObjectName(QString::fromUtf8("termChangeEdit"));
        termChangeEdit->setMinimumSize(QSize(0, 30));

        mapGLayout->addWidget(termChangeEdit, 3, 2, 1, 1);

        termRemoveLabel = new QLabel(mapPage);
        termRemoveLabel->setObjectName(QString::fromUtf8("termRemoveLabel"));
        termRemoveLabel->setIndent(20);

        mapGLayout->addWidget(termRemoveLabel, 4, 0, 1, 2);

        termRemoveEdit = new QLineEdit(mapPage);
        termRemoveEdit->setObjectName(QString::fromUtf8("termRemoveEdit"));
        termRemoveEdit->setMinimumSize(QSize(0, 30));

        mapGLayout->addWidget(termRemoveEdit, 4, 2, 1, 1);

        ctrRscCheck = new QCheckBox(mapPage);
        ctrRscCheck->setObjectName(QString::fromUtf8("ctrRscCheck"));
        ctrRscCheck->setCheckable(true);

        mapGLayout->addWidget(ctrRscCheck, 5, 0, 1, 4);

        seaMapCheck = new QCheckBox(mapPage);
        seaMapCheck->setObjectName(QString::fromUtf8("seaMapCheck"));
        seaMapCheck->setCheckable(true);

        mapGLayout->addWidget(seaMapCheck, 6, 0, 1, 1);

        gridVSpacer3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        mapGLayout->addItem(gridVSpacer3, 7, 0, 1, 1);

        stackedWidget->addWidget(mapPage);
        mediaPage = new QWidget();
        mediaPage->setObjectName(QString::fromUtf8("mediaPage"));
        gridLayout = new QGridLayout(mediaPage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        mediaFrame = new QFrame(mediaPage);
        mediaFrame->setObjectName(QString::fromUtf8("mediaFrame"));
        mediaFrame->setFrameShape(QFrame::StyledPanel);
        mediaFrame->setFrameShadow(QFrame::Raised);
        gridLayout_2 = new QGridLayout(mediaFrame);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        micLabel = new QLabel(mediaFrame);
        micLabel->setObjectName(QString::fromUtf8("micLabel"));
        sizePolicy3.setHeightForWidth(micLabel->sizePolicy().hasHeightForWidth());
        micLabel->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(micLabel, 0, 0, 1, 1);

        micCombo = new QComboBox(mediaFrame);
        micCombo->setObjectName(QString::fromUtf8("micCombo"));
        sizePolicy1.setHeightForWidth(micCombo->sizePolicy().hasHeightForWidth());
        micCombo->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(micCombo, 0, 1, 1, 2);

        audioInBar = new QProgressBar(mediaFrame);
        audioInBar->setObjectName(QString::fromUtf8("audioInBar"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(audioInBar->sizePolicy().hasHeightForWidth());
        audioInBar->setSizePolicy(sizePolicy4);
        audioInBar->setMinimumSize(QSize(0, 10));
        audioInBar->setMaximumSize(QSize(16777215, 10));
        audioInBar->setBaseSize(QSize(0, 10));
        audioInBar->setStyleSheet(QString::fromUtf8(""));
        audioInBar->setValue(0);
        audioInBar->setTextVisible(false);

        gridLayout_2->addWidget(audioInBar, 1, 1, 1, 2);

        outVoiceLabel = new QLabel(mediaFrame);
        outVoiceLabel->setObjectName(QString::fromUtf8("outVoiceLabel"));
        QSizePolicy sizePolicy5(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(outVoiceLabel->sizePolicy().hasHeightForWidth());
        outVoiceLabel->setSizePolicy(sizePolicy5);

        gridLayout_2->addWidget(outVoiceLabel, 2, 0, 1, 1);

        speakerCombo = new QComboBox(mediaFrame);
        speakerCombo->setObjectName(QString::fromUtf8("speakerCombo"));

        gridLayout_2->addWidget(speakerCombo, 2, 1, 1, 1);

        playButton = new QPushButton(mediaFrame);
        playButton->setObjectName(QString::fromUtf8("playButton"));
        QSizePolicy sizePolicy6(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(playButton->sizePolicy().hasHeightForWidth());
        playButton->setSizePolicy(sizePolicy6);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_audio_stop.png"), QSize(), QIcon::Normal, QIcon::On);
        playButton->setIcon(icon9);
        playButton->setCheckable(true);

        gridLayout_2->addWidget(playButton, 2, 2, 1, 1);

        audioVSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout_2->addItem(audioVSpacer, 3, 0, 1, 1);

        camLabel = new QLabel(mediaFrame);
        camLabel->setObjectName(QString::fromUtf8("camLabel"));
        sizePolicy3.setHeightForWidth(camLabel->sizePolicy().hasHeightForWidth());
        camLabel->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(camLabel, 4, 0, 1, 1);

        camCombo = new QComboBox(mediaFrame);
        camCombo->setObjectName(QString::fromUtf8("camCombo"));
        sizePolicy1.setHeightForWidth(camCombo->sizePolicy().hasHeightForWidth());
        camCombo->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(camCombo, 4, 1, 1, 2);

        resLabel = new QLabel(mediaFrame);
        resLabel->setObjectName(QString::fromUtf8("resLabel"));
        sizePolicy3.setHeightForWidth(resLabel->sizePolicy().hasHeightForWidth());
        resLabel->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(resLabel, 5, 0, 1, 1);

        resCombo = new QComboBox(mediaFrame);
        resCombo->setObjectName(QString::fromUtf8("resCombo"));
        sizePolicy1.setHeightForWidth(resCombo->sizePolicy().hasHeightForWidth());
        resCombo->setSizePolicy(sizePolicy1);

        gridLayout_2->addWidget(resCombo, 5, 1, 1, 2);

        videoLabel = new QLabel(mediaFrame);
        videoLabel->setObjectName(QString::fromUtf8("videoLabel"));
        sizePolicy3.setHeightForWidth(videoLabel->sizePolicy().hasHeightForWidth());
        videoLabel->setSizePolicy(sizePolicy3);
        videoLabel->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(videoLabel, 6, 1, 1, 2);


        gridLayout->addWidget(mediaFrame, 0, 0, 1, 1);

        stackedWidget->addWidget(mediaPage);
        aboutPage = new QWidget();
        aboutPage->setObjectName(QString::fromUtf8("aboutPage"));
        accountGLayout = new QGridLayout(aboutPage);
        accountGLayout->setObjectName(QString::fromUtf8("accountGLayout"));
        agencyLabel = new QLabel(aboutPage);
        agencyLabel->setObjectName(QString::fromUtf8("agencyLabel"));
        agencyLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_agency.png")));
        agencyLabel->setScaledContents(false);
        agencyLabel->setAlignment(Qt::AlignHCenter);

        accountGLayout->addWidget(agencyLabel, 0, 0, 1, 1);

        agencyNameLabel = new QLabel(aboutPage);
        agencyNameLabel->setObjectName(QString::fromUtf8("agencyNameLabel"));
        agencyNameLabel->setMinimumSize(QSize(0, 40));
        agencyNameLabel->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

        accountGLayout->addWidget(agencyNameLabel, 1, 0, 1, 1);

        titleLabel = new QLabel(aboutPage);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        titleLabel->setMinimumSize(QSize(0, 24));
        titleLabel->setAlignment(Qt::AlignCenter);

        accountGLayout->addWidget(titleLabel, 2, 0, 1, 1);

        registrationLabel = new QLabel(aboutPage);
        registrationLabel->setObjectName(QString::fromUtf8("registrationLabel"));
        registrationLabel->setAlignment(Qt::AlignCenter);

        accountGLayout->addWidget(registrationLabel, 3, 0, 1, 1);

        licenseInfoLabel = new QLabel(aboutPage);
        licenseInfoLabel->setObjectName(QString::fromUtf8("licenseInfoLabel"));
        licenseInfoLabel->setMinimumSize(QSize(0, 24));
        licenseInfoLabel->setAlignment(Qt::AlignCenter);

        accountGLayout->addWidget(licenseInfoLabel, 4, 0, 1, 1);

        gridVSpacer4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        accountGLayout->addItem(gridVSpacer4, 5, 0, 1, 1);

        stackedWidget->addWidget(aboutPage);

        verticalLayout_5->addWidget(stackedWidget);


        verticalLayout_2->addWidget(stackedFrame);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        okButton = new QPushButton(bodyFrame);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        okButton->setMinimumSize(QSize(90, 40));

        horizontalLayout->addWidget(okButton);

        horizontalSpacer_4 = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        cancelButton = new QPushButton(bodyFrame);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setMinimumSize(QSize(90, 40));

        horizontalLayout->addWidget(cancelButton);

        horizontalSpacer2 = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer2);

        applyButton = new QPushButton(bodyFrame);
        applyButton->setObjectName(QString::fromUtf8("applyButton"));
        applyButton->setMinimumSize(QSize(90, 40));

        horizontalLayout->addWidget(applyButton);


        verticalLayout_2->addLayout(horizontalLayout);


        settingsHLayout->addWidget(bodyFrame);


        verticalLayout->addLayout(settingsHLayout);


        retranslateUi(SettingsUi);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsUi);
    } // setupUi

    void retranslateUi(QDialog *SettingsUi)
    {
        SettingsUi->setWindowTitle(QCoreApplication::translate("SettingsUi", "Options", nullptr));
        generalButton->setText(QCoreApplication::translate("SettingsUi", "General", nullptr));
        resourceButton->setText(QCoreApplication::translate("SettingsUi", "Resources", nullptr));
        serverButton->setText(QCoreApplication::translate("SettingsUi", "Server", nullptr));
        mapButton->setText(QCoreApplication::translate("SettingsUi", "Map", nullptr));
        mediaButton->setText(QCoreApplication::translate("SettingsUi", "Media", nullptr));
        aboutButton->setText(QCoreApplication::translate("SettingsUi", "About", nullptr));
        categoryLabel->setText(QCoreApplication::translate("SettingsUi", "TextLabel", nullptr));
        logLabel->setText(QCoreApplication::translate("SettingsUi", "Log File", nullptr));
        logBrowseButton->setText(QCoreApplication::translate("SettingsUi", "Browse", nullptr));
        logLevelLabel->setText(QCoreApplication::translate("SettingsUi", "Log Level", nullptr));
        logLevelDefButton->setText(QCoreApplication::translate("SettingsUi", "Default", nullptr));
        dlLabel->setText(QCoreApplication::translate("SettingsUi", "MMS Download", nullptr));
#if QT_CONFIG(tooltip)
        dlEdit->setToolTip(QCoreApplication::translate("SettingsUi", "Folder", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        dlClrButton->setToolTip(QCoreApplication::translate("SettingsUi", "Use default", nullptr));
#endif // QT_CONFIG(tooltip)
        dlBrowseButton->setText(QCoreApplication::translate("SettingsUi", "Browse", nullptr));
        themeLabel->setText(QCoreApplication::translate("SettingsUi", "Color Theme", nullptr));
        pttLabel->setText(QCoreApplication::translate("SettingsUi", "PTT Key", nullptr));
        pttCtrlCheck->setText(QCoreApplication::translate("SettingsUi", "Ctrl-", nullptr));
        pttAltCheck->setText(QCoreApplication::translate("SettingsUi", "Alt-", nullptr));
        gpsMonButton->setText(QCoreApplication::translate("SettingsUi", "GPS Monitoring", nullptr));
        branchButton->setText(QCoreApplication::translate("SettingsUi", "Fleet Branches", nullptr));
        grpDspLabel->setText(QCoreApplication::translate("SettingsUi", "Group Display", nullptr));
        subsDspLabel->setText(QCoreApplication::translate("SettingsUi", "Subscriber Display", nullptr));
        termLblLabel->setText(QCoreApplication::translate("SettingsUi", "Map Terminal", nullptr));
        serverIPLabel->setText(QCoreApplication::translate("SettingsUi", "Server IP", nullptr));
        portLabel->setText(QCoreApplication::translate("SettingsUi", "Server Port", nullptr));
        changePswdButton->setText(QCoreApplication::translate("SettingsUi", "Change Password", nullptr));
        mapLabel->setText(QCoreApplication::translate("SettingsUi", "Map Location", nullptr));
        browseButton->setText(QCoreApplication::translate("SettingsUi", "Browse", nullptr));
        termTimeLabel->setText(QCoreApplication::translate("SettingsUi", "Terminal Location Update Time Thresholds (minutes)", nullptr));
        termChangeLabel->setText(QCoreApplication::translate("SettingsUi", "Change Appearance", nullptr));
        termRemoveLabel->setText(QCoreApplication::translate("SettingsUi", "Remove From Map", nullptr));
        micLabel->setText(QCoreApplication::translate("SettingsUi", "Input", nullptr));
        outVoiceLabel->setText(QCoreApplication::translate("SettingsUi", "Output\n"
"(Voice)", nullptr));
        camLabel->setText(QCoreApplication::translate("SettingsUi", "Camera", nullptr));
        resLabel->setText(QCoreApplication::translate("SettingsUi", "Resolution", nullptr));
        registrationLabel->setText(QCoreApplication::translate("SettingsUi", "License Info:", nullptr));
        okButton->setText(QCoreApplication::translate("SettingsUi", "OK", nullptr));
        cancelButton->setText(QCoreApplication::translate("SettingsUi", "Cancel", nullptr));
        applyButton->setText(QCoreApplication::translate("SettingsUi", "Apply", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsUi: public Ui_SettingsUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSUI_H
