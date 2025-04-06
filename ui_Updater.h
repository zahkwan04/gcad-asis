/********************************************************************************
** Form generated from reading UI file 'Updater.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UPDATER_H
#define UI_UPDATER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_Updater
{
public:
    QGridLayout *gridLayout;
    QLabel *lblIcon;
    QLabel *lblMsg;
    QLabel *lblDownload;
    QProgressBar *progressBar;
    QHBoxLayout *buttonHLayout;
    QSpacerItem *buttonHSpacer;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *Updater)
    {
        if (Updater->objectName().isEmpty())
            Updater->setObjectName(QString::fromUtf8("Updater"));
        Updater->resize(440, 293);
        gridLayout = new QGridLayout(Updater);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lblIcon = new QLabel(Updater);
        lblIcon->setObjectName(QString::fromUtf8("lblIcon"));
        lblIcon->setPixmap(QPixmap(QString::fromUtf8(":/Images/images/icon_update.png")));
        lblIcon->setScaledContents(false);
        lblIcon->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        gridLayout->addWidget(lblIcon, 0, 0, 1, 1);

        lblMsg = new QLabel(Updater);
        lblMsg->setObjectName(QString::fromUtf8("lblMsg"));
        lblMsg->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lblMsg->setMargin(5);

        gridLayout->addWidget(lblMsg, 0, 1, 1, 1);

        lblDownload = new QLabel(Updater);
        lblDownload->setObjectName(QString::fromUtf8("lblDownload"));
        lblDownload->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

        gridLayout->addWidget(lblDownload, 1, 1, 1, 1);

        progressBar = new QProgressBar(Updater);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setMinimumSize(QSize(250, 2));
        progressBar->setValue(23);
        progressBar->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        progressBar->setTextVisible(false);

        gridLayout->addWidget(progressBar, 2, 1, 1, 1);

        buttonHLayout = new QHBoxLayout();
        buttonHLayout->setObjectName(QString::fromUtf8("buttonHLayout"));
        buttonHSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonHLayout->addItem(buttonHSpacer);

        okButton = new QPushButton(Updater);
        okButton->setObjectName(QString::fromUtf8("okButton"));

        buttonHLayout->addWidget(okButton);

        cancelButton = new QPushButton(Updater);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        buttonHLayout->addWidget(cancelButton);


        gridLayout->addLayout(buttonHLayout, 3, 1, 1, 2);


        retranslateUi(Updater);

        QMetaObject::connectSlotsByName(Updater);
    } // setupUi

    void retranslateUi(QDialog *Updater)
    {
        Updater->setWindowTitle(QCoreApplication::translate("Updater", "Updater", nullptr));
        okButton->setText(QCoreApplication::translate("Updater", "Install", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Updater: public Ui_Updater {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UPDATER_H
