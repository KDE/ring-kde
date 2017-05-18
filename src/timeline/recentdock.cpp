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
#include "recentdock.h"

#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtCore/QMimeData>

#include <qrc_recentdock.cpp>
#include <../ringapplication.h>

#include "peerstimelinemodel.h"
#include "contactmethod.h"
#include "itemdataroles.h"

class ModelIconBinder2 : public QQuickPaintedItem
{
   Q_OBJECT
   Q_PROPERTY(QVariant pixmap READ pixmap WRITE setPixmap)

public:
    explicit ModelIconBinder2(QQuickItem* parent = nullptr) : QQuickPaintedItem(parent) {}

    QPixmap pixmap() const {
        return m_pixmap;
    }
    void setPixmap(const QVariant& var) {
        m_pixmap = qvariant_cast<QPixmap>(var);
        m_icon   = qvariant_cast<QIcon  >(var);
    }

    virtual void paint(QPainter *painter) override {
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

private:
    QPixmap m_pixmap;
    QIcon   m_icon  ;
};

class RecentDockPrivate {
public:
    QQuickWidget* m_pQuickWidget;
};

RecentDock::RecentDock(QWidget* parent) :
    QDockWidget(parent), d_ptr(new RecentDockPrivate)
{
    qmlRegisterType<ModelIconBinder2>("Ring", 1,0, "ModelIconBinder2");
    d_ptr->m_pQuickWidget = new QQuickWidget(RingApplication::engine(), this);

    d_ptr->m_pQuickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    d_ptr->m_pQuickWidget->setSource(QUrl("qrc:/peerstimeline.qml"));

    setWidget(d_ptr->m_pQuickWidget);
    setAcceptDrops(true);
    installEventFilter(this);

    QObject *item = d_ptr->m_pQuickWidget->rootObject();
    connect(item, SIGNAL(contactMethodSelected(QVariant)),
        this, SLOT(slotViewContactMethod(QVariant)));
}

RecentDock::~RecentDock()
{
    delete d_ptr;
}

bool RecentDock::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    Q_UNUSED(event)
    //TODO the context menu
    return false;
}

void RecentDock::slotViewContactMethod(const QVariant& cm)
{
    const int idx = cm.toInt();
    emit viewContactMethod(
        qvariant_cast<ContactMethod*>(
            PeersTimelineModel::instance().index(idx, 0).data((int)Ring::Role::Object)
        )
    );
}

#include <recentdock.moc>
// kate: space-indent on; indent-width 4; replace-tabs on;
