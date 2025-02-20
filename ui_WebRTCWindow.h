/********************************************************************************
** Form generated from reading UI file 'WebRTCWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WEBRTCWINDOW_H
#define UI_WEBRTCWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE

class Ui_WebRTCWindow
{
public:

    void setupUi(QMainWindow *WebRTCWindow)
    {
        if (WebRTCWindow->objectName().isEmpty())
            WebRTCWindow->setObjectName(QString::fromUtf8("WebRTCWindow"));
        WebRTCWindow->resize(800, 600);

        retranslateUi(WebRTCWindow);

        QMetaObject::connectSlotsByName(WebRTCWindow);
    } // setupUi

    void retranslateUi(QMainWindow *WebRTCWindow)
    {
        WebRTCWindow->setWindowTitle(QCoreApplication::translate("WebRTCWindow", "WebRTC Viewer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WebRTCWindow: public Ui_WebRTCWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WEBRTCWINDOW_H
