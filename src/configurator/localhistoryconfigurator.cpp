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
#include "localhistoryconfigurator.h"
#include "conf/dlggeneral.h"

#include <categorizedhistorymodel.h>

#include <KLocalizedString>

#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>

LocalHistoryConfigurator::LocalHistoryConfigurator(QObject* parent) : CollectionConfigurationInterface(parent),m_pDialog(nullptr)
{
}

QByteArray LocalHistoryConfigurator::id() const
{
   return "localHistoryConfigurator";
}

QString LocalHistoryConfigurator::name() const
{
   return i18n("Local history configurator");
}

QVariant LocalHistoryConfigurator::icon() const
{
   return QVariant();
}

void LocalHistoryConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
   Q_UNUSED(col)
   if (parent && qobject_cast<QWidget*>(parent)) {
      QWidget* w = qobject_cast<QWidget*>(parent);
      if (!m_pDialog) {
         m_pDialog = new DlgGeneral(nullptr);
         QHBoxLayout* l = new QHBoxLayout(w);
         l->addWidget(m_pDialog);

         m_pDialog->m_pKeepHistory->setChecked(CategorizedHistoryModel::instance().isHistoryLimited());
         m_pDialog->m_pHistoryMax->setValue(CategorizedHistoryModel::instance().historyLimit());

         connect(m_pDialog->m_pKeepHistory, &QCheckBox::toggled, &CategorizedHistoryModel::instance(), &CategorizedHistoryModel::setHistoryLimited);
         connect(m_pDialog->m_pHistoryMax , SIGNAL(valueChanged(int)), &CategorizedHistoryModel::instance(), SLOT(setHistoryLimit(int)));
         connect(m_pDialog, &DlgGeneral::updateButtons, [this]() {
            emit this->changed();
         });
      }
   }
}
