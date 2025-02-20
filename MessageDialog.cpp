/**
 * Custom QDialog implementation.
 * It contains only 3 main components:
 * - text label to display the message header,
 * - vertical splitter to display any widgets,
 * - OK button.
 * This allows any type of data to be displayed.
 * Each instance has 3 main UI components to be specified:
 * - the window title,
 * - the message header,
 * - message text or other custom data to be displayed in the splitter.
 * Constructor with message text parameter adds the text inside a QTextEdit
 * within the splitter.
 * Other custom data shall be added through addWidget() either directly or via
 * an appropriate setData() that creates the widget.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2024. All Rights Reserved.
 *
 * @file
 * @version $Id: MessageDialog.cpp 1826 2024-03-18 01:17:01Z rosnin $
 * @author Mohd Rozaimi
 * @author Mohd Rashid
 */
#include <QHeaderView>
#include <QPalette>
#include <QScrollBar>
#include <QTableWidget>
#include <QTextEdit>

#include "MessageDialog.h"
#include "ui_MessageDialog.h"

MessageDialog::MessageDialog(const QString &title,
                             const QString &header,
                             const QIcon   &icon,
                             bool           deleteOnClose,
                             QWidget       *parent) :
QDialog(parent), ui(new Ui::MessageDialog), mLcdNum(0), mTime(0), mTimer(0)
{
    init(title, header, icon, deleteOnClose);
}

MessageDialog::MessageDialog(const QString &title,
                             const QString &header,
                             const QString &text,
                             const QIcon   &icon,
                             bool           deleteOnClose,
                             QWidget       *parent) :
QDialog(parent), ui(new Ui::MessageDialog), mLcdNum(0), mTime(0), mTimer(0)
{
    init(title, header, icon, deleteOnClose);
    if (!text.isEmpty())
    {
        auto *te = new QTextEdit();
        te->setReadOnly(true);
        te->setText(text);
        ui->splitter->addWidget(te);
    }
    adjustSize();
}

MessageDialog::~MessageDialog()
{
    delete mTimer;
    delete mTime;
    delete mLcdNum;
    delete ui;
}

void MessageDialog::addWidget(QWidget *w)
{
    ui->splitter->addWidget(w);
}

void MessageDialog::setData(const MessageDialog::TableDataT &data)
{
    if (data.isEmpty())
        return;
    auto it = data.begin();
    //number of columns is based on header columns
    //number of rows is data size minus the header row
    int cols = it->size();
    auto *tw = new QTableWidget(data.size() - 1, cols, this);
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setAlternatingRowColors(true);
    tw->setHorizontalHeaderLabels(*it);
    tw->horizontalHeader()->setStretchLastSection(true);
    tw->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    int row = 0;
    int col;
    QStringList::const_iterator sit;
    for (++it; it!=data.end(); ++it, ++row)
    {
        for (sit=(*it).begin(), col=0; sit!=(*it).end() && col<cols;
             ++sit, ++col)
        {
            tw->setItem(row, col, new QTableWidgetItem(*sit));
        }
    }
    addWidget(tw);
    int w = tw->verticalHeader()->width();
    for (--cols; cols>=0; --cols)
    {
        w += tw->horizontalHeader()->sectionSize(cols);
    }
    if (tw->verticalScrollBar() != 0)
        w += tw->verticalScrollBar()->width();
    tw->setMinimumWidth(w);
    adjustSize();
}

void MessageDialog::setData(int timerMinutes, const QString &okBtnText)
{
    ui->okButton->setText(okBtnText);
    mLcdNum = new QLCDNumber(this);
    mLcdNum->setMinimumSize(350, 150);
    addWidget(mLcdNum);
    mTime = new QTime(0, timerMinutes, 0);
    mLcdNum->display(mTime->toString());
    QPalette p(mLcdNum->palette());
    p.setColor(QPalette::WindowText, Qt::red);
    mLcdNum->setPalette(p);
    mTimer = new QTimer();
    connect(mTimer, &QTimer::timeout, this,
            [this]
            {
                //update timer dialog
                *mTime = mTime->addSecs(-1);
                if (mTime->hour() == 0 && mTime->minute() == 0 &&
                    mTime->second() == 0)
                {
                    mTimer->stop();
                    ui->okButton->click();
                }
                else
                {
                    mLcdNum->display(mTime->toString());
                }
            });
    mTimer->start(1000);
}

void MessageDialog::showNoOk(QWidget *widget)
{
    if (widget != 0)
        addWidget(widget);
    ui->okButton->hide();
    adjustSize();
    show();
}

MessageDialog *MessageDialog::showMessage(const QString &title,
                                          const QString &header,
                                          const QString &text,
                                          const QIcon   &icon,
                                          bool           deleteOnClose,
                                          QWidget       *parent)
{
    auto *md = new MessageDialog(title, header, text, icon, deleteOnClose,
                                 parent);
    md->show();
    return md;
}

MessageDialog *MessageDialog::showMessage(const QString    &title,
                                          const QString    &header,
                                          const TableDataT &data,
                                          const QIcon      &icon,
                                          bool              deleteOnClose,
                                          QWidget          *parent)
{
    auto *md = new MessageDialog(title, header, icon, deleteOnClose, parent);
    md->setData(data);
    md->show();
    return md;
}

MessageDialog *MessageDialog::showTimer(const QString &title,
                                        const QString &header,
                                        const QString &okBtnText,
                                        const QIcon   &icon,
                                        int            minutes,
                                        QWidget       *parent)
{
    auto *md = new MessageDialog(title, header, icon, true, parent);
    md->setData(minutes, okBtnText);
    md->show();
    return md;
}

void MessageDialog::showMessage(const QString &title,
                                const QString &header,
                                const QString &text,
                                const QIcon   &icon,
                                QWidget       *parent)
{
    (new MessageDialog(title, header, text, icon, true, parent))->show();
}

void MessageDialog::showStdMessage(const QString     &title,
                                   const QString     &text,
                                   QMessageBox::Icon  icon,
                                   bool               modal,
                                   QWidget           *parent)
{
    auto *mb = new QMessageBox(icon, title, text, QMessageBox::Ok, parent);
    mb->setAttribute(Qt::WA_DeleteOnClose);
    mb->setModal(modal);
    if (modal)
        mb->exec();
    else
        mb->show();
}

void MessageDialog::init(const QString &title,
                         const QString &header,
                         const QIcon   &icon,
                         bool           deleteOnClose)
{
    ui->setupUi(this);
    connect(ui->okButton, &QPushButton::clicked, this,
            [this] { done(QDialog::Accepted); });
    //remove title bar help button and disable close button
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
    setModal(false);
    if (deleteOnClose)
        setAttribute(Qt::WA_DeleteOnClose);
    setWindowIcon(icon);
    setWindowTitle(title);
    ui->textLabel->setText(header);
}
