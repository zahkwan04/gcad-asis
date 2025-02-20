/**
 * Class for generating a document that can be printed or saved to a file.
 *
 * The document contains:
 *   -a header with a logo (not in Excel) and title,
 *   -a description, if provided,
 *   -one or more tables (in separate worksheets in Excel).
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Document.h 1812 2024-02-14 03:07:20Z rosnin $
 * @author Zulzaidi Atan
 */
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <vector>
#include <QObject>
#include <QPrinter>
#include <QTableView>
#include <QTableWidget>
#include <QXlsx/xlsxdocument.h>

class Document : public QObject
{
    Q_OBJECT

public:
    enum ePrintType
    {
        PRINTTYPE_EXCEL,
        PRINTTYPE_PDF,
        PRINTTYPE_PREVIEW
    };

    enum eType
    {
        TYPE_INCIDENT,  //custom format
        TYPE_TABLE      //common format
    };

    static const QString LINEBREAK;

    /**
     * Constructor.
     *
     * @param[in] printType ePrintType.
     * @param[in] title     The title.
     * @param[in] desc      The description.
     * @param[in] type      The first table type - eType.
     *                      Creates the first table if this is not TYPE_TABLE.
     */
    Document(int            printType,
             const QString &title,
             const QString &desc = "",
             int            type = TYPE_TABLE);

    ~Document();

    /**
     * Begins a new table.
     *
     * @param[in] name The table name.
     */
    void newTable(const QString &name = "");

    /**
     * Adds a column to the current table.
     *
     * @param[in] col The column name.
     */
    void addColumn(const QString &col);

    /**
     * Begins a new table row.
     */
    void beginRow();

    /**
     * Ends a table row.
     */
    void endRow();

    /**
     * Adds a table item to the current row.
     *
     * @param[in] item The item.
     */
    void addItem(const QString &item);

    /**
     * Adds a table item to the current row, centered in the cell.
     *
     * @param[in] item The item.
     */
    void addItemCentered(const QString &item);

    /**
     * Adds a table row containing 4 columns with either 1 or 2 header-value
     * pairs. If only 1 pair, val1 column spans to the table end.
     *
     * @param[in] hdr1   Header 1.
     * @param[in] val1   Value 1.
     * @param[in] multi1 true if value 1 may contain multiple lines.
     * @param[in] hdr2   Header 2.
     * @param[in] val2   Value 2.
     * @param[in] multi2 true if value 2 may contain multiple lines.
     * @return this object.
     */
    Document &addRow(const QString &hdr1,
                     const QString &val1,
                     bool           multi1 = false,
                     const QString &hdr2   = "",
                     const QString &val2   = "",
                     bool           multi2 = false);

    /**
     * Adds and fills up a table.
     *
     * @param[in] tw    The data source.
     * @param[in] name  The table name.
     * @param[in] dtCol Column index that contains date and time.
     *                  Negative means none.
     * @param[in] tpCol Column index that contains message type.
     * @param[in] drCol Column index that contains call/message direction type.
     */
    void addTable(const QTableWidget *tw,
                  const QString      &name,
                  int                 dtCol = -1,
                  int                 tpCol = -1,
                  int                 drCol = -1);

    /**
     * Adds and fills up a table.
     *
     * @param[in] tv    The data source.
     * @param[in] name  The table name.
     * @param[in] dtCol Column index that contains date and time.
     *                  Negative means none.
     * @param[in] tpCol Column index that contains message type.
     */
    void addTable(const QTableView *tv,
                  const QString    &name,
                  int               dtCol = -1,
                  int               tpCol = -1);

    /**
     * Ends a table.
     */
    void endTable();

    bool empty() { return mTbls.empty(); }

    /**
     * Either shows print preview or saves to file.
     *
     * @param[in] parent The parent widget.
     * @param[in] name   The suggested filename, if any. Will be appended with
     *                   timestamp in the form "-yyMMddhhmm".
     */
    void print(QWidget *parent, const QString &name = "");

private:
    struct ExcelData
    {
        int              col = 0;     //1-based current column
        int              row = 0;     //1-based current row
        QString          totalCell;   //cell reference to write row count
        QXlsx::CellRange resizeRange; //range to set auto size column width
    };

    struct HtmlData
    {
        QString cols{"<th width=\"5%\">#</th>"};
        QString data;
    };

    struct TableData
    {
        TableData() {}
        TableData(int tp) : type(tp) {}
        TableData(const QString &nm) : name(nm) {}

        int       type     = TYPE_TABLE;
        int       rowCount = 0;
        QString   name;
        HtmlData  htmlData;
        ExcelData excelData;
    };

    int                     mPrintType;
    QString                 mTitle;
    QString                 mDesc;
    std::vector<TableData>  mTbls;
    QXlsx::Format           mFmt;
    QXlsx::Document        *mExcel;

    static QString sSaveDir;

    /**
     * Generates HTML formatted text.
     *
     * @return The text.
     */
    QString generateHtml();

    /**
     * Adds header for Excel file.
     *
     * @param[in] type The table type. See eType.
     */
    void addExcelHeader(int type);

    /**
     * Writes a value into a cell. For use when the value may be multi-line.
     *
     * @param[in] row   The row.
     * @param[in] col   The column.
     * @param[in] val   The value.
     * @param[in] multi true to check for and handle multi-line value.
     */
    void writeExcel(int row, int col, const QString &val, bool multi = false);

    /**
     * Saves to a PDF file.
     *
     * @param[in] parent The parent widget.
     * @param[in] name   See print().
     */
    void saveToPdf(QWidget *parent, const QString &name);

    /**
     * Saves to an Excel file.
     *
     * @param[in] parent The parent widget.
     * @param[in] name   See print().
     */
    void saveToExcel(QWidget *parent, const QString &name);

    /**
     * Shows print preview dialog.
     *
     * @param[in] parent The parent widget.
     */
    void printPreview(QWidget *parent);
};
#endif //DOCUMENT_H
