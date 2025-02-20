/**
 * Implementation for saving and printing document.
 * Does either of the following depending on the print type:
 * 1. Prepares the content in HTML format.
 * 2. Prints to the paint device such as QPrinter and QPdfWriter class.
 * 3. Writes content to Excel file.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: Document.cpp 1812 2024-02-14 03:07:20Z rosnin $
 * @author Zulzaidi Atan
 */
#include <assert.h>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QPdfWriter>
#include <QPrintPreviewDialog>
#include <QStandardItemModel>
#include <QTextDocument>

#include "CmnTypes.h"
#include "DateTimeDelegate.h"
#include "QtUtils.h"
#include "Style.h"
#include "Version.h"
#include "Document.h"

static const QString ICON_LOGO(":/Images/images/icon_main.png");

const QString Document::LINEBREAK("<br>");

QString Document::sSaveDir;

Document::Document(int            printType,
                   const QString &title,
                   const QString &desc,
                   int            type) :
mPrintType(printType), mTitle(title), mDesc(desc), mExcel(0)
{
    if (mPrintType == PRINTTYPE_EXCEL)
    {
        mExcel = new QXlsx::Document();
        //top-align all cells instead of default bottom
        mFmt.setVerticalAlignment(QXlsx::Format::AlignTop);
    }
    else
    {
        mDesc.replace("\n", LINEBREAK);
    }
    if (type != TYPE_TABLE)
    {
        mTbls.push_back(TableData(type));
        if (mPrintType == PRINTTYPE_EXCEL)
        {
            mExcel->addSheet(title);
            addExcelHeader(type);
        }
    }
}

Document::~Document()
{
    delete mExcel;
}

void Document::newTable(const QString &name)
{
    mTbls.push_back(TableData(name));
    if (mPrintType == PRINTTYPE_EXCEL)
    {
        mExcel->addSheet(name);
        addExcelHeader(TYPE_TABLE);
    }
}

void Document::addColumn(const QString &col)
{
    assert(!mTbls.empty());
    TableData &t(mTbls.back());
    if (mPrintType == PRINTTYPE_EXCEL)
    {
        QXlsx::Format fmt(mFmt);
        fmt.setFontBold(true);
        fmt.setFillPattern(QXlsx::Format::PatternSolid);
        fmt.setPatternBackgroundColor(Qt::darkGray);
        fmt.setFontColor(Qt::white);
        mExcel->write(t.excelData.row, ++t.excelData.col, col, fmt);
    }
    else
    {
        t.htmlData.cols.append("<th>").append(col).append("</th>");
    }
}

void Document::beginRow()
{
    assert(!mTbls.empty());
    TableData &t(mTbls.back());
    t.rowCount++;
    if (mPrintType == PRINTTYPE_EXCEL)
    {
        t.excelData.row++;
        t.excelData.col = 0;
        return;
    }
    if (t.rowCount % 2 == 0) //alternate row color
        t.htmlData.data.append(QString("<tr style=\"background-color:%1\">")
                           .arg(Style::getThemeColor(Style::THEME_COLOR_IDX3)));
    else
        t.htmlData.data.append("<tr>");
    if (t.type == TYPE_TABLE)
        t.htmlData.data.append(QString("<td>%1</td>").arg(t.rowCount));
}

void Document::endRow()
{
    if (mPrintType != PRINTTYPE_EXCEL)
    {
        assert(!mTbls.empty() && mTbls.back().rowCount > 0);
        mTbls.back().htmlData.data.append("</tr>");
    }
}

void Document::addItem(const QString &item)
{
    assert(!mTbls.empty() && mTbls.back().rowCount > 0);
    TableData &t(mTbls.back());
    if (mPrintType == PRINTTYPE_EXCEL)
        mExcel->write(t.excelData.row, ++t.excelData.col, item, mFmt);
    else
        t.htmlData.data.append("<td>").append(item).append("</td>");
}

void Document::addItemCentered(const QString &item)
{
    if (mPrintType != PRINTTYPE_EXCEL)
    {
        assert(!mTbls.empty() && mTbls.back().rowCount > 0);
        mTbls.back().htmlData.data.append("<td align=\"center\">").append(item)
                                  .append("</td>");
    }
}

