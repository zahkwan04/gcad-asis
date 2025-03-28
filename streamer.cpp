//#include "streamer.h"
//#include <QDebug>

//Streamer::Streamer(QObject *parent) : QObject(parent)
//{
//    //restart when FFmpeg process exits
//    connect(&ffmpegProcess,
//            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//            this, &Streamer::restartStreaming);
//}

//void Streamer::startStreaming(const QString &filePath, const QString &rtspUrl)
//{
//    //save streaming parameters
//    mFilePath = filePath;
//    mRtspUrl = rtspUrl;
//    //path to ffmpeg.exe
//    QString path = "C:/ffmpeg-7.1.1/bin/ffmpeg.exe";

//    //stopped existing process before starting a new one
//    stopStreaming();

//    //FFmpeg command and arguments
//    QStringList args = { "-re", "-stream_loop", "-1",  // Infinite loop
//                         "-i", filePath, "-vcodec", "libx264", "-preset",
//                         "ultrafast", "-tune", "zerolatency", "-f", "rtsp",
//                         rtspUrl
//    };

//    //start FFmpeg process
//    ffmpegProcess.start(path, args);

//    //for debugging purposes
//    if (!ffmpegProcess.waitForStarted())
//        qDebug() << "Failed to start FFmpeg process!";
//    else
//        qDebug() << "Streaming started: " << rtspUrl;
//}

//void Streamer::stopStreaming()
//{
//    if (ffmpegProcess.state() == QProcess::Running)
//    {
//        ffmpegProcess.terminate();
//        if (!ffmpegProcess.waitForFinished(3000))
//            ffmpegProcess.kill();
//        qDebug() << "Streaming stopped.";
//    }
//}

//void Streamer::restartStreaming()
//{
//    qDebug() << "FFmpeg process stopped. Restarting...";
//    startStreaming(mFilePath, mRtspUrl);
//}

#include "streamer.h"

#include <QThread>

//Streamer::Streamer(QObject *parent) : QObject(parent), hNamedPipe(INVALID_HANDLE_VALUE)
//{
//    // Restart FFmpeg when process exits
//    connect(&ffmpegProcess,
//            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//            this, &Streamer::startStreaming);
//}
Streamer::Streamer(QObject *parent) : QObject(parent), hNamedPipe(INVALID_HANDLE_VALUE)
{
    // Restart FFmpeg when process exits, but only if needed
    connect(&ffmpegProcess,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::CrashExit) {
                    qDebug() << "FFmpeg crashed! Restarting stream...";
                    startStreaming();
                } else {
                    qDebug() << "FFmpeg exited normally.";
                }
            });
}

Streamer::~Streamer()
{
    qDebug() << "Destroying Streamer...";
    stopStreaming();  // Ensure cleanup before destruction
}


//void Streamer::startStreaming()
//{
//    // Close existing pipe if open
//    if (hNamedPipe != INVALID_HANDLE_VALUE)
//    {
//        CloseHandle(hNamedPipe);
//        hNamedPipe = INVALID_HANDLE_VALUE;
//    }

//    // Create Windows Named Pipe
//    hNamedPipe = CreateNamedPipe(
//        pipePath.toStdWString().c_str(),  // Pipe name
//        PIPE_ACCESS_DUPLEX,               // Read/Write pipe
//        PIPE_TYPE_BYTE | PIPE_WAIT,       // Byte stream mode, waits for connections
//        1,                                // Max instances
//        640 * 480 * 3 / 2,              // Output buffer size
//        640 * 480 * 3 / 2,              // Input buffer size
//        0,                                // Default timeout
//        nullptr                           // Security attributes
//        );

//    if (hNamedPipe == INVALID_HANDLE_VALUE)
//    {
//        qDebug() << "Failed to create named pipe!";
//        return;
//    }

////    // Start FFmpeg process
//    QString ffmpegPath = "C:/ffmpeg-7.1.1/bin/ffmpeg.exe";
//    QStringList args = {
//        "-f", "rawvideo",
//        "-pix_fmt", "yuv420p",
//        "-s", "640x480",  // Set resolution
//        "-r", "30",         // Set frame rate
//        "-i", pipePath,     // Input from Windows named pipe
//        "-c:v", "libx264",
//        "-preset", "ultrafast",
//        "-tune", "zerolatency",
//        "-f", "rtsp",
//        "rtsp://localhost:8554/mystream"
//    };



//    ffmpegProcess.start(ffmpegPath, args);

//    if (!ffmpegProcess.waitForStarted())
//    {
//        qDebug() << "Failed to start FFmpeg process!";
//        return;
//    }
//    else
//    {
//        qDebug() << "Streaming started.";
//    }

//    // Wait for FFmpeg to connect
//    if (!ConnectNamedPipe(hNamedPipe, nullptr))
//    {
//        if (GetLastError() != ERROR_PIPE_CONNECTED)
//        {
//            qDebug() << "Failed to connect named pipe!";
//            CloseHandle(hNamedPipe);
//            hNamedPipe = INVALID_HANDLE_VALUE;
//            return;
//        }
//    }

//    qDebug() << "Named pipe connected, ready to receive data.";
//}

