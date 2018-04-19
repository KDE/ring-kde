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
#include <QtGui/QIcon>
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

    QPixmap paintSvg(const QString &id, QSize *size, const QSize &requestedSize, const QColor& col);
    QPixmap paintTheme(const QString &id, QSize *size, const QSize &requestedSize, const QColor& col);
};

SymbolicColorizer::SymbolicColorizer()
    : QQuickImageProvider(QQuickImageProvider::Pixmap), d_ptr(new SymbolicColorizerPrivate)
{}

SymbolicColorizer::~SymbolicColorizer()
{
    delete d_ptr;
}

// Paint a rqc or local SVG file.
QPixmap SymbolicColorizerPrivate::paintSvg(const QString &id, QSize *size, const QSize &requestedSize, const QColor& color)
{
    auto r = m_hRenderers.value(id);

    if (!r)
        m_hRenderers[id] = r = new QSvgRenderer(id);

    // Always honor the original aspect ratio
    const qreal aspectRatio = r->defaultSize().width()/r->defaultSize().height();
    const bool hOrW = r->defaultSize().width() >= r->defaultSize().height();

    size->setWidth(
        hOrW ? requestedSize.height()*aspectRatio : requestedSize.width()
    );

    size->setHeight(
        hOrW ? requestedSize.height() : requestedSize.width()*aspectRatio
    );

    // avoid 150 qpainter warnings being printed on stderr
    if (size->width() == -1 || size->height() == -1) {
        qWarning() << "Failed to render due to an invalid size" << id;
        return {};
    }

    QPixmap px(size->width(), size->height());

    QRect rect {0, 0, size->width(), size->height()};
    px.fill(Qt::transparent);

    QPainter painter(&px);

    if (!painter.isActive()) {
        qWarning() << "Failed to create painter" << id;
        return {};
    }

    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rect, QBrush(Qt::white));
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    r->render(&painter, rect);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(rect, color);

    return px;
}

QPixmap SymbolicColorizerPrivate::paintTheme(const QString &id, QSize *size, const QSize &requestedSize, const QColor& col)
{
    const auto realId = id.right(id.size()-13);

    const auto icn = QIcon::fromTheme(realId);

    auto px = icn.pixmap(requestedSize.width(), requestedSize.height());
    size->setWidth (px.width ());
    size->setHeight(px.height());

    QPainter painter(&px);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(0, 0, size->width(), size->height(), col);

    return px;
}

QPixmap SymbolicColorizer::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString realId = id;

    QColor color = QGuiApplication::palette().text().color();

    if (realId.size() && realId.left(7) == QLatin1String("?color=")) {
        const auto s = realId.split(';');
        realId = s[1];
        color = QColor(s[0].mid(7, s[0].size()-7));
    }

    if (realId.left(8) != QLatin1String("image://"))
        return d_ptr->paintSvg(realId, size, requestedSize, color);
    else if (realId.mid(8, 4) == QLatin1String("icon"))
        return d_ptr->paintTheme(realId, size, requestedSize, color);
    else
        Q_ASSERT(false);

    return {};
}

#include <symboliccolorizer.moc>
