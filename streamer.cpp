#include "streamer.h"
#include <QDebug>

Streamer::Streamer(QObject *parent) : QObject(parent)
{
    //restart when FFmpeg process exits
    connect(&ffmpegProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Streamer::restartStreaming);
}

void Streamer::startStreaming(const QString &filePath, const QString &rtspUrl)
{
    //save streaming parameters
    mFilePath = filePath;
    mRtspUrl = rtspUrl;
    //path to ffmpeg.exe
    QString path = "C:/ffmpeg-7.1.1/bin/ffmpeg.exe";

    //stopped existing process before starting a new one
    stopStreaming();

    //TODO: Modify to pass through rtp data/rtp url -> create udp socket and pass through the url.
    //FFmpeg command and arguments
    QStringList args = { "-re", "-stream_loop", "-1",  // Infinite loop
        "-i", filePath, "-vcodec", "libx264", "-preset",
        "ultrafast", "-tune", "zerolatency", "-f", "rtsp",
        rtspUrl
    };

    //Read from stdin
//    QStringList args = {
//        "-f", "rtp",  // Format is RTP
//        "-i", "pipe:0",  // Read from stdin
//        "-vcodec", "libx264", "-preset", "ultrafast",
//        "-tune", "zerolatency", "-f", "rtsp",
//        rtspUrl
//    };

    //start FFmpeg process
    ffmpegProcess.start(path, args);

    //for debugging purposes
    if (!ffmpegProcess.waitForStarted())
        qDebug() << "Failed to start FFmpeg process!";
    else
        qDebug() << "Streaming started: " << rtspUrl;
}

void Streamer::stopStreaming()
{
    if (ffmpegProcess.state() == QProcess::Running)
    {
        ffmpegProcess.terminate();
        if (!ffmpegProcess.waitForFinished(3000))
            ffmpegProcess.kill();
        qDebug() << "Streaming stopped.";
    }
}

void Streamer::restartStreaming()
{
    qDebug() << "FFmpeg process stopped. Restarting...";
    startStreaming(mFilePath, mRtspUrl);
}