Document &Document::addRow(const QString &hdr1,
                           const QString &val1,
                           bool           multi1,
                           const QString &hdr2,
                           const QString &val2,
                           bool           multi2)
{
    beginRow();
    TableData &t(mTbls.back());
    if (mPrintType == PRINTTYPE_EXCEL)
    {
        mFmt.setFontBold(true);
        mExcel->write(t.excelData.row, ++t.excelData.col, hdr1, mFmt);
        mFmt.setFontBold(false);
        writeExcel(t.excelData.row, ++t.excelData.col, val1, multi1);
        if (hdr2.isEmpty())
        {
            mExcel->mergeCells(QXlsx::CellRange(t.excelData.row,
                                                t.excelData.col,
                                                t.excelData.row,
                                                t.excelData.col + 2));
        }
        else
        {
            mFmt.setFontBold(true);
            mExcel->write(t.excelData.row, ++t.excelData.col, hdr2, mFmt);
            mFmt.setFontBold(false);
            writeExcel(t.excelData.row, ++t.excelData.col, val2, multi2);
        }
    }
    else
    {
        QString v1(val1);
        if (multi1)
            v1.replace('\n', LINEBREAK);
        QString v2(val2);
        if (multi2)
            v2.replace('\n', LINEBREAK);
        //on first row, set smaller width for column headers
        t.htmlData.data.append(QString("<td%1>%2</td>")
                               .arg((t.rowCount == 1)? " width=\"15%\"": "",
                                    hdr1));
        if (hdr2.isEmpty())
            t.htmlData.data.append(QString("<td colspan=\"3\">%1</td>").arg(v1));
        else
            t.htmlData.data.append(QString("<td>%1</td><td%2>%3</td><td>%4</td>")
                                   .arg(v1,
                                        (t.rowCount == 1)? " width=\"15%\"": "",
                                        hdr2, v2));
    }
    endRow();
    return *this;
}

void Document::addTable(const QTableWidget *tw,
                        const QString      &name,
                        int                 dtCol,
                        int                 tpCol,
                        int                 drCol)
{
    if (tw == 0)
    {
        assert("Invalid param in Document::addTable" == 0);
        return;
    }
    newTable(name);
    int colCount = tw->columnCount();
    int j = 0;
    for (; j<colCount; ++j)
    {
        if (!tw->isColumnHidden(j))
            addColumn(tw->horizontalHeaderItem(j)->text());
    }
    QString str;
    QTableWidgetItem *item;
    //note: not possible for tw to be modified elsewhere while this is running
    //because it holds up the main thread
    int type;
    int rowCount = tw->rowCount();
    int i = 0;
    for (; i<rowCount; ++i)
    {
        if (tw->isRowHidden(i))
            continue;
        beginRow();
        for (j=0; j<colCount; ++j)
        {
            if (tw->isColumnHidden(j))
                continue;
            if (j == tpCol || j == drCol)
            {
                type = qobject_cast<QLabel *>(tw->cellWidget(i, j))
                       ->objectName().toInt();
                if (mPrintType == PRINTTYPE_EXCEL)
                    addItem(QtUtils::getCommsText(type));
                else
                    addItemCentered("<img src=\"" +
                                    QtUtils::getCommsIconSrc(type) +
                                    "\" width=\"15\"/>");
                continue;
            }
            item = tw->item(i, j);
            if (item == 0)
            {
                addItem("");
            }
            else if (j == dtCol)
            {
                addItem(DateTimeDelegate::getDateTime(
                                                  item->data(Qt::DisplayRole)));
            }
            //item may have tooltip, usually for error info
            else if (item->toolTip().isEmpty())
            {
                addItem(item->text());
            }
            else
            {
                str = item->text();
                if (!str.isEmpty())
                    str.append(" ");
                if (mPrintType == PRINTTYPE_EXCEL)
                    addItem(str.append("(").append(item->toolTip()).append(")"));
                else
                    addItem(str.append("<i>(").append(item->toolTip())
                               .append(")</i>"));
            }
        }
        endRow();
    }
    endTable();
}

