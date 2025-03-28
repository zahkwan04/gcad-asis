//#ifndef STREAMER_H
//#define STREAMER_H

//#include <QObject>
//#include <QProcess>
//#include <QString>

//class Streamer : public QObject
//{
//    Q_OBJECT

//public:
//    explicit Streamer(QObject *parent = nullptr);

//    /**
//     * Starts streaming source video to RSTP URL.
//     *
//     * @param[in] filePath Video source.
//     * @param[in] rtspUrl  The URL.
//     */
//    void startStreaming(const QString &filePath, const QString &rtspUrl);

//    /**
//     * Stops streaming source video to RSTP URL.
//     */
//    void stopStreaming();

//private slots:
//    /**
//     * Restart if FFmpeg stops unexpectedly.
//     */
//    void restartStreaming();

//private:
//    QProcess ffmpegProcess;
//    QString  mFilePath;
//    QString  mRtspUrl;
//};

//#endif // STREAMER_H

#ifndef STREAMER_H
#define STREAMER_H

#include <QObject>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <windows.h>

class Streamer : public QObject
{
    Q_OBJECT
public:
    explicit Streamer(QObject *parent = nullptr);
     ~Streamer();
    void startStreaming();
    void stopStreaming();
    void sendFrameData(const QByteArray &frameData);

private:
    QProcess ffmpegProcess;
    QString pipePath = R"(\\.\pipe\video_pipe)";
    HANDLE hNamedPipe;
};

#endif // STREAMER_H
