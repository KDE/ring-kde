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
#include "symboliccolorizer.h"

// Qt
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtGui/QPalette>
#include <QtGui/QPainter>
#include <QtGui/QGuiApplication>
#include <QtSvg/QSvgRenderer>

#include <call.h>

#include <callmodel.h>
#include <video/previewmanager.h>

class SymbolicColorizerPrivate final : public QObject
{
    Q_OBJECT
public:
    QHash<QString,QSvgRenderer*> m_hRenderers;
};

SymbolicColorizer::SymbolicColorizer()
    : QQuickImageProvider(QQuickImageProvider::Pixmap), d_ptr(new SymbolicColorizerPrivate)
{}

SymbolicColorizer::~SymbolicColorizer()
{
    delete d_ptr;
}

QPixmap SymbolicColorizer::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{

    auto r = d_ptr->m_hRenderers.value(id);

    if (!r)
        d_ptr->m_hRenderers[id] = r = new QSvgRenderer(id);

    // Always honor the original aspect ratio
    const qreal aspectRatio = r->defaultSize().width()/r->defaultSize().height();
    const bool hOrW = r->defaultSize().width() >= r->defaultSize().height();

    size->setWidth(
        hOrW ? requestedSize.height()*aspectRatio : requestedSize.width()
    );

    size->setHeight(
        hOrW ? requestedSize.height() : requestedSize.width()*aspectRatio
    );

    QPixmap px(size->width(), size->height());

    QRect rect {0, 0, size->width(), size->height()};
    px.fill(Qt::transparent);

    QPainter painter(&px);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rect, QBrush(Qt::white));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    r->render(&painter, rect);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(rect, QGuiApplication::palette().text());

    return px;
}

#include <symboliccolorizer.moc>
