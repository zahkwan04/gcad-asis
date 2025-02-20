/**
 * The UI styling implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Style.cpp 1892 2024-12-10 01:52:04Z hazim.rujhan $
 * @author Mohd Rozaimi
 * @author Mohd Rashid
 */
#include <QObject>

#include "Style.h"

static const QString COLOR_HIGHLIGHT("color:red;");

Style::StyleMapT Style::sStyleMap;
Style::ThemeMapT Style::sThemeMap;
//init to negative to allow first setTheme(x) to any x
int              Style::sTheme = -1;

//get color (c) value after applying tint percentage (p), ranging from
//0.0 (clear) to 1.0 (solid)
#define GETCOLOR(c, p) (static_cast<int>((c + ((1.0 - p) * (255 - c)))))
//get tinted RGB string
#define GETRGB(r, g, b, p1, p2, p3) \
    QString("rgb(%1,%2,%3)").arg(GETCOLOR(r, p1)).arg(GETCOLOR(g, p1)) \
                            .arg(GETCOLOR(b, p1)) \
    << QString("rgb(%1,%2,%3)").arg(GETCOLOR(r, p2)).arg(GETCOLOR(g, p2)) \
                               .arg(GETCOLOR(b, p2)) \
    << QString("rgb(%1,%2,%3)").arg(GETCOLOR(r, p3)).arg(GETCOLOR(g, p3)) \
                               .arg(GETCOLOR(b, p3))

void Style::init()
{
    const QString fontFamily("font-family:\"Sapura\";");
    sStyleMap[OBJ_MAX]        = ""; //dummy
    sStyleMap[OBJ_FONT_TITLE] = "font:18pt;" + fontFamily;
    sStyleMap[OBJ_FONT1]      = "font:13pt;" + fontFamily;
    sStyleMap[OBJ_FONT2]      = "font:14pt;" + fontFamily;
    sStyleMap[OBJ_FONT2B]     = sStyleMap[OBJ_FONT2] + "font-weight:bold;";
    sStyleMap[OBJ_FONT3]      = "font:10pt;" + fontFamily;

    sStyleMap[OBJ_BACKGROUND_DARK]  = "black";
    sStyleMap[OBJ_BACKGROUND_PANEL] = "rgb(65,65,66)";

    sStyleMap[OBJ_LABEL_BLACK] =
        "QLabel {color:black;" + sStyleMap[OBJ_FONT1] + "}";
    sStyleMap[OBJ_LABEL] = sStyleMap[OBJ_LABEL_BLACK];
    sStyleMap[OBJ_LABEL_RED] =
        "QLabel {color:red;" + sStyleMap[OBJ_FONT1] + "}";
    sStyleMap[OBJ_LABEL_WHITE] =
        "QLabel {color:white;" + sStyleMap[OBJ_FONT1] + "}";
    sStyleMap[OBJ_LABEL_SMALL_BLACK] =
        "QLabel {color:black;" + sStyleMap[OBJ_FONT3] + "}";
    sStyleMap[OBJ_LABEL_SMALL] = sStyleMap[OBJ_LABEL_SMALL_BLACK];
    sStyleMap[OBJ_LABEL_SMALL_WHITE] =
        "QLabel {color:white;" + sStyleMap[OBJ_FONT3] + "}";
    sStyleMap[OBJ_LABEL_STATUS_OFFLINE] =
        "QLabel {color:red;" + sStyleMap[OBJ_FONT1] + "}";
    sStyleMap[OBJ_LABEL_STATUS_ONLINE_NODATA] =
        "QLabel {color:yellow;" + sStyleMap[OBJ_FONT1] + "}";
    sStyleMap[OBJ_LABEL_TITLE] =
        "QLabel {color:white;" + sStyleMap[OBJ_FONT_TITLE] + "}";
    sStyleMap[OBJ_LABEL_LOGO] =
        "QLabel {color:black;background:rgba(0,0,0,0%)}";
    //theme-dependent styles are initialized in setTheme()

    sThemeMap[THEME_BLUE]   << QObject::tr("Blue")
                            << GETRGB(47, 117, 181, 1.0, 0.6, 0.1);
    sThemeMap[THEME_BROWN]  << QObject::tr("Brown")
                            << GETRGB(139, 69, 19, 1.0, 0.6, 0.1);
    sThemeMap[THEME_GRAY]   << QObject::tr("Gray")
                            << GETRGB(98, 98, 98, 1.0, 0.6, 0.1);
    sThemeMap[THEME_GREEN]  << QObject::tr("Green")
                            << GETRGB(0, 100, 0, 1.0, 0.6, 0.1);
    sThemeMap[THEME_ORANGE] << QObject::tr("Orange")
                            << GETRGB(255, 140, 0, 1.0, 0.6, 0.1);
    sThemeMap[THEME_PURPLE] << QObject::tr("Purple")
                            << GETRGB(75, 0, 130, 1.0, 0.6, 0.1);
    sThemeMap[THEME_RED]    << QObject::tr("Red")
                            << GETRGB(204, 0, 0, 1.0, 0.6, 0.1);
    sThemeMap[THEME_DARK]   << QObject::tr("Dark")
                            << GETRGB(40, 40, 40, 1.0, 0.6, 0.1);
}

