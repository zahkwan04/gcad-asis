/********************************************************************************
** Form generated from reading UI file 'webrtc_test.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WEBRTC_TEST_H
#define UI_WEBRTC_TEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_webrtc_test
{
public:
    QWidget *verticalLayoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QWidget *browserContainer;

    void setupUi(QDialog *webrtc_test)
    {
        if (webrtc_test->objectName().isEmpty())
            webrtc_test->setObjectName(QString::fromUtf8("webrtc_test"));
        webrtc_test->resize(920, 580);
        verticalLayoutWidget_2 = new QWidget(webrtc_test);
        verticalLayoutWidget_2->setObjectName(QString::fromUtf8("verticalLayoutWidget_2"));
        verticalLayoutWidget_2->setGeometry(QRect(110, 90, 291, 221));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget_2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        browserContainer = new QWidget(verticalLayoutWidget_2);
        browserContainer->setObjectName(QString::fromUtf8("browserContainer"));

        verticalLayout_2->addWidget(browserContainer);


        retranslateUi(webrtc_test);

        QMetaObject::connectSlotsByName(webrtc_test);
    } // setupUi

    void retranslateUi(QDialog *webrtc_test)
    {
        webrtc_test->setWindowTitle(QCoreApplication::translate("webrtc_test", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class webrtc_test: public Ui_webrtc_test {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WEBRTC_TEST_H
