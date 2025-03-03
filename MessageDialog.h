/**
 * A custom QDialog module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: MessageDialog.h 1905 2025-02-21 02:55:53Z rosnin $
 * @author Mohd Rozaimi
 * @author Mohd Rashid
 */
#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include <QDialog>
#include <QLCDNumber>
#include <QMessageBox>
#include <QStringList>
#include <QTime>
#include <QTimer>

namespace Ui {
class MessageDialog;
}

class MessageDialog : public QDialog
{
    Q_OBJECT

public:
    typedef QList<QStringList> TableDataT;

    /**
     * Basic constructor with no data.
     *
     * @param[in] title         The message title.
     * @param[in] header        The message header.
     * @param[in] icon          The window icon.
     * @param[in] deleteOnClose true to delete on close.
     * @param[in] parent        The parent widget, if any.
     */
    MessageDialog(const QString &title,
                  const QString &header,
                  const QIcon   &icon,
                  bool           deleteOnClose,
                  QWidget       *parent = 0);

    /**
     * Constructor with no data and no header.
     *
     * @param[in] title         The message title.
     * @param[in] icon          The window icon.
     * @param[in] deleteOnClose true to delete on close.
     * @param[in] parent        The parent widget, if any.
     */
    MessageDialog(const QString &title,
                  const QIcon   &icon,
                  bool           deleteOnClose,
                  QWidget       *parent = 0);

    /**
     * Constructor to display a string.
     *
     * @param[in] title         The message title.
     * @param[in] header        The message header.
     * @param[in] text          The message text.
     * @param[in] icon          The window icon.
     * @param[in] deleteOnClose true to delete on close.
     * @param[in] parent        The parent widget, if any.
     */
    MessageDialog(const QString &title,
                  const QString &header,
                  const QString &text,
                  const QIcon   &icon,
                  bool           deleteOnClose,
                  QWidget       *parent = 0);

    ~MessageDialog();

    /**
     * Adds a widget to the vertical splitter.
     *
     * @param[in] w The widget.
     */
    void addWidget(QWidget *w);

    /**
     * Sets the message data in a table.
     *
     * @param[in] data The table data.
     */
    void setData(const TableDataT &data);

    /**
     * Sets the message data for a timer dialog.
     *
     * @param[in] timerMinutes The timer duration.
     * @param[in] okBtnText    The OK button text.
     */
    void setData(int timerMinutes, const QString &okBtnText);

    /**
     * Shows this instance without OK button, which means it cannot be dismissed
     * by user, and must be deleted by caller.
     *
     * @param[in] widget Widget to add to the vertical splitter, if any.
     */
    void showNoOk(QWidget *widget = 0);

    /**
     * Shows a non-modal message dialog.
     *
     * @param[in] title         The message title.
     * @param[in] header        The message header.
     * @param[in] text          The message text.
     * @param[in] icon          The window icon.
     * @param[in] deleteOnClose true to delete on close.
     * @param[in] parent        The parent widget, if any.
     * @return The created message dialog.
     */
    static MessageDialog *showMessage(const QString &title,
                                      const QString &header,
                                      const QString &text,
                                      const QIcon   &icon,
                                      bool           deleteOnClose,
                                      QWidget       *parent = 0);

    /**
     * Shows a non-modal message dialog with data in a table.
     *
     * @param[in] title         The message title.
     * @param[in] header        The message header.
     * @param[in] data          The table data.
     * @param[in] icon          The window icon.
     * @param[in] deleteOnClose true to delete on close.
     * @param[in] parent        The parent widget, if any.
     * @return The created message dialog.
     */
    static MessageDialog *showMessage(const QString    &title,
                                      const QString    &header,
                                      const TableDataT &data,
                                      const QIcon      &icon,
                                      bool              deleteOnClose,
                                      QWidget          *parent = 0);

    /**
     * Shows a non-modal message dialog with a countdown timer that
     * automatically dismisses when the countdown reaches zero.
     *
     * @param[in] title     The message title.
     * @param[in] header    The message header.
     * @param[in] okBtnText The OK button text.
     * @param[in] icon      The window icon.
     * @param[in] minutes   The duration to show before dismissal.
     * @param[in] parent    The parent widget, if any.
     * @return The created message dialog.
     */
    static MessageDialog *showTimer(const QString &title,
                                    const QString &header,
                                    const QString &okBtnText,
                                    const QIcon   &icon,
                                    int            minutes,
                                    QWidget       *parent = 0);

    /**
     * Shows a non-modal message dialog which is deleted on close.
     *
     * @param[in] title  The message title.
     * @param[in] header The message header.
     * @param[in] text   The message text.
     * @param[in] icon   The window icon.
     * @param[in] parent The parent widget, if any.
     */
    static void showMessage(const QString &title,
                            const QString &header,
                            const QString &text,
                            const QIcon   &icon,
                            QWidget       *parent = 0);

    /**
     * Shows a standard message dialog which is deleted on close.
     *
     * @param[in] title  The message title.
     * @param[in] text   The message.
     * @param[in] icon   The message box icon.
     * @param[in] modal  true for modal dialog.
     * @param[in] parent The parent widget, if any.
     */
    static void showStdMessage(const QString     &title,
                               const QString     &text,
                               QMessageBox::Icon  icon,
                               bool               modal,
                               QWidget           *parent = 0);

private:
    Ui::MessageDialog *ui;
    QLCDNumber        *mLcdNum;
    QTime             *mTime;
    QTimer            *mTimer;

    /**
     * Initializes the instance.
     *
     * @param[in] title         The message title.
     * @param[in] header        The message header.
     * @param[in] icon          The window icon.
     * @param[in] deleteOnClose true to delete on close.
     */
    void init(const QString &title,
              const QString &header,
              const QIcon   &icon,
              bool           deleteOnClose);
};
#endif //MESSAGEDIALOG_H
