/**
 * The Map Legend implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2016-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisLegend.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Mohd Fashan Abdul Munir
 */
#include <assert.h>
#include <map>
#include <QDomDocument>

#include "Style.h"
#include "GisLegend.h"
#include "ui_GisLegend.h"

static const QString XML_FILE(":/Qml/qml/Legend.xml");

GisLegend::GisLegend(Logger *logger, GisCanvas *canvas, QWidget *parent) :
QWidget(parent), ui(new Ui::GisLegend), mCanvas(canvas), mLogger(logger)
{
    ui->setupUi(this);
    setWindowTitle(parentWidget()->windowTitle()); //for use in connect() below
    connect(ui->floatButton, &QToolButton::clicked, this,
            [this]
            {
                static QSize dlgSize; //to preserve size between floats
                if (ui->floatButton->isChecked())
                {
                    //move self from parent into new floating dialog
                    auto *layout = new QVBoxLayout();
                    layout->setMargin(0);
                    layout->addWidget(this);
                    auto *dlg = new QDialog(parentWidget());
                    dlg->setWindowTitle(windowTitle());
                    //remove title bar help button and disable close button
                    dlg->setWindowFlags(dlg->windowFlags() &
                                        ~Qt::WindowContextHelpButtonHint &
                                        ~Qt::WindowCloseButtonHint);
                    dlg->setLayout(layout);
                    if (dlgSize.isValid())
                        dlg->resize(dlgSize);
                    else
                        dlg->resize(width(), height());
                    dlg->show();
                    ui->floatButton->setToolTip(tr("Dock"));
                    if (!ui->toggleBtn->isChecked())
                        ui->toggleBtn->click();
                }
                else
                {
                    auto *dlg = qobject_cast<QDialog *>(parentWidget());
                    if (dlg != 0)
                    {
                        //dock back into parent, which will delete dialog
                        dlgSize = dlg->size();
                        ui->floatButton->setToolTip(tr("Float"));
                        emit dock(dlg);
                    }
                }
            });
    mSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum,
                              QSizePolicy::Expanding);
    connect(ui->toggleBtn, &QToolButton::clicked, this,
            [this]
            {
                bool visible = ui->toggleBtn->isChecked();
                ui->frame->setVisible(visible);
                if (visible)
                    ui->verticalLayout->removeItem(mSpacer);
                else
                    ui->verticalLayout->addSpacerItem(mSpacer);
            });
    ui->toggleBtn->click();
    connect(ui->expandBtn, &QPushButton::clicked, this,
            [this]
            {
                if (ui->expandBtn->isChecked())
                {
                    ui->treeView->expandAll();
                    ui->expandBtn->setToolTip(tr("Collapse all"));
                }
                else
                {
                    ui->treeView->collapseAll();
                    ui->expandBtn->setToolTip(tr("Expand all"));
                }
            });
}

GisLegend::~GisLegend()
{
    if (ui->toggleBtn->isChecked())
        delete mSpacer; //it is not in ui
    delete ui->treeView->model();
    delete ui;
}

#define TR(x) trMap[x] = tr(x)
#define GETTR(x) ((trMap.count(x) == 0)? x: trMap.at(x))

