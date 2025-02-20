#ifndef WEBRTC_TEST_H
#define WEBRTC_TEST_H

#include <QDialog>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QUrl>

namespace Ui {
class webrtc_test;
}

class webrtc_test : public QDialog
{
    Q_OBJECT

public:
    explicit webrtc_test(QWidget *parent = nullptr);
    ~webrtc_test();

private slots:
    void handleFeaturePermission(const QUrl &origin, QWebEnginePage::Feature feature);

private:
    Ui::webrtc_test *ui;
    QWebEngineView *browser;
};

#endif // WEBRTC_TEST_H
