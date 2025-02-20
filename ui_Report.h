/********************************************************************************
** Form generated from reading UI file 'Report.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPORT_H
#define UI_REPORT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Report
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *mainFrame;
    QVBoxLayout *verticalLayout_3;
    QScrollArea *typeScroll;
    QGroupBox *categoryBox;
    QHBoxLayout *horizontalLayout;
    QRadioButton *callRadioBtn;
    QRadioButton *sdsRadioBtn;
    QRadioButton *stsRadioBtn;
    QRadioButton *mmsRadioBtn;
    QRadioButton *msgRadioBtn;
    QRadioButton *incidentRadioBtn;
    QRadioButton *locRadioBtn;
    QFrame *filterFrame;
    QVBoxLayout *verticalLayout_5;
    QGridLayout *filterGLayout;
    QLabel *searchByLabel;
    QGroupBox *searchByBox;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *idRadioBtn;
    QRadioButton *detailsRadioBtn;
    QLabel *startDateLabel;
    QDateEdit *startDate;
    QTimeEdit *startTime;
    QLabel *endDateLabel;
    QDateEdit *endDate;
    QTimeEdit *endTime;
    QLabel *fromLabel;
    QComboBox *fromCombo;
    QComboBox *andOrCombo;
    QLabel *toLabel;
    QFrame *toFrame;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *toTypeCombo;
    QComboBox *toCombo;
    QLabel *stsLabel;
    QComboBox *stsCombo;
    QLabel *typeLabel;
    QComboBox *typeCombo;
    QLabel *idLabel;
    QComboBox *idCombo;
    QLabel *priorityLabel;
    QComboBox *priorityCombo;
    QLabel *stateLabel;
    QComboBox *stateCombo;
    QLabel *categoryLabel;
    QComboBox *categoryCombo;
    QLabel *statusLabel;
    QComboBox *statusCombo;
    QPushButton *locButton;
    QLabel *locRscLabel;
    QComboBox *locRscCombo;
    QHBoxLayout *msgHLayout;
    QLabel *msgLabel;
    QComboBox *msgCombo;
    QHBoxLayout *descHLayout;
    QLabel *descLabel;
    QComboBox *descCombo;
    QHBoxLayout *rscHLayout;
    QLabel *resourceLabel;
    QComboBox *resourceCombo;
    QHBoxLayout *buttonHLayout;
    QSpacerItem *horizontalSpacer;
    QCheckBox *plotCheck;
    QPushButton *displayButton;
    QFrame *resultTitleFrame;
    QHBoxLayout *horizontalLayout_1;
    QLabel *resultTitleLabel;
    QSpacerItem *resultTitleFrameSpacer;
    QPushButton *printButton;
    QFrame *resultFrame;
    QHBoxLayout *horizontalLayout_2;
    QTableView *resultTable;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *Report)
    {
        if (Report->objectName().isEmpty())
            Report->setObjectName(QString::fromUtf8("Report"));
        Report->resize(339, 476);
        verticalLayout = new QVBoxLayout(Report);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        mainFrame = new QFrame(Report);
        mainFrame->setObjectName(QString::fromUtf8("mainFrame"));
        mainFrame->setFrameShape(QFrame::StyledPanel);
        mainFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(mainFrame);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        typeScroll = new QScrollArea(mainFrame);
        typeScroll->setObjectName(QString::fromUtf8("typeScroll"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(typeScroll->sizePolicy().hasHeightForWidth());
        typeScroll->setSizePolicy(sizePolicy);
        typeScroll->setFrameShape(QFrame::NoFrame);
        typeScroll->setWidgetResizable(true);
        categoryBox = new QGroupBox();
        categoryBox->setObjectName(QString::fromUtf8("categoryBox"));
        horizontalLayout = new QHBoxLayout(categoryBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout->setContentsMargins(5, 0, 5, 0);
        callRadioBtn = new QRadioButton(categoryBox);
        callRadioBtn->setObjectName(QString::fromUtf8("callRadioBtn"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(callRadioBtn->sizePolicy().hasHeightForWidth());
        callRadioBtn->setSizePolicy(sizePolicy1);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_call.png"), QSize(), QIcon::Normal, QIcon::Off);
        callRadioBtn->setIcon(icon);
        callRadioBtn->setIconSize(QSize(24, 24));
        callRadioBtn->setChecked(true);

        horizontalLayout->addWidget(callRadioBtn);

        sdsRadioBtn = new QRadioButton(categoryBox);
        sdsRadioBtn->setObjectName(QString::fromUtf8("sdsRadioBtn"));
        sizePolicy1.setHeightForWidth(sdsRadioBtn->sizePolicy().hasHeightForWidth());
        sdsRadioBtn->setSizePolicy(sizePolicy1);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_sds.png"), QSize(), QIcon::Normal, QIcon::Off);
        sdsRadioBtn->setIcon(icon1);
        sdsRadioBtn->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(sdsRadioBtn);

        stsRadioBtn = new QRadioButton(categoryBox);
        stsRadioBtn->setObjectName(QString::fromUtf8("stsRadioBtn"));
        sizePolicy1.setHeightForWidth(stsRadioBtn->sizePolicy().hasHeightForWidth());
        stsRadioBtn->setSizePolicy(sizePolicy1);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_statusMsg.png"), QSize(), QIcon::Normal, QIcon::Off);
        stsRadioBtn->setIcon(icon2);
        stsRadioBtn->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(stsRadioBtn);

        mmsRadioBtn = new QRadioButton(categoryBox);
        mmsRadioBtn->setObjectName(QString::fromUtf8("mmsRadioBtn"));
        sizePolicy1.setHeightForWidth(mmsRadioBtn->sizePolicy().hasHeightForWidth());
        mmsRadioBtn->setSizePolicy(sizePolicy1);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_mms.png"), QSize(), QIcon::Normal, QIcon::Off);
        mmsRadioBtn->setIcon(icon3);
        mmsRadioBtn->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(mmsRadioBtn);

        msgRadioBtn = new QRadioButton(categoryBox);
        msgRadioBtn->setObjectName(QString::fromUtf8("msgRadioBtn"));
        sizePolicy1.setHeightForWidth(msgRadioBtn->sizePolicy().hasHeightForWidth());
        msgRadioBtn->setSizePolicy(sizePolicy1);
        msgRadioBtn->setIcon(icon1);
        msgRadioBtn->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(msgRadioBtn);

        incidentRadioBtn = new QRadioButton(categoryBox);
        incidentRadioBtn->setObjectName(QString::fromUtf8("incidentRadioBtn"));
        sizePolicy1.setHeightForWidth(incidentRadioBtn->sizePolicy().hasHeightForWidth());
        incidentRadioBtn->setSizePolicy(sizePolicy1);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_incident.png"), QSize(), QIcon::Normal, QIcon::Off);
        incidentRadioBtn->setIcon(icon4);
        incidentRadioBtn->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(incidentRadioBtn);

        locRadioBtn = new QRadioButton(categoryBox);
        locRadioBtn->setObjectName(QString::fromUtf8("locRadioBtn"));
        sizePolicy1.setHeightForWidth(locRadioBtn->sizePolicy().hasHeightForWidth());
        locRadioBtn->setSizePolicy(sizePolicy1);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_dropPin.png"), QSize(), QIcon::Normal, QIcon::Off);
        locRadioBtn->setIcon(icon5);
        locRadioBtn->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(locRadioBtn);

        typeScroll->setWidget(categoryBox);

        verticalLayout_3->addWidget(typeScroll);

        filterFrame = new QFrame(mainFrame);
        filterFrame->setObjectName(QString::fromUtf8("filterFrame"));
        verticalLayout_5 = new QVBoxLayout(filterFrame);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setSizeConstraint(QLayout::SetMinimumSize);
        verticalLayout_5->setContentsMargins(5, 0, 5, 0);
        filterGLayout = new QGridLayout();
        filterGLayout->setObjectName(QString::fromUtf8("filterGLayout"));
        filterGLayout->setSizeConstraint(QLayout::SetMinimumSize);
        searchByLabel = new QLabel(filterFrame);
        searchByLabel->setObjectName(QString::fromUtf8("searchByLabel"));
        sizePolicy1.setHeightForWidth(searchByLabel->sizePolicy().hasHeightForWidth());
        searchByLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(searchByLabel, 0, 0, 1, 1);

        searchByBox = new QGroupBox(filterFrame);
        searchByBox->setObjectName(QString::fromUtf8("searchByBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(searchByBox->sizePolicy().hasHeightForWidth());
        searchByBox->setSizePolicy(sizePolicy2);
        searchByBox->setFlat(false);
        horizontalLayout_3 = new QHBoxLayout(searchByBox);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_3->setContentsMargins(5, 0, 5, 0);
        idRadioBtn = new QRadioButton(searchByBox);
        idRadioBtn->setObjectName(QString::fromUtf8("idRadioBtn"));
        sizePolicy1.setHeightForWidth(idRadioBtn->sizePolicy().hasHeightForWidth());
        idRadioBtn->setSizePolicy(sizePolicy1);
        idRadioBtn->setChecked(true);

        horizontalLayout_3->addWidget(idRadioBtn);

        detailsRadioBtn = new QRadioButton(searchByBox);
        detailsRadioBtn->setObjectName(QString::fromUtf8("detailsRadioBtn"));
        sizePolicy1.setHeightForWidth(detailsRadioBtn->sizePolicy().hasHeightForWidth());
        detailsRadioBtn->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(detailsRadioBtn);


        filterGLayout->addWidget(searchByBox, 0, 1, 1, 3);

        startDateLabel = new QLabel(filterFrame);
        startDateLabel->setObjectName(QString::fromUtf8("startDateLabel"));
        sizePolicy1.setHeightForWidth(startDateLabel->sizePolicy().hasHeightForWidth());
        startDateLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(startDateLabel, 1, 0, 1, 1);

        startDate = new QDateEdit(filterFrame);
        startDate->setObjectName(QString::fromUtf8("startDate"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(startDate->sizePolicy().hasHeightForWidth());
        startDate->setSizePolicy(sizePolicy3);
        startDate->setMinimumSize(QSize(0, 30));
        startDate->setCalendarPopup(true);

        filterGLayout->addWidget(startDate, 1, 1, 1, 1);

        startTime = new QTimeEdit(filterFrame);
        startTime->setObjectName(QString::fromUtf8("startTime"));
        sizePolicy3.setHeightForWidth(startTime->sizePolicy().hasHeightForWidth());
        startTime->setSizePolicy(sizePolicy3);
        startTime->setMinimumSize(QSize(0, 30));

        filterGLayout->addWidget(startTime, 1, 2, 1, 1);

        endDateLabel = new QLabel(filterFrame);
        endDateLabel->setObjectName(QString::fromUtf8("endDateLabel"));
        sizePolicy1.setHeightForWidth(endDateLabel->sizePolicy().hasHeightForWidth());
        endDateLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(endDateLabel, 1, 3, 1, 1);

        endDate = new QDateEdit(filterFrame);
        endDate->setObjectName(QString::fromUtf8("endDate"));
        sizePolicy3.setHeightForWidth(endDate->sizePolicy().hasHeightForWidth());
        endDate->setSizePolicy(sizePolicy3);
        endDate->setMinimumSize(QSize(0, 30));
        endDate->setCalendarPopup(true);

        filterGLayout->addWidget(endDate, 1, 4, 1, 1);

        endTime = new QTimeEdit(filterFrame);
        endTime->setObjectName(QString::fromUtf8("endTime"));
        sizePolicy3.setHeightForWidth(endTime->sizePolicy().hasHeightForWidth());
        endTime->setSizePolicy(sizePolicy3);
        endTime->setMinimumSize(QSize(0, 30));

        filterGLayout->addWidget(endTime, 1, 5, 1, 1);

        fromLabel = new QLabel(filterFrame);
        fromLabel->setObjectName(QString::fromUtf8("fromLabel"));
        sizePolicy1.setHeightForWidth(fromLabel->sizePolicy().hasHeightForWidth());
        fromLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(fromLabel, 2, 0, 1, 1);

        fromCombo = new QComboBox(filterFrame);
        fromCombo->setObjectName(QString::fromUtf8("fromCombo"));
        fromCombo->setEditable(true);
        fromCombo->setMaxCount(20);
        fromCombo->setInsertPolicy(QComboBox::NoInsert);
        fromCombo->setDuplicatesEnabled(false);

        filterGLayout->addWidget(fromCombo, 2, 1, 1, 1);

        andOrCombo = new QComboBox(filterFrame);
        andOrCombo->setObjectName(QString::fromUtf8("andOrCombo"));
        sizePolicy3.setHeightForWidth(andOrCombo->sizePolicy().hasHeightForWidth());
        andOrCombo->setSizePolicy(sizePolicy3);

        filterGLayout->addWidget(andOrCombo, 2, 2, 1, 1);

        toLabel = new QLabel(filterFrame);
        toLabel->setObjectName(QString::fromUtf8("toLabel"));
        sizePolicy1.setHeightForWidth(toLabel->sizePolicy().hasHeightForWidth());
        toLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(toLabel, 2, 3, 1, 1);

        toFrame = new QFrame(filterFrame);
        toFrame->setObjectName(QString::fromUtf8("toFrame"));
        toFrame->setFrameShape(QFrame::StyledPanel);
        toFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(toFrame);
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        toTypeCombo = new QComboBox(toFrame);
        toTypeCombo->setObjectName(QString::fromUtf8("toTypeCombo"));
        sizePolicy1.setHeightForWidth(toTypeCombo->sizePolicy().hasHeightForWidth());
        toTypeCombo->setSizePolicy(sizePolicy1);

        horizontalLayout_4->addWidget(toTypeCombo);

        toCombo = new QComboBox(toFrame);
        toCombo->setObjectName(QString::fromUtf8("toCombo"));
        sizePolicy3.setHeightForWidth(toCombo->sizePolicy().hasHeightForWidth());
        toCombo->setSizePolicy(sizePolicy3);
        toCombo->setEditable(true);
        toCombo->setMaxCount(20);
        toCombo->setInsertPolicy(QComboBox::NoInsert);
        toCombo->setDuplicatesEnabled(false);

        horizontalLayout_4->addWidget(toCombo);


        filterGLayout->addWidget(toFrame, 2, 4, 1, 2);

        stsLabel = new QLabel(filterFrame);
        stsLabel->setObjectName(QString::fromUtf8("stsLabel"));
        sizePolicy1.setHeightForWidth(stsLabel->sizePolicy().hasHeightForWidth());
        stsLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(stsLabel, 3, 0, 1, 1);

        stsCombo = new QComboBox(filterFrame);
        stsCombo->setObjectName(QString::fromUtf8("stsCombo"));
        QSizePolicy sizePolicy4(QSizePolicy::Ignored, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(stsCombo->sizePolicy().hasHeightForWidth());
        stsCombo->setSizePolicy(sizePolicy4);
        stsCombo->setEditable(true);
        stsCombo->setInsertPolicy(QComboBox::NoInsert);
        stsCombo->setDuplicatesEnabled(false);

        filterGLayout->addWidget(stsCombo, 3, 1, 1, 5);

        typeLabel = new QLabel(filterFrame);
        typeLabel->setObjectName(QString::fromUtf8("typeLabel"));
        sizePolicy1.setHeightForWidth(typeLabel->sizePolicy().hasHeightForWidth());
        typeLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(typeLabel, 4, 0, 1, 1);

        typeCombo = new QComboBox(filterFrame);
        typeCombo->setObjectName(QString::fromUtf8("typeCombo"));
        sizePolicy3.setHeightForWidth(typeCombo->sizePolicy().hasHeightForWidth());
        typeCombo->setSizePolicy(sizePolicy3);

        filterGLayout->addWidget(typeCombo, 4, 1, 1, 2);

        idLabel = new QLabel(filterFrame);
        idLabel->setObjectName(QString::fromUtf8("idLabel"));
        sizePolicy1.setHeightForWidth(idLabel->sizePolicy().hasHeightForWidth());
        idLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(idLabel, 5, 0, 1, 1);

        idCombo = new QComboBox(filterFrame);
        idCombo->setObjectName(QString::fromUtf8("idCombo"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(idCombo->sizePolicy().hasHeightForWidth());
        idCombo->setSizePolicy(sizePolicy5);
        idCombo->setEditable(true);
        idCombo->setMaxCount(20);
        idCombo->setInsertPolicy(QComboBox::NoInsert);
        idCombo->setDuplicatesEnabled(false);

        filterGLayout->addWidget(idCombo, 5, 1, 1, 1);

        priorityLabel = new QLabel(filterFrame);
        priorityLabel->setObjectName(QString::fromUtf8("priorityLabel"));
        sizePolicy1.setHeightForWidth(priorityLabel->sizePolicy().hasHeightForWidth());
        priorityLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(priorityLabel, 6, 0, 1, 1);

        priorityCombo = new QComboBox(filterFrame);
        priorityCombo->setObjectName(QString::fromUtf8("priorityCombo"));
        sizePolicy3.setHeightForWidth(priorityCombo->sizePolicy().hasHeightForWidth());
        priorityCombo->setSizePolicy(sizePolicy3);

        filterGLayout->addWidget(priorityCombo, 6, 1, 1, 2);

        stateLabel = new QLabel(filterFrame);
        stateLabel->setObjectName(QString::fromUtf8("stateLabel"));
        sizePolicy1.setHeightForWidth(stateLabel->sizePolicy().hasHeightForWidth());
        stateLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(stateLabel, 6, 3, 1, 1);

        stateCombo = new QComboBox(filterFrame);
        stateCombo->setObjectName(QString::fromUtf8("stateCombo"));
        sizePolicy3.setHeightForWidth(stateCombo->sizePolicy().hasHeightForWidth());
        stateCombo->setSizePolicy(sizePolicy3);

        filterGLayout->addWidget(stateCombo, 6, 4, 1, 2);

        categoryLabel = new QLabel(filterFrame);
        categoryLabel->setObjectName(QString::fromUtf8("categoryLabel"));
        sizePolicy1.setHeightForWidth(categoryLabel->sizePolicy().hasHeightForWidth());
        categoryLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(categoryLabel, 7, 0, 1, 1);

        categoryCombo = new QComboBox(filterFrame);
        categoryCombo->setObjectName(QString::fromUtf8("categoryCombo"));
        sizePolicy3.setHeightForWidth(categoryCombo->sizePolicy().hasHeightForWidth());
        categoryCombo->setSizePolicy(sizePolicy3);

        filterGLayout->addWidget(categoryCombo, 7, 1, 1, 2);

        statusLabel = new QLabel(filterFrame);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        sizePolicy1.setHeightForWidth(statusLabel->sizePolicy().hasHeightForWidth());
        statusLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(statusLabel, 7, 3, 1, 1);

        statusCombo = new QComboBox(filterFrame);
        statusCombo->setObjectName(QString::fromUtf8("statusCombo"));
        sizePolicy3.setHeightForWidth(statusCombo->sizePolicy().hasHeightForWidth());
        statusCombo->setSizePolicy(sizePolicy3);

        filterGLayout->addWidget(statusCombo, 7, 4, 1, 2);

        locButton = new QPushButton(filterFrame);
        locButton->setObjectName(QString::fromUtf8("locButton"));
        sizePolicy3.setHeightForWidth(locButton->sizePolicy().hasHeightForWidth());
        locButton->setSizePolicy(sizePolicy3);

        filterGLayout->addWidget(locButton, 8, 0, 1, 3);

        locRscLabel = new QLabel(filterFrame);
        locRscLabel->setObjectName(QString::fromUtf8("locRscLabel"));
        sizePolicy1.setHeightForWidth(locRscLabel->sizePolicy().hasHeightForWidth());
        locRscLabel->setSizePolicy(sizePolicy1);

        filterGLayout->addWidget(locRscLabel, 8, 3, 1, 1);

        locRscCombo = new QComboBox(filterFrame);
        locRscCombo->setObjectName(QString::fromUtf8("locRscCombo"));
        sizePolicy3.setHeightForWidth(locRscCombo->sizePolicy().hasHeightForWidth());
        locRscCombo->setSizePolicy(sizePolicy3);
        locRscCombo->setEditable(true);
        locRscCombo->setMaxCount(20);
        locRscCombo->setInsertPolicy(QComboBox::NoInsert);

        filterGLayout->addWidget(locRscCombo, 8, 4, 1, 2);


        verticalLayout_5->addLayout(filterGLayout);


        verticalLayout_3->addWidget(filterFrame);

        msgHLayout = new QHBoxLayout();
        msgHLayout->setObjectName(QString::fromUtf8("msgHLayout"));
        msgHLayout->setSizeConstraint(QLayout::SetMinimumSize);
        msgHLayout->setContentsMargins(5, 0, 5, -1);
        msgLabel = new QLabel(mainFrame);
        msgLabel->setObjectName(QString::fromUtf8("msgLabel"));
        sizePolicy1.setHeightForWidth(msgLabel->sizePolicy().hasHeightForWidth());
        msgLabel->setSizePolicy(sizePolicy1);

        msgHLayout->addWidget(msgLabel);

        msgCombo = new QComboBox(mainFrame);
        msgCombo->setObjectName(QString::fromUtf8("msgCombo"));
        sizePolicy3.setHeightForWidth(msgCombo->sizePolicy().hasHeightForWidth());
        msgCombo->setSizePolicy(sizePolicy3);
        msgCombo->setEditable(true);
        msgCombo->setMaxCount(20);
        msgCombo->setInsertPolicy(QComboBox::NoInsert);
        msgCombo->setDuplicatesEnabled(false);

        msgHLayout->addWidget(msgCombo);


        verticalLayout_3->addLayout(msgHLayout);

        descHLayout = new QHBoxLayout();
        descHLayout->setObjectName(QString::fromUtf8("descHLayout"));
        descHLayout->setSizeConstraint(QLayout::SetMinimumSize);
        descHLayout->setContentsMargins(5, 0, 5, -1);
        descLabel = new QLabel(mainFrame);
        descLabel->setObjectName(QString::fromUtf8("descLabel"));
        sizePolicy1.setHeightForWidth(descLabel->sizePolicy().hasHeightForWidth());
        descLabel->setSizePolicy(sizePolicy1);

        descHLayout->addWidget(descLabel);

        descCombo = new QComboBox(mainFrame);
        descCombo->setObjectName(QString::fromUtf8("descCombo"));
        sizePolicy3.setHeightForWidth(descCombo->sizePolicy().hasHeightForWidth());
        descCombo->setSizePolicy(sizePolicy3);
        descCombo->setEditable(true);
        descCombo->setMaxCount(20);
        descCombo->setInsertPolicy(QComboBox::NoInsert);
        descCombo->setDuplicatesEnabled(false);

        descHLayout->addWidget(descCombo);


        verticalLayout_3->addLayout(descHLayout);

        rscHLayout = new QHBoxLayout();
        rscHLayout->setObjectName(QString::fromUtf8("rscHLayout"));
        rscHLayout->setSizeConstraint(QLayout::SetMinimumSize);
        rscHLayout->setContentsMargins(5, 0, 5, -1);
        resourceLabel = new QLabel(mainFrame);
        resourceLabel->setObjectName(QString::fromUtf8("resourceLabel"));
        sizePolicy1.setHeightForWidth(resourceLabel->sizePolicy().hasHeightForWidth());
        resourceLabel->setSizePolicy(sizePolicy1);

        rscHLayout->addWidget(resourceLabel);

        resourceCombo = new QComboBox(mainFrame);
        resourceCombo->setObjectName(QString::fromUtf8("resourceCombo"));
        resourceCombo->setEditable(true);
        resourceCombo->setMaxCount(20);
        resourceCombo->setInsertPolicy(QComboBox::NoInsert);
        resourceCombo->setDuplicatesEnabled(false);

        rscHLayout->addWidget(resourceCombo);


        verticalLayout_3->addLayout(rscHLayout);

        buttonHLayout = new QHBoxLayout();
        buttonHLayout->setObjectName(QString::fromUtf8("buttonHLayout"));
        buttonHLayout->setSizeConstraint(QLayout::SetMinimumSize);
        buttonHLayout->setContentsMargins(5, 0, 5, 5);
        horizontalSpacer = new QSpacerItem(5, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonHLayout->addItem(horizontalSpacer);

        plotCheck = new QCheckBox(mainFrame);
        plotCheck->setObjectName(QString::fromUtf8("plotCheck"));
        plotCheck->setEnabled(false);

        buttonHLayout->addWidget(plotCheck);

        displayButton = new QPushButton(mainFrame);
        displayButton->setObjectName(QString::fromUtf8("displayButton"));

        buttonHLayout->addWidget(displayButton);


        verticalLayout_3->addLayout(buttonHLayout);


        verticalLayout->addWidget(mainFrame);

        resultTitleFrame = new QFrame(Report);
        resultTitleFrame->setObjectName(QString::fromUtf8("resultTitleFrame"));
        resultTitleFrame->setFrameShape(QFrame::StyledPanel);
        resultTitleFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_1 = new QHBoxLayout(resultTitleFrame);
        horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));
        horizontalLayout_1->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_1->setContentsMargins(5, 5, 5, 5);
        resultTitleLabel = new QLabel(resultTitleFrame);
        resultTitleLabel->setObjectName(QString::fromUtf8("resultTitleLabel"));

        horizontalLayout_1->addWidget(resultTitleLabel);

        resultTitleFrameSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_1->addItem(resultTitleFrameSpacer);

        printButton = new QPushButton(resultTitleFrame);
        printButton->setObjectName(QString::fromUtf8("printButton"));
        printButton->setEnabled(false);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_print.png"), QSize(), QIcon::Normal, QIcon::Off);
        printButton->setIcon(icon6);
        printButton->setIconSize(QSize(25, 25));

        horizontalLayout_1->addWidget(printButton);


        verticalLayout->addWidget(resultTitleFrame);

        resultFrame = new QFrame(Report);
        resultFrame->setObjectName(QString::fromUtf8("resultFrame"));
        resultFrame->setFrameShape(QFrame::StyledPanel);
        resultFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(resultFrame);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_2->setContentsMargins(0, 5, 0, 0);
        resultTable = new QTableView(resultFrame);
        resultTable->setObjectName(QString::fromUtf8("resultTable"));
        resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultTable->setAlternatingRowColors(true);
        resultTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
        resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        resultTable->setSortingEnabled(true);
        resultTable->horizontalHeader()->setStretchLastSection(true);

        horizontalLayout_2->addWidget(resultTable);

        verticalSpacer = new QSpacerItem(0, 380, QSizePolicy::Minimum, QSizePolicy::Expanding);

        horizontalLayout_2->addItem(verticalSpacer);


        verticalLayout->addWidget(resultFrame);

        QWidget::setTabOrder(callRadioBtn, sdsRadioBtn);
        QWidget::setTabOrder(sdsRadioBtn, stsRadioBtn);
        QWidget::setTabOrder(stsRadioBtn, msgRadioBtn);
        QWidget::setTabOrder(msgRadioBtn, incidentRadioBtn);
        QWidget::setTabOrder(incidentRadioBtn, idRadioBtn);
        QWidget::setTabOrder(idRadioBtn, detailsRadioBtn);
        QWidget::setTabOrder(detailsRadioBtn, startDate);
        QWidget::setTabOrder(startDate, startTime);
        QWidget::setTabOrder(startTime, endDate);
        QWidget::setTabOrder(endDate, endTime);
        QWidget::setTabOrder(endTime, fromCombo);
        QWidget::setTabOrder(fromCombo, stsCombo);
        QWidget::setTabOrder(stsCombo, typeCombo);
        QWidget::setTabOrder(typeCombo, idCombo);
        QWidget::setTabOrder(idCombo, priorityCombo);
        QWidget::setTabOrder(priorityCombo, stateCombo);
        QWidget::setTabOrder(stateCombo, categoryCombo);
        QWidget::setTabOrder(categoryCombo, statusCombo);
        QWidget::setTabOrder(statusCombo, msgCombo);
        QWidget::setTabOrder(msgCombo, descCombo);
        QWidget::setTabOrder(descCombo, resourceCombo);
        QWidget::setTabOrder(resourceCombo, displayButton);
        QWidget::setTabOrder(displayButton, resultTable);

        retranslateUi(Report);

        QMetaObject::connectSlotsByName(Report);
    } // setupUi

    void retranslateUi(QWidget *Report)
    {
        Report->setWindowTitle(QCoreApplication::translate("Report", "Report", nullptr));
        callRadioBtn->setText(QCoreApplication::translate("Report", "Call", nullptr));
        sdsRadioBtn->setText(QCoreApplication::translate("Report", "SDS", nullptr));
        stsRadioBtn->setText(QCoreApplication::translate("Report", "Status Message", nullptr));
        mmsRadioBtn->setText(QCoreApplication::translate("Report", "MMS", nullptr));
        msgRadioBtn->setText(QCoreApplication::translate("Report", "All Messages", nullptr));
        incidentRadioBtn->setText(QCoreApplication::translate("Report", "Incident", nullptr));
        locRadioBtn->setText(QCoreApplication::translate("Report", "Location", nullptr));
        searchByLabel->setText(QCoreApplication::translate("Report", "Search By:", nullptr));
        searchByBox->setTitle(QString());
        idRadioBtn->setText(QCoreApplication::translate("Report", "ID", nullptr));
        detailsRadioBtn->setText(QCoreApplication::translate("Report", "Details", nullptr));
        startDateLabel->setText(QCoreApplication::translate("Report", "Start:", nullptr));
        startDate->setDisplayFormat(QCoreApplication::translate("Report", "d/M/yyyy", nullptr));
        startTime->setDisplayFormat(QCoreApplication::translate("Report", "hh:mm", nullptr));
        endDateLabel->setText(QCoreApplication::translate("Report", "End:", nullptr));
        endDate->setDisplayFormat(QCoreApplication::translate("Report", "d/M/yyyy", nullptr));
        endTime->setDisplayFormat(QCoreApplication::translate("Report", "hh:mm", nullptr));
        fromLabel->setText(QCoreApplication::translate("Report", "From:", nullptr));
        toLabel->setText(QCoreApplication::translate("Report", "To:", nullptr));
        stsLabel->setText(QCoreApplication::translate("Report", "Status:", nullptr));
        typeLabel->setText(QCoreApplication::translate("Report", "Type:", nullptr));
        idLabel->setText(QCoreApplication::translate("Report", "ID:", nullptr));
        priorityLabel->setText(QCoreApplication::translate("Report", "Priority:", nullptr));
        stateLabel->setText(QCoreApplication::translate("Report", "State:", nullptr));
        categoryLabel->setText(QCoreApplication::translate("Report", "Category:", nullptr));
        statusLabel->setText(QCoreApplication::translate("Report", "Status:", nullptr));
        locButton->setText(QCoreApplication::translate("Report", "Types", nullptr));
        locRscLabel->setText(QCoreApplication::translate("Report", "Resources:", nullptr));
        msgLabel->setText(QCoreApplication::translate("Report", "Message Keywords:", nullptr));
        descLabel->setText(QCoreApplication::translate("Report", "Description Keywords:", nullptr));
        resourceLabel->setText(QCoreApplication::translate("Report", "Resource(s):", nullptr));
        plotCheck->setText(QCoreApplication::translate("Report", "Plot On Map", nullptr));
        displayButton->setText(QCoreApplication::translate("Report", "Display", nullptr));
        resultTitleLabel->setText(QCoreApplication::translate("Report", "Results", nullptr));
#if QT_CONFIG(tooltip)
        printButton->setToolTip(QCoreApplication::translate("Report", "Print", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class Report: public Ui_Report {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPORT_H
