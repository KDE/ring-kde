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
#include "pixmapwrapper.h"

#include <QtGui/QPainter>

PixmapWrapper::PixmapWrapper(QQuickItem* parent) : QQuickPaintedItem(parent)
{}

QPixmap PixmapWrapper::pixmap() const {
    return m_pixmap;
}

void PixmapWrapper::setPixmap(const QVariant& var)
{
    m_pixmap = qvariant_cast<QPixmap>(var);
    m_icon   = qvariant_cast<QIcon  >(var);
    update();
}

void PixmapWrapper::paint(QPainter *painter)
{
    if (!m_icon.isNull()) {
        const QPixmap pxm = m_icon.pixmap(boundingRect().size().toSize());

        painter->drawPixmap(boundingRect().toRect(),pxm);
    }
    else if (!m_pixmap.isNull()) {
        painter->drawPixmap(
            boundingRect(),
            m_pixmap,
            QRect( 0,0, width(), height())
        );
    }
}
