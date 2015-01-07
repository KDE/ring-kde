/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "numbercategory.h"

#include <QtCore/QSize>

#include "visitors/pixmapmanipulationvisitor.h"

class NumberCategoryPrivate
{
public:
   NumberCategoryPrivate();
   //Attributes
   QString m_Name;
   QPixmap* m_pIcon;
};

NumberCategoryPrivate::NumberCategoryPrivate() : m_pIcon(nullptr), m_Name()
{
}

NumberCategory::NumberCategory(QObject* parent, const QString& name) : QObject(parent), d_ptr(new NumberCategoryPrivate())
{
   d_ptr->m_Name = name;
}

NumberCategory::~NumberCategory()
{
   delete d_ptr;
}

QVariant NumberCategory::icon(bool isTracked, bool isPresent) const
{
   return PixmapManipulationVisitor::instance()->numberCategoryIcon(d_ptr->m_pIcon,QSize(),isTracked,isPresent);
}

QString  NumberCategory::name() const
{
   return d_ptr->m_Name;
}

void NumberCategory::setIcon(QPixmap*pixmap)
{
   d_ptr->m_pIcon = pixmap;
}

void NumberCategory::setName(const QString& name)
{
   d_ptr->m_Name = name;
}
