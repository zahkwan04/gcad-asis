/********************************************************************************
** Form generated from reading UI file 'CallWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALLWINDOW_H
#define UI_CALLWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CallWindow
{
public:
    QGridLayout *gridLayout;
    QFrame *callFrame;
    QVBoxLayout *verticalLayout;
    QFrame *callDetailsFrame;
    QVBoxLayout *callDetailsVLayout;
    QHBoxLayout *timeHLayout;
    QSpacerItem *timeHSpacer1;
    QLabel *startLabel;
    QLabel *startTimeLabel;
    QSpacerItem *timeHSpacer2;
    QLabel *durationLabel;
    QLabel *timeDurationLabel;
    QSpacerItem *timeHSpacer3;
    QHBoxLayout *callDetailsHLayout;
    QSpacerItem *callDetailsHSpacer1;
    QLabel *callingPartyLabel;
    QSpacerItem *callDetailsHSpacer2;
    QLabel *arrowLabel;
    QSpacerItem *callDetailsHSpacer3;
    QLabel *calledIcon;
    QLabel *calledPartyLabel;
    QSpacerItem *callDetailsHSpacer4;
    QHBoxLayout *callHlayout;
    QSpacerItem *callHSpacer1;
    QLabel *txPartyIconLabel;
    QLabel *txPartyLabel;
    QFrame *statFrame;
    QHBoxLayout *statHLayout;
    QLabel *audStatLabel;
    QLabel *vidStatLabel;
    QSpacerItem *callHSpacer2;
    QHBoxLayout *otherCallHLayout;
    QRadioButton *noHook;
    QRadioButton *hook;
    QRadioButton *ambience;
    QHBoxLayout *buttonHLayout;
    QToolButton *listenButton;
    QToolButton *videoAiButton;
    QToolButton *videoButton;
    QToolButton *callButton;
    QToolButton *pttButton;
    QPushButton *micButton;
    QPushButton *speakerButton;
    QToolButton *speakerSingleButton;
    QToolButton *endButton;
    QHBoxLayout *closeHLayout;
    QCheckBox *keepOpenCheck;
    QCheckBox *preemptiveCheck;
    QSpacerItem *closeHSpacer;
    QPushButton *closeButton;
    QVBoxLayout *videoVLayout;
    QGraphicsView *videoView;
    QGridLayout *videoButtonGLayout;
    QSpacerItem *videoHSpacer1;
    QFrame *videoButtonFrame;
    QHBoxLayout *videoButtonHLayout;
    QToolButton *hidePreviewButton;
    QToolButton *fullscreenButton;
    QSpacerItem *videoVSpacer1;
    QVBoxLayout *previewVLayout;
    QLabel *previewLabel;
    QButtonGroup *categoryGroup;

    void setupUi(QDialog *CallWindow)
    {
        if (CallWindow->objectName().isEmpty())
            CallWindow->setObjectName(QString::fromUtf8("CallWindow"));
        CallWindow->resize(640, 290);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_main.png"), QSize(), QIcon::Normal, QIcon::Off);
        CallWindow->setWindowIcon(icon);
        CallWindow->setAutoFillBackground(false);
        gridLayout = new QGridLayout(CallWindow);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetMinimumSize);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        callFrame = new QFrame(CallWindow);
        callFrame->setObjectName(QString::fromUtf8("callFrame"));
        callFrame->setFrameShape(QFrame::StyledPanel);
        callFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(callFrame);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        callDetailsFrame = new QFrame(callFrame);
        callDetailsFrame->setObjectName(QString::fromUtf8("callDetailsFrame"));
        callDetailsVLayout = new QVBoxLayout(callDetailsFrame);
        callDetailsVLayout->setSpacing(5);
        callDetailsVLayout->setObjectName(QString::fromUtf8("callDetailsVLayout"));
        timeHLayout = new QHBoxLayout();
        timeHLayout->setSpacing(5);
        timeHLayout->setObjectName(QString::fromUtf8("timeHLayout"));
        timeHSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        timeHLayout->addItem(timeHSpacer1);

        startLabel = new QLabel(callDetailsFrame);
        startLabel->setObjectName(QString::fromUtf8("startLabel"));

        timeHLayout->addWidget(startLabel);

        startTimeLabel = new QLabel(callDetailsFrame);
        startTimeLabel->setObjectName(QString::fromUtf8("startTimeLabel"));

        timeHLayout->addWidget(startTimeLabel);

        timeHSpacer2 = new QSpacerItem(30, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        timeHLayout->addItem(timeHSpacer2);

        durationLabel = new QLabel(callDetailsFrame);
        durationLabel->setObjectName(QString::fromUtf8("durationLabel"));
        durationLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_timer.png")));
        durationLabel->setScaledContents(true);

        timeHLayout->addWidget(durationLabel);

        timeDurationLabel = new QLabel(callDetailsFrame);
        timeDurationLabel->setObjectName(QString::fromUtf8("timeDurationLabel"));
        timeDurationLabel->setAlignment(Qt::AlignCenter);

        timeHLayout->addWidget(timeDurationLabel);

        timeHSpacer3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        timeHLayout->addItem(timeHSpacer3);


        callDetailsVLayout->addLayout(timeHLayout);

        callDetailsHLayout = new QHBoxLayout();
        callDetailsHLayout->setObjectName(QString::fromUtf8("callDetailsHLayout"));
        callDetailsHSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        callDetailsHLayout->addItem(callDetailsHSpacer1);

        callingPartyLabel = new QLabel(callDetailsFrame);
        callingPartyLabel->setObjectName(QString::fromUtf8("callingPartyLabel"));
        callingPartyLabel->setAlignment(Qt::AlignCenter);

        callDetailsHLayout->addWidget(callingPartyLabel);

        callDetailsHSpacer2 = new QSpacerItem(15, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        callDetailsHLayout->addItem(callDetailsHSpacer2);

        arrowLabel = new QLabel(callDetailsFrame);
        arrowLabel->setObjectName(QString::fromUtf8("arrowLabel"));
        arrowLabel->setMaximumSize(QSize(100, 20));
        arrowLabel->setFrameShape(QFrame::NoFrame);
        arrowLabel->setFrameShadow(QFrame::Plain);
        arrowLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_arrow.png")));
        arrowLabel->setScaledContents(true);

        callDetailsHLayout->addWidget(arrowLabel);

        callDetailsHSpacer3 = new QSpacerItem(15, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        callDetailsHLayout->addItem(callDetailsHSpacer3);

        calledIcon = new QLabel(callDetailsFrame);
        calledIcon->setObjectName(QString::fromUtf8("calledIcon"));
        calledIcon->setMaximumSize(QSize(25, 25));
        calledIcon->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_mobile.png")));
        calledIcon->setScaledContents(true);

        callDetailsHLayout->addWidget(calledIcon);

        calledPartyLabel = new QLabel(callDetailsFrame);
        calledPartyLabel->setObjectName(QString::fromUtf8("calledPartyLabel"));
        calledPartyLabel->setAlignment(Qt::AlignCenter);

        callDetailsHLayout->addWidget(calledPartyLabel);

        callDetailsHSpacer4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        callDetailsHLayout->addItem(callDetailsHSpacer4);


        callDetailsVLayout->addLayout(callDetailsHLayout);

        callHlayout = new QHBoxLayout();
        callHlayout->setObjectName(QString::fromUtf8("callHlayout"));
        callHSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        callHlayout->addItem(callHSpacer1);

        txPartyIconLabel = new QLabel(callDetailsFrame);
        txPartyIconLabel->setObjectName(QString::fromUtf8("txPartyIconLabel"));
        txPartyIconLabel->setMaximumSize(QSize(25, 25));
        txPartyIconLabel->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_mobile.png")));
        txPartyIconLabel->setScaledContents(true);

        callHlayout->addWidget(txPartyIconLabel);

        txPartyLabel = new QLabel(callDetailsFrame);
        txPartyLabel->setObjectName(QString::fromUtf8("txPartyLabel"));
        txPartyLabel->setMaximumSize(QSize(16777215, 25));
        txPartyLabel->setFrameShape(QFrame::Panel);
        txPartyLabel->setFrameShadow(QFrame::Raised);

        callHlayout->addWidget(txPartyLabel);

        statFrame = new QFrame(callDetailsFrame);
        statFrame->setObjectName(QString::fromUtf8("statFrame"));
        statFrame->setVisible(false);
        statFrame->setContentsMargins(0, 0, 0, 0);
        statHLayout = new QHBoxLayout(statFrame);
        statHLayout->setSpacing(10);
        statHLayout->setObjectName(QString::fromUtf8("statHLayout"));
        statHLayout->setContentsMargins(10, 0, 0, 0);
        audStatLabel = new QLabel(statFrame);
        audStatLabel->setObjectName(QString::fromUtf8("audStatLabel"));

        statHLayout->addWidget(audStatLabel);

        vidStatLabel = new QLabel(statFrame);
        vidStatLabel->setObjectName(QString::fromUtf8("vidStatLabel"));

        statHLayout->addWidget(vidStatLabel);


        callHlayout->addWidget(statFrame);

        callHSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        callHlayout->addItem(callHSpacer2);


        callDetailsVLayout->addLayout(callHlayout);


        verticalLayout->addWidget(callDetailsFrame);

        otherCallHLayout = new QHBoxLayout();
        otherCallHLayout->setObjectName(QString::fromUtf8("otherCallHLayout"));
        otherCallHLayout->setSizeConstraint(QLayout::SetFixedSize);
        otherCallHLayout->setContentsMargins(10, 10, 10, -1);
        noHook = new QRadioButton(callFrame);
        categoryGroup = new QButtonGroup(CallWindow);
        categoryGroup->setObjectName(QString::fromUtf8("categoryGroup"));
        categoryGroup->addButton(noHook);
        noHook->setObjectName(QString::fromUtf8("noHook"));
        noHook->setEnabled(false);

        otherCallHLayout->addWidget(noHook);

        hook = new QRadioButton(callFrame);
        categoryGroup->addButton(hook);
        hook->setObjectName(QString::fromUtf8("hook"));
        hook->setEnabled(false);

        otherCallHLayout->addWidget(hook);

        ambience = new QRadioButton(callFrame);
        categoryGroup->addButton(ambience);
        ambience->setObjectName(QString::fromUtf8("ambience"));
        ambience->setEnabled(false);

        otherCallHLayout->addWidget(ambience);


        verticalLayout->addLayout(otherCallHLayout);

        buttonHLayout = new QHBoxLayout();
        buttonHLayout->setSpacing(5);
        buttonHLayout->setObjectName(QString::fromUtf8("buttonHLayout"));
        buttonHLayout->setContentsMargins(10, 5, 10, -1);
        listenButton = new QToolButton(callFrame);
        listenButton->setObjectName(QString::fromUtf8("listenButton"));
        listenButton->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listenButton->sizePolicy().hasHeightForWidth());
        listenButton->setSizePolicy(sizePolicy);
        listenButton->setMinimumSize(QSize(62, 61));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_listen.png"), QSize(), QIcon::Normal, QIcon::Off);
        listenButton->setIcon(icon1);
        listenButton->setIconSize(QSize(55, 55));
        listenButton->setAutoRaise(false);

        buttonHLayout->addWidget(listenButton);

        videoAiButton = new QToolButton(callFrame);
        videoAiButton->setObjectName(QString::fromUtf8("videoAiButton"));
        videoAiButton->setEnabled(true);
        videoAiButton->setVisible(false);
        sizePolicy.setHeightForWidth(videoAiButton->sizePolicy().hasHeightForWidth());
        videoAiButton->setSizePolicy(sizePolicy);
        videoAiButton->setMinimumSize(QSize(62, 61));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8("images/icon_video_ai.png"), QSize(), QIcon::Normal, QIcon::Off);
        videoAiButton->setIcon(icon2);
        videoAiButton->setIconSize(QSize(55, 55));

        buttonHLayout->addWidget(videoAiButton);

        videoButton = new QToolButton(callFrame);
        videoButton->setObjectName(QString::fromUtf8("videoButton"));
        videoButton->setEnabled(true);
        sizePolicy.setHeightForWidth(videoButton->sizePolicy().hasHeightForWidth());
        videoButton->setSizePolicy(sizePolicy);
        videoButton->setMinimumSize(QSize(62, 61));
        videoButton->setVisible(false);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_video.png"), QSize(), QIcon::Normal, QIcon::Off);
        videoButton->setIcon(icon3);
        videoButton->setIconSize(QSize(55, 55));

        buttonHLayout->addWidget(videoButton);

        callButton = new QToolButton(callFrame);
        callButton->setObjectName(QString::fromUtf8("callButton"));
        callButton->setEnabled(true);
        sizePolicy.setHeightForWidth(callButton->sizePolicy().hasHeightForWidth());
        callButton->setSizePolicy(sizePolicy);
        callButton->setMinimumSize(QSize(62, 61));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_joinCall.png"), QSize(), QIcon::Normal, QIcon::Off);
        callButton->setIcon(icon4);
        callButton->setIconSize(QSize(55, 55));

        buttonHLayout->addWidget(callButton);

        pttButton = new QToolButton(callFrame);
        pttButton->setObjectName(QString::fromUtf8("pttButton"));
        pttButton->setEnabled(true);
        sizePolicy.setHeightForWidth(pttButton->sizePolicy().hasHeightForWidth());
        pttButton->setSizePolicy(sizePolicy);
        pttButton->setMinimumSize(QSize(62, 61));
        pttButton->setAutoFillBackground(false);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_ptt.png"), QSize(), QIcon::Normal, QIcon::Off);
        pttButton->setIcon(icon5);
        pttButton->setIconSize(QSize(55, 55));
        pttButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

        buttonHLayout->addWidget(pttButton);

        micButton = new QPushButton(callFrame);
        micButton->setObjectName(QString::fromUtf8("micButton"));
        micButton->setEnabled(true);
        sizePolicy.setHeightForWidth(micButton->sizePolicy().hasHeightForWidth());
        micButton->setSizePolicy(sizePolicy);
        micButton->setMinimumSize(QSize(62, 61));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_microphone_mute.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_microphone.png"), QSize(), QIcon::Normal, QIcon::On);
        micButton->setIcon(icon6);
        micButton->setIconSize(QSize(45, 45));
        micButton->setCheckable(true);
        micButton->setAutoDefault(false);
        micButton->setFlat(false);

        buttonHLayout->addWidget(micButton);

        speakerButton = new QPushButton(callFrame);
        speakerButton->setObjectName(QString::fromUtf8("speakerButton"));
        sizePolicy.setHeightForWidth(speakerButton->sizePolicy().hasHeightForWidth());
        speakerButton->setSizePolicy(sizePolicy);
        speakerButton->setMinimumSize(QSize(62, 61));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_speaker_mute.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_speaker.png"), QSize(), QIcon::Normal, QIcon::On);
        speakerButton->setIcon(icon7);
        speakerButton->setIconSize(QSize(45, 45));
        speakerButton->setCheckable(true);
        speakerButton->setAutoDefault(false);

        buttonHLayout->addWidget(speakerButton);

        speakerSingleButton = new QToolButton(callFrame);
        speakerSingleButton->setObjectName(QString::fromUtf8("speakerSingleButton"));
        sizePolicy.setHeightForWidth(speakerSingleButton->sizePolicy().hasHeightForWidth());
        speakerSingleButton->setSizePolicy(sizePolicy);
        speakerSingleButton->setMinimumSize(QSize(62, 61));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Images/images/icon_speaker_single.png"), QSize(), QIcon::Normal, QIcon::Off);
        speakerSingleButton->setIcon(icon8);
        speakerSingleButton->setIconSize(QSize(45, 45));

        buttonHLayout->addWidget(speakerSingleButton);

        endButton = new QToolButton(callFrame);
        endButton->setObjectName(QString::fromUtf8("endButton"));
        endButton->setEnabled(false);
        sizePolicy.setHeightForWidth(endButton->sizePolicy().hasHeightForWidth());
        endButton->setSizePolicy(sizePolicy);
        endButton->setMinimumSize(QSize(62, 61));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_endCall.png"), QSize(), QIcon::Disabled, QIcon::On);
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_endCall2.png"), QSize(), QIcon::Active, QIcon::On);
        endButton->setIcon(icon9);
        endButton->setIconSize(QSize(55, 55));

        buttonHLayout->addWidget(endButton);


        verticalLayout->addLayout(buttonHLayout);

        closeHLayout = new QHBoxLayout();
        closeHLayout->setSpacing(0);
        closeHLayout->setObjectName(QString::fromUtf8("closeHLayout"));
        closeHLayout->setContentsMargins(10, 5, 10, 5);
        keepOpenCheck = new QCheckBox(callFrame);
        keepOpenCheck->setObjectName(QString::fromUtf8("keepOpenCheck"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(keepOpenCheck->sizePolicy().hasHeightForWidth());
        keepOpenCheck->setSizePolicy(sizePolicy1);
        keepOpenCheck->setCheckable(true);

        closeHLayout->addWidget(keepOpenCheck);

        preemptiveCheck = new QCheckBox(callFrame);
        preemptiveCheck->setObjectName(QString::fromUtf8("preemptiveCheck"));
        sizePolicy1.setHeightForWidth(preemptiveCheck->sizePolicy().hasHeightForWidth());
        preemptiveCheck->setSizePolicy(sizePolicy1);
        preemptiveCheck->setCheckable(true);

        closeHLayout->addWidget(preemptiveCheck);

        closeHSpacer = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        closeHLayout->addItem(closeHSpacer);

        closeButton = new QPushButton(callFrame);
        closeButton->setObjectName(QString::fromUtf8("closeButton"));
        sizePolicy.setHeightForWidth(closeButton->sizePolicy().hasHeightForWidth());
        closeButton->setSizePolicy(sizePolicy);
        closeButton->setMinimumSize(QSize(90, 40));

        closeHLayout->addWidget(closeButton);


        verticalLayout->addLayout(closeHLayout);


        gridLayout->addWidget(callFrame, 0, 0, 1, 2);

        videoVLayout = new QVBoxLayout();
        videoVLayout->setObjectName(QString::fromUtf8("videoVLayout"));
        videoView = new QGraphicsView(CallWindow);
        videoView->setObjectName(QString::fromUtf8("videoView"));
        videoView->setVisible(false);
        videoView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        videoView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        videoView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        QBrush brush(QColor(80, 80, 80, 255));
        brush.setStyle(Qt::SolidPattern);
        videoView->setBackgroundBrush(brush);
        videoView->setInteractive(false);

        videoVLayout->addWidget(videoView);


        gridLayout->addLayout(videoVLayout, 1, 0, 2, 2);

        videoButtonGLayout = new QGridLayout();
        videoButtonGLayout->setSpacing(0);
        videoButtonGLayout->setObjectName(QString::fromUtf8("videoButtonGLayout"));
        videoButtonGLayout->setSizeConstraint(QLayout::SetMinimumSize);
        videoButtonGLayout->setContentsMargins(0, 10, 10, 0);
        videoHSpacer1 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        videoButtonGLayout->addItem(videoHSpacer1, 0, 0, 1, 1);

        videoButtonFrame = new QFrame(CallWindow);
        videoButtonFrame->setObjectName(QString::fromUtf8("videoButtonFrame"));
        videoButtonFrame->setVisible(false);
        videoButtonFrame->setStyleSheet(QString::fromUtf8("QFrame { padding: 5px; background: rgba(80, 80, 80, 50) }"));
        videoButtonFrame->setFrameShape(QFrame::NoFrame);
        videoButtonFrame->setFrameShadow(QFrame::Plain);
        videoButtonHLayout = new QHBoxLayout(videoButtonFrame);
        videoButtonHLayout->setObjectName(QString::fromUtf8("videoButtonHLayout"));
        videoButtonHLayout->setContentsMargins(0, 0, 0, 0);
        hidePreviewButton = new QToolButton(videoButtonFrame);
        hidePreviewButton->setObjectName(QString::fromUtf8("hidePreviewButton"));
        hidePreviewButton->setEnabled(true);
        sizePolicy.setHeightForWidth(hidePreviewButton->sizePolicy().hasHeightForWidth());
        hidePreviewButton->setSizePolicy(sizePolicy);
        hidePreviewButton->setMinimumSize(QSize(32, 32));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/Images/images/icon_preview_hide.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon10.addFile(QString::fromUtf8(":/Images/images/icon_preview.png"), QSize(), QIcon::Normal, QIcon::On);
        hidePreviewButton->setIcon(icon10);
        hidePreviewButton->setIconSize(QSize(32, 32));
        hidePreviewButton->setCheckable(true);

        videoButtonHLayout->addWidget(hidePreviewButton);

        fullscreenButton = new QToolButton(videoButtonFrame);
        fullscreenButton->setObjectName(QString::fromUtf8("fullscreenButton"));
        fullscreenButton->setEnabled(true);
        sizePolicy.setHeightForWidth(fullscreenButton->sizePolicy().hasHeightForWidth());
        fullscreenButton->setSizePolicy(sizePolicy);
        fullscreenButton->setMinimumSize(QSize(32, 32));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/Images/images/icon_fullscreen.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon11.addFile(QString::fromUtf8(":/Images/images/icon_fullscreen_exit.png"), QSize(), QIcon::Normal, QIcon::On);
        fullscreenButton->setIcon(icon11);
        fullscreenButton->setIconSize(QSize(32, 32));
        fullscreenButton->setCheckable(true);

        videoButtonHLayout->addWidget(fullscreenButton);


        videoButtonGLayout->addWidget(videoButtonFrame, 0, 1, 1, 1);

        videoVSpacer1 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        videoButtonGLayout->addItem(videoVSpacer1, 1, 1, 1, 1);


        gridLayout->addLayout(videoButtonGLayout, 1, 0, 1, 2);

        previewVLayout = new QVBoxLayout();
        previewVLayout->setObjectName(QString::fromUtf8("previewVLayout"));
        previewVLayout->setContentsMargins(0, -1, 10, 10);
        previewLabel = new QLabel(CallWindow);
        previewLabel->setObjectName(QString::fromUtf8("previewLabel"));
        previewLabel->setVisible(false);
        previewLabel->setFrameShape(QFrame::Box);

        previewVLayout->addWidget(previewLabel);


        gridLayout->addLayout(previewVLayout, 2, 1, 1, 1);

        gridLayout->setRowStretch(1, 1);
        gridLayout->setRowStretch(2, 1);

        retranslateUi(CallWindow);

        QMetaObject::connectSlotsByName(CallWindow);
    } // setupUi

    void retranslateUi(QDialog *CallWindow)
    {
        CallWindow->setWindowTitle(QCoreApplication::translate("CallWindow", "Call", nullptr));
        startLabel->setText(QCoreApplication::translate("CallWindow", "Start Time:", nullptr));
        startTimeLabel->setText(QString());
#if QT_CONFIG(tooltip)
        durationLabel->setToolTip(QCoreApplication::translate("CallWindow", "Duration", nullptr));
#endif // QT_CONFIG(tooltip)
        timeDurationLabel->setText(QString());
        callingPartyLabel->setText(QString());
        arrowLabel->setText(QString());
        calledIcon->setText(QString());
        calledPartyLabel->setText(QString());
        txPartyIconLabel->setText(QString());
        txPartyLabel->setText(QString());
        audStatLabel->setText(QString());
        vidStatLabel->setText(QString());
        noHook->setText(QCoreApplication::translate("CallWindow", "No Hook Signaling", nullptr));
        hook->setText(QCoreApplication::translate("CallWindow", "Hook Signaling", nullptr));
        ambience->setText(QCoreApplication::translate("CallWindow", "Ambience Listening", nullptr));
#if QT_CONFIG(tooltip)
        listenButton->setToolTip(QCoreApplication::translate("CallWindow", "Listen", nullptr));
#endif // QT_CONFIG(tooltip)
        videoAiButton->setText(QString());
#if QT_CONFIG(tooltip)
        videoButton->setToolTip(QCoreApplication::translate("CallWindow", "Video Call", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        callButton->setToolTip(QCoreApplication::translate("CallWindow", "Join", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        pttButton->setToolTip(QCoreApplication::translate("CallWindow", "PTT", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        micButton->setToolTip(QCoreApplication::translate("CallWindow", "Talk/mute", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        speakerButton->setToolTip(QCoreApplication::translate("CallWindow", "Listen/silence", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        speakerSingleButton->setToolTip(QCoreApplication::translate("CallWindow", "Listen to this call only", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        endButton->setToolTip(QCoreApplication::translate("CallWindow", "End Call", nullptr));
#endif // QT_CONFIG(tooltip)
        keepOpenCheck->setText(QCoreApplication::translate("CallWindow", "Keep Open", nullptr));
        preemptiveCheck->setText(QCoreApplication::translate("CallWindow", "Preemptive Priority", nullptr));
        closeButton->setText(QCoreApplication::translate("CallWindow", "Close", nullptr));
#if QT_CONFIG(tooltip)
        hidePreviewButton->setToolTip(QCoreApplication::translate("CallWindow", "Hide preview", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        fullscreenButton->setToolTip(QCoreApplication::translate("CallWindow", "Full screen", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class CallWindow: public Ui_CallWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALLWINDOW_H
