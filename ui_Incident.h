/********************************************************************************
** Form generated from reading UI file 'Incident.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INCIDENT_H
#define UI_INCIDENT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Incident
{
public:
    QVBoxLayout *outerVLayout;
    QSplitter *mainSplitter;
    QFrame *mainFrame;
    QVBoxLayout *mainVLayout;
    QFrame *latlongFrame;
    QHBoxLayout *latLongHLayout;
    QLabel *latLabel;
    QLineEdit *latEdit;
    QLabel *longLabel;
    QLineEdit *lonEdit;
    QToolButton *locButton;
    QFrame *addressFrame;
    QFormLayout *addressFLayout;
    QLabel *addressLabel;
    QLineEdit *address1Edit;
    QLabel *address2Label;
    QLineEdit *address2Edit;
    QLabel *stateLabel;
    QComboBox *stateCombo;
    QFrame *incidentFrame;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QLabel *newIncidentLabel;
    QLabel *frameIdLabel;
    QSpacerItem *incidentFrameSpacer;
    QPushButton *printButton;
    QFrame *detailsFrame;
    QHBoxLayout *horizontalLayout_4;
    QScrollArea *tabScroll;
    QTabWidget *tabWidget;
    QWidget *detailsTab;
    QVBoxLayout *detailsVLayout;
    QSplitter *splitter;
    QFrame *detailsLeftFrame;
    QFormLayout *formLayout;
    QLabel *idLabel;
    QLineEdit *idEdit;
    QLabel *callCardLabel;
    QLineEdit *callCardNumEdit;
    QLabel *priorityLabel;
    QComboBox *priorityCombo;
    QLabel *categoryLabel;
    QComboBox *categoryCombo;
    QLabel *createdLabel;
    QLineEdit *createdByEdit;
    QLabel *updateLabel;
    QLineEdit *updatedByEdit;
    QLabel *lastUpdateLabel;
    QLineEdit *lastUpdateEdit;
    QFrame *descFrame;
    QVBoxLayout *descLayout;
    QLabel *descriptionLabel;
    QTextEdit *descriptionEdit;
    QGridLayout *statUpdateGLayout;
    QRadioButton *callRadioBtn;
    QDateEdit *callDate;
    QTimeEdit *callTime;
    QPushButton *nowButton;
    QRadioButton *dispatchRadioBtn;
    QDateEdit *dispatchDate;
    QTimeEdit *dispatchTime;
    QRadioButton *sceneRadioBtn;
    QDateEdit *sceneDate;
    QTimeEdit *sceneTime;
    QRadioButton *closeRadioBtn;
    QDateEdit *closeDate;
    QTimeEdit *closeTime;
    QHBoxLayout *buttonsHLayout;
    QPushButton *newButton;
    QPushButton *createButton;
    QPushButton *editButton;
    QPushButton *updateButton;
    QWidget *resourcesTab;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *srcVLayout;
    QLineEdit *filterEdit;
    QSplitter *srcSplitter;
    QFrame *srcTrmFrame;
    QVBoxLayout *srcTrmLayout;
    QLabel *srcTrmLabel;
    QFrame *srcMobFrame;
    QVBoxLayout *srcMobLayout;
    QLabel *srcMobLabel;
    QVBoxLayout *buttonsVLayout;
    QToolButton *sdsButton;
    QToolButton *selectButton;
    QToolButton *autoSelButton;
    QToolButton *addButton;
    QToolButton *removeButton;
    QVBoxLayout *assignVLayout;
    QFrame *assignFrame;
    QVBoxLayout *verticalLayout_8;
    QLabel *assignedLabel;
    QWidget *commsTab;
    QVBoxLayout *commsVLayout;
    QSplitter *splitter2;
    QFrame *callFrame;
    QVBoxLayout *callVLayout;
    QTableView *callTable;
    QFrame *msgFrame;
    QVBoxLayout *msgVLayout;
    QTableView *msgTable;

    void setupUi(QWidget *Incident)
    {
        if (Incident->objectName().isEmpty())
            Incident->setObjectName(QString::fromUtf8("Incident"));
        Incident->resize(562, 812);
        outerVLayout = new QVBoxLayout(Incident);
        outerVLayout->setSpacing(0);
        outerVLayout->setObjectName(QString::fromUtf8("outerVLayout"));
        outerVLayout->setContentsMargins(0, 0, 0, 0);
        mainSplitter = new QSplitter(Incident);
        mainSplitter->setObjectName(QString::fromUtf8("mainSplitter"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainSplitter->sizePolicy().hasHeightForWidth());
        mainSplitter->setSizePolicy(sizePolicy);
        mainSplitter->setOrientation(Qt::Horizontal);
        mainSplitter->setHandleWidth(2);
        mainSplitter->setChildrenCollapsible(false);
        mainFrame = new QFrame(mainSplitter);
        mainFrame->setObjectName(QString::fromUtf8("mainFrame"));
        sizePolicy.setHeightForWidth(mainFrame->sizePolicy().hasHeightForWidth());
        mainFrame->setSizePolicy(sizePolicy);
        mainVLayout = new QVBoxLayout(mainFrame);
        mainVLayout->setSpacing(0);
        mainVLayout->setObjectName(QString::fromUtf8("mainVLayout"));
        mainVLayout->setContentsMargins(0, 2, 2, 2);
        latlongFrame = new QFrame(mainFrame);
        latlongFrame->setObjectName(QString::fromUtf8("latlongFrame"));
        latLongHLayout = new QHBoxLayout(latlongFrame);
        latLongHLayout->setSpacing(5);
        latLongHLayout->setObjectName(QString::fromUtf8("latLongHLayout"));
        latLongHLayout->setSizeConstraint(QLayout::SetMinimumSize);
        latLongHLayout->setContentsMargins(5, 5, 5, 0);
        latLabel = new QLabel(latlongFrame);
        latLabel->setObjectName(QString::fromUtf8("latLabel"));

        latLongHLayout->addWidget(latLabel);

        latEdit = new QLineEdit(latlongFrame);
        latEdit->setObjectName(QString::fromUtf8("latEdit"));
        latEdit->setMinimumSize(QSize(0, 30));

        latLongHLayout->addWidget(latEdit);

        longLabel = new QLabel(latlongFrame);
        longLabel->setObjectName(QString::fromUtf8("longLabel"));

        latLongHLayout->addWidget(longLabel);

        lonEdit = new QLineEdit(latlongFrame);
        lonEdit->setObjectName(QString::fromUtf8("lonEdit"));
        lonEdit->setMinimumSize(QSize(0, 30));

        latLongHLayout->addWidget(lonEdit);

        locButton = new QToolButton(latlongFrame);
        locButton->setObjectName(QString::fromUtf8("locButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_dropPin.png"), QSize(), QIcon::Normal, QIcon::Off);
        locButton->setIcon(icon);
        locButton->setIconSize(QSize(28, 28));

        latLongHLayout->addWidget(locButton);


        mainVLayout->addWidget(latlongFrame);

        addressFrame = new QFrame(mainFrame);
        addressFrame->setObjectName(QString::fromUtf8("addressFrame"));
        addressFLayout = new QFormLayout(addressFrame);
        addressFLayout->setObjectName(QString::fromUtf8("addressFLayout"));
        addressFLayout->setSizeConstraint(QLayout::SetMinimumSize);
        addressFLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        addressFLayout->setHorizontalSpacing(5);
        addressFLayout->setVerticalSpacing(5);
        addressFLayout->setContentsMargins(5, 5, 5, 5);
        addressLabel = new QLabel(addressFrame);
        addressLabel->setObjectName(QString::fromUtf8("addressLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(addressLabel->sizePolicy().hasHeightForWidth());
        addressLabel->setSizePolicy(sizePolicy1);

        addressFLayout->setWidget(0, QFormLayout::LabelRole, addressLabel);

        address1Edit = new QLineEdit(addressFrame);
        address1Edit->setObjectName(QString::fromUtf8("address1Edit"));
        address1Edit->setMinimumSize(QSize(0, 30));

        addressFLayout->setWidget(0, QFormLayout::FieldRole, address1Edit);

        address2Label = new QLabel(addressFrame);
        address2Label->setObjectName(QString::fromUtf8("address2Label"));
        sizePolicy1.setHeightForWidth(address2Label->sizePolicy().hasHeightForWidth());
        address2Label->setSizePolicy(sizePolicy1);
        address2Label->setVisible(false);

        addressFLayout->setWidget(1, QFormLayout::LabelRole, address2Label);

        address2Edit = new QLineEdit(addressFrame);
        address2Edit->setObjectName(QString::fromUtf8("address2Edit"));
        address2Edit->setMinimumSize(QSize(0, 30));

        addressFLayout->setWidget(1, QFormLayout::FieldRole, address2Edit);

        stateLabel = new QLabel(addressFrame);
        stateLabel->setObjectName(QString::fromUtf8("stateLabel"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(stateLabel->sizePolicy().hasHeightForWidth());
        stateLabel->setSizePolicy(sizePolicy2);

        addressFLayout->setWidget(2, QFormLayout::LabelRole, stateLabel);

        stateCombo = new QComboBox(addressFrame);
        stateCombo->setObjectName(QString::fromUtf8("stateCombo"));

        addressFLayout->setWidget(2, QFormLayout::FieldRole, stateCombo);


        mainVLayout->addWidget(addressFrame);

        incidentFrame = new QFrame(mainFrame);
        incidentFrame->setObjectName(QString::fromUtf8("incidentFrame"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(incidentFrame->sizePolicy().hasHeightForWidth());
        incidentFrame->setSizePolicy(sizePolicy3);
        incidentFrame->setFrameShape(QFrame::StyledPanel);
        incidentFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(incidentFrame);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(5);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_3->setContentsMargins(5, 5, 5, 5);
        newIncidentLabel = new QLabel(incidentFrame);
        newIncidentLabel->setObjectName(QString::fromUtf8("newIncidentLabel"));

        horizontalLayout_3->addWidget(newIncidentLabel);

        frameIdLabel = new QLabel(incidentFrame);
        frameIdLabel->setObjectName(QString::fromUtf8("frameIdLabel"));

        horizontalLayout_3->addWidget(frameIdLabel);

        incidentFrameSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(incidentFrameSpacer);

        printButton = new QPushButton(incidentFrame);
        printButton->setObjectName(QString::fromUtf8("printButton"));
        printButton->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_print.png"), QSize(), QIcon::Normal, QIcon::Off);
        printButton->setIcon(icon1);
        printButton->setIconSize(QSize(25, 25));

        horizontalLayout_3->addWidget(printButton);


        horizontalLayout_2->addLayout(horizontalLayout_3);


        mainVLayout->addWidget(incidentFrame);

        detailsFrame = new QFrame(mainFrame);
        detailsFrame->setObjectName(QString::fromUtf8("detailsFrame"));
        detailsFrame->setFrameShape(QFrame::NoFrame);
        detailsFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(detailsFrame);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_4->setContentsMargins(0, 5, 0, 0);
        tabScroll = new QScrollArea(detailsFrame);
        tabScroll->setObjectName(QString::fromUtf8("tabScroll"));
        tabScroll->setFrameShape(QFrame::NoFrame);
        tabScroll->setWidgetResizable(true);
        tabWidget = new QTabWidget();
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        detailsTab = new QWidget();
        detailsTab->setObjectName(QString::fromUtf8("detailsTab"));
        detailsVLayout = new QVBoxLayout(detailsTab);
        detailsVLayout->setSpacing(5);
        detailsVLayout->setObjectName(QString::fromUtf8("detailsVLayout"));
        detailsVLayout->setContentsMargins(5, 5, 5, 5);
        splitter = new QSplitter(detailsTab);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        sizePolicy.setHeightForWidth(splitter->sizePolicy().hasHeightForWidth());
        splitter->setSizePolicy(sizePolicy);
        splitter->setOrientation(Qt::Horizontal);
        splitter->setHandleWidth(2);
        splitter->setChildrenCollapsible(false);
        detailsLeftFrame = new QFrame(splitter);
        detailsLeftFrame->setObjectName(QString::fromUtf8("detailsLeftFrame"));
        formLayout = new QFormLayout(detailsLeftFrame);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setSizeConstraint(QLayout::SetMinimumSize);
        formLayout->setHorizontalSpacing(5);
        formLayout->setVerticalSpacing(5);
        formLayout->setContentsMargins(5, 5, 5, 5);
        idLabel = new QLabel(detailsLeftFrame);
        idLabel->setObjectName(QString::fromUtf8("idLabel"));
        sizePolicy2.setHeightForWidth(idLabel->sizePolicy().hasHeightForWidth());
        idLabel->setSizePolicy(sizePolicy2);

        formLayout->setWidget(0, QFormLayout::LabelRole, idLabel);

        idEdit = new QLineEdit(detailsLeftFrame);
        idEdit->setObjectName(QString::fromUtf8("idEdit"));
        idEdit->setEnabled(true);
        idEdit->setMinimumSize(QSize(0, 30));
        idEdit->setReadOnly(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, idEdit);

        callCardLabel = new QLabel(detailsLeftFrame);
        callCardLabel->setObjectName(QString::fromUtf8("callCardLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, callCardLabel);

        callCardNumEdit = new QLineEdit(detailsLeftFrame);
        callCardNumEdit->setObjectName(QString::fromUtf8("callCardNumEdit"));
        callCardNumEdit->setMinimumSize(QSize(0, 30));

        formLayout->setWidget(1, QFormLayout::FieldRole, callCardNumEdit);

        priorityLabel = new QLabel(detailsLeftFrame);
        priorityLabel->setObjectName(QString::fromUtf8("priorityLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, priorityLabel);

        priorityCombo = new QComboBox(detailsLeftFrame);
        priorityCombo->setObjectName(QString::fromUtf8("priorityCombo"));

        formLayout->setWidget(2, QFormLayout::FieldRole, priorityCombo);

        categoryLabel = new QLabel(detailsLeftFrame);
        categoryLabel->setObjectName(QString::fromUtf8("categoryLabel"));

        formLayout->setWidget(3, QFormLayout::LabelRole, categoryLabel);

        categoryCombo = new QComboBox(detailsLeftFrame);
        categoryCombo->setObjectName(QString::fromUtf8("categoryCombo"));

        formLayout->setWidget(3, QFormLayout::FieldRole, categoryCombo);

        createdLabel = new QLabel(detailsLeftFrame);
        createdLabel->setObjectName(QString::fromUtf8("createdLabel"));

        formLayout->setWidget(4, QFormLayout::LabelRole, createdLabel);

        createdByEdit = new QLineEdit(detailsLeftFrame);
        createdByEdit->setObjectName(QString::fromUtf8("createdByEdit"));
        createdByEdit->setEnabled(true);
        createdByEdit->setMinimumSize(QSize(0, 30));
        createdByEdit->setReadOnly(true);

        formLayout->setWidget(4, QFormLayout::FieldRole, createdByEdit);

        updateLabel = new QLabel(detailsLeftFrame);
        updateLabel->setObjectName(QString::fromUtf8("updateLabel"));

        formLayout->setWidget(5, QFormLayout::LabelRole, updateLabel);

        updatedByEdit = new QLineEdit(detailsLeftFrame);
        updatedByEdit->setObjectName(QString::fromUtf8("updatedByEdit"));
        updatedByEdit->setMinimumSize(QSize(0, 30));
        updatedByEdit->setReadOnly(true);

        formLayout->setWidget(5, QFormLayout::FieldRole, updatedByEdit);

        lastUpdateLabel = new QLabel(detailsLeftFrame);
        lastUpdateLabel->setObjectName(QString::fromUtf8("lastUpdateLabel"));

        formLayout->setWidget(6, QFormLayout::LabelRole, lastUpdateLabel);

        lastUpdateEdit = new QLineEdit(detailsLeftFrame);
        lastUpdateEdit->setObjectName(QString::fromUtf8("lastUpdateEdit"));
        lastUpdateEdit->setMinimumSize(QSize(0, 30));
        lastUpdateEdit->setReadOnly(true);

        formLayout->setWidget(6, QFormLayout::FieldRole, lastUpdateEdit);

        splitter->addWidget(detailsLeftFrame);
        descFrame = new QFrame(splitter);
        descFrame->setObjectName(QString::fromUtf8("descFrame"));
        descLayout = new QVBoxLayout(descFrame);
        descLayout->setSpacing(5);
        descLayout->setObjectName(QString::fromUtf8("descLayout"));
        descLayout->setContentsMargins(5, 5, 5, 5);
        descriptionLabel = new QLabel(descFrame);
        descriptionLabel->setObjectName(QString::fromUtf8("descriptionLabel"));
        sizePolicy2.setHeightForWidth(descriptionLabel->sizePolicy().hasHeightForWidth());
        descriptionLabel->setSizePolicy(sizePolicy2);

        descLayout->addWidget(descriptionLabel);

        descriptionEdit = new QTextEdit(descFrame);
        descriptionEdit->setObjectName(QString::fromUtf8("descriptionEdit"));
        sizePolicy.setHeightForWidth(descriptionEdit->sizePolicy().hasHeightForWidth());
        descriptionEdit->setSizePolicy(sizePolicy);

        descLayout->addWidget(descriptionEdit);

        splitter->addWidget(descFrame);

        detailsVLayout->addWidget(splitter);

        statUpdateGLayout = new QGridLayout();
        statUpdateGLayout->setSpacing(5);
        statUpdateGLayout->setObjectName(QString::fromUtf8("statUpdateGLayout"));
        statUpdateGLayout->setSizeConstraint(QLayout::SetMinimumSize);
        statUpdateGLayout->setContentsMargins(5, 0, 5, 0);
        callRadioBtn = new QRadioButton(detailsTab);
        callRadioBtn->setObjectName(QString::fromUtf8("callRadioBtn"));

        statUpdateGLayout->addWidget(callRadioBtn, 0, 0, 1, 1);

        callDate = new QDateEdit(detailsTab);
        callDate->setObjectName(QString::fromUtf8("callDate"));
        callDate->setMinimumSize(QSize(0, 30));
        callDate->setAlignment(Qt::AlignCenter);
        callDate->setDateTime(QDateTime(QDate(2016, 1, 1), QTime(0, 0, 0)));
        callDate->setCalendarPopup(true);

        statUpdateGLayout->addWidget(callDate, 0, 1, 1, 1);

        callTime = new QTimeEdit(detailsTab);
        callTime->setObjectName(QString::fromUtf8("callTime"));
        callTime->setMinimumSize(QSize(0, 30));
        callTime->setAlignment(Qt::AlignCenter);
        callTime->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

        statUpdateGLayout->addWidget(callTime, 0, 2, 1, 1);

        nowButton = new QPushButton(detailsTab);
        nowButton->setObjectName(QString::fromUtf8("nowButton"));
        sizePolicy2.setHeightForWidth(nowButton->sizePolicy().hasHeightForWidth());
        nowButton->setSizePolicy(sizePolicy2);

        statUpdateGLayout->addWidget(nowButton, 0, 3, 4, 1);

        dispatchRadioBtn = new QRadioButton(detailsTab);
        dispatchRadioBtn->setObjectName(QString::fromUtf8("dispatchRadioBtn"));

        statUpdateGLayout->addWidget(dispatchRadioBtn, 1, 0, 1, 1);

        dispatchDate = new QDateEdit(detailsTab);
        dispatchDate->setObjectName(QString::fromUtf8("dispatchDate"));
        dispatchDate->setEnabled(false);
        dispatchDate->setMinimumSize(QSize(0, 30));
        dispatchDate->setAlignment(Qt::AlignCenter);
        dispatchDate->setDateTime(QDateTime(QDate(2016, 1, 1), QTime(0, 0, 0)));
        dispatchDate->setCalendarPopup(true);

        statUpdateGLayout->addWidget(dispatchDate, 1, 1, 1, 1);

        dispatchTime = new QTimeEdit(detailsTab);
        dispatchTime->setObjectName(QString::fromUtf8("dispatchTime"));
        dispatchTime->setEnabled(false);
        dispatchTime->setMinimumSize(QSize(0, 30));
        dispatchTime->setAlignment(Qt::AlignCenter);
        dispatchTime->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

        statUpdateGLayout->addWidget(dispatchTime, 1, 2, 1, 1);

        sceneRadioBtn = new QRadioButton(detailsTab);
        sceneRadioBtn->setObjectName(QString::fromUtf8("sceneRadioBtn"));

        statUpdateGLayout->addWidget(sceneRadioBtn, 2, 0, 1, 1);

        sceneDate = new QDateEdit(detailsTab);
        sceneDate->setObjectName(QString::fromUtf8("sceneDate"));
        sceneDate->setEnabled(false);
        sceneDate->setMinimumSize(QSize(0, 30));
        sceneDate->setAlignment(Qt::AlignCenter);
        sceneDate->setDateTime(QDateTime(QDate(2016, 1, 1), QTime(0, 0, 0)));
        sceneDate->setCalendarPopup(true);

        statUpdateGLayout->addWidget(sceneDate, 2, 1, 1, 1);

        sceneTime = new QTimeEdit(detailsTab);
        sceneTime->setObjectName(QString::fromUtf8("sceneTime"));
        sceneTime->setEnabled(false);
        sceneTime->setMinimumSize(QSize(0, 30));
        sceneTime->setAlignment(Qt::AlignCenter);
        sceneTime->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

        statUpdateGLayout->addWidget(sceneTime, 2, 2, 1, 1);

        closeRadioBtn = new QRadioButton(detailsTab);
        closeRadioBtn->setObjectName(QString::fromUtf8("closeRadioBtn"));

        statUpdateGLayout->addWidget(closeRadioBtn, 3, 0, 1, 1);

        closeDate = new QDateEdit(detailsTab);
        closeDate->setObjectName(QString::fromUtf8("closeDate"));
        closeDate->setEnabled(false);
        closeDate->setMinimumSize(QSize(0, 30));
        closeDate->setAlignment(Qt::AlignCenter);
        closeDate->setDateTime(QDateTime(QDate(2016, 1, 1), QTime(0, 0, 0)));
        closeDate->setCalendarPopup(true);

        statUpdateGLayout->addWidget(closeDate, 3, 1, 1, 1);

        closeTime = new QTimeEdit(detailsTab);
        closeTime->setObjectName(QString::fromUtf8("closeTime"));
        closeTime->setEnabled(false);
        closeTime->setMinimumSize(QSize(0, 30));
        closeTime->setAlignment(Qt::AlignCenter);
        closeTime->setButtonSymbols(QAbstractSpinBox::UpDownArrows);

        statUpdateGLayout->addWidget(closeTime, 3, 2, 1, 1);


        detailsVLayout->addLayout(statUpdateGLayout);

        buttonsHLayout = new QHBoxLayout();
        buttonsHLayout->setSpacing(5);
        buttonsHLayout->setObjectName(QString::fromUtf8("buttonsHLayout"));
        buttonsHLayout->setSizeConstraint(QLayout::SetMinimumSize);
        buttonsHLayout->setContentsMargins(0, 0, 0, 0);
        newButton = new QPushButton(detailsTab);
        newButton->setObjectName(QString::fromUtf8("newButton"));
        newButton->setEnabled(false);
        sizePolicy1.setHeightForWidth(newButton->sizePolicy().hasHeightForWidth());
        newButton->setSizePolicy(sizePolicy1);
        newButton->setMinimumSize(QSize(0, 40));

        buttonsHLayout->addWidget(newButton);

        createButton = new QPushButton(detailsTab);
        createButton->setObjectName(QString::fromUtf8("createButton"));
        sizePolicy1.setHeightForWidth(createButton->sizePolicy().hasHeightForWidth());
        createButton->setSizePolicy(sizePolicy1);
        createButton->setMinimumSize(QSize(0, 40));

        buttonsHLayout->addWidget(createButton);

        editButton = new QPushButton(detailsTab);
        editButton->setObjectName(QString::fromUtf8("editButton"));
        editButton->setCheckable(true);
        editButton->setEnabled(true);
        sizePolicy1.setHeightForWidth(editButton->sizePolicy().hasHeightForWidth());
        editButton->setSizePolicy(sizePolicy1);
        editButton->setMinimumSize(QSize(0, 40));

        buttonsHLayout->addWidget(editButton);

        updateButton = new QPushButton(detailsTab);
        updateButton->setObjectName(QString::fromUtf8("updateButton"));
        updateButton->setEnabled(false);
        sizePolicy1.setHeightForWidth(updateButton->sizePolicy().hasHeightForWidth());
        updateButton->setSizePolicy(sizePolicy1);
        updateButton->setMinimumSize(QSize(0, 40));

        buttonsHLayout->addWidget(updateButton);


        detailsVLayout->addLayout(buttonsHLayout);

        tabWidget->addTab(detailsTab, QString());
        resourcesTab = new QWidget();
        resourcesTab->setObjectName(QString::fromUtf8("resourcesTab"));
        horizontalLayout = new QHBoxLayout(resourcesTab);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        srcVLayout = new QVBoxLayout();
        srcVLayout->setSpacing(0);
        srcVLayout->setObjectName(QString::fromUtf8("srcVLayout"));
        srcVLayout->setContentsMargins(0, 0, 0, 0);
        filterEdit = new QLineEdit(resourcesTab);
        filterEdit->setObjectName(QString::fromUtf8("filterEdit"));
        filterEdit->setMinimumSize(QSize(0, 30));

        srcVLayout->addWidget(filterEdit);

        srcSplitter = new QSplitter(resourcesTab);
        srcSplitter->setObjectName(QString::fromUtf8("srcSplitter"));
        sizePolicy.setHeightForWidth(srcSplitter->sizePolicy().hasHeightForWidth());
        srcSplitter->setSizePolicy(sizePolicy);
        srcSplitter->setOrientation(Qt::Vertical);
        srcSplitter->setHandleWidth(2);
        srcSplitter->setChildrenCollapsible(false);
        srcTrmFrame = new QFrame(srcSplitter);
        srcTrmFrame->setObjectName(QString::fromUtf8("srcTrmFrame"));
        srcTrmFrame->setFrameShape(QFrame::StyledPanel);
        srcTrmFrame->setFrameShadow(QFrame::Raised);
        srcTrmLayout = new QVBoxLayout(srcTrmFrame);
        srcTrmLayout->setSpacing(5);
        srcTrmLayout->setObjectName(QString::fromUtf8("srcTrmLayout"));
        srcTrmLayout->setContentsMargins(0, 0, 0, 0);
        srcTrmLabel = new QLabel(srcTrmFrame);
        srcTrmLabel->setObjectName(QString::fromUtf8("srcTrmLabel"));

        srcTrmLayout->addWidget(srcTrmLabel);

        srcSplitter->addWidget(srcTrmFrame);
        srcMobFrame = new QFrame(srcSplitter);
        srcMobFrame->setObjectName(QString::fromUtf8("srcMobFrame"));
        srcMobFrame->setFrameShape(QFrame::StyledPanel);
        srcMobFrame->setFrameShadow(QFrame::Raised);
        srcMobLayout = new QVBoxLayout(srcMobFrame);
        srcMobLayout->setSpacing(5);
        srcMobLayout->setObjectName(QString::fromUtf8("srcMobLayout"));
        srcMobLayout->setContentsMargins(0, 0, 0, 0);
        srcMobLabel = new QLabel(srcMobFrame);
        srcMobLabel->setObjectName(QString::fromUtf8("srcMobLabel"));

        srcMobLayout->addWidget(srcMobLabel);

        srcSplitter->addWidget(srcMobFrame);

        srcVLayout->addWidget(srcSplitter);


        horizontalLayout->addLayout(srcVLayout);

        buttonsVLayout = new QVBoxLayout();
        buttonsVLayout->setSpacing(5);
        buttonsVLayout->setObjectName(QString::fromUtf8("buttonsVLayout"));
        buttonsVLayout->setContentsMargins(5, 5, 5, 5);
        sdsButton = new QToolButton(resourcesTab);
        sdsButton->setObjectName(QString::fromUtf8("sdsButton"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_sds.png"), QSize(), QIcon::Normal, QIcon::Off);
        sdsButton->setIcon(icon2);
        sdsButton->setIconSize(QSize(30, 30));

        buttonsVLayout->addWidget(sdsButton);

        selectButton = new QToolButton(resourcesTab);
        selectButton->setObjectName(QString::fromUtf8("selectButton"));
        selectButton->setEnabled(false);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/cursor_sel_resource.png"), QSize(), QIcon::Normal, QIcon::Off);
        selectButton->setIcon(icon3);
        selectButton->setIconSize(QSize(30, 30));

        buttonsVLayout->addWidget(selectButton);

        autoSelButton = new QToolButton(resourcesTab);
        autoSelButton->setObjectName(QString::fromUtf8("autoSelButton"));
        autoSelButton->setEnabled(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/cursor_auto_sel.png"), QSize(), QIcon::Normal, QIcon::Off);
        autoSelButton->setIcon(icon4);
        autoSelButton->setIconSize(QSize(30, 30));

        buttonsVLayout->addWidget(autoSelButton);

        addButton = new QToolButton(resourcesTab);
        addButton->setObjectName(QString::fromUtf8("addButton"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_add_data.png"), QSize(), QIcon::Normal, QIcon::Off);
        addButton->setIcon(icon5);
        addButton->setIconSize(QSize(30, 30));

        buttonsVLayout->addWidget(addButton);

        removeButton = new QToolButton(resourcesTab);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_delete_data.png"), QSize(), QIcon::Normal, QIcon::Off);
        removeButton->setIcon(icon6);
        removeButton->setIconSize(QSize(30, 30));

        buttonsVLayout->addWidget(removeButton);


        horizontalLayout->addLayout(buttonsVLayout);

        assignVLayout = new QVBoxLayout();
        assignVLayout->setSpacing(0);
        assignVLayout->setObjectName(QString::fromUtf8("assignVLayout"));
        assignVLayout->setContentsMargins(0, 0, 0, 0);
        assignFrame = new QFrame(resourcesTab);
        assignFrame->setObjectName(QString::fromUtf8("assignFrame"));
        assignFrame->setFrameShape(QFrame::StyledPanel);
        assignFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_8 = new QVBoxLayout(assignFrame);
        verticalLayout_8->setSpacing(5);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(5, 5, 5, 5);
        assignedLabel = new QLabel(assignFrame);
        assignedLabel->setObjectName(QString::fromUtf8("assignedLabel"));

        verticalLayout_8->addWidget(assignedLabel);


        assignVLayout->addWidget(assignFrame);


        horizontalLayout->addLayout(assignVLayout);

        tabWidget->addTab(resourcesTab, QString());
        commsTab = new QWidget();
        commsTab->setObjectName(QString::fromUtf8("commsTab"));
        commsVLayout = new QVBoxLayout(commsTab);
        commsVLayout->setSpacing(0);
        commsVLayout->setObjectName(QString::fromUtf8("commsVLayout"));
        commsVLayout->setContentsMargins(0, 0, 0, 0);
        splitter2 = new QSplitter(commsTab);
        splitter2->setObjectName(QString::fromUtf8("splitter2"));
        sizePolicy.setHeightForWidth(splitter2->sizePolicy().hasHeightForWidth());
        splitter2->setSizePolicy(sizePolicy);
        splitter2->setOrientation(Qt::Vertical);
        splitter2->setHandleWidth(2);
        splitter2->setChildrenCollapsible(false);
        callFrame = new QFrame(splitter2);
        callFrame->setObjectName(QString::fromUtf8("callFrame"));
        callFrame->setFrameShape(QFrame::NoFrame);
        callFrame->setFrameShadow(QFrame::Raised);
        callFrame->setVisible(false);
        callVLayout = new QVBoxLayout(callFrame);
        callVLayout->setSpacing(0);
        callVLayout->setObjectName(QString::fromUtf8("callVLayout"));
        callVLayout->setContentsMargins(0, 0, 0, 0);
        callTable = new QTableView(callFrame);
        callTable->setObjectName(QString::fromUtf8("callTable"));
        callTable->setFrameShape(QFrame::NoFrame);
        callTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        callTable->setAlternatingRowColors(true);
        callTable->setSelectionMode(QAbstractItemView::SingleSelection);
        callTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        callTable->setSortingEnabled(true);
        callTable->horizontalHeader()->setMinimumSectionSize(50);
        callTable->horizontalHeader()->setStretchLastSection(true);

        callVLayout->addWidget(callTable);

        splitter2->addWidget(callFrame);
        msgFrame = new QFrame(splitter2);
        msgFrame->setObjectName(QString::fromUtf8("msgFrame"));
        msgFrame->setFrameShape(QFrame::NoFrame);
        msgFrame->setFrameShadow(QFrame::Raised);
        msgFrame->setVisible(false);
        msgVLayout = new QVBoxLayout(msgFrame);
        msgVLayout->setSpacing(0);
        msgVLayout->setObjectName(QString::fromUtf8("msgVLayout"));
        msgVLayout->setContentsMargins(0, 0, 0, 0);
        msgTable = new QTableView(msgFrame);
        msgTable->setObjectName(QString::fromUtf8("msgTable"));
        msgTable->setFrameShape(QFrame::NoFrame);
        msgTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        msgTable->setAlternatingRowColors(true);
        msgTable->setSelectionMode(QAbstractItemView::SingleSelection);
        msgTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        msgTable->setSortingEnabled(true);
        msgTable->horizontalHeader()->setMinimumSectionSize(50);
        msgTable->horizontalHeader()->setStretchLastSection(true);

        msgVLayout->addWidget(msgTable);

        splitter2->addWidget(msgFrame);

        commsVLayout->addWidget(splitter2);

        tabWidget->addTab(commsTab, QString());
        tabScroll->setWidget(tabWidget);

        horizontalLayout_4->addWidget(tabScroll);


        mainVLayout->addWidget(detailsFrame);

        mainSplitter->addWidget(mainFrame);

        outerVLayout->addWidget(mainSplitter);


        retranslateUi(Incident);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Incident);
    } // setupUi

    void retranslateUi(QWidget *Incident)
    {
        Incident->setWindowTitle(QCoreApplication::translate("Incident", "Incident", nullptr));
        latLabel->setText(QCoreApplication::translate("Incident", "Latitude:", nullptr));
#if QT_CONFIG(tooltip)
        latEdit->setToolTip(QCoreApplication::translate("Incident", "Negative for South", nullptr));
#endif // QT_CONFIG(tooltip)
        longLabel->setText(QCoreApplication::translate("Incident", "Longitude:", nullptr));
#if QT_CONFIG(tooltip)
        lonEdit->setToolTip(QCoreApplication::translate("Incident", "Negative for West", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        locButton->setToolTip(QCoreApplication::translate("Incident", "Locate on map", nullptr));
#endif // QT_CONFIG(tooltip)
        addressLabel->setText(QCoreApplication::translate("Incident", "Address:", nullptr));
        address2Label->setText(QCoreApplication::translate("Incident", "Address2:", nullptr));
        stateLabel->setText(QCoreApplication::translate("Incident", "State:", nullptr));
        newIncidentLabel->setText(QCoreApplication::translate("Incident", "Incident Details", nullptr));
#if QT_CONFIG(tooltip)
        printButton->setToolTip(QCoreApplication::translate("Incident", "Print", nullptr));
#endif // QT_CONFIG(tooltip)
        idLabel->setText(QCoreApplication::translate("Incident", "ID:", nullptr));
        callCardLabel->setText(QCoreApplication::translate("Incident", "Call Card No:", nullptr));
        priorityLabel->setText(QCoreApplication::translate("Incident", "Priority:", nullptr));
        categoryLabel->setText(QCoreApplication::translate("Incident", "Category:", nullptr));
        createdLabel->setText(QCoreApplication::translate("Incident", "Created by:", nullptr));
        updateLabel->setText(QCoreApplication::translate("Incident", "Updated by:", nullptr));
        lastUpdateLabel->setText(QCoreApplication::translate("Incident", "Last Update:", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("Incident", "Description:", nullptr));
        callRadioBtn->setText(QCoreApplication::translate("Incident", "Call Received:", nullptr));
        callDate->setDisplayFormat(QCoreApplication::translate("Incident", "d/M/yyyy", nullptr));
        callTime->setDisplayFormat(QCoreApplication::translate("Incident", "hh:mm:ss", nullptr));
        nowButton->setText(QCoreApplication::translate("Incident", "Now", nullptr));
        dispatchRadioBtn->setText(QCoreApplication::translate("Incident", "Dispatched:", nullptr));
        dispatchDate->setDisplayFormat(QCoreApplication::translate("Incident", "d/M/yyyy", nullptr));
        dispatchTime->setDisplayFormat(QCoreApplication::translate("Incident", "hh:mm:ss", nullptr));
        sceneRadioBtn->setText(QCoreApplication::translate("Incident", "On Scene:", nullptr));
        sceneDate->setDisplayFormat(QCoreApplication::translate("Incident", "d/M/yyyy", nullptr));
        sceneTime->setDisplayFormat(QCoreApplication::translate("Incident", "hh:mm:ss", nullptr));
        closeRadioBtn->setText(QCoreApplication::translate("Incident", "Closed:", nullptr));
        closeDate->setDisplayFormat(QCoreApplication::translate("Incident", "d/M/yyyy", nullptr));
        closeTime->setDisplayFormat(QCoreApplication::translate("Incident", "hh:mm:ss", nullptr));
        newButton->setText(QCoreApplication::translate("Incident", "New", nullptr));
        createButton->setText(QCoreApplication::translate("Incident", "Create Incident", nullptr));
        editButton->setText(QCoreApplication::translate("Incident", "Edit", nullptr));
        updateButton->setText(QCoreApplication::translate("Incident", "Update", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(detailsTab), QCoreApplication::translate("Incident", "Details", nullptr));
        srcTrmLabel->setText(QCoreApplication::translate("Incident", "Terminals", nullptr));
        srcMobLabel->setText(QCoreApplication::translate("Incident", "Mobiles", nullptr));
        selectButton->setText(QCoreApplication::translate("Incident", "...", nullptr));
        autoSelButton->setText(QCoreApplication::translate("Incident", "...", nullptr));
        addButton->setText(QString());
        assignedLabel->setText(QCoreApplication::translate("Incident", "Assigned", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(resourcesTab), QCoreApplication::translate("Incident", "Resource Assignment", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(commsTab), QCoreApplication::translate("Incident", "Communications", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Incident: public Ui_Incident {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INCIDENT_H
