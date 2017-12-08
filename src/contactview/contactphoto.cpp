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
#include "contactphoto.h"

#include <QtGui/QPainter>
#include <QtGui/QGuiApplication>
#include <QtGui/QColor>
#include <QtGui/QPalette>

#include <contactmethod.h>
#include <person.h>

// KDE
#include <KColorScheme>

class ContactPhotoPrivate
{
public:
    ContactMethod* m_pContactMethod {nullptr};
    Person* m_pPerson {nullptr};
};

ContactPhoto::ContactPhoto(QQuickItem* parent) : QQuickPaintedItem(parent),
    d_ptr(new ContactPhotoPrivate)
{}

ContactPhoto::~ContactPhoto()
{
    delete d_ptr;
}

void ContactPhoto::paint(QPainter *painter)
{
    const qreal w = width ();
    const qreal h = height();
    const qreal s = std::min(w, h);

    static QPainterPath imageClip, cornerPath;
    static QColor presentBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::PositiveText )
        .brush(QPalette::Normal).color();
    static QColor awayBrush    = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText )
        .brush(QPalette::Normal).color();

    Person* p = d_ptr->m_pPerson;

    if ((!p) && d_ptr->m_pContactMethod) {
        p = d_ptr->m_pContactMethod->contact();
    }

    const bool isTracked = p ? p->isTracked() :
        d_ptr->m_pContactMethod ? d_ptr->m_pContactMethod->isTracked() : false;
    const bool isPresent = p ? p->isPresent() :
        d_ptr->m_pContactMethod ? d_ptr->m_pContactMethod->isPresent() : false;

    static std::atomic_flag init_flag {ATOMIC_FLAG_INIT};
    if (!init_flag.test_and_set()) {
        imageClip.addEllipse(3, 3, s-6, s-6);

        cornerPath.moveTo(s/2, s);
        cornerPath.arcTo(0, 0, s, s, 270, 90);
        cornerPath.lineTo(s,s-6);
        cornerPath.arcTo(s-12, s-12, 12, 12, 0, -90);
        cornerPath.lineTo(s/2, s);
    }

    painter->setRenderHint(QPainter::Antialiasing);

    auto pen = painter->pen();
    pen.setColor(QGuiApplication::palette().text().color());
    pen.setWidthF(1.5);

    painter->setPen(pen);

    painter->drawEllipse(1.5, 1.5, s-3, s-3);

    if (p && p->photo().isValid()) {
        if (isTracked) {
            painter->setPen({});
            painter->setBrush(pen.color());
            painter->drawPath(cornerPath);

            painter->setBrush(isPresent ? presentBrush : awayBrush);

            painter->drawEllipse(s-12, s-12, 12, 12);
        }

        painter->setClipPath(imageClip);

        QPixmap original(qvariant_cast<QPixmap>(p->photo()));

        painter->drawPixmap(5,5,s-10,s-10, original);
    }
    else {
        painter->setClipPath(imageClip);

        if (isTracked)
            pen.setColor(isPresent ? presentBrush : awayBrush);

        painter->setBrush({});
        painter->setPen(pen);

        const qreal headSize = (s/2) / 2;
        const qreal bodySize = headSize*2.5;

        painter->drawEllipse(s/2-headSize/2, headSize, headSize, headSize);
        painter->drawEllipse(s/2-bodySize/2, bodySize, bodySize, bodySize);
    }
}

ContactMethod* ContactPhoto::contactMethod() const
{
    return d_ptr->m_pContactMethod;
}

void ContactPhoto::setContactMethod(ContactMethod* cm)
{
    d_ptr->m_pContactMethod = cm;
    update();
}

Person* ContactPhoto::person() const
{
    return d_ptr->m_pPerson;
}

void ContactPhoto::setPerson(Person* p)
{
    d_ptr->m_pPerson = p;
    update();
}
