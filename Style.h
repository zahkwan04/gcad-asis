/**
 * The styling module that provides stylesheet strings for UI components and
 * manages the color theme.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Style.h 1892 2024-12-10 01:52:04Z hazim.rujhan $
 * @author Mohd Rozaimi
 * @author Mohd Rashid
 */
#ifndef STYLE_H
#define STYLE_H

#include <QMap>
#include <QString>
#include <QStringList>

class Style
{
public:
    enum eObject
    {
        OBJ_ABSTRACTITEMVIEW,
        OBJ_BACKGROUND,
        OBJ_BACKGROUND_DARK,
        OBJ_BACKGROUND_PANEL,
        OBJ_CALLBUTTON,
        OBJ_CHECKBOX,
        OBJ_COMBOBOX,
        OBJ_COMMON,
        OBJ_FONT_TITLE,
        OBJ_FONT1,
        OBJ_FONT2,
        OBJ_FONT2B,
        OBJ_FONT3,
        OBJ_FRAME_TITLE,
        OBJ_FRAME_TITLE2,
        OBJ_INPUTDIALOG,
        OBJ_LABEL,
        OBJ_LABEL_BLACK,
        OBJ_LABEL_LOGO,
        OBJ_LABEL_RED,
        OBJ_LABEL_SMALL,
        OBJ_LABEL_SMALL_BLACK,
        OBJ_LABEL_SMALL_WHITE,
        OBJ_LABEL_STATUS_OFFLINE,
        OBJ_LABEL_STATUS_ONLINE_NODATA,
        OBJ_LABEL_TITLE,
        OBJ_LABEL_WHITE,
        OBJ_MENU,
        OBJ_MENU_BASE,
        OBJ_MESSAGEBOX,
        OBJ_PUSHBUTTON,
        OBJ_PUSHBUTTON_SIMPLE,
        OBJ_RADIOBUTTON,
        OBJ_RESOURCEBUTTON,
        OBJ_SLIDER_BASE,
        OBJ_SLIDER_HORIZONTAL,
        OBJ_SLIDER_VERTICAL,
        OBJ_SLIDER_ZOOM,
        OBJ_TOOLBUTTON,
        OBJ_TOOLBUTTON_BASE,
        OBJ_TOOLBUTTON_HIGHLIGHT,
        OBJ_TOOLBUTTON_THEME_BG,
        OBJ_TREEVIEW,
        OBJ_TREEVIEW_CHECKBOX,
        OBJ_MAX
    };

    enum eTheme
    {
        THEME_DARK,   //the first is default
        THEME_GRAY,
        THEME_BLUE,
        THEME_BROWN,
        THEME_GREEN,
        THEME_ORANGE,
        THEME_PURPLE,
        THEME_RED,
        THEME_MAX
    };

    enum eThemeColorIdx
    {
        //from solid to lighter colors
        THEME_COLOR_IDX1 = 1, //MUST begin with 1
        THEME_COLOR_IDX2,
        THEME_COLOR_IDX3
    };

    /**
     * Initializes static members.
     * This needs to be done at runtime due to text content that is subject
     * to translation.
     */
    static void init();

    /**
     * Gets all theme names.
     *
     * @return The theme names.
     */
    static QStringList getThemeNames();

    /**
     * Sets the theme and theme-dependent styles.
     *
     * @param[in] theme The theme ID. See eTheme.
     * @return true if the theme is changed, false if theme is invalid.
     */
    static bool setTheme(int theme);

    static int getTheme() { return sTheme; }

    /**
     * Gets the stylesheet string for a UI component type.
     *
     * @param[in] type The UI component type. See eObject.
     * @return The stylesheet string.
     */
    static const QString &getStyle(int type);

    /**
     * Checks whether a UI component is highlighted given its stylesheet
     * string.
     *
     * @param[in] ss The stylesheet string.
     * @return true if highlighted.
     */
    static bool isHighlighted(const QString &ss);

    /**
     * Gets the color style string for the current theme.
     *
     * @param[in] idx The color index. See eThemeColorIdx. If outside the
     *                valid range, uses the first enum value.
     * @return The color style string.
     */
    static const QString &getThemeColor(int idx);

private:
    typedef QMap<int, QString>     StyleMapT;
    typedef QMap<int, QStringList> ThemeMapT;

    static StyleMapT sStyleMap;
    static ThemeMapT sThemeMap;
    static int       sTheme;    //this MUST be declared AFTER the maps
};
#endif //STYLE_H
