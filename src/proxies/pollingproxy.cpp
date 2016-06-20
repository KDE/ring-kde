/****************************************************************************
 *   Copyright (C) 2016 by Emmanuel Lepage Vallee                           *
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
#include "pollingproxy.h"

// Qt
#include <QtCore/QTimer>
#include <QtCore/QDebug>

PollingProxy::PollingProxy(QObject* parent) : QIdentityProxyModel(parent),
m_pTimer(new QTimer(this))
{
   m_pTimer->setInterval(1000);
   m_pTimer->start();
   connect(m_pTimer, &QTimer::timeout, this, &PollingProxy::pollSlot);
}

PollingProxy::PollingProxy(QAbstractItemModel* model) : PollingProxy(
   static_cast<QObject*>(model)
)
{
   setSourceModel(model);
}

void PollingProxy::pollSlot()
{
   emit dataChanged(index(0,0), index(rowCount(), columnCount()));
}

//kate: space-indent on; indent-width 3; replace-tabs on;
