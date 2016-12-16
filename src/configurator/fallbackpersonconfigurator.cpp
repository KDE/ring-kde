/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
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
#include "fallbackpersonconfigurator.h"
#include "ui_dlgfallbackperson.h"

#include <KLocalizedString>

FallbackPersonConfigurator::FallbackPersonConfigurator(QObject* parent) : CollectionConfigurationInterface(parent),m_pDialog(nullptr)
{

}

QByteArray FallbackPersonConfigurator::id() const
{
   return "fallbackPersonConfigurator";
}

QString FallbackPersonConfigurator::name() const
{
   return i18n("Fallback person configurator");
}

QVariant FallbackPersonConfigurator::icon() const
{
   return QVariant();
}

void FallbackPersonConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
   Q_UNUSED(col)
   if (parent && qobject_cast<QWidget*>(parent)) {
      QWidget* w = qobject_cast<QWidget*>(parent);
      if (!m_pDialog) {
         m_pDialog = new QWidget();
         Ui_DlgFallbackPerson* ui = new Ui_DlgFallbackPerson();
         ui->setupUi(m_pDialog);
         QHBoxLayout* l = new QHBoxLayout(w);
         l->addWidget(m_pDialog);
      }
   }
}
