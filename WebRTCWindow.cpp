#include "WebRTCWindow.h"
#include "ui_WebRTCWindow.h"
#include <QWebEngineView>
#include <qwebenginesettings.h>

WebRTCWindow::WebRTCWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::WebRTCWindow),
    webRtcView(new QWebEngineView(this)) {
    ui->setupUi(this);

    // Enable WebRTC & JavaScript
    webRtcView->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    webRtcView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    webRtcView->settings()->setAttribute(QWebEngineSettings::AutoLoadImages, true);

    // Load WebRTC stream URL
    webRtcView->setUrl(QUrl("https://www.youtube.com/watch?v=gTJLjDQ9jhs"));

    // Set as the main widget
    setCentralWidget(webRtcView);
}

WebRTCWindow::~WebRTCWindow() {
    delete ui;
}