void Document::addTable(const QTableView *tv,
                        const QString    &name,
                        int               dtCol,
                        int               tpCol)
{
    if (tv == 0)
    {
        assert("Invalid param in Document::addTable" == 0);
        return;
    }
    QStandardItemModel *mdl = qobject_cast<QStandardItemModel *>(tv->model());
    if (mdl == 0)
        return;
    newTable(name);
    int colCount = mdl->columnCount();
    int j = 0;
    for (; j<colCount; ++j)
    {
        if (!tv->isColumnHidden(j))
            addColumn(mdl->horizontalHeaderItem(j)->text());
    }
    QString str;
    QStandardItem *item;
    //note: not possible for mdl to be modified elsewhere while this is running
    //because it holds up the main thread
    int rowCount = mdl->rowCount();
    int i = 0;
    for (; i<rowCount; ++i)
    {
        beginRow();
        for (j=0; j<colCount; ++j)
        {
            if (tv->isColumnHidden(j))
                continue;
            item = mdl->item(i, j);
            if (item == 0)
            {
                addItem("");
            }
            else if (j == dtCol)
            {
                addItem(DateTimeDelegate::getDateTime(
                                                  item->data(Qt::DisplayRole)));
            }
            else if (j == tpCol)
            {
                str = item->text();
                if (mPrintType == PRINTTYPE_EXCEL)
                    addItem(str);
                else
                    addItem("<img src=\"" +
                            QtUtils::getCommsIconSrc(
                                           (str == tr("SDS"))?
                                               CmnTypes::COMMS_MSG_SDS:
                                               (str == tr("MMS"))?
                                                   CmnTypes::COMMS_MSG_MMS:
                                                   CmnTypes::COMMS_MSG_STATUS) +
                            "\" width=\"15\"/> " + str);
            }
            //item may have tooltip, usually for error info
            else if (item->toolTip().isEmpty())
            {
                addItem(item->text());
            }
            else
            {
                str = item->text();
                if (!str.isEmpty())
                    str.append(" ");
                if (mPrintType == PRINTTYPE_EXCEL)
                    addItem(str.append(item->toolTip()));
                else
                    addItem(str.append("<i>(").append(item->toolTip())
                               .append(")</i>"));
            }
        }
        endRow();
    }
    endTable();
}

void Document::endTable()
{
    if (mPrintType == PRINTTYPE_EXCEL)
    {
        assert(!mTbls.empty());
        TableData &t(mTbls.back());
        if (!t.excelData.totalCell.isEmpty())
        {
            mFmt.setHorizontalAlignment(QXlsx::Format::AlignLeft);
            mExcel->write(t.excelData.totalCell, t.rowCount, mFmt);
            mFmt.setHorizontalAlignment(QXlsx::Format::AlignHGeneral);
        }
        t.excelData.resizeRange.setLastRow(t.excelData.row);
        t.excelData.resizeRange.setLastColumn((t.type == TYPE_TABLE)?
                                              t.excelData.col: 4);
        mExcel->autosizeColumnWidth(t.excelData.resizeRange);
        mExcel->selectSheet(mTitle);
    }
}

void Document::print(QWidget *parent, const QString &name)
{
    switch (mPrintType)
    {
        case PRINTTYPE_EXCEL:
            saveToExcel(parent, name);
            break;
        case PRINTTYPE_PDF:
            saveToPdf(parent, name);
            break;
        case PRINTTYPE_PREVIEW:
        default:
            printPreview(parent);
            break;
    }
}

QString Document::generateHtml()
{
    //header styles, logo & caption, title, description
    QString html(QString("<html><head><style>"
                         "th {text-align:left; padding:4px; color:white;"
                         "background-color:%1}"
                         "td {text-align:left; padding:4px;}</style></head>"
                         "<table><tr><td><img src=\"%2\" width=\"100\"></td>"
                         "<td valign=\"middle\"><h2>%3<h3>%4</h3></h2></td>"
                         "</tr></table>"
                         "<table width=\"100%\"><tr><td><h3>%5</h3></td>"
                         "<td valign=\"bottom\" align=\"right\">"
                         "<i>%6, %7</i></td></tr></table><body><p>%8</p>")
                 .arg(Style::getThemeColor(Style::THEME_COLOR_IDX1), ICON_LOGO,
                      Version::NWK_NAME, Version::APP_NAME, mTitle,
                      Version::APP_NAME_VERSION, QtUtils::getTimestamp(false),
                      mDesc));
    //table data
    for (auto const &t : mTbls)
    {
        if (t.type == TYPE_INCIDENT)
        {
            html.append("<table width=\"100%\">").append(t.htmlData.data)
                .append("</table><br>");
            continue;
        }
        //show number of rows first if no table name
        if (t.name.isEmpty())
            html.append(QString("<p>%1: %2</p>").arg(tr("Total"), t.rowCount));
        //use <thead> to repeat on pages
        html.append("<table width=\"100%\"><thead>");
        //table name cell spans 3 columns just to avoid expanding the first
        //(numbering) column and to keep it as one line
        if (!t.name.isEmpty())
            html.append(QString("<tr><th colspan=\"3\">%1 (%2)</th></tr>")
                        .arg(t.name, t.rowCount));
        html.append(QString("<tr>%1</tr></thead><tbody>%2</tbody>")
                    .arg(t.htmlData.cols, t.htmlData.data))
            .append("</table><br>");
    }
    return html;
}

