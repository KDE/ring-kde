/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "localhistoryconfigurator.h"
#include "conf/dlggeneral.h"

LocalHistoryConfigurator::LocalHistoryConfigurator(QObject* parent) : CollectionConfigurationInterface(parent),m_pDialog(nullptr)
{

}

QByteArray LocalHistoryConfigurator::id() const
{
   return "localHistoryConfigurator";
}

QString LocalHistoryConfigurator::name() const
{
   return tr("Local history configurator");
}

QVariant LocalHistoryConfigurator::icon() const
{
   return QVariant();
}

void LocalHistoryConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
   qDebug() << "\n\n\n\n\nWORKS!!!!!" << col << parent;
   if (parent && qobject_cast<QWidget*>(parent)) {
      QWidget* w = qobject_cast<QWidget*>(parent);
      if (!m_pDialog) {
      qDebug() << "IN";
         m_pDialog = new DlgGeneral(nullptr);
         QHBoxLayout* l = new QHBoxLayout(w);
         l->addWidget(m_pDialog);
      }
   }
}
