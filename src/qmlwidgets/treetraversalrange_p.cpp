/***************************************************************************
 *   Copyright (C) 2018 by Bluesystems                                     *
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
#include "treetraversalrange_p.h"

bool TreeTraversalRange::incrementUpward()
{
    return false;
}

bool TreeTraversalRange::incrementDownward()
{
    return false;
}

bool TreeTraversalRange::decrementUpward()
{
    return false;
}

bool TreeTraversalRange::decrementDownward()
{
    return false;
}

/*TreeTraversalRange::Iterator TreeTraversalRange::begin()
{
    return {};
}

TreeTraversalRange::Iterator TreeTraversalRange::end()
{
    return {};
}*/

const TreeTraversalRange::Subset TreeTraversalRange::subset(const QModelIndex& idx) const
{
    return {};
}
