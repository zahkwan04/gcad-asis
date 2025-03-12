/********************************************************************************
** Form generated from reading UI file 'GisMenuUserPoi.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GISMENUUSERPOI_H
#define UI_GISMENUUSERPOI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GisMenuUserPoi
{
public:
    QHBoxLayout *horizontalLayout_3;
    QFrame *frame;
    QVBoxLayout *verticalLayout_2;
    QFrame *titleFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *poiIconLbl;
    QLabel *poiLbl;
    QFormLayout *formLayout_2;
    QLabel *nameLbl;
    QLineEdit *txtName;
    QLabel *shortnameLbl;
    QLineEdit *txtShortName;
    QLabel *addrLbl;
    QTextEdit *txtAddress;
    QLabel *catLbl;
    QComboBox *cmbCategory;
    QLabel *descLbl;
    QTextEdit *txtDescription;
    QLabel *latLbl;
    QLineEdit *txtLat;
    QLineEdit *txtLon;
    QLabel *lonLbl;
    QLabel *ownerLbl;
    QLineEdit *txtOwner;
    QLabel *publicLbl;
    QCheckBox *publicChk;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *deleteBtn;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

    void setupUi(QDialog *GisMenuUserPoi)
    {
        if (GisMenuUserPoi->objectName().isEmpty())
            GisMenuUserPoi->setObjectName(QString::fromUtf8("GisMenuUserPoi"));
        GisMenuUserPoi->setWindowModality(Qt::WindowModal);
        GisMenuUserPoi->resize(214, 327);
        GisMenuUserPoi->setStyleSheet(QString::fromUtf8(""));
        GisMenuUserPoi->setSizeGripEnabled(true);
        GisMenuUserPoi->setModal(true);
        horizontalLayout_3 = new QHBoxLayout(GisMenuUserPoi);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(GisMenuUserPoi);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        verticalLayout_2 = new QVBoxLayout(frame);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(6, 6, 6, 6);
        titleFrame = new QFrame(frame);
        titleFrame->setObjectName(QString::fromUtf8("titleFrame"));
        horizontalLayout = new QHBoxLayout(titleFrame);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        poiIconLbl = new QLabel(titleFrame);
        poiIconLbl->setObjectName(QString::fromUtf8("poiIconLbl"));
        poiIconLbl->setMaximumSize(QSize(30, 30));
        poiIconLbl->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_dropPin.png")));
        poiIconLbl->setScaledContents(true);

        horizontalLayout->addWidget(poiIconLbl);

        poiLbl = new QLabel(titleFrame);
        poiLbl->setObjectName(QString::fromUtf8("poiLbl"));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        poiLbl->setFont(font);

        horizontalLayout->addWidget(poiLbl);


        verticalLayout_2->addWidget(titleFrame);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        nameLbl = new QLabel(frame);
        nameLbl->setObjectName(QString::fromUtf8("nameLbl"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, nameLbl);

        txtName = new QLineEdit(frame);
        txtName->setObjectName(QString::fromUtf8("txtName"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, txtName);

        shortnameLbl = new QLabel(frame);
        shortnameLbl->setObjectName(QString::fromUtf8("shortnameLbl"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, shortnameLbl);

        txtShortName = new QLineEdit(frame);
        txtShortName->setObjectName(QString::fromUtf8("txtShortName"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, txtShortName);

        addrLbl = new QLabel(frame);
        addrLbl->setObjectName(QString::fromUtf8("addrLbl"));

        formLayout_2->setWidget(4, QFormLayout::LabelRole, addrLbl);

        txtAddress = new QTextEdit(frame);
        txtAddress->setObjectName(QString::fromUtf8("txtAddress"));

        formLayout_2->setWidget(4, QFormLayout::FieldRole, txtAddress);

        catLbl = new QLabel(frame);
        catLbl->setObjectName(QString::fromUtf8("catLbl"));

        formLayout_2->setWidget(5, QFormLayout::LabelRole, catLbl);

        cmbCategory = new QComboBox(frame);
        cmbCategory->setObjectName(QString::fromUtf8("cmbCategory"));

        formLayout_2->setWidget(5, QFormLayout::FieldRole, cmbCategory);

        descLbl = new QLabel(frame);
        descLbl->setObjectName(QString::fromUtf8("descLbl"));

        formLayout_2->setWidget(6, QFormLayout::LabelRole, descLbl);

        txtDescription = new QTextEdit(frame);
        txtDescription->setObjectName(QString::fromUtf8("txtDescription"));

        formLayout_2->setWidget(6, QFormLayout::FieldRole, txtDescription);

        latLbl = new QLabel(frame);
        latLbl->setObjectName(QString::fromUtf8("latLbl"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, latLbl);

        txtLat = new QLineEdit(frame);
        txtLat->setObjectName(QString::fromUtf8("txtLat"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, txtLat);

        txtLon = new QLineEdit(frame);
        txtLon->setObjectName(QString::fromUtf8("txtLon"));

        formLayout_2->setWidget(3, QFormLayout::FieldRole, txtLon);

        lonLbl = new QLabel(frame);
        lonLbl->setObjectName(QString::fromUtf8("lonLbl"));

        formLayout_2->setWidget(3, QFormLayout::LabelRole, lonLbl);

        ownerLbl = new QLabel(frame);
        ownerLbl->setObjectName(QString::fromUtf8("ownerLbl"));

        formLayout_2->setWidget(7, QFormLayout::LabelRole, ownerLbl);

        txtOwner = new QLineEdit(frame);
        txtOwner->setObjectName(QString::fromUtf8("txtOwner"));

        formLayout_2->setWidget(7, QFormLayout::FieldRole, txtOwner);

        publicLbl = new QLabel(frame);
        publicLbl->setObjectName(QString::fromUtf8("publicLbl"));

        formLayout_2->setWidget(8, QFormLayout::LabelRole, publicLbl);

        publicChk = new QCheckBox(frame);
        publicChk->setObjectName(QString::fromUtf8("publicChk"));

        formLayout_2->setWidget(8, QFormLayout::FieldRole, publicChk);


        verticalLayout_2->addLayout(formLayout_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        deleteBtn = new QPushButton(frame);
        deleteBtn->setObjectName(QString::fromUtf8("deleteBtn"));

        horizontalLayout_2->addWidget(deleteBtn);

        okBtn = new QPushButton(frame);
        okBtn->setObjectName(QString::fromUtf8("okBtn"));

        horizontalLayout_2->addWidget(okBtn);

        cancelBtn = new QPushButton(frame);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));

        horizontalLayout_2->addWidget(cancelBtn);


        verticalLayout_2->addLayout(horizontalLayout_2);


        horizontalLayout_3->addWidget(frame);


        retranslateUi(GisMenuUserPoi);

        QMetaObject::connectSlotsByName(GisMenuUserPoi);
    } // setupUi

    void retranslateUi(QDialog *GisMenuUserPoi)
    {
        GisMenuUserPoi->setWindowTitle(QCoreApplication::translate("GisMenuUserPoi", "Dialog", nullptr));
        poiIconLbl->setText(QString());
        poiLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "POI", nullptr));
        nameLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Name", nullptr));
        shortnameLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Short Name", nullptr));
        addrLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Address", nullptr));
        catLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Category", nullptr));
        descLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Description", nullptr));
        latLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Latitude", nullptr));
        lonLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Longitude", nullptr));
        ownerLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Owner", nullptr));
        publicLbl->setText(QCoreApplication::translate("GisMenuUserPoi", "Public", nullptr));
        deleteBtn->setText(QCoreApplication::translate("GisMenuUserPoi", "Delete", nullptr));
        okBtn->setText(QCoreApplication::translate("GisMenuUserPoi", "OK", nullptr));
        cancelBtn->setText(QCoreApplication::translate("GisMenuUserPoi", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GisMenuUserPoi: public Ui_GisMenuUserPoi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GISMENUUSERPOI_H
