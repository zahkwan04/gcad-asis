#ifndef RTSPSTREAMER_H
#define RTSPSTREAMER_H

#include <QObject>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <windows.h>

class RtspStreamer : public QObject
{
    Q_OBJECT
public:
    explicit RtspStreamer(QObject *parent = nullptr);
    ~RtspStreamer();
    void startStreaming();
    void stopStreaming();
    void sendFrameData(const QByteArray &frameData);

private:
    QProcess ffmpegProcess;
    QString pipePath = R"(\\.\pipe\video_pipe)";
    HANDLE hNamedPipe;
};

#endif // RTSPSTREAMER_H
