/**
 * Dialog implementation to display and edit POI details.
 *
 * Copyright (C) Sapura Secured Technologies, 2014-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisMenuUserPoi.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Muhd Hashim Wahab
 */
#include <QIcon>
#include <QMessageBox>
#include <QTimer>
#include <assert.h>

#include "DbInt.h"
#include "GisPoint.h"
#include "Style.h"
#include "ui_GisMenuUserPoi.h"
#include "GisMenuUserPoi.h"

using namespace std;

QStringListModel GisMenuUserPoi::sCatModel;

GisMenuUserPoi::GisMenuUserPoi(const Props::ValueMapT &props,
                               const QString          &userName,
                               QWidget                *parent) :
QDialog(parent), ui(new Ui::GisMenuUserPoi), mUserName(userName.toStdString()),
mProps(props)
{
    if (mProps.count(Props::FLD_KEY) == 0)
    {
        assert("Bad param in GisMenuUserPoi::GisMenuUserPoi" == 0);
        return;
    }
    ui->setupUi(this);
    setWindowOpacity(0.90);
    setTheme();
    ui->cmbCategory->setModel(&sCatModel);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);
    setAttribute(Qt::WA_DeleteOnClose);
    //load attribute values into UI
    ui->txtAddress
      ->setText(
           QString::fromStdString(Props::get(mProps, Props::FLD_USERPOI_ADDR)));
    ui->txtDescription
      ->setText(
           QString::fromStdString(Props::get(mProps, Props::FLD_USERPOI_DESC)));
    string name(Props::get(mProps, Props::FLD_USERPOI_NAME));
    ui->txtName->setText(QString::fromStdString(name));
    //show short name only if different from name
    string value(Props::get(mProps, Props::FLD_USERPOI_SHORTNAME));
    if (value != name)
    {
        ui->txtShortName->setText(QString::fromStdString(value));
        ui->poiLbl->setText(tr("POI ") + ui->txtShortName->text());
    }
    if (Props::get<int>(mProps, Props::FLD_KEY) < 0)
        setWindowTitle(tr("POI"));
    else
        setWindowTitle(ui->poiLbl->text());
    value = Props::get(mProps, Props::FLD_USERPOI_CATEGORY);
    if (!value.empty())
        ui->cmbCategory->setCurrentText(QString::fromStdString(value));
    ui->txtLat
      ->setText(QString::fromStdString(Props::get(mProps, Props::FLD_LAT)));
    ui->txtLon
      ->setText(QString::fromStdString(Props::get(mProps, Props::FLD_LON)));
    ui->txtOwner
      ->setText(QString::fromStdString(Props::get(mProps, Props::FLD_OWNER)));
    ui->txtOwner->setReadOnly(true);
    if (isOwner())
    {
        ui->publicChk
          ->setChecked(Props::get<bool>(mProps, Props::FLD_IS_PUBLIC));
        if (Props::get<int>(mProps, Props::FLD_KEY) < 0)
            ui->deleteBtn->hide(); //no Delete for creation
        //ui->txtName->setFocus() directly does not work here
        if (ui->txtName->text().trimmed().isEmpty())
            QTimer::singleShot(0, ui->txtName, SLOT(setFocus()));
    }
    else
    {
        ui->txtAddress->setReadOnly(true);
        ui->txtDescription->setReadOnly(true);
        ui->txtName->setReadOnly(true);
        ui->txtShortName->setReadOnly(true);
        ui->txtLat->setReadOnly(true);
        ui->txtLon->setReadOnly(true);
        ui->cmbCategory->setEnabled(false);
        ui->deleteBtn->hide();
        ui->cancelBtn->hide();
        ui->publicChk->hide();
        ui->publicLbl->hide();
    }
    connect(ui->deleteBtn, &QPushButton::clicked, this,
            [this]
            {
                //delete POI after confirmation
                auto *mb = new QMessageBox(QMessageBox::Question,
                                       tr("Delete POI"),
                                       tr("Are you sure to delete '%1'?")
                                           .arg(ui->txtName->text()),
                                       QMessageBox::Yes | QMessageBox::No,
                                       dynamic_cast<QWidget *>(this->parent()));
                mb->setAttribute(Qt::WA_DeleteOnClose);
                mb->setWindowIcon(QIcon(*(ui->poiIconLbl->pixmap())));
                mb->setDefaultButton(QMessageBox::No);
                if (mb->exec() == QMessageBox::Yes)
                {
                    emit poiDelete(Props::get<int>(mProps, Props::FLD_KEY));
                    close();
                }
            });
    connect(ui->cancelBtn, &QPushButton::clicked, this,
            [this]
            {
                //cancel POI creation/modification
                emit dismissed(0);
                close();
            });
    connect(ui->okBtn, &QPushButton::clicked, this,
            [this]
            {
                if (ui->cancelBtn->isHidden())
                {
                    //not editing
                    ui->cancelBtn->click();
                    return;
                }
                //accept all values
                auto *p = dynamic_cast<QWidget *>(this->parent());
                QString name(ui->txtName->text().trimmed());
                QString lat(ui->txtLat->text().trimmed());
                QString lon(ui->txtLon->text().trimmed());
                if (name.isEmpty() || lat.isEmpty() || lon.isEmpty())
                {
                    auto *mb = new QMessageBox(QMessageBox::Critical,
                                               tr("POI Error"),
                                               tr("You must specify the %1, %2 "
                                                  "and %3.")
                                                   .arg(ui->nameLbl->text(),
                                                        ui->latLbl->text(),
                                                        ui->lonLbl->text()),
                                               QMessageBox::Ok, p);
                    mb->setAttribute(Qt::WA_DeleteOnClose);
                    mb->setWindowIcon(QIcon(*(ui->poiIconLbl->pixmap())));
                    mb->exec();
                    ui->txtLat->setText(QString::fromStdString(mLat));
                    ui->txtLon->setText(QString::fromStdString(mLon));
                    if (name.isEmpty())
                        ui->txtName->setFocus();
                    else if (lat.isEmpty())
                        ui->txtLat->setFocus();
                    else
                        ui->txtLon->setFocus();
                    return;
                }
                lat.append(",").append(lon);
                QPointF pt;
                if (GisPoint::COORD_VALID !=
                    GisPoint::checkCoords(p, tr("POI Error"), lat, true, pt))
                {
                    ui->txtLat->setText(QString::fromStdString(mLat));
                    ui->txtLon->setText(QString::fromStdString(mLon));
                    return;
                }
                string newVal(ui->txtShortName->text().trimmed().toStdString());
                if (newVal.empty())
                {
                    newVal = name.toStdString(); //copy name for display
                }
                else if (newVal.size() >= name.size())
                {
                    auto *mb = new QMessageBox(QMessageBox::Critical,
                                               tr("POI Error"),
                                               tr("%1 must be shorter than %2.")
                                                   .arg(ui->shortnameLbl->text(),
                                                        ui->nameLbl->text()),
                                               QMessageBox::Ok, p);
                    mb->setAttribute(Qt::WA_DeleteOnClose);
                    mb->setWindowIcon(QIcon(*(ui->poiIconLbl->pixmap())));
                    mb->exec();
                    ui->txtShortName->setFocus();
                    return;
                }
                //this is to be deleted by the end consumer
                auto *pr = new Props::ValueMapT;
                Props::set(*pr, Props::FLD_USERPOI_SHORTNAME, newVal);
                Props::set(*pr, Props::FLD_USERPOI_ADDR,
                         ui->txtAddress->toPlainText().trimmed().toStdString());
                Props::set(*pr, Props::FLD_USERPOI_DESC,
                     ui->txtDescription->toPlainText().trimmed().toStdString());
                Props::set(*pr, Props::FLD_USERPOI_NAME, name.toStdString());
                Props::set(*pr, Props::FLD_USERPOI_CATEGORY,
                           ui->cmbCategory->currentText().toStdString());
                Props::set(*pr, Props::FLD_LAT, GisPoint::getCoord(pt.y()));
                Props::set(*pr, Props::FLD_LON, GisPoint::getCoord(pt.x()));
                Props::set(*pr, Props::FLD_OWNER,
                           ui->txtOwner->text().trimmed().toStdString());
                Props::set(*pr, Props::FLD_IS_PUBLIC,
                           ui->publicChk->isChecked());
                Props::set(*pr, Props::FLD_KEY,
                           Props::get<int>(mProps, Props::FLD_KEY));
                emit dismissed(pr);
                close();
            });
}

GisMenuUserPoi::~GisMenuUserPoi()
{
    delete ui;
}

void GisMenuUserPoi::setTheme()
{
    ui->titleFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
}

bool GisMenuUserPoi::isOwner()
{
    return (Props::get(mProps, Props::FLD_OWNER) == mUserName);
}

void GisMenuUserPoi::setCategories()
{
    string s(DbInt::instance().getPoiCategories());
    if (!s.empty())
        sCatModel.setStringList(QString::fromStdString(s).split(","));
}

void GisMenuUserPoi::onPositionChanged(double lat, double lon)
{
    mLat = GisPoint::getCoord(lat);
    mLon = GisPoint::getCoord(lon);
    ui->txtLat->setText(QString::fromStdString(mLat));
    ui->txtLon->setText(QString::fromStdString(mLon));
}
