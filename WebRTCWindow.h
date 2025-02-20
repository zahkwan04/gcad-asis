#ifndef WEBRTCWINDOW_H
#define WEBRTCWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>

QT_BEGIN_NAMESPACE
namespace Ui {
class WebRTCWindow;
}
QT_END_NAMESPACE

class WebRTCWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit WebRTCWindow(QWidget *parent = nullptr);
    ~WebRTCWindow();

private:
    Ui::WebRTCWindow *ui;
    QWebEngineView *webRtcView;
};

#endif // WEBRTCWINDOW_H