QStringList Style::getThemeNames()
{
    QStringList l;
    ThemeMapT::const_iterator it = sThemeMap.constBegin();
    for (; it!=sThemeMap.constEnd(); ++it)
    {
        l << it.value().at(0);
    }
    return l;
}

bool Style::setTheme(int theme)
{
    if (theme == sTheme || theme < 0 || theme >= THEME_MAX)
        return false;
    sTheme = theme;
    QString bg((sTheme == THEME_DARK)? sStyleMap[OBJ_BACKGROUND_DARK]:
                                       "rgb(169,169,169)");
    QString bgSelect((sTheme == THEME_DARK)? "rgb(0,174,239)":
                                             getThemeColor(THEME_COLOR_IDX1));
    QString fontHl((sTheme == THEME_DARK)? "rgb(0,174,239)": "black");
    sStyleMap[OBJ_BACKGROUND] = bg;
    QString bgColor("background:");
    bgColor += bg + ";";
    QString fgColor("color:");
    fgColor += ((sTheme == THEME_DARK)? "white;": "black;");
    //pushbutton uses theme's first color
    sStyleMap[OBJ_PUSHBUTTON] =
        "QPushButton:!hover {"
        " min-width:80px; min-height:30px; border-radius:10px;"
        " padding-left:5px; padding-right:5px;"
        " background:qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "  stop:0 gray, stop:1 " + bg + ");" + fgColor + sStyleMap[OBJ_FONT1] +
        "}"
        "QPushButton:hover:!pressed {"
        " border-radius:10px; color:black;"
        " background:qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "  stop:0 white, stop:1 " + getThemeColor(THEME_COLOR_IDX1) + ")}"
        "QPushButton:pressed {"
        " border-radius:10px; color:white;"
        " background:qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "  stop:0 gray, stop:1 " + getThemeColor(THEME_COLOR_IDX1) + ")}"
        "QPushButton:disabled {"
        " border-radius:10px; color:rgb(128,128,128); background:transparent}";
    sStyleMap[OBJ_PUSHBUTTON_SIMPLE] =
        "QPushButton {border-radius:5px}"
        "QPushButton::hover:!pressed {background:" +
         getThemeColor(THEME_COLOR_IDX1) + "}"
        "QPushButton::pressed {background:" +
         getThemeColor(THEME_COLOR_IDX2) + "}";
    QString ss(fgColor + bgColor);
    QString ssf(ss + sStyleMap[OBJ_FONT1]);
    sStyleMap[OBJ_ABSTRACTITEMVIEW] =
        "QAbstractItemView {background-color:white;color:black;" +
         sStyleMap[OBJ_FONT1] + "}"
        "QAbstractItemView:selected {background: " + bgSelect + ";color:" +
         ((sTheme == THEME_DARK)? "black": "white") + "}";
    sStyleMap[OBJ_INPUTDIALOG] =
        "QInputDialog {" + ssf + "}";
    sStyleMap[OBJ_LABEL] =
        "QLabel {" + fgColor + sStyleMap[OBJ_FONT1] + "}";
    sStyleMap[OBJ_LABEL_SMALL] =
        "QLabel {" + sStyleMap[OBJ_FONT3] + "color:" + fontHl + "}";
    sStyleMap[OBJ_MESSAGEBOX] =
        "QMessageBox {" + ssf + "}";
    sStyleMap[OBJ_CHECKBOX] =
        "QCheckBox {background:transparent;" + fgColor + "}"
        "QCheckBox::indicator {height:20px;width:20px}"
        "QCheckBox::indicator:unchecked {"
        " image: url(:/Images/images/rc_checkbox_unchecked.png)}"
        "QCheckBox::indicator:unchecked:hover {"
        " image: url(:/Images/images/rc_checkbox_unchecked_focus.png)}"
        "QCheckBox::indicator:unchecked:disabled {"
        " image: url(:/Images/images/rc_checkbox_unchecked_disabled.png)}"
        "QCheckBox::indicator:checked {"
        " image: url(:/Images/images/rc_checkbox_checked_focus.png)}"
        "QCheckBox::indicator:checked:disabled {"
        " image: url(:/Images/images/rc_checkbox_checked_disabled.png)}";
    sStyleMap[OBJ_COMBOBOX] =
        "QComboBox {color:black;background:white;border-radius:6px;"
        " padding:3px 3px 3px 3px;" + sStyleMap[OBJ_FONT1] +
        "}" + "QComboBox::" +
        sStyleMap[OBJ_ABSTRACTITEMVIEW];
    //FRAME_TITLE uses theme's first color
    sStyleMap[OBJ_FRAME_TITLE] =
        "QFrame {border-top-right-radius:4px;border-top-left-radius:4px;"
        " background:" + getThemeColor(THEME_COLOR_IDX1) + "}"
        "QFrame QLabel {color:white;" + sStyleMap[OBJ_FONT2] + "}";
    //FRAME_TITLE2 uses theme's second color
    sStyleMap[OBJ_FRAME_TITLE2] =
        "QFrame {background:" + getThemeColor(THEME_COLOR_IDX2) + "}"
        "QFrame QLabel {color:white;" + sStyleMap[OBJ_FONT2] + "}";
    sStyleMap[OBJ_MENU_BASE] =
        "QMenu {" + bgColor + sStyleMap[OBJ_FONT2] + "}"
        "QMenu::icon {padding-left:30px;width:32px;height:32px}"
        "QMenu::item {padding:4px 24px 4px 28px;" + fgColor + "}"
        "QMenu::item:selected {background-color:" + bgSelect + ";color:" +
         ((sTheme == THEME_DARK)? "black": "white") +
        "}";
    sStyleMap[OBJ_MENU] =
        "QCheckBox {padding-left:14px;" + fgColor + sStyleMap[OBJ_FONT2] + "}" +
        sStyleMap[OBJ_MENU_BASE] +
        sStyleMap[OBJ_CHECKBOX];
    sStyleMap[OBJ_RADIOBUTTON] =
        "QRadioButton {" + ssf + "}"
        "QRadioButton::indicator {height:20px;width:20px}"
        "QRadioButton::indicator:unchecked {"
        " image: url(:/Images/images/rc_radio_unchecked.png)}"
        "QRadioButton::indicator:unchecked:hover {"
        " image: url(:/Images/images/rc_radio_unchecked_focus.png)}"
        "QRadioButton::indicator:unchecked:disabled {"
        " image: url(:/Images/images/rc_radio_unchecked_disabled.png)}"
        "QRadioButton::indicator:checked {"
        " image: url(:/Images/images/rc_radio_checked_focus.png)}"
        "QRadioButton::indicator:checked:disabled {"
        " image: url(:/Images/images/rc_radio_checked_disabled.png)}";
    sStyleMap[OBJ_SLIDER_BASE] =
        "QSlider::groove {background:white; border-radius:4px}"
        "QSlider::handle {"
        " background:qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 white,"
        "  stop:1 lightgray);"
        " border:1px solid gray; width:20px; height:20px; border-radius:4px}"
        "QSlider::handle:hover {"
        " background:qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 white,"
        "  stop:1 " + getThemeColor(THEME_COLOR_IDX2) + ");"
        " border:1px solid gray; border-radius:4px}";
    sStyleMap[OBJ_SLIDER_HORIZONTAL] =
        "QSlider::add-page {"
        " background:white; border:1px solid gray; border-radius:4px}"
        "QSlider::sub-page {"
        " background:qlineargradient(x1:0, y1:0.2, x2:1, y2:1,"
        "  stop:0 " + getThemeColor(THEME_COLOR_IDX1) + ", stop:1 " +
        getThemeColor(THEME_COLOR_IDX3) + ");"
        " border:1px solid gray; border-radius:4px}" +
        sStyleMap[OBJ_SLIDER_BASE];
    sStyleMap[OBJ_SLIDER_VERTICAL] =
        "QSlider::add-page {"
        " background:qlineargradient(x1:0, y1:0.2, x2:1, y2:1,"
        "  stop:0 " + getThemeColor(THEME_COLOR_IDX1) + ", stop:1 " +
        getThemeColor(THEME_COLOR_IDX3) + ");"
        " border:1px solid gray; border-radius:4px}"
        "QSlider::sub-page {"
        " background:white; border:1px solid gray; border-radius:4px}" +
        sStyleMap[OBJ_SLIDER_BASE];
    sStyleMap[OBJ_SLIDER_ZOOM] =
        "QSlider::handle {background:" +
         getThemeColor(Style::THEME_COLOR_IDX2) + "}"
        "QSlider::handle:hover {background:" +
         getThemeColor(Style::THEME_COLOR_IDX3) + "}"
        "QSlider::handle:pressed {background:black}";
    sStyleMap[OBJ_TOOLBUTTON_BASE] =
        "QToolButton {border-radius:5px}"
        "QToolButton::hover:!pressed {background:" +
         getThemeColor(THEME_COLOR_IDX1) + "}"
        "QToolButton::pressed {background:" +
         getThemeColor(THEME_COLOR_IDX2) + "}";
    sStyleMap[OBJ_TOOLBUTTON] =
        "QToolButton {color:white;" + sStyleMap[OBJ_FONT1] + "}"
        "QToolButton::checked {color:rgb(0,174,239)}" +
        sStyleMap[OBJ_TOOLBUTTON_BASE];
    sStyleMap[OBJ_TOOLBUTTON_THEME_BG] =
        "QToolButton {color:white;background:" +
         getThemeColor(THEME_COLOR_IDX1) + ";" + sStyleMap[OBJ_FONT1] + "}" +
        sStyleMap[OBJ_TOOLBUTTON_BASE];
    sStyleMap[OBJ_TOOLBUTTON_HIGHLIGHT] =
        "QToolButton {" + COLOR_HIGHLIGHT + sStyleMap[OBJ_FONT2B] + "}" +
        sStyleMap[OBJ_TOOLBUTTON_BASE];
    sStyleMap[OBJ_CALLBUTTON] =
        "QToolButton {" + ssf + "}" + sStyleMap[OBJ_TOOLBUTTON_BASE];
    sStyleMap[OBJ_RESOURCEBUTTON] =
        "QToolButton {color:" + fontHl + ";" + sStyleMap[OBJ_FONT1] + bgColor +
        "}" + sStyleMap[OBJ_TOOLBUTTON_BASE];
    sStyleMap[OBJ_TREEVIEW] =
        "QTreeView::branch:!has-children:has-siblings:adjoins-item {"
        " border-image: url(:/Images/images/rc_branch_more.png) 0}"
        "QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
        " border-image: url(:/Images/images/rc_branch_end.png) 0}"
        "QTreeView::branch:closed:has-children:!has-siblings,"
        "QTreeView::branch:closed:has-children:has-siblings {"
        " image: url(:/Images/images/rc_branch_closed.png)}"
        "QTreeView::branch:closed:hover:has-children:!has-siblings,"
        "QTreeView::branch:closed:hover:has-children:has-siblings {"
        " image: url(:/Images/images/rc_branch_closed_focus.png)}"
        "QTreeView::branch:open:has-children:!has-siblings,"
        "QTreeView::branch:open:has-children:has-siblings {"
        " image: url(:/Images/images/rc_branch_opened.png)}"
        "QTreeView::branch:open:hover:has-children:!has-siblings,"
        "QTreeView::branch:open:hover:has-children:has-siblings {"
        " image: url(:/Images/images/rc_branch_opened_focus.png)}";
    sStyleMap[OBJ_TREEVIEW_CHECKBOX] =
        "QTreeView::indicator {height:20px;width:20px}"
        "QTreeView::indicator:unchecked {"
        " image: url(:/Images/images/rc_checkbox_unchecked.png)}"
        "QTreeView::indicator:unchecked:hover {"
        " image: url(:/Images/images/rc_checkbox_unchecked_focus.png)}"
        "QTreeView::indicator:unchecked:disabled {"
        " image: url(:/Images/images/rc_checkbox_unchecked_disabled.png)}"
        "QTreeView::indicator:checked {"
        " image: url(:/Images/images/rc_checkbox_checked_focus.png)}"
        "QTreeView::indicator:checked:disabled {"
        " image: url(:/Images/images/rc_checkbox_checked_disabled.png)}";
    sStyleMap[OBJ_COMMON] =
        ".QWidget, QTabWidget, QFrame {" + bgColor + "}"
        "QFrame QListWidget{background:white;color:black;" +
         sStyleMap[OBJ_FONT1] + "}"
        "QCheckBox {" + fgColor + sStyleMap[OBJ_FONT1] + "}"
        "QDateEdit, QTimeEdit {color:black;background:white;" +
         sStyleMap[OBJ_FONT1] + "}"
        "QDialog, QListView, QListWidget, QMessageBox, QTreeView {" + ssf +
        "}"
        "QHeaderView::section {color:white;border:1px solid white;background:" +
         getThemeColor(THEME_COLOR_IDX2) + ";" + sStyleMap[OBJ_FONT1] + "}"
        "QLineEdit:read-only {background:rgb(236,236,236);" +
         sStyleMap[OBJ_FONT1] + "}"
        "QLineEdit, QTextEdit, QPlainTextEdit {border-radius:6px;"
        " color:black;background:white;" + sStyleMap[OBJ_FONT1] + "}"
        "QListView {border-radius:6px}"
        "QMenu::QCheckBox {" + fgColor + sStyleMap[OBJ_FONT1] + "}"
        "QGroupBox {color:white;" + sStyleMap[OBJ_FONT1] + "font-weight:bold;}"
        "QGroupBox#categoryBox, #searchByBox{border:0}"
        "QProgressBar::chunk {background:" +
         getThemeColor(THEME_COLOR_IDX2) + ";width:10px;margin:1px}"
        "QRadioButton {" + fgColor + sStyleMap[OBJ_FONT1] + "}"
        "QScrollArea, QScrollArea > QWidget > QWidget {" + ss + "}"
        "QScrollBar:vertical {background-color:rgb(128,128,128);"
        " border:1px solid grey;border-radius:4px;margin:16px 2px 16px 2px;"
        " width:16px}"
        "QScrollBar::handle:vertical {background-color:rgb(96,121,139);"
        " border:1px solid rgb(69,83,100);border-radius:4px;min-height:8px}"
        "QScrollBar::add-line:vertical {margin:3px 0px 3px 0px;height:12px;"
        " width:12px;border-image:url(:/Images/images/rc_arrow_down.png);"
        " subcontrol-position:bottom;subcontrol-origin:margin}"
        "QScrollBar::handle:vertical:hover {background-color:" + bgSelect + ";"
        " border-radius:4px;min-height:8px}"
        "QScrollBar::add-line:vertical:hover, QScrollBar::add-line:vertical:on"
        "{border-image:url(:/Images/images/rc_arrow_down_focus.png);"
        " height:12px;width:12px;subcontrol-position:bottom;"
        " subcontrol-origin:margin}"
        "QScrollBar::sub-line:vertical {margin:3px 0px 3px 0px;height:12px;"
        " width:12px;border-image:url(:/Images/images/rc_arrow_up.png);"
        " subcontrol-position:top;subcontrol-origin:margin}"
        "QScrollBar::sub-line:vertical:hover, QScrollBar::sub-line:vertical:on"
        "{border-image: url(:/Images/images/rc_arrow_up_focus.png);height:12px;"
        " width:12px;subcontrol-position:top;subcontrol-origin: margin}"
        "QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical"
        "{background:none}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical"
        "{background:none}"
        "QScrollBar:horizontal {background-color:rgb(128,128,128);"
        " border:1px solid grey;border-radius:4px;margin:2px 16px 2px 16px;"
        " height:16px}"
        "QScrollBar::handle:horizontal {background-color:rgb(96,121,139);"
        " border:1px solid rgb(69,83,100);border-radius:4px;min-width:8px}"
        "QScrollBar::add-line:horizontal {margin:0px 0px 0px 0px;height:12px;"
        " border-image:url(:/Images/images/rc_arrow_right.png);"
        " width:12px;subcontrol-position:right;subcontrol-origin:margin}"
        "QScrollBar::handle:horizontal:hover {background-color:" + bgSelect +
        " ;border-radius:4px;min-width:8px}"
        "QScrollBar::add-line:horizontal:hover, "
        "QScrollBar::add-line:horizontal:on {height:12px;width:12px;"
        " border-image:url(:/Images/images/rc_arrow_right_focus.png);"
        " subcontrol-position: right;subcontrol-origin: margin}"
        "QScrollBar::sub-line:horizontal {margin:0px 3px 0px 3px;height:12px;"
        " width:12px;border-image:url(:/Images/images/rc_arrow_left.png);"
        " subcontrol-position:left;subcontrol-origin:margin}"
        "QScrollBar::sub-line:horizontal:hover, "
        "QScrollBar::sub-line:horizontal:on {height:12px;width:12px;"
        " border-image:url(:/Images/images/rc_arrow_left_focus.png);"
        " subcontrol-position:left;subcontrol-origin:margin}"
        "QScrollBar::up-arrow:horizontal, QScrollBar::down-arrow:horizontal"
        "{background:none}"
        "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal"
        "{background:none}"
        "QSpinBox {color:black;background:white;" + sStyleMap[OBJ_FONT1] + "}"
        "QSplitter {background-color:" + sStyleMap[OBJ_BACKGROUND_PANEL] + "}"
        "QSplitter::handle {spacing:0px;padding:2px;margin:0px;"
        " background-color:" + sStyleMap[OBJ_BACKGROUND_PANEL] + ";"
        " border:0px solid " + sStyleMap[OBJ_BACKGROUND_PANEL] +
        "}"
        "QSplitter::handle:horizontal {width:15px;"
        " image: url(:/Images/images/rc_line_vertical.png)}"
        "QSplitter::handle:vertical {height:15px;"
        "image: url(:/Images/images/rc_line_horizontal.png)}"
        "QStatusBar {" + bgColor + "}"
        "QTabBar {" + bgColor + sStyleMap[OBJ_FONT2] + "}"
        "QTabBar::tab {" + fgColor + "background:rgb(196,196,195);"
        " border:1px solid rgb(196,196,195);border-top-left-radius:4px;"
        " border-top-right-radius:4px;margin-left:2px;padding-left:10px;"
        " padding-right:10px;padding-top:2px;padding-bottom:2px}"
        "QTabBar::tab:hover {" + bgColor + "}"
        "QTabBar::tab:selected {border-color:rgb(155,155,155);"
        " border-bottom:4px solid " + bgSelect + ";" + bgColor +
        "}"
        "QTabBar::tab:!selected {margin-top:2px}"
        "QTableView, QTableWidget {gridline-color:" + bg + ";" + ssf +
        " alternate-background-color:" +
         getThemeColor((sTheme == THEME_DARK)? THEME_COLOR_IDX2:
                                               THEME_COLOR_IDX3) +
        "}"
        "QTableView::item:selected:active {background-color:rgb(52,103,146)}"
        "QTableView::item:selected:!active {color:rgb(224,225,227);"
        " background-color:rgb(55,65,79)}"
        "QTableCornerButton:section {" + bgColor + "}"
        "QToolBar {" + bgColor + "}" +
        sStyleMap[OBJ_CHECKBOX] +
        sStyleMap[OBJ_COMBOBOX] +
        sStyleMap[OBJ_INPUTDIALOG] +
        sStyleMap[OBJ_LABEL] +
        sStyleMap[OBJ_MENU_BASE] +
        sStyleMap[OBJ_PUSHBUTTON] +
        sStyleMap[OBJ_RADIOBUTTON] +
        sStyleMap[OBJ_TOOLBUTTON];
    return true;
}

const QString &Style::getStyle(int type)
{
    if (type < 0 || type > OBJ_MAX)
        return sStyleMap[OBJ_MAX];
    return sStyleMap[type];
}

bool Style::isHighlighted(const QString &ss)
{
    return (ss.contains(COLOR_HIGHLIGHT));
}

const QString &Style::getThemeColor(int idx)
{
    if (idx < THEME_COLOR_IDX1 || idx >= sThemeMap[sTheme].size())
        idx = THEME_COLOR_IDX1;
    return sThemeMap[sTheme].at(idx);
}
