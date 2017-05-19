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
#include "personiconengine.h"

// Qt
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QPalette>
#include <QtGui/QGuiApplication>
#include <QtCore/QDebug>

// KDE
#include <KColorScheme>

// Ring
#include <person.h>

// Qml
#include "cmiconengine.h"

class PersonIconEnginePrivate
{
public:
    Person* m_pPerson;
};

PersonIconEngine::PersonIconEngine(Person* person) : QIconEngine(), d_ptr(new PersonIconEnginePrivate)
{
    d_ptr->m_pPerson = person;
}

void PersonIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)

    bool displayPresence = true;

    // Given this has a transparent area, clear the rectangle
    painter->setCompositionMode(QPainter::CompositionMode_Clear);
    painter->fillRect(rect, QBrush(Qt::white));
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    const int radius = (rect.height() > 35) ? 7 : 5;

    const bool isTracked = displayPresence && d_ptr->m_pPerson->isTracked();
    const bool isPresent = displayPresence && d_ptr->m_pPerson->isPresent();

    static QColor presentBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::PositiveText ).brush(QPalette::Normal).color();
    static QColor awayBrush    = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();

    if (d_ptr->m_pPerson->photo().isValid()) {
        QPixmap contactPhoto((qvariant_cast<QPixmap>(d_ptr->m_pPerson->photo())).scaledToWidth(rect.height()-6));

        //Add corner radius to the Pixmap
        QRect pxRect = contactPhoto.rect();
        QBitmap mask(pxRect.size());
        QPainter customPainter(&mask);
        customPainter.setRenderHint  (QPainter::Antialiasing, true      );
        customPainter.fillRect       (pxRect                , Qt::white );
        customPainter.setBackground  (Qt::black                         );
        customPainter.setBrush       (Qt::black                         );
        customPainter.drawRoundedRect(pxRect,radius,radius);
        contactPhoto.setMask(mask);
        painter->drawPixmap(3,3,contactPhoto);
        painter->setBrush(Qt::NoBrush);
        painter->setPen(Qt::white);
        painter->setRenderHint  (QPainter::Antialiasing, true   );
        painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter->drawRoundedRect(3,3,rect.height()-6,rect.height()-6,radius,radius);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

        //Draw the glow around pixmaps
        if (isTracked) {
            QPen pen(QGuiApplication::palette().color(QPalette::Disabled,QPalette::Text));
            pen.setWidth(1);

            if (isPresent)
                pen.setColor(presentBrush);
            else
                pen.setColor(awayBrush);

            for (int i=2;i<=7;i+=2) {
                pen.setWidth(i);
                painter->setPen(pen);
                painter->setOpacity(0.3f-(((i-2)*0.8f)/10.0f));
                painter->drawRoundedRect(3,3,rect.height()-6,rect.height()-6,radius,radius);
            }
        }
    }
    else
        CMIconEngine::staticPaint(painter, rect, isPresent, isTracked);

}

auto PersonIconEngine::clone() const -> QIconEngine*
{
    auto e = new PersonIconEngine(d_ptr->m_pPerson);
    delete e->d_ptr;
    e->d_ptr = d_ptr;

    return e;
}

QSize PersonIconEngine::actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(mode)
    Q_UNUSED(state)
    return {
        std::min(size.height(), size.width()),
        std::min(size.height(), size.width())
    };
}
