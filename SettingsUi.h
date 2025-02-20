/**
 * The Settings UI module.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: SettingsUi.h 1881 2024-11-19 08:15:16Z hazim.rujhan $
 * @author Mazdiana Makmor
 */
#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include <set>
#include <QCloseEvent>
#include <QDialog>
#include <QLineEdit>
#include <QSettings>

#include "AudioDevice.h"
#include "Logger.h"
#include "ServerSession.h"
#include "SubsData.h"

namespace Ui {
class SettingsUi;
}

class SettingsUi : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] parent Parent widget, if any.
     */
    explicit SettingsUi(QWidget *parent = 0);

    ~SettingsUi();

    void setLogger(Logger *logger);

    /**
     * Loads and validates settings data from permanent storage.
     */
    void load();

    /**
     * Saves settings data to permanent storage.
     *
     * @return true if successful.
     */
    bool save();

    void setSession(ServerSession *ss);

    void setOrganization(const QString &org);

    /**
     * Updates the setting for GPS Monitoring.
     *
     * @param[in] issiList The new setting.
     */
    void gpsMonChanged(const std::string &issiList);

    /**
     * Sets the allowed branches for selection.
     *
     * @param[in] branches The branches.
     */
    void setAllowedBranches(const SubsData::BranchMapT &branches);

    /**
     * Shows video preview.
     *
     * @param[in] frame The image.
     */
    void onVideoPreviewReceived(QPixmap frame);

    /**
     * Callback function for video preview frame.
     *
     * @param[in] obj   SettungsUi object, owner of the callback function.
     * @param[in] frame The image.
     */
    static void previewCb(void *obj, QPixmap frame);

signals:
    void audioInChanged(const QString &device);
    void audioOutChanged(const QString &device);
    void branchChanged();
    void isFinished(bool ok);
    void gpsMon();
    void mmsDownloadDirChanged();
    void rscDspOptChanged(int);
    void setTheme();
    void terminalCheckTimeChanged();
    void terminalLblOptChanged();

private slots:
    /**
     * Marks whether the source text box content has changed against the saved
     * settings. Disregards any leading and trailing spaces.
     *
     * @param[in] text The text.
     */
    void onTextChanged(const QString &text);

    /**
     * Marks whether the source ComboBox selected index has changed against the
     * saved settings.
     *
     * @param[in] idx The selected index.
     */
    void onComboIndexChanged(int idx);

    /**
     * Marks whether the source CheckBox state has changed against the saved
     * settings.
     *
     * @param[in] isChecked true if the CheckBox item is checked.
     */
    void onCheckBoxClicked(bool isChecked);

protected:
    /**
     * Starts audio input device if the current page is Audio Settings.
     */
    void showEvent(QShowEvent *);

private:
    Ui::SettingsUi *ui;
    Logger         *mLogger;
    ServerSession  *mSession;
    AudioDevice    *mInDevice;
    AudioDevice    *mOutDevice;
    QString         mPath;      //.ini file or registry path
    QString         mBranches;
    std::set<int>   mChangedKeys;

    /**
     * Adds a settings key to the list if its value has changed against the
     * saved settings, or removes it otherwise.
     * Enables the OK and Apply buttons if at least one setting has changed.
     * Disables them otherwise.
     *
     * @param[in] key       The settings key - Props::eField.
     * @param[in] isChanged true if the setting is changed.
     */
    void setModState(int key, bool isChanged);

    /**
     * Sets stylesheet of UI components.
     */
    void setStyle();

    /**
     * Shows a settings error dialog.
     *
     * @param[in] key The settings key - Props::eField.
     */
    void showSettingsError(int key);

    /**
     * Handles close event from window close button.
     */
    void closeEvent(QCloseEvent *event);

    /**
     * Displays a file dialog to select an existing directory.
     *
     * @param[in] le        The text box to put the selected directory name.
     *                      Uses current content as the working directory.
     * @param[in] writeable true to verify writeable.
     */
    void selectDir(QLineEdit *le, bool writeable);

    /**
     * Checks whether a given name is a valid existing path.
     * Appends a '/' to a valid name if it does not end with one.
     *
     * @param[in] name The path name.
     * @return true if valid.
     */
    bool validatePath(QString &name);

    /**
     * Checks whether a given name is an actual file name instead of a path, and
     * has a valid path (in an existing directory).
     *
     * @param[in] name         The file name.
     * @param[in] isEmptyValid true to accept an empty name as valid.
     * @return true if valid.
     */
    bool validateFileName(const QString &name, bool isEmptyValid);

    /**
     * Checks a value string after loading or before saving, and corrects a
     * negative value to "0".
     * After loading:
     *  -string given as input,
     *  -correct if necessary,
     *  -set to UI text field.
     * Before saving:
     *  -read string from UI text field,
     *  -correct if necessary,
     *  -set back to UI text field if correction done.
     *
     * @param[in]     onLoad   true if for loading, otherwise for saving.
     * @param[in]     lineEdit The UI text field.
     * @param[in,out] str      The value.
     * @return true if correction done.
     */
    bool checkNegative(bool onLoad, QLineEdit *lnEdit, QString &str);

    /**
     * Sets the page.
     *
     * @param[in] page The page index as defined in UI. Omit in initial call
     *                 from constructor.
     */
    void setPage(int page = -1);
};
#endif //SETTINGSUI_H
