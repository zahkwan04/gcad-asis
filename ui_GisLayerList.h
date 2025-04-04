/********************************************************************************
** Form generated from reading UI file 'GisLayerList.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISLAYERLIST_H
#define UI_GISLAYERLIST_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GisLayerList
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *layersTitleFrame;
    QHBoxLayout *mapLayersHLayout;
    QLabel *mapLayersIconLbl;
    QLabel *mapLayersLbl;
    QToolButton *floatButton;
    QToolButton *toggleBtn;
    QFrame *layerListFrame;
    QVBoxLayout *verticalLayout_19;
    QTreeView *treeView;

    void setupUi(QWidget *GisLayerList)
    {
        if (GisLayerList->objectName().isEmpty())
            GisLayerList->setObjectName(QString::fromUtf8("GisLayerList"));
        GisLayerList->resize(400, 300);
        verticalLayout = new QVBoxLayout(GisLayerList);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        layersTitleFrame = new QFrame(GisLayerList);
        layersTitleFrame->setObjectName(QString::fromUtf8("layersTitleFrame"));
        layersTitleFrame->setMaximumSize(QSize(16777215, 37));
        layersTitleFrame->setFrameShape(QFrame::StyledPanel);
        layersTitleFrame->setFrameShadow(QFrame::Raised);
        mapLayersHLayout = new QHBoxLayout(layersTitleFrame);
        mapLayersHLayout->setSpacing(5);
        mapLayersHLayout->setObjectName(QString::fromUtf8("mapLayersHLayout"));
        mapLayersHLayout->setContentsMargins(5, 5, 5, 5);
        mapLayersIconLbl = new QLabel(layersTitleFrame);
        mapLayersIconLbl->setObjectName(QString::fromUtf8("mapLayersIconLbl"));
        mapLayersIconLbl->setMaximumSize(QSize(30, 30));
        mapLayersIconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_mapLayers.png")));
        mapLayersIconLbl->setScaledContents(true);

        mapLayersHLayout->addWidget(mapLayersIconLbl);

        mapLayersLbl = new QLabel(layersTitleFrame);
        mapLayersLbl->setObjectName(QString::fromUtf8("mapLayersLbl"));

        mapLayersHLayout->addWidget(mapLayersLbl);

        floatButton = new QToolButton(layersTitleFrame);
        floatButton->setObjectName(QString::fromUtf8("floatButton"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QString::fromUtf8(":/Images/images/icon_float_down.png"), QSize(), QIcon::Normal, QIcon::On);
        floatButton->setIcon(icon);
        floatButton->setIconSize(QSize(25, 25));
        floatButton->setCheckable(true);

        mapLayersHLayout->addWidget(floatButton);

        toggleBtn = new QToolButton(layersTitleFrame);
        toggleBtn->setObjectName(QString::fromUtf8("toggleBtn"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_downward.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/Images/images/icon_upward.png"), QSize(), QIcon::Normal, QIcon::On);
        toggleBtn->setIcon(icon1);
        toggleBtn->setCheckable(true);
        toggleBtn->setChecked(true);
        toggleBtn->setAutoRaise(true);

        mapLayersHLayout->addWidget(toggleBtn);


        verticalLayout->addWidget(layersTitleFrame);

        layerListFrame = new QFrame(GisLayerList);
        layerListFrame->setObjectName(QString::fromUtf8("layerListFrame"));
        layerListFrame->setMaximumSize(QSize(16777215, 16777215));
        layerListFrame->setFrameShape(QFrame::NoFrame);
        layerListFrame->setFrameShadow(QFrame::Raised);
        verticalLayout_19 = new QVBoxLayout(layerListFrame);
        verticalLayout_19->setSpacing(0);
        verticalLayout_19->setObjectName(QString::fromUtf8("verticalLayout_19"));
        verticalLayout_19->setContentsMargins(5, 5, 5, 5);
        treeView = new QTreeView(layerListFrame);
        treeView->setObjectName(QString::fromUtf8("treeView"));

        verticalLayout_19->addWidget(treeView);


        verticalLayout->addWidget(layerListFrame);


        retranslateUi(GisLayerList);

        QMetaObject::connectSlotsByName(GisLayerList);
    } // setupUi

    void retranslateUi(QWidget *GisLayerList)
    {
        mapLayersIconLbl->setText(QString());
        mapLayersLbl->setText(QCoreApplication::translate("GisLayerList", "Map Layers", nullptr));
#if QT_CONFIG(tooltip)
        floatButton->setToolTip(QCoreApplication::translate("GisLayerList", "Float", nullptr));
#endif // QT_CONFIG(tooltip)
        (void)GisLayerList;
    } // retranslateUi

};

namespace Ui {
    class GisLayerList: public Ui_GisLayerList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISLAYERLIST_H