void Document::addExcelHeader(int type)
{
    assert(mPrintType == PRINTTYPE_EXCEL && !mTbls.empty());
    TableData &t(mTbls.back());
    mFmt.setFontBold(true);
    mExcel->write("A1", Version::NWK_NAME, mFmt);
    mExcel->mergeCells("A1:E1");
    mExcel->write("A3", mTitle, mFmt);
    mFmt.setFontBold(false);
    mExcel->mergeCells("A3:E3");
    mFmt.setFontItalic(true);
    mExcel->write("A2",
                  Version::APP_NAME_VERSION + ", " +
                      QtUtils::getTimestamp(false),
                  mFmt);
    mFmt.setFontItalic(false);
    mExcel->mergeCells("A2:E2");
    if (type != TYPE_TABLE)
    {
        t.excelData.row = 4; //starting row to write record
    }
    else if (mDesc.isEmpty())
    {
        t.excelData.totalCell = "A5";
        t.excelData.row = 6; //starting row to write table data
    }
    else
    {
        mFmt.setFontBold(true);
        writeExcel(4, 1, mDesc, true);
        mFmt.setFontBold(false);
        mExcel->mergeCells("A4:E4");
        t.excelData.totalCell = "A6";
        t.excelData.row = 7;
    }
    t.excelData.resizeRange.setFirstRow(t.excelData.row);
    t.excelData.resizeRange.setFirstColumn(1);
}

inline void Document::writeExcel(int            row,
                                 int            col,
                                 const QString &val,
                                 bool           multi)
{
    assert(mPrintType == PRINTTYPE_EXCEL);
    if (multi)
    {
        int n = val.count('\n');
        if (n != 0)
        {
            mFmt.setTextWrap(true);
            mExcel->write(row, col, val, mFmt);
            mFmt.setTextWrap(false);
            mExcel->setRowHeight(row, (n + 1) * mExcel->rowHeight(row));
            return;
        }
    }
    mExcel->write(row, col, val, mFmt);
}

void Document::saveToPdf(QWidget *parent, const QString &name)
{
    QString fn(sSaveDir);
    fn.append(name)
      .append(QDateTime::currentDateTime().toString("-yyMMddhhmm"));
    fn = QFileDialog::getSaveFileName(parent, tr("Export to PDF"), fn,
                                      "PDF (*.pdf)");
    if (fn.isEmpty())
        return;
    sSaveDir = QFileInfo(fn).absolutePath().append("/");
    if (!fn.endsWith(".pdf"))
        fn.append(".pdf");
    QPdfWriter pdfWriter(fn);
    pdfWriter.setCreator(Version::APP_NAME_VERSION);
    pdfWriter.setPageSize(QPdfWriter::A4);
    QTextDocument doc;
    doc.setHtml(generateHtml());
    doc.print(&pdfWriter);
}

void Document::saveToExcel(QWidget *parent, const QString &name)
{
    QString fn(sSaveDir);
    fn.append(name)
      .append(QDateTime::currentDateTime().toString("-yyMMddhhmm"));
    fn = QFileDialog::getSaveFileName(parent, tr("Export to Excel"), fn,
                                      "Excel Workbook (*.xlsx)");
    if (fn.isEmpty())
        return;
    sSaveDir = QFileInfo(fn).absolutePath().append("/");
    if (!fn.endsWith(".xlsx"))
        fn.append(".xlsx");
    if (!mExcel->saveAs(fn))
        QMessageBox::critical(parent, tr("Export Error"),
                              tr("Failed to export to Excel file"));
}

void Document::printPreview(QWidget *parent)
{
    QPrinter p;
    p.setPageSize(QPrinter::A4);
    QPrintPreviewDialog d(&p, parent, parent->windowFlags());
    //just to prevent error:
    //QWindowsWindow::setGeometry: Unable to set geometry 148x30...
    d.setMinimumHeight(300);
    connect(&d, &QPrintPreviewDialog::paintRequested, this,
            [this](QPrinter *p)
            {
                //print document to a paint device
                QTextDocument doc;
                doc.setHtml(generateHtml());
                doc.print(p);
            });
    d.exec();
}
