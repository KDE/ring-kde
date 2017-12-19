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

class ContactPhotoPrivate : public QObject
{
    Q_OBJECT
public:
    ContactMethod* m_pContactMethod {nullptr};
    Person* m_pPerson {nullptr};
    Person* m_pCurrentPerson {nullptr};
    bool m_DisplayEmpty {true};
    bool m_DrawEmptyOutline {true};

    ContactPhoto* q_ptr;
public Q_SLOTS:
    void slotPhotoChanged();
    void slotContactChanged();
};

ContactPhoto::ContactPhoto(QQuickItem* parent) : QQuickPaintedItem(parent),
    d_ptr(new ContactPhotoPrivate)
{
    d_ptr->q_ptr = this;
}

ContactPhoto::~ContactPhoto()
{
    delete d_ptr;
}

void ContactPhoto::paint(QPainter *painter)
{
    const qreal w = width ();
    const qreal h = height();
    const qreal s = std::min(w, h);

    QPainterPath imageClip, cornerPath;
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

// for tests
//     const bool isTracked(true), isPresent(rand()%2);

    imageClip.addEllipse(3, 3, s-6, s-6);

    cornerPath.moveTo(s/2, s);
    cornerPath.arcTo(0, 0, s, s, 270, 90);
    cornerPath.lineTo(s,s-6);
    cornerPath.arcTo(s-12, s-12, 12, 12, 0, -90);
    cornerPath.lineTo(s/2, s);

    painter->setRenderHint(QPainter::Antialiasing);

    auto pen = painter->pen();
    pen.setColor(QGuiApplication::palette().text().color());
    pen.setWidthF(1.5);

    painter->setPen(pen);


    if (p && hasPhoto()) {
        painter->drawEllipse(1.5, 1.5, s-1.5, s-1.5);

        painter->setClipPath(imageClip);

        const QPixmap original(qvariant_cast<QPixmap>(p->photo()));

        painter->drawPixmap(3,3,s-6,s-6, original);

        painter->setClipping(false);

        if (isTracked) {
            pen.setWidthF(1);
            painter->setPen(pen);
            painter->setBrush(pen.color());
            painter->drawPath(cornerPath);

            painter->setBrush(isPresent ? presentBrush : awayBrush);

            painter->drawEllipse(s-12, s-12, 12, 12);
        }
    }
    else if (d_ptr->m_DisplayEmpty) {
        if (d_ptr->m_DrawEmptyOutline)
            painter->drawEllipse(1.5, 1.5, s-1.5, s-1.5);

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
    if (d_ptr->m_pPerson) {
        disconnect(d_ptr->m_pPerson, &Person::photoChanged,
            d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
        d_ptr->m_pPerson = nullptr;
    }

    if (d_ptr->m_pContactMethod) {
        disconnect(d_ptr->m_pContactMethod, &ContactMethod::contactChanged,
            d_ptr, &ContactPhotoPrivate::slotContactChanged);
    }

    d_ptr->m_pContactMethod = cm;

    d_ptr->slotContactChanged();

    emit hasPhotoChanged();

    update();
}

Person* ContactPhoto::person() const
{
    return d_ptr->m_pPerson;
}

void ContactPhoto::setPerson(Person* p)
{
    if (d_ptr->m_pPerson) {
        disconnect(d_ptr->m_pPerson, &Person::photoChanged,
            d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
    }

    if (d_ptr->m_pCurrentPerson) {
        disconnect(d_ptr->m_pCurrentPerson, &Person::photoChanged,
            d_ptr, &ContactPhotoPrivate::slotPhotoChanged);
        d_ptr->m_pCurrentPerson = nullptr;
    }

    d_ptr->m_pPerson = p;

    connect(d_ptr->m_pPerson, &Person::photoChanged,
        d_ptr, &ContactPhotoPrivate::slotPhotoChanged);

    update();

    emit hasPhotoChanged();
}

void ContactPhotoPrivate::slotPhotoChanged()
{
    emit q_ptr->hasPhotoChanged();

    q_ptr->update();
}

void ContactPhotoPrivate::slotContactChanged()
{
    if (m_pCurrentPerson) {
        disconnect(m_pCurrentPerson, &Person::photoChanged,
            this, &ContactPhotoPrivate::slotPhotoChanged);
    }

    m_pCurrentPerson = m_pContactMethod->contact();

    if (m_pCurrentPerson) {
        connect(m_pContactMethod, &ContactMethod::contactChanged,
            this, &ContactPhotoPrivate::slotContactChanged);
    }
}

bool ContactPhoto::hasPhoto() const
{
    if (d_ptr->m_pPerson)
        return !d_ptr->m_pPerson->photo().isNull();

    if (d_ptr->m_pCurrentPerson)
        return !d_ptr->m_pCurrentPerson->photo().isNull();

    return false;
}

bool ContactPhoto::displayEmpty() const
{
    return d_ptr->m_DisplayEmpty;
}

void ContactPhoto::setDisplayEmpty(bool val)
{
    d_ptr->m_DisplayEmpty = val;
    update();
}

bool ContactPhoto::drawEmptyOutline() const
{
    return d_ptr->m_DrawEmptyOutline;
}

void ContactPhoto::setDrawEmptyOutline(bool val)
{
    d_ptr->m_DrawEmptyOutline = val;
}

#include <contactphoto.moc>
