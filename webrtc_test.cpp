#include "webrtc_test.h"
#include "ui_webrtc_test.h"
#include <QVBoxLayout>

webrtc_test::webrtc_test(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::webrtc_test)
{
    ui->setupUi(this);
    setWindowTitle("WebRTC Test Window");

    // Initialize QWebEngineView
    browser = new QWebEngineView(this);

    // Enable WebRTC and required settings
    QWebEngineSettings *settings = browser->settings();
    settings->setAttribute(QWebEngineSettings::WebRTCPublicInterfacesOnly, false);
    settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    settings->setAttribute(QWebEngineSettings::AutoLoadImages, true);
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
//    settings->setAttribute(QWebEngineSettings::DeveloperExtrasEnabled, true); // Enable DevTools

    // Load URL
    browser->setUrl(QUrl("https://tv.cctv.com/live/"));

    // Connect permissions handler
    connect(browser->page(), &QWebEnginePage::featurePermissionRequested,
            this, &webrtc_test::handleFeaturePermission);

    // Add browser to layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(browser);
    setLayout(layout);

    // Run JavaScript verification for WebRTC
    browser->page()->runJavaScript(R"(
        let rtcAvailable = (typeof RTCPeerConnection !== 'undefined');
        let whepAvailable = (typeof WHEPClient !== 'undefined');
        console.log('RTCPeerConnection available:', rtcAvailable);
        console.log('WHEPClient available:', whepAvailable);
    )");
}

void webrtc_test::handleFeaturePermission(const QUrl &origin, QWebEnginePage::Feature feature) {
    if (feature == QWebEnginePage::MediaAudioCapture ||
        feature == QWebEnginePage::MediaVideoCapture ||
        feature == QWebEnginePage::MediaAudioVideoCapture) {
        browser->page()->setFeaturePermission(
            origin, feature, QWebEnginePage::PermissionPolicy::PermissionGrantedByUser);
    }
}

webrtc_test::~webrtc_test()
{
    delete ui;
}
