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
#include "dialdock.h"

#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QMimeData>
#include <QtCore/QTimer>
#include <QtCore/QSortFilterProxyModel>
#include <QQmlContext>

#include <../ringapplication.h>
#include "peerstimelinemodel.h"
#include <contactmethod.h>
#include <person.h>
#include <call.h>
#include <callmodel.h>

// #include "qrc_dialview.cpp"
// #include "src/contactview/qrc_contactview.cpp" //FIXME

class DialDockPrivate {
public:
    QQuickWidget* m_pQuickWidget;
};

DialDock::DialDock(QWidget* parent) :
    QDockWidget(parent), d_ptr(new DialDockPrivate)
{
    d_ptr->m_pQuickWidget = new QQuickWidget(RingApplication::engine(), this);
    setWidget(d_ptr->m_pQuickWidget);

    d_ptr->m_pQuickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    d_ptr->m_pQuickWidget->setSource(QUrl(QStringLiteral("qrc:/ContactList.qml")));
}

DialDock::~DialDock()
{
    setWidget(nullptr);

    d_ptr->m_pQuickWidget->setVisible(false);
    d_ptr->m_pQuickWidget->hide();
    d_ptr->m_pQuickWidget->setParent(nullptr);

    delete d_ptr;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
