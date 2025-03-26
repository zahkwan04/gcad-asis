/********************************************************************************
** Form generated from reading UI file 'GisLegend.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISLEGEND_H
#define UI_GISLEGEND_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GisLegend
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *legend;
    QVBoxLayout *verticalLayout2;
    QFrame *lblFrame;
    QHBoxLayout *horizontalLayout_2;
    QLabel *iconLbl;
    QLabel *label;
    QToolButton *floatButton;
    QToolButton *toggleBtn;
    QFrame *frame;
    QVBoxLayout *verticalLayout3;
    QPushButton *expandBtn;
    QTreeView *treeView;

    void setupUi(QWidget *GisLegend)
    {
        if (GisLegend->objectName().isEmpty())
            GisLegend->setObjectName(QString::fromUtf8("GisLegend"));
        GisLegend->setEnabled(true);
        GisLegend->resize(432, 328);
        verticalLayout = new QVBoxLayout(GisLegend);
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        legend = new QFrame(GisLegend);
        legend->setObjectName(QString::fromUtf8("legend"));
        legend->setFrameShape(QFrame::NoFrame);
        legend->setFrameShadow(QFrame::Raised);
        verticalLayout2 = new QVBoxLayout(legend);
        verticalLayout2->setSpacing(0);
        verticalLayout2->setObjectName(QString::fromUtf8("verticalLayout2"));
        verticalLayout2->setContentsMargins(0, 0, 0, 0);
        lblFrame = new QFrame(legend);
        lblFrame->setObjectName(QString::fromUtf8("lblFrame"));
        lblFrame->setMaximumSize(QSize(16777215, 37));
        lblFrame->setFrameShape(QFrame::StyledPanel);
        lblFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(lblFrame);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        iconLbl = new QLabel(lblFrame);
        iconLbl->setObjectName(QString::fromUtf8("iconLbl"));
        iconLbl->setMaximumSize(QSize(30, 30));
        iconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_legend.png")));
        iconLbl->setScaledContents(true);

        horizontalLayout_2->addWidget(iconLbl);

        label = new QLabel(lblFrame);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        floatButton = new QToolButton(lblFrame);
        floatButton->setObjectName(QString::fromUtf8("floatButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_down.png"), QSize(), QIcon::Normal, QIcon::On);
        floatButton->setIcon(icon);
        floatButton->setIconSize(QSize(25, 25));
        floatButton->setCheckable(true);

        horizontalLayout_2->addWidget(floatButton);

        toggleBtn = new QToolButton(lblFrame);
        toggleBtn->setObjectName(QString::fromUtf8("toggleBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::On);
        toggleBtn->setIcon(icon1);
        toggleBtn->setCheckable(true);
        toggleBtn->setChecked(true);
        toggleBtn->setAutoRaise(true);

        horizontalLayout_2->addWidget(toggleBtn);


        verticalLayout2->addWidget(lblFrame);

        frame = new QFrame(legend);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMaximumSize(QSize(16777215, 16777215));
        frame->setFrameShape(QFrame::NoFrame);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout3 = new QVBoxLayout(frame);
        verticalLayout3->setObjectName(QString::fromUtf8("verticalLayout3"));
        expandBtn = new QPushButton(frame);
        expandBtn->setObjectName(QString::fromUtf8("expandBtn"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::On);
        expandBtn->setIcon(icon2);
        expandBtn->setCheckable(true);
        expandBtn->setChecked(false);

        verticalLayout3->addWidget(expandBtn);

        treeView = new QTreeView(frame);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        verticalLayout3->addWidget(treeView);


        verticalLayout2->addWidget(frame);


        verticalLayout->addWidget(legend);


        retranslateUi(GisLegend);

        QMetaObject::connectSlotsByName(GisLegend);
    } // setupUi

    void retranslateUi(QWidget *GisLegend)
    {
        iconLbl->setText(QString());
        label->setText(QCoreApplication::translate("GisLegend", "Map Legend", nullptr));
#if QT_CONFIG(tooltip)
        floatButton->setToolTip(QCoreApplication::translate("GisLegend", "Float", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(tooltip)
        expandBtn->setToolTip(QCoreApplication::translate("GisLegend", "Expand all", nullptr));
#endif // QT_CONFIG(tooltip)
        (void)GisLegend;
    } // retranslateUi

};

namespace Ui {
    class GisLegend: public Ui_GisLegend {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISLEGEND_H
