/**
 * Map view history implementation.
 *
 * Copyright (C) Sapura Secured Technologies, 2017-2025. All Rights Reserved.
 *
 * @file
 * @version $Id: GisHistory.cpp 1898 2025-01-17 00:50:10Z rosnin $
 * @author Zunnur Zafirah
 */
#include <assert.h>

#include "GisHistory.h"

static const int MAX_VIEWS = 20;

GisHistory::GisHistory(GisCanvas *canvas):
mNavigate(false), mMapCanvas(canvas)
{
    if (canvas == 0)
    {
        assert("Bad param in GisHistory::GisHistory" == 0);
        return;
    }
    mViewListIt = mViewList.begin();
}

bool GisHistory::save(QPointF center, double zoomLvl)
{
    if (!mViewList.empty())
    {
        ViewListT::iterator it = mViewList.end();
        --it;
        if (it != mViewListIt)
        {
            //remove all forward history
            do
            {
                mViewList.pop_back();
                it = mViewList.end();
                --it;
            }
            while (it != mViewListIt);
        }
        else if (mViewList.size() == MAX_VIEWS)
        {
            mViewList.pop_front();
        }
    }
    mViewList.push_back(ViewData(center, zoomLvl));
    mViewListIt = mViewList.end();
    --mViewListIt;
    return (mViewListIt != mViewList.begin());
}

bool GisHistory::prev()
{
    --mViewListIt;
    mNavigate = true;
    mMapCanvas->setGeomCenter(mViewListIt->center, mViewListIt->zoomLvl);
    return (mViewListIt != mViewList.begin());
}

bool GisHistory::next()
{
    ++mViewListIt;
    mNavigate = true;
    mMapCanvas->setGeomCenter(mViewListIt->center, mViewListIt->zoomLvl);
    ViewListT::iterator it = mViewListIt;
    return (++it != mViewList.end());
}

bool GisHistory::checkNavigation()
{
    if (!mNavigate)
        return false;
    mNavigate = false;
    return true;
}
