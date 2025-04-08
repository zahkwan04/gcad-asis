#include "RtspStreamer.h"
#include <QThread>

/**
 * Constructor: Initializes the FFmpeg process handler.
 */
RtspStreamer::RtspStreamer(void* cbObj, StatusCallbackFn cbFn)
    : hNamedPipe(INVALID_HANDLE_VALUE), mCbObj(cbObj), mCbFn(cbFn)
{
    // Set up finished handler
    QObject::connect(&ffmpegProcess,
                     QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     [this](int exitCode, QProcess::ExitStatus exitStatus) {
                         handleProcessFinished(exitCode, exitStatus);
                     });
}

/**
 * Destructor: Cleans up by stopping the stream and closing the pipe.
 */
RtspStreamer::~RtspStreamer()
{
    qDebug() << "Destroying Streamer...";
    stopStreaming();

}

/**
 * Handles the FFmpeg process finished event.
 */
void RtspStreamer::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        qDebug() << "FFmpeg crashed! Restarting stream...";
        startStreaming();
    } else {
        qDebug() << "FFmpeg exited normally.";

        // Notify via callback if available
        if (mCbFn != nullptr) {
            mCbFn(mCbObj, false);
        }
    }
}

/**
 * Starts FFmpeg and sets up a Windows named pipe to receive raw video frames.
 */
void RtspStreamer::startStreaming()
{
    if (ffmpegProcess.state() == QProcess::Running) {
        qDebug() << "FFmpeg is already running!";
        return;
    }

    // Ensure previous pipe is cleaned up
    if (hNamedPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
    }

    // Create a new named pipe for raw video input
    hNamedPipe = CreateNamedPipe(
        pipePath.toStdWString().c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        640 * 480 * 3 / 2,  // Output buffer size (YUV420p)
        640 * 480 * 3 / 2,  // Input buffer size (YUV420p)
        0,
        nullptr
        );

    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        qDebug() << "Failed to create named pipe!";
        return;
    }

    qDebug() << "Named pipe created, starting FFmpeg...";

    // Configure and start FFmpeg process to read from pipe and stream via RTSP
    QString ffmpegPath = "C:/ffmpeg-7.1.1/bin/ffmpeg.exe";
    QStringList args = {
        "-f", "rawvideo",
        "-pix_fmt", "yuv420p",
        "-s", "640x480",
        "-r", "30",
        "-i", pipePath,
        "-c:v", "libx264",
        "-preset", "ultrafast",
        "-tune", "zerolatency",
        "-f", "rtsp",
        "rtsp://localhost:8554/mystream"
    };

    ffmpegProcess.start(ffmpegPath, args);

    if (!ffmpegProcess.waitForStarted()) {
        qDebug() << "Failed to start FFmpeg!";
        return;
    }

    qDebug() << "Streaming started.";

    // Notify via callback if available
    if (mCbFn != nullptr) {
        mCbFn(mCbObj, true);
    }

    // Wait for connection from writer (client)
    if (!ConnectNamedPipe(hNamedPipe, nullptr)) {
        if (GetLastError() != ERROR_PIPE_CONNECTED) {
            qDebug() << "Failed to connect named pipe!";
            CloseHandle(hNamedPipe);
            hNamedPipe = INVALID_HANDLE_VALUE;
            return;
        }
    }

    qDebug() << "Named pipe connected, ready to receive data.";
}

/**
 * Stops the FFmpeg process and safely closes the named pipe.
 */
void RtspStreamer::stopStreaming()
{
    qDebug() << "Stopping streaming...";

    // Terminate FFmpeg gracefully, or forcefully if needed
    if (ffmpegProcess.state() == QProcess::Running) {
        ffmpegProcess.terminate();

        if (!ffmpegProcess.waitForFinished(5000)) {
            qDebug() << "FFmpeg did not terminate, forcing kill!";
            ffmpegProcess.kill();
            ffmpegProcess.waitForFinished();
        }
        qDebug() << "Streaming stopped.";
    }

    // Clean up named pipe
    if (hNamedPipe != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(hNamedPipe);
        DisconnectNamedPipe(hNamedPipe);
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
        qDebug() << "Named pipe closed.";
    }

    // Notify via callback if available
    if (mCbFn != nullptr) {
        mCbFn(mCbObj, false);
    }
}

/**
 * Writes a raw YUV420p video frame to the pipe.
 * Frame size is validated to be 460800 bytes (640x480 YUV420p).
 */
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

    // Write data to the pipe
    DWORD bytesWritten;
    BOOL result = WriteFile(hNamedPipe, frameData.constData(), frameData.size(), &bytesWritten, nullptr);

    if (!result || bytesWritten != (DWORD)frameData.size()) {
        qDebug() << "Failed to write data to named pipe!";
    } else {
        qDebug() << "Frame written to pipe: " << bytesWritten << " bytes";
    }

    FlushFileBuffers(hNamedPipe);
}
