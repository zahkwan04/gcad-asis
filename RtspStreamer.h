#ifndef RTSPSTREAMER_H
#define RTSPSTREAMER_H

#include <QProcess>
#include <QFile>
#include <QDebug>
#include <windows.h>
#include <mutex>

/**
 * @class RtspStreamer
 * @brief Handles RTSP streaming using FFmpeg and named pipes.
 *
 * This class manages the RTSP streaming process using FFmpeg.
 * It initializes, starts, and stops the streaming process while also
 * providing an interface to send video frame data via a named pipe.
 */
class RtspStreamer
{
public:
    /**
     * @brief Callback function type for streaming status changes.
     * @param cbObj Callback object pointer.
     * @param isRunning Boolean indicating whether streaming is running.
     */
    typedef void (*StatusCallbackFn)(void* cbObj, bool isRunning);

    /**
     * @brief Constructs an RtspStreamer instance.
     * @param[in] cbObj Optional callback object pointer.
     * @param[in] cbFn Optional status callback function.
     */
    explicit RtspStreamer(void* cbObj = nullptr, StatusCallbackFn cbFn = nullptr);

    /**
     * @brief Destroys the RtspStreamer instance and releases resources.
     */
    ~RtspStreamer();

    /**
     * @brief Starts the RTSP streaming process.
     *
     * This function initializes FFmpeg and the named pipe for streaming.
     */
    void startStreaming();

    /**
     * @brief Stops the RTSP streaming process.
     *
     * This function stops FFmpeg and cleans up the named pipe.
     */
    void stopStreaming();

    /**
     * @brief Sends frame data to the named pipe for streaming.
     * @param[in] frameData QByteArray containing the frame data.
     */
    void sendFrameData(const QByteArray &frameData);

private:
    /**
     * @brief Handles the FFmpeg process finished event.
     * @param exitCode Process exit code.
     * @param exitStatus Process exit status.
     */
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    QProcess ffmpegProcess;   ///< Process handling FFmpeg execution.
    HANDLE hNamedPipe;        ///< Handle for the named pipe.
    const QString pipePath = R"(\\.\pipe\my_pipe)"; ///< Named pipe path.

    // Callback mechanism
    void* mCbObj;             ///< Callback object pointer.
    StatusCallbackFn mCbFn;   ///< Status callback function.
    std::mutex mMutex;
    bool mIsDestroying = false;
};

#endif // RTSPSTREAMER_H
