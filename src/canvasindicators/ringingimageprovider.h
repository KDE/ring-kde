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

#include <QQuickImageProvider>

class RingingImageProviderPrivate;

/**
 * This image provider mainly exist to edit SVG files to set some colors or
 * effects from the palette.
 */
class RingingImageProvider : public QQuickImageProvider
{
public:
    explicit RingingImageProvider();
    virtual ~RingingImageProvider();

    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    Q_DISABLE_COPY(RingingImageProvider)
    RingingImageProviderPrivate* d_ptr;
    Q_DECLARE_PRIVATE(RingingImageProvider)
};
