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
#include "ringingimageprovider.h"

// LibStdC++
#include <cmath>

// Qt
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtGui/QColor>
#include <QtGui/QPalette>
#include <QtGui/QPainter>
#include <QtGui/QGuiApplication>
#include <QtCore/QIODevice>
#include <QtSvg/QSvgRenderer>

struct RingingTipData;
struct InitTipData;
struct SearchingTipData;

class RingingImageProviderPrivate
{
public:
    // Attributes
    RingingTipData*   m_pRing   {nullptr};
    InitTipData*      m_pInit   {nullptr};
    SearchingTipData* m_pSearch {nullptr};

    QByteArray loadSvg(const QString& path);
    bool brightOrDarkBase() const;
};

struct RingingTipData
{
    explicit RingingTipData(RingingImageProviderPrivate* d) {

        static const QRectF m_Rect[4] = {
            {0 , 0 , 19.319489*6 , 4.73458*7          },
            {0 , 0 , 13.757887*6 , 3.7651761*7        },
            {0 , 0 , 9.3203869*6 , 2.9834957*7        },
            {0 , 0 , 135         , 135*0.346975929367 }
        };

        int i=0;
        for (auto s : {
            QStringLiteral(":/canvasassets/Ring3.svg"),
            QStringLiteral(":/canvasassets/Ring2.svg"),
            QStringLiteral(":/canvasassets/Ring1.svg"),
            QStringLiteral(":/canvasassets/PhoneDown.svg")
        }) {
            auto content = d->loadSvg(s);
            QSvgRenderer r(content);

            QPainter p2;
            p2.begin(&m_pPhonePix[i]);
            p2.setCompositionMode(QPainter::CompositionMode_Clear);
            p2.fillRect(m_Rect[i], QBrush(Qt::white));
            p2.setCompositionMode(QPainter::CompositionMode_SourceOver);
            r.render(&p2, m_Rect[i]);
            p2.end();
            i++;
        }
    };

    QPixmap toPixmap(int count) {
        QPixmap pxm(135, 120);

        QPainter p(&pxm);

        static QPointF rects[4] {
            {0                   , 75},
            {(135-9.3203869*6)/2 , 50},
            {(135-13.757887*6)/2 , 25},
            {(135-19.319489*6)/2 , 0 },
        };

        const float opacity[4] {
            1,
            (sin((float)(count/35.0f)*2.0f*3.14159f + 5 + (2*1.0472f)) +  0.8),
            (sin((float)(count/35.0f)*2.0f*3.14159f + 5 + (1.0472f  )) +  0.8),
            (sin((float)(count/35.0f)*2.0f*3.14159f + 5              ) +  0.8),
        };

        // For now only black background is used
        p.fillRect(0,0,135,120, Qt::black);

        for (int i = 0; i < 4; i++) {
            p.setOpacity(opacity[i]);
            p.drawPixmap(rects[i] , m_pPhonePix[3-i]);
        }

        return pxm;
    }

    QPixmap m_pPhonePix[4] {
        QPixmap(19.319489*6 , 4.73458*7         ),
        QPixmap(13.757887*6 , 3.7651761*7       ),
        QPixmap(9.3203869*6 , 2.9834957*7       ),
        QPixmap(135         , 135*0.346975929367),
    };
};

struct InitTipData final {
    explicit InitTipData(RingingImageProviderPrivate* d) {
        auto content = d->loadSvg(QStringLiteral(":/canvasassets/Searching.svg"));
        m_Render = new QSvgRenderer(content);
    }
    ~InitTipData() {delete m_Render;}

    QPixmap toPixmap(int count) {
        QPixmap pxm(128, 128);

        QPainter p(&pxm);
        p.fillRect(QRect{0,0,128,128},QColor("#0000ff"));
        m_Render->render(&p, {0,0,128,128});

        return pxm;
    }

    QSvgRenderer* m_Render;
};

struct SearchingTipData {
    explicit SearchingTipData(RingingImageProviderPrivate* d) {
        //
    }
};

RingingImageProvider::RingingImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap), d_ptr(new RingingImageProviderPrivate)
{
}

QPixmap RingingImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize)

    auto conf = id.split('/');

    Q_ASSERT(conf.size() == 2);

    QPixmap pxm;
    const int count = conf[1].toInt();

    if (conf[0] == QLatin1String("ringing")) {
        if (!d_ptr->m_pRing)
            d_ptr->m_pRing = new RingingTipData(d_ptr);
        pxm = d_ptr->m_pRing->toPixmap(count);
    }
    else if (conf[0] == QLatin1String("init")) {
        if (!d_ptr->m_pInit)
            d_ptr->m_pInit = new InitTipData(d_ptr);
        pxm = d_ptr->m_pInit->toPixmap(count);
        qDebug() << "\n\nPAINT" << conf[0];
    }

    (*size) = pxm.size();

    return pxm;
}

QByteArray RingingImageProviderPrivate::loadSvg(const QString& path)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray content = file.readAll();
    content.replace("BACKGROUD_COLOR_ROLE",brightOrDarkBase()?"#000000":"#ffffff");
    content.replace("BASE_ROLE_COLOR",QGuiApplication::palette().base().color().name().toLatin1());
    file.close();

    return content;
}

///Check if the theme color scheme is darker than #888888
///@return true = dark, false = bright
bool RingingImageProviderPrivate::brightOrDarkBase() const
{
    //QColor color = QGuiApplication::palette().base().color();
    //return (color.red() > 128 && color.green() > 128 && color.blue() > 128);
    return false;
}
