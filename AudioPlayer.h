/**
 * Class for MP3 audio playback of voice call recording at a local or remote
 * location.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: AudioPlayer.h 1826 2024-03-18 01:17:01Z rosnin $
 * @author Zulzaidi Atan
 */
#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QDialog>
#include <QList>
#include <QMediaPlayer>
#include <QMovie>

#include "Logger.h"
#include "MessageDialog.h"
#include "QtTableUtils.h"

namespace Ui {
class AudioPlayer;
}

class AudioPlayer : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] logger App logger.
     * @param[in] parent Parent widget, if any.
     */
    explicit AudioPlayer(Logger *logger, QWidget *parent = 0);

    ~AudioPlayer();

    /**
     * Sets the audio details. If successful, shows the dialog, and plays the
     * audio.
     *
     * @param[in] data Audio data.
     */
    void setDetails(const QtTableUtils::AudioData &data);

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

protected:
    bool eventFilter(QObject *obj, QEvent *evt) override;

private slots:
    /**
     * Sets the play position if playing, and the Tx party label if required.
     *
     * @param[in] pos The position.
     */
    void onAudioSliderMoved(int pos);

private:
    struct PttData
    {
        PttData(const QString &p, int s, int e) :
            txParty(p), start(s), end(e) {}

        QString txParty;
        int     start;
        int     end;
    };
    typedef QList<PttData> PttHistoryT;

    Ui::AudioPlayer         *ui;
    Logger                  *mLogger;
    QMediaPlayer            *mMediaPlayer;
    MessageDialog           *mMsgDlg; //shown while retrieving video data
    QMovie                  *mMovie;  //animated GIF for mMsgDlg
    int                      mRetry;
    QString                  mPath;
    PttHistoryT              mPttHistory;
    QtTableUtils::AudioData  mAudData;

    /**
     * Resets the UI.
     */
    void resetUi();

    /**
     * Gets a time string in "mm:ss" format.
     *
     * @param[in] time The time in milliseconds.
     * @return The formatted time.
     */
    QString getTimeString(int time);

    /**
     * Gets PTT data from database for a particular call.
     *
     * @param[in] callKey The call key.
     * @return true if successful.
     */
    bool getPttData(const QString &callKey);

    /**
     * Shows or hides the PTT Tx party label based on the audio position.
     *
     * @param[in] position The position in seconds.
     */
    void showTxParty(int position);

    /**
     * Shows or hides this dialog.
     * When hiding, saves the current screen position first.
     * When showing, restores the saved position.
     *
     * @param[in] visible true to show.
     */
    void setVisibility(bool visible);
};
#endif //AUDIOPLAYER_H
