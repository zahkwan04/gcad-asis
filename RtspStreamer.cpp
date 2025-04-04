#include "RtspStreamer.h"
#include <QThread>
#include <QDateTime>


RtspStreamer::RtspStreamer(QObject *parent) : QObject(parent), hNamedPipe(INVALID_HANDLE_VALUE)
{
    connect(&ffmpegProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::CrashExit) {
                    qDebug() << "FFmpeg crashed! Restarting stream...";
                    startStreaming();
                } else {
                    qDebug() << "FFmpeg exited normally.";
                }
            });
}

RtspStreamer::~RtspStreamer()
{
    qDebug() << "Destroying Streamer...";
    stopStreaming();
}

bool RtspStreamer::startStreaming()
{
    if (ffmpegProcess.state() == QProcess::Running) {
        qDebug() << "FFmpeg is already running!";
        return true;  // Already running is considered a success
    }

    if (hNamedPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
    }

    // Generate unique pipe name each time
    QString uniqueId = QString::number(QDateTime::currentMSecsSinceEpoch());
    QString currentPipePath = "\\\\.\\pipe\\videostream_" + uniqueId;

    // Create Windows Named Pipe
    hNamedPipe = CreateNamedPipe(
        currentPipePath.toStdWString().c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        640 * 480 * 3 / 2,
        640 * 480 * 3 / 2,
        0,
        nullptr
        );

    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to create named pipe! Error:" << GetLastError();
        return false;
    }

    qDebug() << "Named pipe created at" << currentPipePath << ", starting FFmpeg...";

    QString ffmpegPath = "C:/ffmpeg-7.1.1/bin/ffmpeg.exe";
    QStringList args = {
        "-f", "rawvideo",
        "-pix_fmt", "yuv420p",
        "-s", "640x480",
        "-r", "30",
        "-i", currentPipePath,  // Use the newly generated pipe path
        "-c:v", "libx264",
        "-preset", "ultrafast",
        "-tune", "zerolatency",
        "-f", "rtsp",
        "rtsp://localhost:8554/mystream"
    };

    ffmpegProcess.start(ffmpegPath, args);

    if (!ffmpegProcess.waitForStarted()) {
        qDebug() << "Failed to start FFmpeg!";
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
        return false;
    }

    qDebug() << "Streaming started.";

    if (!ConnectNamedPipe(hNamedPipe, nullptr)) {
        if (GetLastError() != ERROR_PIPE_CONNECTED) {
            qDebug() << "Failed to connect named pipe! Error:" << GetLastError();
            CloseHandle(hNamedPipe);
            hNamedPipe = INVALID_HANDLE_VALUE;
            return false;
        }
    }

    qDebug() << "Named pipe connected, ready to receive data.";
    return true;
}

void RtspStreamer::stopStreaming()
{
    if (QThread::currentThread() != this->thread()) {
        qDebug() << "stopStreaming called from a different thread, redirecting to main thread.";
        QMetaObject::invokeMethod(this, "stopStreaming", Qt::QueuedConnection);
        return;
    }

    qDebug() << "Stopping streaming...";

    // Prevent automatic restart before stopping
    disconnect(&ffmpegProcess,
               QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
               this,
               nullptr);

    if (ffmpegProcess.state() == QProcess::Running) {
        ffmpegProcess.terminate();

        if (!ffmpegProcess.waitForFinished(5000)) {
            qDebug() << "FFmpeg did not terminate, forcing kill!";
            ffmpegProcess.kill();
            ffmpegProcess.waitForFinished();
        }
        qDebug() << "Streaming stopped.";
    }

    if (hNamedPipe != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(hNamedPipe);
        DisconnectNamedPipe(hNamedPipe);
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
        qDebug() << "Named pipe closed.";
    }

    // Add a delay to ensure the system releases the pipe resources
    QThread::msleep(500);

    emit streamingStopped();
}

void RtspStreamer::sendFrameData(const QByteArray &frameData)
{
    if (frameData.size() != (640 * 480 * 3 / 2)) {
        qDebug() << "Error: Invalid frame size! Expected:" << (640 * 480 * 3 / 2) << "but got:" << frameData.size();
        return;
    }

    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Pipe not available!";
        return;
    }

    DWORD bytesWritten;
    BOOL result = WriteFile(hNamedPipe, frameData.constData(), frameData.size(), &bytesWritten, nullptr);

    if (!result || bytesWritten != (DWORD)frameData.size()) {
        qDebug() << "Failed to write data to named pipe!";
    } else {
        qDebug() << "Frame written to pipe: " << bytesWritten << " bytes";
    }

    FlushFileBuffers(hNamedPipe);
}
