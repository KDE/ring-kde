/***************************************************************************
 *   Copyright (C)  2013 by Savoir-Faire Linux                             *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "videotoolbar.h"

//Qt
#include <QtGui/QHBoxLayout>

//SFLPhone
#include "sflphone.h"
#include "actioncollection.h"
#include "extendedaction.h"

///Constructor
VideoToolbar::VideoToolbar(QWidget* parent) : QToolBar(parent)
{
   QToolBar::setIconSize(QSize(16,16));
   addAction(ActionCollection::instance()->videoRotateLeftAction     ());
   addAction(ActionCollection::instance()->videoRotateRightAction    ());
   addAction(ActionCollection::instance()->videoFlipHorizontalAction ());
   addAction(ActionCollection::instance()->videoFlipVerticalAction   ());
   addAction(ActionCollection::instance()->videoScaleAction          ());
   addAction(ActionCollection::instance()->videoFullscreenAction     ());
   addAction(ActionCollection::instance()->videoMuteAction           ());
   addAction(ActionCollection::instance()->videoPreviewAction        ());
}

///Destructor
VideoToolbar::~VideoToolbar()
{
}
