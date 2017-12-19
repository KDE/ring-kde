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

#include <video/renderer.h>
// #include <video/model.h>

class SymbolicColorizerPrivate;

/**
 * SymbolicColorizer is a QtQuick image provider designed to paint any theme
 * or QRC icons it receive the same color as the QPalette text color.
 */
class SymbolicColorizer final : public QQuickImageProvider
{
public:
    explicit SymbolicColorizer();
    virtual ~SymbolicColorizer();

    virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    Q_DISABLE_COPY(SymbolicColorizer)
    SymbolicColorizerPrivate* d_ptr;
    Q_DECLARE_PRIVATE(SymbolicColorizer)
};
