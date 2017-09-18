/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "devices.h"

// Qt
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QtWidgets/QHBoxLayout>

// Ring
#include <account.h>
#include <ringapplication.h>

Pages::Devices::Devices(QWidget* parent) : QWidget(parent),m_pAccount(nullptr)
{
    m_pWidget = new QQuickWidget(RingApplication::engine(), this);

    for (auto w : {static_cast<QWidget*>(this), static_cast<QWidget*>(m_pWidget)})
        w->setStyleSheet(QStringLiteral("margin:0px; spacing:0px; padding:0px;"));

    auto l = new QHBoxLayout(this);
    l->setContentsMargins(0,0,0,0);
    l->addWidget(m_pWidget);

    m_pWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_pWidget->setSource(QUrl(QStringLiteral("qrc:/Devices.qml")));
}

void Pages::Devices::setAccount(Account* a)
{
    m_pWidget->rootObject()->setProperty("account", QVariant::fromValue(a));
    m_pAccount = a;
}
