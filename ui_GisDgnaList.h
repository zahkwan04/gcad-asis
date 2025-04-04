/********************************************************************************
** Form generated from reading UI file 'GisDgnaList.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISDGNALIST_H
#define UI_GISDGNALIST_H

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

class Ui_GisDgnaList
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *dgna;
    QVBoxLayout *verticalLayout_9;
    QFrame *dgnaLblFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *dgnaIconLbl;
    QLabel *dgnaLabel;
    QToolButton *floatButton;
    QToolButton *toggleBtn;
    QFrame *dgnaFrame;
    QVBoxLayout *verticalLayout_2;
    QListView *listView;
    QHBoxLayout *dissolveLO;
    QSpacerItem *horizontalSpacer;
    QPushButton *assignBtn;
    QPushButton *viewBtn;
    QPushButton *dissolveBtn;
    QPushButton *removeBtn;

    void setupUi(QWidget *GisDgnaList)
    {
        if (GisDgnaList->objectName().isEmpty())
            GisDgnaList->setObjectName(QString::fromUtf8("GisDgnaList"));
        GisDgnaList->resize(432, 328);
        verticalLayout = new QVBoxLayout(GisDgnaList);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        dgna = new QFrame(GisDgnaList);
        dgna->setObjectName(QString::fromUtf8("dgna"));
        dgna->setFrameShape(QFrame::NoFrame);
        dgna->setFrameShadow(QFrame::Raised);
        verticalLayout_9 = new QVBoxLayout(dgna);
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        dgnaLblFrame = new QFrame(dgna);
        dgnaLblFrame->setObjectName(QString::fromUtf8("dgnaLblFrame"));
        dgnaLblFrame->setMaximumSize(QSize(16777215, 37));
        dgnaLblFrame->setFrameShape(QFrame::StyledPanel);
        dgnaLblFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(dgnaLblFrame);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        dgnaIconLbl = new QLabel(dgnaLblFrame);
        dgnaIconLbl->setObjectName(QString::fromUtf8("dgnaIconLbl"));
        dgnaIconLbl->setMaximumSize(QSize(30, 30));
        dgnaIconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_dgna_ind.png")));
        dgnaIconLbl->setScaledContents(true);

        horizontalLayout->addWidget(dgnaIconLbl);

        dgnaLabel = new QLabel(dgnaLblFrame);
        dgnaLabel->setObjectName(QString::fromUtf8("dgnaLabel"));

        horizontalLayout->addWidget(dgnaLabel);

        floatButton = new QToolButton(dgnaLblFrame);
        floatButton->setObjectName(QString::fromUtf8("floatButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_down.png"), QSize(), QIcon::Normal, QIcon::On);
        floatButton->setIcon(icon);
        floatButton->setIconSize(QSize(25, 25));
        floatButton->setCheckable(true);

        horizontalLayout->addWidget(floatButton);

        toggleBtn = new QToolButton(dgnaLblFrame);
        toggleBtn->setObjectName(QString::fromUtf8("toggleBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::On);
        toggleBtn->setIcon(icon1);
        toggleBtn->setCheckable(true);
        toggleBtn->setChecked(true);
        toggleBtn->setAutoRaise(true);

        horizontalLayout->addWidget(toggleBtn);


        verticalLayout_9->addWidget(dgnaLblFrame);

        dgnaFrame = new QFrame(dgna);
        dgnaFrame->setObjectName(QString::fromUtf8("dgnaFrame"));
        dgnaFrame->setMaximumSize(QSize(16777215, 16777215));
        dgnaFrame->setFrameShape(QFrame::NoFrame);
        dgnaFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(dgnaFrame);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        listView = new QListView(dgnaFrame);
        listView->setObjectName(QString::fromUtf8("listView"));

        verticalLayout_2->addWidget(listView);

        dissolveLO = new QHBoxLayout();
        dissolveLO->setSpacing(5);
        dissolveLO->setObjectName(QString::fromUtf8("dissolveLO"));
        dissolveLO->setContentsMargins(-1, 5, -1, 5);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        dissolveLO->addItem(horizontalSpacer);

        assignBtn = new QPushButton(dgnaFrame);
        assignBtn->setObjectName(QString::fromUtf8("assignBtn"));
        assignBtn->setMinimumSize(QSize(90, 40));

        dissolveLO->addWidget(assignBtn);

        viewBtn = new QPushButton(dgnaFrame);
        viewBtn->setObjectName(QString::fromUtf8("viewBtn"));
        viewBtn->setEnabled(false);
        viewBtn->setMinimumSize(QSize(90, 40));

        dissolveLO->addWidget(viewBtn);

        dissolveBtn = new QPushButton(dgnaFrame);
        dissolveBtn->setObjectName(QString::fromUtf8("dissolveBtn"));
        dissolveBtn->setEnabled(false);
        dissolveBtn->setMinimumSize(QSize(90, 40));

        dissolveLO->addWidget(dissolveBtn);

        removeBtn = new QPushButton(dgnaFrame);
        removeBtn->setObjectName(QString::fromUtf8("removeBtn"));
        removeBtn->setEnabled(false);
        removeBtn->setMinimumSize(QSize(90, 40));

        dissolveLO->addWidget(removeBtn);


        verticalLayout_2->addLayout(dissolveLO);


        verticalLayout_9->addWidget(dgnaFrame);


        verticalLayout->addWidget(dgna);


        retranslateUi(GisDgnaList);

        QMetaObject::connectSlotsByName(GisDgnaList);
    } // setupUi

    void retranslateUi(QWidget *GisDgnaList)
    {
        dgnaIconLbl->setText(QString());
        dgnaLabel->setText(QCoreApplication::translate("GisDgnaList", "DGNA on Map", nullptr));
#if QT_CONFIG(tooltip)
        floatButton->setToolTip(QCoreApplication::translate("GisDgnaList", "Float", nullptr));
#endif // QT_CONFIG(tooltip)
        assignBtn->setText(QCoreApplication::translate("GisDgnaList", "Assign", nullptr));
        viewBtn->setText(QCoreApplication::translate("GisDgnaList", "Members", nullptr));
        dissolveBtn->setText(QCoreApplication::translate("GisDgnaList", "Dissolve", nullptr));
        removeBtn->setText(QCoreApplication::translate("GisDgnaList", "Remove", nullptr));
        (void)GisDgnaList;
    } // retranslateUi

};

namespace Ui {
    class GisDgnaList: public Ui_GisDgnaList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISDGNALIST_H
