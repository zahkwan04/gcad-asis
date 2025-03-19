/********************************************************************************
** Form generated from reading UI file 'GisTrackingReplay.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISTRACKINGREPLAY_H
#define UI_GISTRACKINGREPLAY_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_GisTrackingReplay
{
public:
    QGridLayout *gridLayout;
    QFrame *startEndFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *startLbl;
    QLabel *pointsLbl;
    QLabel *endLbl;
    QFrame *buttonFrame;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *playButton;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *stopButton;
    QSpacerItem *horizontalSpacer_3;
    QComboBox *speedCb;
    QSpacerItem *horizontalSpacer_4;
    QFrame *sliderFrame;
    QHBoxLayout *horizontalLayout_3;
    QSlider *replaySlider;
    QFrame *currentFrame;
    QHBoxLayout *horizontalLayout_4;
    QLabel *currTimeLbl;
    QLabel *currPosLbl;

    void setupUi(QDialog *GisTrackingReplay)
    {
        if (GisTrackingReplay->objectName().isEmpty())
            GisTrackingReplay->setObjectName(QString::fromUtf8("GisTrackingReplay"));
        GisTrackingReplay->resize(551, 126);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_main.png"), QSize(), QIcon::Normal, QIcon::Off);
        GisTrackingReplay->setWindowIcon(icon);
        gridLayout = new QGridLayout(GisTrackingReplay);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        startEndFrame = new QFrame(GisTrackingReplay);
        startEndFrame->setObjectName(QString::fromUtf8("startEndFrame"));
        horizontalLayout = new QHBoxLayout(startEndFrame);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        startLbl = new QLabel(startEndFrame);
        startLbl->setObjectName(QString::fromUtf8("startLbl"));

        horizontalLayout->addWidget(startLbl);

        pointsLbl = new QLabel(startEndFrame);
        pointsLbl->setObjectName(QString::fromUtf8("pointsLbl"));
        pointsLbl->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(pointsLbl);

        endLbl = new QLabel(startEndFrame);
        endLbl->setObjectName(QString::fromUtf8("endLbl"));
        endLbl->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(endLbl);


        gridLayout->addWidget(startEndFrame, 0, 1, 1, 1);

        buttonFrame = new QFrame(GisTrackingReplay);
        buttonFrame->setObjectName(QString::fromUtf8("buttonFrame"));
        horizontalLayout_2 = new QHBoxLayout(buttonFrame);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        playButton = new QPushButton(buttonFrame);
        playButton->setObjectName(QString::fromUtf8("playButton"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(playButton->sizePolicy().hasHeightForWidth());
        playButton->setSizePolicy(sizePolicy);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_audio_pause.png"), QSize(), QIcon::Normal, QIcon::On);
        playButton->setIcon(icon1);
        playButton->setCheckable(true);

        horizontalLayout_2->addWidget(playButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        stopButton = new QPushButton(buttonFrame);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));
        sizePolicy.setHeightForWidth(stopButton->sizePolicy().hasHeightForWidth());
        stopButton->setSizePolicy(sizePolicy);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_audio_stop.png"), QSize(), QIcon::Normal, QIcon::On);
        stopButton->setIcon(icon2);

        horizontalLayout_2->addWidget(stopButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        speedCb = new QComboBox(buttonFrame);
        speedCb->setObjectName(QString::fromUtf8("speedCb"));

        horizontalLayout_2->addWidget(speedCb);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        gridLayout->addWidget(buttonFrame, 1, 1, 1, 1);

        sliderFrame = new QFrame(GisTrackingReplay);
        sliderFrame->setObjectName(QString::fromUtf8("sliderFrame"));
        horizontalLayout_3 = new QHBoxLayout(sliderFrame);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        replaySlider = new QSlider(sliderFrame);
        replaySlider->setObjectName(QString::fromUtf8("replaySlider"));
        replaySlider->setOrientation(Qt::Horizontal);

        horizontalLayout_3->addWidget(replaySlider);


        gridLayout->addWidget(sliderFrame, 2, 1, 1, 1);

        currentFrame = new QFrame(GisTrackingReplay);
        currentFrame->setObjectName(QString::fromUtf8("currentFrame"));
        horizontalLayout_4 = new QHBoxLayout(currentFrame);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        currTimeLbl = new QLabel(currentFrame);
        currTimeLbl->setObjectName(QString::fromUtf8("currTimeLbl"));
        currTimeLbl->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        currTimeLbl->setMargin(10);

        horizontalLayout_4->addWidget(currTimeLbl);

        currPosLbl = new QLabel(currentFrame);
        currPosLbl->setObjectName(QString::fromUtf8("currPosLbl"));
        currPosLbl->setMargin(10);

        horizontalLayout_4->addWidget(currPosLbl);


        gridLayout->addWidget(currentFrame, 3, 1, 1, 1);


        retranslateUi(GisTrackingReplay);

        QMetaObject::connectSlotsByName(GisTrackingReplay);
    } // setupUi

    void retranslateUi(QDialog *GisTrackingReplay)
    {
        GisTrackingReplay->setWindowTitle(QString());
        startLbl->setText(QString());
        pointsLbl->setText(QString());
        endLbl->setText(QString());
        playButton->setText(QString());
        stopButton->setText(QString());
        currTimeLbl->setText(QString());
        currPosLbl->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GisTrackingReplay: public Ui_GisTrackingReplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISTRACKINGREPLAY_H
