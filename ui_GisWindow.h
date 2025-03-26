/********************************************************************************
** Form generated from reading UI file 'GisWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISWINDOW_H
#define UI_GISWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GisWindow
{
public:
    QSplitter *centralwidget;
    QFrame *sideFrame;
    QVBoxLayout *sideLayout;
    QFrame *titleFrame;
    QHBoxLayout *titleLayout;
    QLabel *mainIcon;
    QLabel *versionLbl;
    QToolButton *reloadTlBtn;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidget;
    QVBoxLayout *scrollLayout;
    QSplitter *splitter1;
    QFrame *mapFrame;
    QVBoxLayout *mapLayout;
    QFrame *mapBtnFrame;
    QHBoxLayout *btnLayout;
    QToolButton *sideTlBtn;
    QToolButton *selectTlBtn;
    QToolButton *zoomTlBtn;
    QToolButton *measureTlBtn;
    QToolButton *zoomOutTlBtn;
    QSlider *zoomSlider;
    QToolButton *zoomInTlBtn;
    QToolButton *backTlBtn;
    QToolButton *fwdTlBtn;
    QSpacerItem *menuHSpacer2;
    QToolButton *saveTlBtn;
    QSpacerItem *menuHSpacer3;
    QComboBox *searchCb;
    QToolButton *searchTlBtn;
    QToolButton *clearTlBtn;
    QToolButton *filterBtn;
    QVBoxLayout *mapCanvLo;
    QHBoxLayout *coordLayout;
    QFrame *mapInfoFrame;
    QHBoxLayout *mapInfoLayout;
    QLabel *scaleLbl;
    QSpacerItem *mapInfoHSpacer;
    QLabel *coorLbl;

    void setupUi(QMainWindow *GisWindow)
    {
        if (GisWindow->objectName().isEmpty())
            GisWindow->setObjectName(QString::fromUtf8("GisWindow"));
        GisWindow->resize(1336, 811);
        QFont font;
        font.setFamily(QString::fromUtf8("Trebuchet MS"));
        font.setPointSize(10);
        GisWindow->setFont(font);
        centralwidget = new QSplitter(GisWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        centralwidget->setOrientation(Qt::Horizontal);
        centralwidget->setHandleWidth(2);
        centralwidget->setChildrenCollapsible(false);
        sideFrame = new QFrame(centralwidget);
        sideFrame->setObjectName(QString::fromUtf8("sideFrame"));
        sideFrame->setMinimumSize(QSize(30, 0));
        sideFrame->setMaximumSize(QSize(550, 16777215));
        sideLayout = new QVBoxLayout(sideFrame);
        sideLayout->setSpacing(0);
        sideLayout->setObjectName(QString::fromUtf8("sideLayout"));
        sideLayout->setContentsMargins(0, 0, 0, 0);
        titleFrame = new QFrame(sideFrame);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(titleFrame->sizePolicy().hasHeightForWidth());
        titleFrame->setSizePolicy(sizePolicy1);
        titleFrame->setMinimumSize(QSize(0, 110));
        titleFrame->setMaximumSize(QSize(16777215, 110));
        titleFrame->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);"));
        titleFrame->setFrameShape(QFrame::StyledPanel);
        titleFrame->setFrameShadow(QFrame::Raised);
        titleLayout = new QHBoxLayout(titleFrame);
        titleLayout->setSpacing(10);
        titleLayout->setObjectName(QString::fromUtf8("titleLayout"));
        titleLayout->setContentsMargins(10, 5, 10, 5);
        mainIcon = new QLabel(titleFrame);
        mainIcon->setObjectName(QString::fromUtf8("mainIcon"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(mainIcon->sizePolicy().hasHeightForWidth());
        mainIcon->setSizePolicy(sizePolicy2);
        mainIcon->setMaximumSize(QSize(85, 85));
        mainIcon->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_map.png")));
        mainIcon->setScaledContents(true);

        titleLayout->addWidget(mainIcon);

        versionLbl = new QLabel(titleFrame);
        versionLbl->setObjectName(QString::fromUtf8("versionLbl"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(versionLbl->sizePolicy().hasHeightForWidth());
        versionLbl->setSizePolicy(sizePolicy3);
        versionLbl->setWordWrap(true);

        titleLayout->addWidget(versionLbl);

        reloadTlBtn = new QToolButton(titleFrame);
        reloadTlBtn->setObjectName(QString::fromUtf8("reloadTlBtn"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        reloadTlBtn->setIcon(icon);
        reloadTlBtn->setIconSize(QSize(50, 50));
        reloadTlBtn->setAutoRaise(true);

        titleLayout->addWidget(reloadTlBtn);


        sideLayout->addWidget(titleFrame);

        scrollArea = new QScrollArea(sideFrame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        sizePolicy.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy);
        scrollArea->setStyleSheet(QString::fromUtf8("border:0;"));
        scrollArea->setFrameShape(QFrame::StyledPanel);
        scrollArea->setLineWidth(1);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidget = new QWidget();
        scrollAreaWidget->setObjectName(QString::fromUtf8("scrollAreaWidget"));
        scrollAreaWidget->setGeometry(QRect(0, 0, 502, 405));
        sizePolicy.setHeightForWidth(scrollAreaWidget->sizePolicy().hasHeightForWidth());
        scrollAreaWidget->setSizePolicy(sizePolicy);
        scrollLayout = new QVBoxLayout(scrollAreaWidget);
        scrollLayout->setSpacing(0);
        scrollLayout->setObjectName(QString::fromUtf8("scrollLayout"));
        scrollLayout->setContentsMargins(0, 0, 0, 0);
        splitter1 = new QSplitter(scrollAreaWidget);
        splitter1->setObjectName(QString::fromUtf8("splitter1"));
        sizePolicy.setHeightForWidth(splitter1->sizePolicy().hasHeightForWidth());
        splitter1->setSizePolicy(sizePolicy);
        splitter1->setOrientation(Qt::Vertical);
        splitter1->setHandleWidth(2);
        splitter1->setChildrenCollapsible(false);

        scrollLayout->addWidget(splitter1);

        scrollArea->setWidget(scrollAreaWidget);

        sideLayout->addWidget(scrollArea);

        centralwidget->addWidget(sideFrame);
        mapFrame = new QFrame(centralwidget);
        mapFrame->setObjectName(QString::fromUtf8("mapFrame"));
        mapFrame->setMinimumSize(QSize(580, 0));
        mapLayout = new QVBoxLayout(mapFrame);
        mapLayout->setSpacing(0);
        mapLayout->setObjectName(QString::fromUtf8("mapLayout"));
        mapLayout->setContentsMargins(0, 0, 0, 0);
        mapBtnFrame = new QFrame(mapFrame);
        mapBtnFrame->setObjectName(QString::fromUtf8("mapBtnFrame"));
        sizePolicy1.setHeightForWidth(mapBtnFrame->sizePolicy().hasHeightForWidth());
        mapBtnFrame->setSizePolicy(sizePolicy1);
        mapBtnFrame->setMinimumSize(QSize(0, 45));
        mapBtnFrame->setFrameShape(QFrame::StyledPanel);
        mapBtnFrame->setFrameShadow(QFrame::Raised);
        btnLayout = new QHBoxLayout(mapBtnFrame);
        btnLayout->setSpacing(5);
        btnLayout->setObjectName(QString::fromUtf8("btnLayout"));
        btnLayout->setContentsMargins(5, 0, 5, 0);
        sideTlBtn = new QToolButton(mapBtnFrame);
        sideTlBtn->setObjectName(QString::fromUtf8("sideTlBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_backward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_forward.png"), QSize(), QIcon::Normal, QIcon::On);
        sideTlBtn->setIcon(icon1);
        sideTlBtn->setCheckable(true);
        sideTlBtn->setAutoRaise(true);

        btnLayout->addWidget(sideTlBtn);

        selectTlBtn = new QToolButton(mapBtnFrame);
        selectTlBtn->setObjectName(QString::fromUtf8("selectTlBtn"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_pointer.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_pointer2.png"), QSize(), QIcon::Normal, QIcon::On);
        selectTlBtn->setIcon(icon2);
        selectTlBtn->setIconSize(QSize(30, 30));
        selectTlBtn->setCheckable(true);
        selectTlBtn->setAutoRaise(true);

        btnLayout->addWidget(selectTlBtn);

        zoomTlBtn = new QToolButton(mapBtnFrame);
        zoomTlBtn->setObjectName(QString::fromUtf8("zoomTlBtn"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_selection.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_selection2.png"), QSize(), QIcon::Normal, QIcon::On);
        zoomTlBtn->setIcon(icon3);
        zoomTlBtn->setIconSize(QSize(30, 30));
        zoomTlBtn->setCheckable(true);
        zoomTlBtn->setAutoRaise(true);

        btnLayout->addWidget(zoomTlBtn);

        measureTlBtn = new QToolButton(mapBtnFrame);
        measureTlBtn->setObjectName(QString::fromUtf8("measureTlBtn"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_measure.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_measure2.png"), QSize(), QIcon::Normal, QIcon::On);
        measureTlBtn->setIcon(icon4);
        measureTlBtn->setIconSize(QSize(30, 30));
        measureTlBtn->setCheckable(true);
        measureTlBtn->setAutoRaise(true);

        btnLayout->addWidget(measureTlBtn);

        zoomOutTlBtn = new QToolButton(mapBtnFrame);
        zoomOutTlBtn->setObjectName(QString::fromUtf8("zoomOutTlBtn"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_zoomOut.png"), QSize(), QIcon::Normal, QIcon::Off);
        zoomOutTlBtn->setIcon(icon5);
        zoomOutTlBtn->setIconSize(QSize(30, 30));
        zoomOutTlBtn->setAutoRaise(true);

        btnLayout->addWidget(zoomOutTlBtn);

        zoomSlider = new QSlider(mapBtnFrame);
        zoomSlider->setObjectName(QString::fromUtf8("zoomSlider"));
        zoomSlider->setOrientation(Qt::Horizontal);

        btnLayout->addWidget(zoomSlider);

        zoomInTlBtn = new QToolButton(mapBtnFrame);
        zoomInTlBtn->setObjectName(QString::fromUtf8("zoomInTlBtn"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_zoomIn.png"), QSize(), QIcon::Normal, QIcon::Off);
        zoomInTlBtn->setIcon(icon6);
        zoomInTlBtn->setIconSize(QSize(30, 30));
        zoomInTlBtn->setAutoRaise(true);

        btnLayout->addWidget(zoomInTlBtn);

        backTlBtn = new QToolButton(mapBtnFrame);
        backTlBtn->setObjectName(QString::fromUtf8("backTlBtn"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_view_back.png"), QSize(), QIcon::Normal, QIcon::Off);
        backTlBtn->setIcon(icon7);
        backTlBtn->setIconSize(QSize(30, 30));
        backTlBtn->setAutoRaise(true);

        btnLayout->addWidget(backTlBtn);

        fwdTlBtn = new QToolButton(mapBtnFrame);
        fwdTlBtn->setObjectName(QString::fromUtf8("fwdTlBtn"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Images/images/icon_view_forward.png"), QSize(), QIcon::Normal, QIcon::Off);
        fwdTlBtn->setIcon(icon8);
        fwdTlBtn->setIconSize(QSize(30, 30));
        fwdTlBtn->setAutoRaise(true);

        btnLayout->addWidget(fwdTlBtn);

        menuHSpacer2 = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        btnLayout->addItem(menuHSpacer2);

        saveTlBtn = new QToolButton(mapBtnFrame);
        saveTlBtn->setObjectName(QString::fromUtf8("saveTlBtn"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_save.png"), QSize(), QIcon::Normal, QIcon::Off);
        saveTlBtn->setIcon(icon9);
        saveTlBtn->setIconSize(QSize(30, 30));
        saveTlBtn->setAutoRaise(true);

        btnLayout->addWidget(saveTlBtn);

        menuHSpacer3 = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        btnLayout->addItem(menuHSpacer3);

        searchCb = new QComboBox(mapBtnFrame);
        searchCb->setObjectName(QString::fromUtf8("searchCb"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(searchCb->sizePolicy().hasHeightForWidth());
        searchCb->setSizePolicy(sizePolicy4);
        searchCb->setMinimumSize(QSize(160, 0));
        searchCb->setEditable(true);
        searchCb->setInsertPolicy(QComboBox::NoInsert);

        btnLayout->addWidget(searchCb);

        searchTlBtn = new QToolButton(mapBtnFrame);
        searchTlBtn->setObjectName(QString::fromUtf8("searchTlBtn"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/Images/images/icon_search.png"), QSize(), QIcon::Normal, QIcon::Off);
        searchTlBtn->setIcon(icon10);
        searchTlBtn->setIconSize(QSize(30, 30));
        searchTlBtn->setAutoRaise(true);

        btnLayout->addWidget(searchTlBtn);

        clearTlBtn = new QToolButton(mapBtnFrame);
        clearTlBtn->setObjectName(QString::fromUtf8("clearTlBtn"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/Images/images/icon_clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        clearTlBtn->setIcon(icon11);
        clearTlBtn->setIconSize(QSize(30, 30));
        clearTlBtn->setAutoRaise(true);

        btnLayout->addWidget(clearTlBtn);

        filterBtn = new QToolButton(mapBtnFrame);
        filterBtn->setObjectName(QString::fromUtf8("filterBtn"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/Images/images/icon_filter.png"), QSize(), QIcon::Normal, QIcon::Off);
        filterBtn->setIcon(icon12);
        filterBtn->setPopupMode(QToolButton::MenuButtonPopup);
        filterBtn->setIconSize(QSize(30, 30));
        filterBtn->setAutoRaise(true);

        btnLayout->addWidget(filterBtn);


        mapLayout->addWidget(mapBtnFrame);

        mapCanvLo = new QVBoxLayout();
        mapCanvLo->setSpacing(0);
        mapCanvLo->setObjectName(QString::fromUtf8("mapCanvLo"));

        mapLayout->addLayout(mapCanvLo);

        coordLayout = new QHBoxLayout();
        coordLayout->setSpacing(0);
        coordLayout->setObjectName(QString::fromUtf8("coordLayout"));
        mapInfoFrame = new QFrame(mapFrame);
        mapInfoFrame->setObjectName(QString::fromUtf8("mapInfoFrame"));
        sizePolicy1.setHeightForWidth(mapInfoFrame->sizePolicy().hasHeightForWidth());
        mapInfoFrame->setSizePolicy(sizePolicy1);
        mapInfoFrame->setMinimumSize(QSize(0, 35));
        mapInfoFrame->setFrameShape(QFrame::StyledPanel);
        mapInfoFrame->setFrameShadow(QFrame::Raised);
        mapInfoLayout = new QHBoxLayout(mapInfoFrame);
        mapInfoLayout->setSpacing(5);
        mapInfoLayout->setObjectName(QString::fromUtf8("mapInfoLayout"));
        mapInfoLayout->setContentsMargins(5, 5, 5, 5);
        scaleLbl = new QLabel(mapInfoFrame);
        scaleLbl->setObjectName(QString::fromUtf8("scaleLbl"));

        mapInfoLayout->addWidget(scaleLbl);

        mapInfoHSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        mapInfoLayout->addItem(mapInfoHSpacer);

        coorLbl = new QLabel(mapInfoFrame);
        coorLbl->setObjectName(QString::fromUtf8("coorLbl"));

        mapInfoLayout->addWidget(coorLbl);


        coordLayout->addWidget(mapInfoFrame);


        mapLayout->addLayout(coordLayout);

        centralwidget->addWidget(mapFrame);
        GisWindow->setCentralWidget(centralwidget);

        retranslateUi(GisWindow);

        QMetaObject::connectSlotsByName(GisWindow);
    } // setupUi

    void retranslateUi(QMainWindow *GisWindow)
    {
        GisWindow->setWindowTitle(QCoreApplication::translate("GisWindow", "GIS", nullptr));
        mainIcon->setText(QString());
#if QT_CONFIG(tooltip)
        reloadTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Reload map", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        sideTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Hide side panel", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        selectTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Select/Pan", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        zoomTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Zoom", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        measureTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Measure", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        zoomSlider->setToolTip(QCoreApplication::translate("GisWindow", "Zoom Level", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        backTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Back", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        fwdTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Forward", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        saveTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Save map image", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        searchCb->setToolTip(QCoreApplication::translate("GisWindow", "Search text", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        searchTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Search", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        clearTlBtn->setToolTip(QCoreApplication::translate("GisWindow", "Clear search", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        filterBtn->setToolTip(QCoreApplication::translate("GisWindow", "Filter", nullptr));
#endif // QT_CONFIG(tooltip)
        scaleLbl->setText(QCoreApplication::translate("GisWindow", "Map Scale", nullptr));
        coorLbl->setText(QCoreApplication::translate("GisWindow", "Coordinates", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GisWindow: public Ui_GisWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISWINDOW_H
