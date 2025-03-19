/********************************************************************************
** Form generated from reading UI file 'Sds.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SDS_H
#define UI_SDS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Sds
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *mainFrame;
    QVBoxLayout *mainLayout;
    QSplitter *splitter1;
    QFrame *inpTabFrame;
    QHBoxLayout *tabFrameLayout;
    QTabWidget *inpTabWidget;
    QWidget *inpSdsTab;
    QVBoxLayout *sdsTabLayout;
    QFrame *sdsFrame;
    QHBoxLayout *sdsHorLayout;
    QSpacerItem *sdsHorSpacer;
    QToolButton *attHistButton;
    QToolButton *msgHistButton;
    QToolButton *tplButton;
    QToolButton *stsButton;
    QToolButton *clearButton;
    QLabel *charLength;
    QSplitter *mmsSplitter;
    QTextEdit *msgEdit;
    QFrame *mmsFrame;
    QHBoxLayout *mmsLayout;
    QToolButton *mmsFileButton;
    QListWidget *mmsFiles;
    QToolButton *mmsClrButton;
    QWidget *inpStsTab;
    QVBoxLayout *stsTabLayout;
    QTableWidget *stsTable;
    QToolButton *sendButton;

    void setupUi(QWidget *Sds)
    {
        if (Sds->objectName().isEmpty())
            Sds->setObjectName(QString::fromUtf8("Sds"));
        Sds->resize(562, 610);
        verticalLayout = new QVBoxLayout(Sds);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        mainFrame = new QFrame(Sds);
        mainFrame->setObjectName(QString::fromUtf8("mainFrame"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainFrame->sizePolicy().hasHeightForWidth());
        mainFrame->setSizePolicy(sizePolicy);
        mainFrame->setFrameShape(QFrame::NoFrame);
        mainFrame->setLineWidth(0);
        mainLayout = new QVBoxLayout(mainFrame);
        mainLayout->setSpacing(5);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(2, 2, 2, 0);
        splitter1 = new QSplitter(mainFrame);
        splitter1->setObjectName(QString::fromUtf8("splitter1"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(splitter1->sizePolicy().hasHeightForWidth());
        splitter1->setSizePolicy(sizePolicy1);
        splitter1->setLineWidth(0);
        splitter1->setOrientation(Qt::Vertical);
        splitter1->setHandleWidth(2);
        splitter1->setChildrenCollapsible(false);
        inpTabFrame = new QFrame(splitter1);
        inpTabFrame->setObjectName(QString::fromUtf8("inpTabFrame"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(inpTabFrame->sizePolicy().hasHeightForWidth());
        inpTabFrame->setSizePolicy(sizePolicy2);
        inpTabFrame->setFrameShape(QFrame::NoFrame);
        inpTabFrame->setFrameShadow(QFrame::Raised);
        inpTabFrame->setLineWidth(0);
        tabFrameLayout = new QHBoxLayout(inpTabFrame);
        tabFrameLayout->setSpacing(0);
        tabFrameLayout->setObjectName(QString::fromUtf8("tabFrameLayout"));
        tabFrameLayout->setContentsMargins(0, 0, 0, 0);
        inpTabWidget = new QTabWidget(inpTabFrame);
        inpTabWidget->setObjectName(QString::fromUtf8("inpTabWidget"));
        sizePolicy1.setHeightForWidth(inpTabWidget->sizePolicy().hasHeightForWidth());
        inpTabWidget->setSizePolicy(sizePolicy1);
        inpTabWidget->setIconSize(QSize(28, 28));
        inpSdsTab = new QWidget();
        inpSdsTab->setObjectName(QString::fromUtf8("inpSdsTab"));
        sdsTabLayout = new QVBoxLayout(inpSdsTab);
        sdsTabLayout->setSpacing(5);
        sdsTabLayout->setObjectName(QString::fromUtf8("sdsTabLayout"));
        sdsTabLayout->setContentsMargins(5, 5, 5, 5);
        sdsFrame = new QFrame(inpSdsTab);
        sdsFrame->setObjectName(QString::fromUtf8("sdsFrame"));
        sizePolicy.setHeightForWidth(sdsFrame->sizePolicy().hasHeightForWidth());
        sdsFrame->setSizePolicy(sizePolicy);
        sdsFrame->setFrameShape(QFrame::NoFrame);
        sdsFrame->setLineWidth(0);
        sdsHorLayout = new QHBoxLayout(sdsFrame);
        sdsHorLayout->setSpacing(5);
        sdsHorLayout->setObjectName(QString::fromUtf8("sdsHorLayout"));
        sdsHorLayout->setSizeConstraint(QLayout::SetMinimumSize);
        sdsHorLayout->setContentsMargins(0, 0, 0, 0);
        sdsHorSpacer = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        sdsHorLayout->addItem(sdsHorSpacer);

        attHistButton = new QToolButton(sdsFrame);
        attHistButton->setObjectName(QString::fromUtf8("attHistButton"));
        attHistButton->setMinimumSize(QSize(40, 40));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_att_hist.png"), QSize(), QIcon::Normal, QIcon::Off);
        attHistButton->setIcon(icon);
        attHistButton->setIconSize(QSize(24, 24));
        attHistButton->setPopupMode(QToolButton::MenuButtonPopup);

        sdsHorLayout->addWidget(attHistButton);

        msgHistButton = new QToolButton(sdsFrame);
        msgHistButton->setObjectName(QString::fromUtf8("msgHistButton"));
        msgHistButton->setMinimumSize(QSize(40, 40));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_msg_hist.png"), QSize(), QIcon::Normal, QIcon::Off);
        msgHistButton->setIcon(icon1);
        msgHistButton->setIconSize(QSize(24, 24));
        msgHistButton->setPopupMode(QToolButton::MenuButtonPopup);

        sdsHorLayout->addWidget(msgHistButton);

        tplButton = new QToolButton(sdsFrame);
        tplButton->setObjectName(QString::fromUtf8("tplButton"));
        tplButton->setMinimumSize(QSize(40, 40));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_template.png"), QSize(), QIcon::Normal, QIcon::Off);
        tplButton->setIcon(icon2);
        tplButton->setIconSize(QSize(24, 24));
        tplButton->setPopupMode(QToolButton::MenuButtonPopup);

        sdsHorLayout->addWidget(tplButton);

        stsButton = new QToolButton(sdsFrame);
        stsButton->setObjectName(QString::fromUtf8("stsButton"));
        stsButton->setMinimumSize(QSize(40, 40));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Images/images/icon_statusMsg.png"), QSize(), QIcon::Normal, QIcon::Off);
        stsButton->setIcon(icon3);
        stsButton->setIconSize(QSize(24, 24));
        stsButton->setCheckable(true);
        stsButton->setPopupMode(QToolButton::MenuButtonPopup);

        sdsHorLayout->addWidget(stsButton);

        clearButton = new QToolButton(sdsFrame);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));
        clearButton->setMinimumSize(QSize(40, 40));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Images/images/icon_clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        clearButton->setIcon(icon4);
        clearButton->setIconSize(QSize(30, 30));

        sdsHorLayout->addWidget(clearButton);

        charLength = new QLabel(sdsFrame);
        charLength->setObjectName(QString::fromUtf8("charLength"));
        charLength->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        sdsHorLayout->addWidget(charLength);


        sdsTabLayout->addWidget(sdsFrame);

        mmsSplitter = new QSplitter(inpSdsTab);
        mmsSplitter->setObjectName(QString::fromUtf8("mmsSplitter"));
        sizePolicy1.setHeightForWidth(mmsSplitter->sizePolicy().hasHeightForWidth());
        mmsSplitter->setSizePolicy(sizePolicy1);
        mmsSplitter->setLineWidth(0);
        mmsSplitter->setOrientation(Qt::Vertical);
        mmsSplitter->setHandleWidth(2);
        mmsSplitter->setChildrenCollapsible(false);
        msgEdit = new QTextEdit(mmsSplitter);
        msgEdit->setObjectName(QString::fromUtf8("msgEdit"));
        sizePolicy1.setHeightForWidth(msgEdit->sizePolicy().hasHeightForWidth());
        msgEdit->setSizePolicy(sizePolicy1);
        msgEdit->setMinimumSize(QSize(0, 40));
        msgEdit->setAcceptRichText(false);
        mmsSplitter->addWidget(msgEdit);
        mmsFrame = new QFrame(mmsSplitter);
        mmsFrame->setObjectName(QString::fromUtf8("mmsFrame"));
        sizePolicy.setHeightForWidth(mmsFrame->sizePolicy().hasHeightForWidth());
        mmsFrame->setSizePolicy(sizePolicy);
        mmsFrame->setFrameShape(QFrame::NoFrame);
        mmsFrame->setFrameShadow(QFrame::Raised);
        mmsFrame->setLineWidth(0);
        mmsLayout = new QHBoxLayout(mmsFrame);
        mmsLayout->setSpacing(5);
        mmsLayout->setObjectName(QString::fromUtf8("mmsLayout"));
        mmsLayout->setSizeConstraint(QLayout::SetMinimumSize);
        mmsLayout->setContentsMargins(0, 5, 0, 0);
        mmsFileButton = new QToolButton(mmsFrame);
        mmsFileButton->setObjectName(QString::fromUtf8("mmsFileButton"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(mmsFileButton->sizePolicy().hasHeightForWidth());
        mmsFileButton->setSizePolicy(sizePolicy3);
        mmsFileButton->setMinimumSize(QSize(40, 40));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/Images/images/icon_att.png"), QSize(), QIcon::Normal, QIcon::Off);
        mmsFileButton->setIcon(icon5);
        mmsFileButton->setIconSize(QSize(24, 24));

        mmsLayout->addWidget(mmsFileButton);

        mmsFiles = new QListWidget(mmsFrame);
        mmsFiles->setObjectName(QString::fromUtf8("mmsFiles"));
        sizePolicy1.setHeightForWidth(mmsFiles->sizePolicy().hasHeightForWidth());
        mmsFiles->setSizePolicy(sizePolicy1);
        mmsFiles->setMinimumSize(QSize(0, 50));
        mmsFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
        mmsFiles->setDragDropMode(QAbstractItemView::DropOnly);
        mmsFiles->setSelectionMode(QAbstractItemView::ExtendedSelection);

        mmsLayout->addWidget(mmsFiles);

        mmsClrButton = new QToolButton(mmsFrame);
        mmsClrButton->setObjectName(QString::fromUtf8("mmsClrButton"));
        sizePolicy3.setHeightForWidth(mmsClrButton->sizePolicy().hasHeightForWidth());
        mmsClrButton->setSizePolicy(sizePolicy3);
        mmsClrButton->setMinimumSize(QSize(40, 40));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/Images/images/icon_clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        mmsClrButton->setIcon(icon6);
        mmsClrButton->setIconSize(QSize(30, 30));

        mmsLayout->addWidget(mmsClrButton);

        mmsSplitter->addWidget(mmsFrame);

        sdsTabLayout->addWidget(mmsSplitter);

        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/Images/images/icon_sds.png"), QSize(), QIcon::Normal, QIcon::Off);
        inpTabWidget->addTab(inpSdsTab, icon7, QString());
        inpStsTab = new QWidget();
        inpStsTab->setObjectName(QString::fromUtf8("inpStsTab"));
        stsTabLayout = new QVBoxLayout(inpStsTab);
        stsTabLayout->setSpacing(5);
        stsTabLayout->setObjectName(QString::fromUtf8("stsTabLayout"));
        stsTabLayout->setContentsMargins(0, 0, 0, 5);
        stsTable = new QTableWidget(inpStsTab);
        if (stsTable->columnCount() < 3)
            stsTable->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        __qtablewidgetitem->setTextAlignment(Qt::AlignCenter);
        stsTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        stsTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        stsTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        stsTable->setObjectName(QString::fromUtf8("stsTable"));
        QSizePolicy sizePolicy4(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(stsTable->sizePolicy().hasHeightForWidth());
        stsTable->setSizePolicy(sizePolicy4);
        stsTable->setMinimumSize(QSize(560, 0));
        stsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        stsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        stsTable->setSelectionMode(QAbstractItemView::SingleSelection);
        stsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        stsTable->setSortingEnabled(true);
        stsTable->horizontalHeader()->setStretchLastSection(true);
        stsTable->verticalHeader()->setVisible(false);

        stsTabLayout->addWidget(stsTable);

        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/Images/images/icon_statusMsg.png"), QSize(), QIcon::Normal, QIcon::Off);
        inpTabWidget->addTab(inpStsTab, icon8, QString());

        tabFrameLayout->addWidget(inpTabWidget);

        sendButton = new QToolButton(inpTabFrame);
        sendButton->setObjectName(QString::fromUtf8("sendButton"));
        sendButton->setEnabled(false);
        sizePolicy3.setHeightForWidth(sendButton->sizePolicy().hasHeightForWidth());
        sendButton->setSizePolicy(sizePolicy3);
        sendButton->setMinimumSize(QSize(40, 40));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_send.png"), QSize(), QIcon::Disabled, QIcon::On);
        icon9.addFile(QString::fromUtf8(":/Images/images/icon_send2.png"), QSize(), QIcon::Active, QIcon::On);
        sendButton->setIcon(icon9);
        sendButton->setIconSize(QSize(40, 40));
        sendButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

        tabFrameLayout->addWidget(sendButton);

        splitter1->addWidget(inpTabFrame);

        mainLayout->addWidget(splitter1);


        verticalLayout->addWidget(mainFrame);


        retranslateUi(Sds);

        inpTabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Sds);
    } // setupUi

    void retranslateUi(QWidget *Sds)
    {
        Sds->setWindowTitle(QCoreApplication::translate("Sds", "Message", nullptr));
#if QT_CONFIG(tooltip)
        attHistButton->setToolTip(QCoreApplication::translate("Sds", "Recent file attachments", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        msgHistButton->setToolTip(QCoreApplication::translate("Sds", "Recent texts", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        tplButton->setToolTip(QCoreApplication::translate("Sds", "Template", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        stsButton->setToolTip(QCoreApplication::translate("Sds", "Status Message", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        clearButton->setToolTip(QCoreApplication::translate("Sds", "Clear text", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        mmsFileButton->setToolTip(QCoreApplication::translate("Sds", "Attach file", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        mmsClrButton->setToolTip(QCoreApplication::translate("Sds", "Clear attachments", nullptr));
#endif // QT_CONFIG(tooltip)
        inpTabWidget->setTabText(inpTabWidget->indexOf(inpSdsTab), QCoreApplication::translate("Sds", "SDS, MMS, Status", nullptr));
        QTableWidgetItem *___qtablewidgetitem = stsTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Sds", "Type", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = stsTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Sds", "Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = stsTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Sds", "Text", nullptr));
        inpTabWidget->setTabText(inpTabWidget->indexOf(inpStsTab), QCoreApplication::translate("Sds", "Status", nullptr));
#if QT_CONFIG(tooltip)
        sendButton->setToolTip(QCoreApplication::translate("Sds", "Send", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class Sds: public Ui_Sds {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SDS_H
