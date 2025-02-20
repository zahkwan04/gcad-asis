/**
 * SCAD Client main module.
 *
 * Copyright (C) Sapura Secured Technologies, 2013-2019. All Rights Reserved.
 *
 * @file
 * @version $Id: Main.cpp 1525 2021-08-06 08:07:05Z rosnin $
 * @author Mazdiana Makmor
 * @author Nurfaizatul Ain Othman
 */
#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "MainWindow.h"
#include "webrtc_test.h"

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    //when building for a particular language, do 3 things here:
    //1. change the default locale below
    //2. add app translator using the prepared translation file Scad_<xx>.qm
    //3. add qt base translator using the prepared file qtbase_<xx>.qm
    //e.g. for malay:
    //QLocale::setDefault(QLocale(QLocale::Malay, QLocale::Malaysia));
    //QTranslator translator;
    //translator.load("Scad_ms", "translations");
    //app.installTranslator(&translator);
    //QTranslator baseTrans;
    //baseTrans.load("qtbase_ms", "translations");
    //app.installTranslator(&baseTrans);
    QLocale::setDefault(QLocale(QLocale::English, QLocale::Malaysia));
    MainWindow mainWindow;
    mainWindow.hide();
    // Create and show webrtc_test window
//    webrtc_test webrtcDialog;
//    webrtcDialog.show();  // Show as a modal dialog
    return app.exec();
}