void GisLegend::showLegend()
{
    if (!mCanvas->isValid())
    {
        LOGGER_ERROR(mLogger, "GisLegend::showLegend: "
                     "Failed to create map legend. Map canvas is not valid.");
        assert("GisLegend::showLegend: Invalid map canvas." == 0);
        return;
    }
    auto *mdl = dynamic_cast<QStandardItemModel *>(ui->treeView->model());
    if (mdl != 0)
        mdl->clear();
    else
        mdl = new QStandardItemModel();
    QStandardItem *parent;
    QStandardItem *child;
    std::map<QString, QString> trMap;
    //fill up translation map - need QT_TR_NOOP for Linguist to pick up
    TR(QT_TR_NOOP("ATM"));
    TR(QT_TR_NOOP("Access road"));
    TR(QT_TR_NOOP("Access road under construction"));
    TR(QT_TR_NOOP("Accident-Dispatched"));
    TR(QT_TR_NOOP("Accident-On-Scene"));
    TR(QT_TR_NOOP("Accident-Opened"));
    TR(QT_TR_NOOP("Administrative facilities"));
    TR(QT_TR_NOOP("Airport"));
    TR(QT_TR_NOOP("Alpine hut"));
    TR(QT_TR_NOOP("Apartment"));
    TR(QT_TR_NOOP("Art gallery"));
    TR(QT_TR_NOOP("Art shop"));
    TR(QT_TR_NOOP("Arts centre"));
    TR(QT_TR_NOOP("Artwork"));
    TR(QT_TR_NOOP("Bag shop"));
    TR(QT_TR_NOOP("Bakery"));
    TR(QT_TR_NOOP("Bank"));
    TR(QT_TR_NOOP("Bar"));
    TR(QT_TR_NOOP("Barber"));
    TR(QT_TR_NOOP("Barriers to bicycle traffic"));
    TR(QT_TR_NOOP("Barriers"));
    TR(QT_TR_NOOP("Beauty services"));
    TR(QT_TR_NOOP("Bell tower"));
    TR(QT_TR_NOOP("Bench"));
    TR(QT_TR_NOOP("Beverage shop"));
    TR(QT_TR_NOOP("Bicycle parking"));
    TR(QT_TR_NOOP("Bicycle repair station"));
    TR(QT_TR_NOOP("Bicycle shop"));
    TR(QT_TR_NOOP("Bicycle-rental station"));
    TR(QT_TR_NOOP("Big electricity pylon"));
    TR(QT_TR_NOOP("Boat rental"));
    TR(QT_TR_NOOP("Book store"));
    TR(QT_TR_NOOP("Boom barrier"));
    TR(QT_TR_NOOP("Bowling centre"));
    TR(QT_TR_NOOP("Bridleway"));
    TR(QT_TR_NOOP("Bridleway/Cycleway/Footway/Steps under construction"));
    TR(QT_TR_NOOP("Briefcase-invalid"));
    TR(QT_TR_NOOP("Briefcase-stale"));
    TR(QT_TR_NOOP("Briefcase-valid"));
    TR(QT_TR_NOOP("Buddhist"));
    TR(QT_TR_NOOP("Bureau de change"));
    TR(QT_TR_NOOP("Bus station"));
    TR(QT_TR_NOOP("Bus stop"));
    TR(QT_TR_NOOP("Bust"));
    TR(QT_TR_NOOP("Butcher"));
    TR(QT_TR_NOOP("Cafe"));
    TR(QT_TR_NOOP("Campsite"));
    TR(QT_TR_NOOP("Car parking"));
    TR(QT_TR_NOOP("Car parts shop"));
    TR(QT_TR_NOOP("Car repair service"));
    TR(QT_TR_NOOP("Car store"));
    TR(QT_TR_NOOP("Car wash"));
    TR(QT_TR_NOOP("Carpets shop"));
    TR(QT_TR_NOOP("Casino"));
    TR(QT_TR_NOOP("Castle"));
    TR(QT_TR_NOOP("Cave entrance"));
    TR(QT_TR_NOOP("Chalet"));
    TR(QT_TR_NOOP("Charging station"));
    TR(QT_TR_NOOP("Charity store"));
    TR(QT_TR_NOOP("Chemist"));
    TR(QT_TR_NOOP("Christian"));
    TR(QT_TR_NOOP("Cinema"));
    TR(QT_TR_NOOP("City gate"));
    TR(QT_TR_NOOP("Cliff"));
    TR(QT_TR_NOOP("Clinic"));
    TR(QT_TR_NOOP("Coffee shop"));
    TR(QT_TR_NOOP("Commercial"));
    TR(QT_TR_NOOP("Communication tower"));
    TR(QT_TR_NOOP("Communication"));
    TR(QT_TR_NOOP("Community centre"));
    TR(QT_TR_NOOP("Computer store"));
    TR(QT_TR_NOOP("Confectionery"));
    TR(QT_TR_NOOP("Consulate"));
    TR(QT_TR_NOOP("Convenience store"));
    TR(QT_TR_NOOP("Cooling tower"));
    TR(QT_TR_NOOP("Copy shop"));
    TR(QT_TR_NOOP("Cosmetics shop"));
    TR(QT_TR_NOOP("Court house"));
    TR(QT_TR_NOOP("Culture, entertainment and arts"));
    TR(QT_TR_NOOP("Cycleway"));
    TR(QT_TR_NOOP("Dairy shop"));
    TR(QT_TR_NOOP("Default"));
    TR(QT_TR_NOOP("Default-Dispatched"));
    TR(QT_TR_NOOP("Default-On-Scene"));
    TR(QT_TR_NOOP("Default-Opened"));
    TR(QT_TR_NOOP("Dentist"));
    TR(QT_TR_NOOP("Department store"));
    TR(QT_TR_NOOP("Desktop-invalid"));
    TR(QT_TR_NOOP("Desktop-stale"));
    TR(QT_TR_NOOP("Desktop-valid"));
    TR(QT_TR_NOOP("Destination"));
    TR(QT_TR_NOOP("Dish communication tower"));
    TR(QT_TR_NOOP("Disposal bin"));
    TR(QT_TR_NOOP("Disused railway"));
    TR(QT_TR_NOOP("DIY store"));
    TR(QT_TR_NOOP("Dog park"));
    TR(QT_TR_NOOP("Dome communication tower"));
    TR(QT_TR_NOOP("Educational"));
    TR(QT_TR_NOOP("Electricity"));
    TR(QT_TR_NOOP("Electronics shop"));
    TR(QT_TR_NOOP("Elevator"));
    TR(QT_TR_NOOP("Embassy"));
    TR(QT_TR_NOOP("Emergency phone"));
    TR(QT_TR_NOOP("Fabric shop"));
    TR(QT_TR_NOOP("Fashion shop"));
    TR(QT_TR_NOOP("Fast food restaurant"));
    TR(QT_TR_NOOP("Ferry route"));
    TR(QT_TR_NOOP("Ferry terminal"));
    TR(QT_TR_NOOP("Finance"));
    TR(QT_TR_NOOP("Fire station"));
    TR(QT_TR_NOOP("Fire-Dispatched"));
    TR(QT_TR_NOOP("Fire-On-Scene"));
    TR(QT_TR_NOOP("Fire-Opened"));
    TR(QT_TR_NOOP("Fish shop"));
    TR(QT_TR_NOOP("Fitness centre"));
    TR(QT_TR_NOOP("Florist"));
    TR(QT_TR_NOOP("Footway"));
    TR(QT_TR_NOOP("Fortified defensive tower"));
    TR(QT_TR_NOOP("Fortress"));
    TR(QT_TR_NOOP("Fountain"));
    TR(QT_TR_NOOP("Furniture store"));
    TR(QT_TR_NOOP("Garden centre"));
    TR(QT_TR_NOOP("Gas station"));
    TR(QT_TR_NOOP("Gastronomy"));
    TR(QT_TR_NOOP("Gate"));
    TR(QT_TR_NOOP("Gift shop"));
    TR(QT_TR_NOOP("Golf course hole"));
    TR(QT_TR_NOOP("Golf course"));
    TR(QT_TR_NOOP("Gourmet foods"));
    TR(QT_TR_NOOP("Government vehicle inspection"));
    TR(QT_TR_NOOP("Greengrocer"));
    TR(QT_TR_NOOP("Guest house"));
    TR(QT_TR_NOOP("Guidepost"));
    TR(QT_TR_NOOP("Handheld-invalid"));
    TR(QT_TR_NOOP("Handheld-stale"));
    TR(QT_TR_NOOP("Handheld-valid"));
    TR(QT_TR_NOOP("Healthcare"));
    TR(QT_TR_NOOP("Helipad"));
    TR(QT_TR_NOOP("Hi-fi store"));
    TR(QT_TR_NOOP("Hindu"));
    TR(QT_TR_NOOP("Historical object"));
    TR(QT_TR_NOOP("Historical shrine"));
    TR(QT_TR_NOOP("Historical"));
    TR(QT_TR_NOOP("Home care"));
    TR(QT_TR_NOOP("Hospital"));
    TR(QT_TR_NOOP("Hostel"));
    TR(QT_TR_NOOP("Hotel"));
    TR(QT_TR_NOOP("Houseware shop"));
    TR(QT_TR_NOOP("Huge tower for transmitting radio applications"));
    TR(QT_TR_NOOP("Ice cream shop"));
    TR(QT_TR_NOOP("Industrial"));
    TR(QT_TR_NOOP("Information board"));
    TR(QT_TR_NOOP("Information terminal"));
    TR(QT_TR_NOOP("Institutional"));
    TR(QT_TR_NOOP("Interior decorations shop"));
    TR(QT_TR_NOOP("Internet cafe"));
    TR(QT_TR_NOOP("Jewellery"));
    TR(QT_TR_NOOP("Jewish"));
    TR(QT_TR_NOOP("Landmarks"));
    TR(QT_TR_NOOP("Lattice communication tower"));
    TR(QT_TR_NOOP("Laundry shop"));
    TR(QT_TR_NOOP("Leisure, recreation and sports"));
    TR(QT_TR_NOOP("Library"));
    TR(QT_TR_NOOP("Lighthouse"));
    TR(QT_TR_NOOP("Machine for picking up and sending parcels"));
    TR(QT_TR_NOOP("Major power line"));
    TR(QT_TR_NOOP("Managed beach"));
    TR(QT_TR_NOOP("Manor house"));
    TR(QT_TR_NOOP("Marinekit-invalid"));
    TR(QT_TR_NOOP("Marinekit-stale"));
    TR(QT_TR_NOOP("Marinekit-valid"));
    TR(QT_TR_NOOP("Marketplace"));
    TR(QT_TR_NOOP("Massage shop"));
    TR(QT_TR_NOOP("Mast with transmitters"));
    TR(QT_TR_NOOP("Mattress shop"));
    TR(QT_TR_NOOP("Medical equipment store"));
    TR(QT_TR_NOOP("Memorial plaque"));
    TR(QT_TR_NOOP("Memorial"));
    TR(QT_TR_NOOP("Military historic fort"));
    TR(QT_TR_NOOP("Mini roundabout"));
    TR(QT_TR_NOOP("Miniature golf course"));
    TR(QT_TR_NOOP("Miniature railway"));
    TR(QT_TR_NOOP("Minor power line"));
    TR(QT_TR_NOOP("Mobile phones shop"));
    TR(QT_TR_NOOP("Monorail railway"));
    TR(QT_TR_NOOP("Monument"));
    TR(QT_TR_NOOP("Motel"));
    TR(QT_TR_NOOP("Motorcycle parking"));
    TR(QT_TR_NOOP("Motorcycle shop"));
    TR(QT_TR_NOOP("Motorway"));
    TR(QT_TR_NOOP("Motorway under construction"));
    TR(QT_TR_NOOP("Multi-storey parking entrance"));
    TR(QT_TR_NOOP("Museum"));
    TR(QT_TR_NOOP("Music shop"));
    TR(QT_TR_NOOP("Muslim"));
    TR(QT_TR_NOOP("Narrow gauge railway"));
    TR(QT_TR_NOOP("Nature"));
    TR(QT_TR_NOOP("Newsstand"));
    TR(QT_TR_NOOP("Nightclub"));
    TR(QT_TR_NOOP("Obelisk"));
    TR(QT_TR_NOOP("Office"));
    TR(QT_TR_NOOP("One-way"));
    TR(QT_TR_NOOP("Optical telescope"));
    TR(QT_TR_NOOP("Optician"));
    TR(QT_TR_NOOP("Other religion"));
    TR(QT_TR_NOOP("Others"));
    TR(QT_TR_NOOP("Outdoor equipment shop"));
    TR(QT_TR_NOOP("Outdoor"));
    TR(QT_TR_NOOP("Paint shop"));
    TR(QT_TR_NOOP("Palace"));
    TR(QT_TR_NOOP("Parking tickets machine"));
    TR(QT_TR_NOOP("Pay-to-play games centre"));
    TR(QT_TR_NOOP("Peak"));
    TR(QT_TR_NOOP("Pedestrian street"));
    TR(QT_TR_NOOP("Pedestrian street under construction"));
    TR(QT_TR_NOOP("Pet shop"));
    TR(QT_TR_NOOP("Pharmacy"));
    TR(QT_TR_NOOP("Phone-invalid"));
    TR(QT_TR_NOOP("Phone-stale"));
    TR(QT_TR_NOOP("Phone-valid"));
    TR(QT_TR_NOOP("Photo studio"));
    TR(QT_TR_NOOP("Picnic site"));
    TR(QT_TR_NOOP("Picnic table"));
    TR(QT_TR_NOOP("Pier"));
    TR(QT_TR_NOOP("Playground"));
    TR(QT_TR_NOOP("Poles for lighting"));
    TR(QT_TR_NOOP("Police station"));
    TR(QT_TR_NOOP("Post box"));
    TR(QT_TR_NOOP("Post office"));
    TR(QT_TR_NOOP("Preserved railway"));
    TR(QT_TR_NOOP("Primary road"));
    TR(QT_TR_NOOP("Primary road under construction"));
    TR(QT_TR_NOOP("Prison"));
    TR(QT_TR_NOOP("Private road"));
    TR(QT_TR_NOOP("Pub"));
    TR(QT_TR_NOOP("Public bookcase"));
    TR(QT_TR_NOOP("Public fishing"));
    TR(QT_TR_NOOP("Public telephone"));
    TR(QT_TR_NOOP("Public toilets"));
    TR(QT_TR_NOOP("RIM-invalid"));
    TR(QT_TR_NOOP("RIM-stale"));
    TR(QT_TR_NOOP("RIM-valid"));
    TR(QT_TR_NOOP("Racetrack (Motorised)"));
    TR(QT_TR_NOOP("Radio telescope"));
    TR(QT_TR_NOOP("Radiodispatcher-invalid"));
    TR(QT_TR_NOOP("Radiodispatcher-stale"));
    TR(QT_TR_NOOP("Radiodispatcher-valid"));
    TR(QT_TR_NOOP("Railroad crossing"));
    TR(QT_TR_NOOP("Railway for full-sized passenger trains"));
    TR(QT_TR_NOOP("Railway full-sized passenger trains service segments"));
    TR(QT_TR_NOOP("Railway station"));
    TR(QT_TR_NOOP("Railway turntable"));
    TR(QT_TR_NOOP("Railway under construction"));
    TR(QT_TR_NOOP("Railways"));
    TR(QT_TR_NOOP("Record shop"));
    TR(QT_TR_NOOP("Recreational"));
    TR(QT_TR_NOOP("Recycling centre"));
    TR(QT_TR_NOOP("Religious place"));
    TR(QT_TR_NOOP("Religious"));
    TR(QT_TR_NOOP("Rent a car"));
    TR(QT_TR_NOOP("Rescue-Dispatched"));
    TR(QT_TR_NOOP("Rescue-On-Scene"));
    TR(QT_TR_NOOP("Rescue-Opened"));
    TR(QT_TR_NOOP("Residential"));
    TR(QT_TR_NOOP("Residential road"));
    TR(QT_TR_NOOP("Restaurant"));
    TR(QT_TR_NOOP("River/Canal"));
    TR(QT_TR_NOOP("River/Canal in tunnel"));
    TR(QT_TR_NOOP("River/Canal intermittent"));
    TR(QT_TR_NOOP("Road features"));
    TR(QT_TR_NOOP("Road with unknown classification under construction"));
    TR(QT_TR_NOOP("Roads"));
    TR(QT_TR_NOOP("Robbery-Dispatched"));
    TR(QT_TR_NOOP("Robbery-On-Scene"));
    TR(QT_TR_NOOP("Robbery-Opened"));
    TR(QT_TR_NOOP("Roller coaster track"));
    TR(QT_TR_NOOP("Rucksack-invalid"));
    TR(QT_TR_NOOP("Rucksack-stale"));
    TR(QT_TR_NOOP("Rucksack-valid"));
    TR(QT_TR_NOOP("Runway"));
    TR(QT_TR_NOOP("Sauna"));
    TR(QT_TR_NOOP("Second hand goods shop"));
    TR(QT_TR_NOOP("Secondary road"));
    TR(QT_TR_NOOP("Secondary road under construction"));
    TR(QT_TR_NOOP("Shelter"));
    TR(QT_TR_NOOP("Shinto"));
    TR(QT_TR_NOOP("Shoe store"));
    TR(QT_TR_NOOP("Shop"));
    TR(QT_TR_NOOP("Shops and services"));
    TR(QT_TR_NOOP("Shower"));
    TR(QT_TR_NOOP("Sikh"));
    TR(QT_TR_NOOP("Slipway for boats"));
    TR(QT_TR_NOOP("Small electricity pole"));
    TR(QT_TR_NOOP("Social facility"));
    TR(QT_TR_NOOP("Source"));
    TR(QT_TR_NOOP("Sports equipment shop"));
    TR(QT_TR_NOOP("Spring"));
    TR(QT_TR_NOOP("Stationery shop"));
    TR(QT_TR_NOOP("Statue"));
    TR(QT_TR_NOOP("Steps"));
    TR(QT_TR_NOOP("Stile"));
    TR(QT_TR_NOOP("Stone memorial"));
    TR(QT_TR_NOOP("Storage tanks"));
    TR(QT_TR_NOOP("Stream/Ditch/Drain"));
    TR(QT_TR_NOOP("Subway entrance"));
    TR(QT_TR_NOOP("Subway railway"));
    TR(QT_TR_NOOP("Supermarket"));
    TR(QT_TR_NOOP("Taoist"));
    TR(QT_TR_NOOP("Taxi"));
    TR(QT_TR_NOOP("Taxiway"));
    TR(QT_TR_NOOP("Tea shop"));
    TR(QT_TR_NOOP("Tertiary road"));
    TR(QT_TR_NOOP("Tertiary road under construction"));
    TR(QT_TR_NOOP("Theatre"));
    TR(QT_TR_NOOP("Tickets shop"));
    TR(QT_TR_NOOP("Tobacco shop"));
    TR(QT_TR_NOOP("Tollbooth"));
    TR(QT_TR_NOOP("Topographic saddle"));
    TR(QT_TR_NOOP("Tourism and accommodation"));
    TR(QT_TR_NOOP("Tourism-Information"));
    TR(QT_TR_NOOP("Tower"));
    TR(QT_TR_NOOP("Towers for lighting"));
    TR(QT_TR_NOOP("Townhall"));
    TR(QT_TR_NOOP("Toy shop"));
    TR(QT_TR_NOOP("Track (Even amount of solid and soft materials)"));
    TR(QT_TR_NOOP("Track (Mostly soft surface)"));
    TR(QT_TR_NOOP("Track (Mostly solid surface)"));
    TR(QT_TR_NOOP("Track (Non-motorised)"));
    TR(QT_TR_NOOP("Track (Soft surface)"));
    TR(QT_TR_NOOP("Track (Solid surface)"));
    TR(QT_TR_NOOP("Track (Unknown surface type)"));
    TR(QT_TR_NOOP("Traffic lights"));
    TR(QT_TR_NOOP("Train tickets machine"));
    TR(QT_TR_NOOP("Tram railway"));
    TR(QT_TR_NOOP("Transportation"));
    TR(QT_TR_NOOP("Travel agency"));
    TR(QT_TR_NOOP("Tree"));
    TR(QT_TR_NOOP("Trunk road"));
    TR(QT_TR_NOOP("Trunk road under construction"));
    TR(QT_TR_NOOP("Tyres shop"));
    TR(QT_TR_NOOP("Underground parking entrance"));
    TR(QT_TR_NOOP("Unknown road type"));
    TR(QT_TR_NOOP("Variety store"));
    TR(QT_TR_NOOP("Vehicular-invalid"));
    TR(QT_TR_NOOP("Vehicular-stale"));
    TR(QT_TR_NOOP("Vehicular-valid"));
    TR(QT_TR_NOOP("Veterinary"));
    TR(QT_TR_NOOP("Video games shop"));
    TR(QT_TR_NOOP("Videos/DVDs shop"));
    TR(QT_TR_NOOP("Viewpoint"));
    TR(QT_TR_NOOP("Volcano"));
    TR(QT_TR_NOOP("Waste management"));
    TR(QT_TR_NOOP("Water dispenser"));
    TR(QT_TR_NOOP("Water park"));
    TR(QT_TR_NOOP("Water slide"));
    TR(QT_TR_NOOP("Water tower"));
    TR(QT_TR_NOOP("Waterfall"));
    TR(QT_TR_NOOP("Waterways"));
    TR(QT_TR_NOOP("Way for guided buses"));
    TR(QT_TR_NOOP("Wholesale store"));
    TR(QT_TR_NOOP("Wilderness hut"));

    QString f;
    QString s;
    QFile file(XML_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        LOGGER_ERROR(mLogger, "GisLegend::showLegend: Failed to open "
                     << XML_FILE.toStdString());
        delete mdl;
        return;
    }
    QDomDocument doc;
    if (!doc.setContent(&file))
    {
        LOGGER_ERROR(mLogger, "GisLegend::showLegend: Failed to parse "
                     << XML_FILE.toStdString());
        file.close();
        delete mdl;
        return;
    }
    file.close();
    auto n = doc.documentElement().firstChildElement();
    if (n.isNull())
    {
        LOGGER_ERROR(mLogger, "GisLegend::showLegend: Cannot find element in "
                     << XML_FILE.toStdString());
        delete mdl;
        return;
    }
    //XML file format sample:
    //<legend>
    // <category>
    //  <name>Administrative facilities</description>
    //  <item>
    //   <name>Consulate</description>
    //   <source>./qml/administrative/consulate.png</source>
    //  </item>
    // </category>
    //</legend>
    QDomElement ct; //category
    QDomElement it; //item
    for (; !n.isNull(); n=n.nextSiblingElement())
    {
        if (n.tagName() != "category")
            continue;
        parent = 0;
        //iterate children of category
        ct = n.firstChildElement();
#ifndef INCIDENT
        //skip sys_Incident and sys_ReportIncident
        if (ct.tagName() == "name" && ct.text().endsWith("Incident"))
            continue;
#endif
        for (; !ct.isNull(); ct=ct.nextSiblingElement())
        {
            if (ct.tagName() == "name")
            {
                if (parent == 0)
                    parent = new QStandardItem(GETTR(ct.text()));
                else
                    parent->setText(GETTR(ct.text()));
            }
            else if (ct.tagName() == "item")
            {
                s.clear();
                f.clear();
                //iterate children of item
                for (it=ct.firstChildElement(); !it.isNull();
                     it=it.nextSiblingElement())
                {
                    if (it.tagName() == "name")
                        s = GETTR(it.text());
                    else if (it.tagName() == "source")
                        f = it.text();
                }
                if (s.isEmpty() || f.isEmpty())
                    continue;
                child = new QStandardItem(QIcon(f), s);
                child->setSelectable(false);
                child->setEditable(false);
                if (parent == 0)
                    parent = new QStandardItem();
                parent->appendRow(child);
            }
        }
        if (parent != 0)
        {
            parent->setSelectable(false);
            parent->setEditable(false);
            mdl->appendRow(parent);
        }
    } //for (; !n.isNull(); n=n.nextSiblingElement())
    ui->treeView->header()->hide();
    ui->treeView->setModel(mdl);
}

void GisLegend::setTheme()
{
    ui->lblFrame->setStyleSheet(Style::getStyle(Style::OBJ_FRAME_TITLE));
    ui->treeView->setStyleSheet(Style::getStyle(Style::OBJ_TREEVIEW));
}
