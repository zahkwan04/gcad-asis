/********************************************************************************
** Form generated from reading UI file 'GisBookmarks.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISBOOKMARKS_H
#define UI_GISBOOKMARKS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GisBookmarks
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *bookmarks;
    QVBoxLayout *verticalLayout_1;
    QFrame *lblFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *iconLbl;
    QLabel *label;
    QToolButton *floatButton;
    QToolButton *toggleBtn;
    QFrame *stdLblFrame;
    QHBoxLayout *horizontalLayout_1;
    QLabel *label1;
    QFrame *stdFrame;
    QVBoxLayout *verticalLayout_2;
    QListView *stdListView;
    QFrame *usrLblFrame;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label2;
    QFrame *usrFrame;
    QVBoxLayout *verticalLayout_3;
    QListView *usrListView;
    QHBoxLayout *btnLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *addBtn;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *renameBtn;
    QSpacerItem *horizontalSpacer;
    QPushButton *deleteBtn;

    void setupUi(QWidget *GisBookmarks)
    {
        if (GisBookmarks->objectName().isEmpty())
            GisBookmarks->setObjectName(QString::fromUtf8("GisBookmarks"));
        GisBookmarks->setEnabled(true);
        GisBookmarks->resize(432, 328);
        verticalLayout = new QVBoxLayout(GisBookmarks);
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        bookmarks = new QFrame(GisBookmarks);
        bookmarks->setObjectName(QString::fromUtf8("bookmarks"));
        bookmarks->setFrameShape(QFrame::NoFrame);
        bookmarks->setFrameShadow(QFrame::Raised);
        verticalLayout_1 = new QVBoxLayout(bookmarks);
        verticalLayout_1->setSpacing(0);
        verticalLayout_1->setObjectName(QString::fromUtf8("verticalLayout_1"));
        verticalLayout_1->setContentsMargins(0, 0, 0, 0);
        lblFrame = new QFrame(bookmarks);
        lblFrame->setObjectName(QString::fromUtf8("lblFrame"));
        lblFrame->setMaximumSize(QSize(16777215, 37));
        lblFrame->setFrameShape(QFrame::StyledPanel);
        lblFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(lblFrame);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        iconLbl = new QLabel(lblFrame);
        iconLbl->setObjectName(QString::fromUtf8("iconLbl"));
        iconLbl->setMaximumSize(QSize(30, 30));
        iconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_bookmarks.png")));
        iconLbl->setScaledContents(true);

        horizontalLayout->addWidget(iconLbl);

        label = new QLabel(lblFrame);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        floatButton = new QToolButton(lblFrame);
        floatButton->setObjectName(QString::fromUtf8("floatButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_down.png"), QSize(), QIcon::Normal, QIcon::On);
        floatButton->setIcon(icon);
        floatButton->setIconSize(QSize(25, 25));
        floatButton->setCheckable(true);

        horizontalLayout->addWidget(floatButton);

        toggleBtn = new QToolButton(lblFrame);
        toggleBtn->setObjectName(QString::fromUtf8("toggleBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::On);
        toggleBtn->setIcon(icon1);
        toggleBtn->setCheckable(true);
        toggleBtn->setChecked(true);
        toggleBtn->setAutoRaise(true);

        horizontalLayout->addWidget(toggleBtn);


        verticalLayout_1->addWidget(lblFrame);

        stdLblFrame = new QFrame(bookmarks);
        stdLblFrame->setObjectName(QString::fromUtf8("stdLblFrame"));
        stdLblFrame->setMaximumSize(QSize(16777215, 37));
        stdLblFrame->setFrameShape(QFrame::NoFrame);
        stdLblFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_1 = new QHBoxLayout(stdLblFrame);
        horizontalLayout_1->setSpacing(5);
        horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));
        horizontalLayout_1->setContentsMargins(5, 5, 5, 5);
        label1 = new QLabel(stdLblFrame);
        label1->setObjectName(QString::fromUtf8("label1"));

        horizontalLayout_1->addWidget(label1);


        verticalLayout_1->addWidget(stdLblFrame);

        stdFrame = new QFrame(bookmarks);
        stdFrame->setObjectName(QString::fromUtf8("stdFrame"));
        stdFrame->setMaximumSize(QSize(16777215, 16777215));
        stdFrame->setFrameShape(QFrame::NoFrame);
        stdFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(stdFrame);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        stdListView = new QListView(stdFrame);
        stdListView->setObjectName(QString::fromUtf8("stdListView"));

        verticalLayout_2->addWidget(stdListView);


        verticalLayout_1->addWidget(stdFrame);

        usrLblFrame = new QFrame(bookmarks);
        usrLblFrame->setObjectName(QString::fromUtf8("usrLblFrame"));
        usrLblFrame->setMaximumSize(QSize(16777215, 37));
        usrLblFrame->setFrameShape(QFrame::NoFrame);
        usrLblFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(usrLblFrame);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        label2 = new QLabel(usrLblFrame);
        label2->setObjectName(QString::fromUtf8("label2"));

        horizontalLayout_2->addWidget(label2);


        verticalLayout_1->addWidget(usrLblFrame);

        usrFrame = new QFrame(bookmarks);
        usrFrame->setObjectName(QString::fromUtf8("usrFrame"));
        usrFrame->setMaximumSize(QSize(16777215, 16777215));
        usrFrame->setFrameShape(QFrame::NoFrame);
        usrFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(usrFrame);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        usrListView = new QListView(usrFrame);
        usrListView->setObjectName(QString::fromUtf8("usrListView"));

        verticalLayout_3->addWidget(usrListView);

        btnLayout = new QHBoxLayout();
        btnLayout->setSpacing(0);
        btnLayout->setObjectName(QString::fromUtf8("btnLayout"));
        btnLayout->setContentsMargins(-1, 5, -1, 5);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        btnLayout->addItem(horizontalSpacer_2);

        addBtn = new QPushButton(usrFrame);
        addBtn->setObjectName(QString::fromUtf8("addBtn"));
        addBtn->setMinimumSize(QSize(90, 40));

        btnLayout->addWidget(addBtn);

        horizontalSpacer_3 = new QSpacerItem(13, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        btnLayout->addItem(horizontalSpacer_3);

        renameBtn = new QPushButton(usrFrame);
        renameBtn->setObjectName(QString::fromUtf8("renameBtn"));
        renameBtn->setEnabled(false);
        renameBtn->setMinimumSize(QSize(90, 40));

        btnLayout->addWidget(renameBtn);

        horizontalSpacer = new QSpacerItem(13, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        btnLayout->addItem(horizontalSpacer);

        deleteBtn = new QPushButton(usrFrame);
        deleteBtn->setObjectName(QString::fromUtf8("deleteBtn"));
        deleteBtn->setEnabled(false);
        deleteBtn->setMinimumSize(QSize(90, 40));

        btnLayout->addWidget(deleteBtn);


        verticalLayout_3->addLayout(btnLayout);


        verticalLayout_1->addWidget(usrFrame);


        verticalLayout->addWidget(bookmarks);


        retranslateUi(GisBookmarks);

        QMetaObject::connectSlotsByName(GisBookmarks);
    } // setupUi

    void retranslateUi(QWidget *GisBookmarks)
    {
        iconLbl->setText(QString());
        label->setText(QCoreApplication::translate("GisBookmarks", "Bookmarks", nullptr));
#if QT_CONFIG(tooltip)
        floatButton->setToolTip(QCoreApplication::translate("GisBookmarks", "Float", nullptr));
#endif // QT_CONFIG(tooltip)
        label1->setText(QCoreApplication::translate("GisBookmarks", "Standard", nullptr));
        label2->setText(QCoreApplication::translate("GisBookmarks", "User Defined", nullptr));
        addBtn->setText(QCoreApplication::translate("GisBookmarks", "Add", nullptr));
        renameBtn->setText(QCoreApplication::translate("GisBookmarks", "Rename", nullptr));
        deleteBtn->setText(QCoreApplication::translate("GisBookmarks", "Delete", nullptr));
        (void)GisBookmarks;
    } // retranslateUi

};

namespace Ui {
    class GisBookmarks: public Ui_GisBookmarks {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISBOOKMARKS_H
