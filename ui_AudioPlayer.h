/********************************************************************************
** Form generated from reading UI file 'AudioPlayer.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUDIOPLAYER_H
#define UI_AUDIOPLAYER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolButton>
#include <qvideowidget.h>

QT_BEGIN_NAMESPACE

class Ui_AudioPlayer
{
public:
    QGridLayout *gridLayout;
    QPushButton *playButton;
    QLabel *txPartyLabel;
    QToolButton *volumeToolButton;
    QLabel *dateLabel;
    QLabel *idLabel;
    QPushButton *stopButton;
    QFrame *sliderFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *elapsedTimeLabel;
    QSlider *audioSlider;
    QLabel *endTimeLabel;
    QFrame *volumeFrame;
    QHBoxLayout *horizontalLayout_3;
    QSlider *volumeSlider;
    QTableView *pttTable;
    QVideoWidget *vidWidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *okButton;

    void setupUi(QDialog *AudioPlayer)
    {
        if (AudioPlayer->objectName().isEmpty())
            AudioPlayer->setObjectName(QString::fromUtf8("AudioPlayer"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AudioPlayer->sizePolicy().hasHeightForWidth());
        AudioPlayer->setSizePolicy(sizePolicy);
        AudioPlayer->setMinimumSize(QSize(700, 0));
        AudioPlayer->setSizeGripEnabled(true);
        gridLayout = new QGridLayout(AudioPlayer);
        gridLayout->setSpacing(4);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(4, 4, 4, 4);
        playButton = new QPushButton(AudioPlayer);
        playButton->setObjectName(QString::fromUtf8("playButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(playButton->sizePolicy().hasHeightForWidth());
        playButton->setSizePolicy(sizePolicy1);
        playButton->setMinimumSize(QSize(0, 0));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_audio_play.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_audio_pause.png"), QSize(), QIcon::Normal, QIcon::On);
        playButton->setIcon(icon);
        playButton->setCheckable(true);
        playButton->setAutoDefault(true);

        gridLayout->addWidget(playButton, 2, 0, 1, 1);

        txPartyLabel = new QLabel(AudioPlayer);
        txPartyLabel->setObjectName(QString::fromUtf8("txPartyLabel"));
        txPartyLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(txPartyLabel, 2, 2, 1, 1);

        volumeToolButton = new QToolButton(AudioPlayer);
        volumeToolButton->setObjectName(QString::fromUtf8("volumeToolButton"));
        sizePolicy1.setHeightForWidth(volumeToolButton->sizePolicy().hasHeightForWidth());
        volumeToolButton->setSizePolicy(sizePolicy1);
        volumeToolButton->setStyleSheet(QString::fromUtf8("::menu-indicator{ image: url(none.jpg); }"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_speaker.png"), QSize(), QIcon::Normal, QIcon::Off);
        volumeToolButton->setIcon(icon1);
        volumeToolButton->setIconSize(QSize(32, 32));
        volumeToolButton->setPopupMode(QToolButton::InstantPopup);
        volumeToolButton->setAutoRaise(true);
        volumeToolButton->setArrowType(Qt::NoArrow);

        gridLayout->addWidget(volumeToolButton, 0, 4, 1, 1);

        dateLabel = new QLabel(AudioPlayer);
        dateLabel->setObjectName(QString::fromUtf8("dateLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(dateLabel->sizePolicy().hasHeightForWidth());
        dateLabel->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(dateLabel, 0, 0, 1, 4);

        idLabel = new QLabel(AudioPlayer);
        idLabel->setObjectName(QString::fromUtf8("idLabel"));

        gridLayout->addWidget(idLabel, 2, 3, 1, 2);

        stopButton = new QPushButton(AudioPlayer);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));
        sizePolicy1.setHeightForWidth(stopButton->sizePolicy().hasHeightForWidth());
        stopButton->setSizePolicy(sizePolicy1);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_audio_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        stopButton->setIcon(icon2);

        gridLayout->addWidget(stopButton, 2, 1, 1, 1);

        sliderFrame = new QFrame(AudioPlayer);
        sliderFrame->setObjectName(QString::fromUtf8("sliderFrame"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(sliderFrame->sizePolicy().hasHeightForWidth());
        sliderFrame->setSizePolicy(sizePolicy3);
        sliderFrame->setFrameShape(QFrame::StyledPanel);
        sliderFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(sliderFrame);
        horizontalLayout->setSpacing(4);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        elapsedTimeLabel = new QLabel(sliderFrame);
        elapsedTimeLabel->setObjectName(QString::fromUtf8("elapsedTimeLabel"));
        sizePolicy2.setHeightForWidth(elapsedTimeLabel->sizePolicy().hasHeightForWidth());
        elapsedTimeLabel->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(elapsedTimeLabel);

        audioSlider = new QSlider(sliderFrame);
        audioSlider->setObjectName(QString::fromUtf8("audioSlider"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(audioSlider->sizePolicy().hasHeightForWidth());
        audioSlider->setSizePolicy(sizePolicy4);
        audioSlider->setMinimumSize(QSize(0, 40));
        audioSlider->setSingleStep(1000);
        audioSlider->setPageStep(1000);
        audioSlider->setOrientation(Qt::Horizontal);

        horizontalLayout->addWidget(audioSlider);

        endTimeLabel = new QLabel(sliderFrame);
        endTimeLabel->setObjectName(QString::fromUtf8("endTimeLabel"));
        sizePolicy2.setHeightForWidth(endTimeLabel->sizePolicy().hasHeightForWidth());
        endTimeLabel->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(endTimeLabel);


        gridLayout->addWidget(sliderFrame, 1, 0, 1, 5);

        volumeFrame = new QFrame(AudioPlayer);
        volumeFrame->setObjectName(QString::fromUtf8("volumeFrame"));
        volumeFrame->setFrameShape(QFrame::Box);
        volumeFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_3 = new QHBoxLayout(volumeFrame);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(4, 4, 4, 4);
        volumeSlider = new QSlider(volumeFrame);
        volumeSlider->setObjectName(QString::fromUtf8("volumeSlider"));
        volumeSlider->setMinimumSize(QSize(0, 120));
        volumeSlider->setMinimum(0);
        volumeSlider->setMaximum(100);
        volumeSlider->setValue(100);
        volumeSlider->setTracking(true);
        volumeSlider->setOrientation(Qt::Vertical);
        volumeSlider->setInvertedAppearance(false);
        volumeSlider->setInvertedControls(false);

        horizontalLayout_3->addWidget(volumeSlider);


        gridLayout->addWidget(volumeFrame, 3, 0, 1, 1);

        pttTable = new QTableView(AudioPlayer);
        pttTable->setObjectName(QString::fromUtf8("pttTable"));
        pttTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        pttTable->setAlternatingRowColors(true);
        pttTable->setSelectionMode(QAbstractItemView::SingleSelection);
        pttTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        pttTable->horizontalHeader()->setStretchLastSection(true);

        gridLayout->addWidget(pttTable, 4, 0, 1, 5);

        vidWidget = new QVideoWidget(AudioPlayer);
        vidWidget->setObjectName(QString::fromUtf8("vidWidget"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(vidWidget->sizePolicy().hasHeightForWidth());
        vidWidget->setSizePolicy(sizePolicy5);
        vidWidget->setMinimumSize(QSize(640, 480));

        gridLayout->addWidget(vidWidget, 4, 0, 1, 5);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(-1, -1, 5, 5);
        horizontalSpacer = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        okButton = new QPushButton(AudioPlayer);
        okButton->setObjectName(QString::fromUtf8("okButton"));
        sizePolicy2.setHeightForWidth(okButton->sizePolicy().hasHeightForWidth());
        okButton->setSizePolicy(sizePolicy2);
        okButton->setMinimumSize(QSize(90, 40));

        horizontalLayout_2->addWidget(okButton);


        gridLayout->addLayout(horizontalLayout_2, 5, 3, 1, 2);


        retranslateUi(AudioPlayer);

        QMetaObject::connectSlotsByName(AudioPlayer);
    } // setupUi

    void retranslateUi(QDialog *AudioPlayer)
    {
        playButton->setText(QString());
        txPartyLabel->setText(QCoreApplication::translate("AudioPlayer", "Tx Party:", nullptr));
        volumeToolButton->setText(QString());
        dateLabel->setText(QString());
        idLabel->setText(QString());
        stopButton->setText(QString());
        elapsedTimeLabel->setText(QCoreApplication::translate("AudioPlayer", "00:00", nullptr));
        endTimeLabel->setText(QCoreApplication::translate("AudioPlayer", "--:--", nullptr));
        okButton->setText(QCoreApplication::translate("AudioPlayer", "OK", nullptr));
        (void)AudioPlayer;
    } // retranslateUi

};

namespace Ui {
    class AudioPlayer: public Ui_AudioPlayer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUDIOPLAYER_H
