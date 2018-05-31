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
#pragma once

#include <QQuickPaintedItem>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

class PixmapWrapper : public QQuickPaintedItem
{
   Q_OBJECT
   Q_PROPERTY(QVariant pixmap READ pixmap WRITE setPixmap)
   Q_PROPERTY(QString themeFallback READ themeFallback WRITE setThemeFallback)
   Q_PROPERTY(bool hasPixmap READ hasPixmap NOTIFY changed)

public:
    explicit PixmapWrapper(QQuickItem* parent = nullptr);

    QPixmap pixmap() const;
    void setPixmap(const QVariant& var);

    QString themeFallback() const;
    void setThemeFallback(const QString& s);

    bool hasPixmap() const;

    virtual void paint(QPainter *painter) override;

private:
    QPixmap m_pixmap;
    QIcon   m_icon  ;
    QString m_themeFallback;
    QIcon   m_fallbackIcon;

Q_SIGNALS:
    void changed();
};
