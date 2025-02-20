/**
 * Class to download updated file.
 *
 * Copyright (C) Sapura Secured Technologies, 2019-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: Updater.h 1899 2025-01-22 05:42:07Z rosnin $
 * @author Rosnin Mustaffa
 */
#ifndef UPDATER_H
#define UPDATER_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

#include "Logger.h"
#include "MsgSp.h"
#include "PalLock.h"

namespace Ui {
class Updater;
}

class Updater : public QDialog
{
    Q_OBJECT

public:
    /**
     * Instantiates the singleton if it has not been created.
     *
     * @return The instance.
     */
    static Updater &instance(QWidget *parent);

    /**
     * Sets the logger. Must be done before calling instance().
     *
     * @param[in] logger App logger.
     */
    static void setLogger(Logger *logger) { sLogger = logger; }

    /**
     * Sets the logout flag, and resets the skip version if user logged out.
     *
     * @param[in] loggedOut true if user logged out.
     */
    static void setLogout(bool loggedOut);

    /**
     * Deletes the single instance.
     */
    static void destroy();

    /**
     * Starts the application update process in either of 2 ways:
     *   1. In the foreground with a dialog.
     *   2. Silently in the background, notifying the user only after
     *      completion.
     *
     * @param[in] msg        The server VERSION-CLIENT message.
     * @param[in] foreground true for foreground.
     */
    void updateApp(bool foreground, MsgSp *msg);

signals:
    void finished(bool doExit);

private slots:
    /**
     * Handles download finished event.
     */
    void onFinished();

private:
    Ui::Updater           *ui;
    int                    mMainDownloadCount;
    int                    mFileIdx;
    bool                   mForeground;
    std::string            mMainChksum;
    QString                mMainLocalPath;
    QString                mFilePath;
    QStringList            mFileList;       //files to download
    QTimer                 mTimer;
    QFile                 *mFile;
    QNetworkAccessManager *mManager;
    QNetworkReply         *mReply;

    static bool            sIsCreated;
    static bool            sLogout;         //true if user logged out
    static QString         sSkipVersion;    //version the user skipped
    static Updater        *sInstance;       //single class instance
    static Logger         *sLogger;         //logger object
    static PalLock::LockT  sSingletonLock;

    /**
     * Constructor is private to prevent direct instantiation.
     *
     * @param[in] parent Parent widget, if any.
     */
    explicit Updater(QWidget *parent = 0);

    ~Updater();

    /**
     * Starts the update process, only if no update is on-going.
     *
     * @param[in] msg       The server update message.
     * @param[in] extRegExp The wildcard regular expression for the main
     *                      filename extension, e.g. "*.exe" for installer.
     * @return true if successful.
     *         Failure reasons - update already in progress or missing main
     *         filename.
     */
    bool start(MsgSp *msg, const QString &extRegExp);

    /**
     * Starts downloading one file in the file list as indexed by mFileIdx.
     */
    void startDownload();

    /**
     * Runs the downloaded update in a new detached process and emits a signal.
     */
    void runUpdate();

    /**
     * Checks validity of the main downloaded file, and deletes it if invalid.
     *
     * @param[in] decrementCount true to decrement the retry count for invalid
     *                           file.
     * @return 0 for valid file. Positive value to retry. Negative for failure.
     */
    int validateMain(bool decrementCount);
};
#endif //UPDATER_H
