/**
 * The Bookmarks module.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: GisBookmarks.h 1833 2024-03-25 07:55:22Z hazim.rujhan $
 * @author Mohd Fashan Abdul Munir
 */
#ifndef GISBOOKMARKS_H
#define GISBOOKMARKS_H

#include <QDialog>
#include <QSpacerItem>
#include <QStandardItemModel>
#include <QWidget>

#include "GisCanvas.h"

namespace Ui {
class GisBookmarks;
}

class GisBookmarks : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param[in] canvas   The map canvas.
     * @param[in] username The username.
     * @param[in] parent   Parent widget, if any.
     */
    explicit GisBookmarks(GisCanvas     *canvas,
                          const QString &username,
                          QWidget       *parent = 0);

    ~GisBookmarks();

    /**
     * Applies color theme to UI components.
     */
    void setTheme();

    /**
     * Sets the user and loads User Defined data if user has changed.
     *
     * @param[in] username The username.
     */
    void setUser(const QString &username);

signals:
    void dock(QDialog *dlg);

private:
    Ui::GisBookmarks   *ui;
    GisCanvas          *mCanvas;
    QSpacerItem        *mSpacer;
    QStandardItemModel *mUsrModel;
    QStandardItemModel *mStdModel;
    QString             mUsername;

    /**
     * Loads Standard or User Defined data.
     *
     * @param[in] user true for User Defined data.
     */
    void loadData(bool user);

    /**
     * Prompts for a new bookmark name.
     *
     * @param[in] title    The dialog title.
     * @param[in] currName The selected bookmark name, if any.
     */
    QString getInput(const QString &title, const QString &currName = "");
};
#endif //GISBOOKMARKS_H