void Streamer::startStreaming()
{
    if (ffmpegProcess.state() == QProcess::Running) {
        qDebug() << "FFmpeg is already running!";
        return;
    }

    if (hNamedPipe != INVALID_HANDLE_VALUE)
    {
        qDebug() << "Closing existing named pipe...";
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
    }

    // Create Windows Named Pipe
    hNamedPipe = CreateNamedPipe(
        pipePath.toStdWString().c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        640 * 480 * 3 / 2,
        640 * 480 * 3 / 2,
        0,
        nullptr
        );

    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Failed to create named pipe!";
        return;
    }

    qDebug() << "Named pipe created, starting FFmpeg...";

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

    if (!ConnectNamedPipe(hNamedPipe, nullptr))
    {
        if (GetLastError() != ERROR_PIPE_CONNECTED)
        {
            qDebug() << "Failed to connect named pipe!";
            CloseHandle(hNamedPipe);
            hNamedPipe = INVALID_HANDLE_VALUE;
            return;
        }
    }

    qDebug() << "Named pipe connected, ready to receive data.";
}


//void Streamer::stopStreaming()
//{
//    // 1. Stop the FFmpeg process
//    if (ffmpegProcess.state() == QProcess::Running)
//    {
//        ffmpegProcess.terminate();

//        // Ensure process stops properly
//        if (!ffmpegProcess.waitForFinished(5000)) // Wait up to 5s
//        {
//            qDebug() << "FFmpeg did not terminate, forcing kill!";
//            ffmpegProcess.kill();
//            ffmpegProcess.waitForFinished(); // Ensure it's fully dead
//        }
//        qDebug() << "Streaming stopped.";
//    }

//    // 2. Close and clean up the named pipe
//    if (hNamedPipe != INVALID_HANDLE_VALUE)
//    {
//        FlushFileBuffers(hNamedPipe);  // Ensure any remaining data is written
//        DisconnectNamedPipe(hNamedPipe); // Forcefully disconnect any client
//        CloseHandle(hNamedPipe);
//        hNamedPipe = INVALID_HANDLE_VALUE;
//        qDebug() << "Named pipe closed.";
//    }

//    // 3. Disconnect Qt signals to prevent automatic restart
//    disconnect(&ffmpegProcess, nullptr, this, nullptr);
//}

//void Streamer::stopStreaming()
//{
//    qDebug() << "Stopping streaming...";

//    // Prevent automatic restart before stopping
//    disconnect(&ffmpegProcess, nullptr, this, nullptr);

//    if (ffmpegProcess.state() == QProcess::Running)
//    {
//        qDebug() << "Terminating FFmpeg...";
//        ffmpegProcess.terminate();

//        if (!ffmpegProcess.waitForFinished(5000))  // Wait up to 5s
//        {
//            qDebug() << "FFmpeg did not terminate, forcing kill!";
//            ffmpegProcess.kill();
//            ffmpegProcess.waitForFinished();
//        }
//    }

//    if (hNamedPipe != INVALID_HANDLE_VALUE)
//    {
//        FlushFileBuffers(hNamedPipe);
//        DisconnectNamedPipe(hNamedPipe);
//        CloseHandle(hNamedPipe);
//        hNamedPipe = INVALID_HANDLE_VALUE;
//        qDebug() << "Named pipe closed.";
//    }

//    qDebug() << "Streaming stopped.";
//}

void Streamer::stopStreaming()
{
    if (QThread::currentThread() != this->thread())
    {
        qDebug() << "stopStreaming called from a different thread, redirecting to main thread.";
        QMetaObject::invokeMethod(this, "stopStreaming", Qt::QueuedConnection);
        return;
    }

    // 1. Stop the FFmpeg process safely
    if (ffmpegProcess.state() == QProcess::Running)
    {
        ffmpegProcess.terminate();

        if (!ffmpegProcess.waitForFinished(5000)) // Wait up to 5s
        {
            qDebug() << "FFmpeg did not terminate, forcing kill!";
            ffmpegProcess.kill();
            ffmpegProcess.waitForFinished();
        }
        qDebug() << "Streaming stopped.";
    }

    // 2. Close the named pipe safely
    if (hNamedPipe != INVALID_HANDLE_VALUE)
    {
        FlushFileBuffers(hNamedPipe);
        DisconnectNamedPipe(hNamedPipe);
        CloseHandle(hNamedPipe);
        hNamedPipe = INVALID_HANDLE_VALUE;
        qDebug() << "Named pipe closed.";
    }

    // 3. Disconnect Qt signals to prevent restart
    disconnect(&ffmpegProcess, nullptr, this, nullptr);
}

void Streamer::sendFrameData(const QByteArray &frameData)
{
    if (frameData.size() != (640 * 480 * 3 / 2)) {
        qDebug() << "Error: Invalid frame size! Expected:" << (640 * 480 * 3 / 2) << "but got:" << frameData.size();
        return;
    }

    if (hNamedPipe == INVALID_HANDLE_VALUE)
    {
        qDebug() << "Pipe not available!";
        return;
    }

    DWORD bytesWritten;
    BOOL result = WriteFile(hNamedPipe, frameData.constData(), frameData.size(), &bytesWritten, nullptr);

    if (!result || bytesWritten != (DWORD)frameData.size())
    {
        qDebug() << "Failed to write data to named pipe!";
    }
    else
    {
        qDebug() << "Frame written to pipe: " << bytesWritten << " bytes";
    }

    FlushFileBuffers(hNamedPipe);

}
