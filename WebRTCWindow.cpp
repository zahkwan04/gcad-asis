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
    // enable insecure contents
    webRtcView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    webRtcView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    //enable WebRTC explicitly
    webRtcView->settings()->setAttribute(QWebEngineSettings::WebRTCPublicInterfacesOnly, false);
    // javascript execution enabled
    webRtcView->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
    webRtcView->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, true);

    // Load WebRTC stream URL
    webRtcView->setUrl(QUrl("http://10.12.49.100/showStream?workflowId=CarsCrossingDecathlon"));

    // Set as the main widget
    setCentralWidget(webRtcView);
}

WebRTCWindow::~WebRTCWindow() {
    delete ui;
}
