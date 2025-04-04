#ifndef RTSPSTREAMER_H
#define RTSPSTREAMER_H

#include <QObject>
#include <QProcess>
#include <QFile>
#include <QDebug>
#include <windows.h>

/**
 * @class RtspStreamer
 * @brief Handles RTSP streaming using FFmpeg and named pipes.
 *
 * This class manages the RTSP streaming process using FFmpeg.
 * It initializes, starts, and stops the streaming process while also
 * providing an interface to send video frame data via a named pipe.
 */
class RtspStreamer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an RtspStreamer instance.
     * @param[in] parent Optional parent QObject.
     */
    explicit RtspStreamer(QObject *parent = nullptr);

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
    QProcess ffmpegProcess;   ///< Process handling FFmpeg execution.
    HANDLE hNamedPipe = INVALID_HANDLE_VALUE; ///< Handle for the named pipe.
    const QString pipePath = R"(\\.\pipe\my_pipe)"; ///< Named pipe path.

signals:
    /**
     * @brief Emitted when the streaming process stops.
     */
    void streamingStopped();
};

#endif // RTSPSTREAMER_H
