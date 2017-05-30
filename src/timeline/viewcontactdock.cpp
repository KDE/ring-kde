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
#include "viewcontactdock.h"

#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QMimeData>

#include <../ringapplication.h>
#include <contactmethod.h>

#include "recentmodel.h"

class ViewContactDockPrivate {
public:
    QQuickWidget* m_pQuickWidget;
    QSharedPointer<QAbstractItemModel> m_CallsModel;
    QSharedPointer<QAbstractItemModel> m_TimelineModel;
};

ViewContactDock::ViewContactDock(QWidget* parent) :
    QWidget(parent), d_ptr(new ViewContactDockPrivate)
{
    d_ptr->m_pQuickWidget = new QQuickWidget(RingApplication::engine(), this);

    d_ptr->m_pQuickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    d_ptr->m_pQuickWidget->setSource(QUrl("qrc:/viewcontact.qml"));

    auto l = new QHBoxLayout(this);
    l->addWidget(d_ptr->m_pQuickWidget);
    setAcceptDrops(true);
    installEventFilter(this);
}

ViewContactDock::~ViewContactDock()
{
    delete d_ptr;
}

bool ViewContactDock::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    Q_UNUSED(event)
    //TODO the context menu
    return false;
}

void ViewContactDock::setContactMethod(ContactMethod* cm)
{
    // Keep a strong reference because QML wont
    d_ptr->m_CallsModel = cm->callsModel();
    d_ptr->m_pQuickWidget->rootObject()->setProperty(
        "currentContactMethod", QVariant::fromValue(cm));
}

#include <viewcontactdock.moc>
// kate: space-indent on; indent-width 4; replace-tabs on;
