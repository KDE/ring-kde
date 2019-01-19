/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "snapshotadapter.h"

#include "imageprovider.h"

#include <call.h>
#include <media/video.h>

SnapshotAdapter::SnapshotAdapter(QObject* parent) : QObject(parent)
{
}

SnapshotAdapter::~SnapshotAdapter()
{
}

void SnapshotAdapter::takeSnapshot(Call* c) const
{
    if (!c->hasVideo())
        return;

    auto vid = c->firstMedia<Media::Video>(Media::Media::Direction::IN);

    if (!vid)
        return;

    vid->takeSnapshot();

    //ImageProvider::takeSnapshot(c);
}
