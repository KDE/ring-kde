/***************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
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
#include "personselector.h"

// Qt
#include <QtCore/QSortFilterProxyModel>

#include <personmodel.h>
#include <contactmethod.h>

PersonSelector::PersonSelector(QWidget* parent, const ContactMethod* cm) : QDialog(parent)
{
   setupUi(this);

   m_pSortedContacts = new QSortFilterProxyModel(this);
   m_pSortedContacts->setSourceModel          ( &PersonModel::instance()  );
   m_pSortedContacts->setSortRole             ( Qt::DisplayRole           );
   m_pSortedContacts->setFilterCaseSensitivity( Qt::CaseInsensitive       );
   m_pSortedContacts->setSortCaseSensitivity  ( Qt::CaseInsensitive       );

   if (cm) {
      m_pNearMatchContact = new QSortFilterProxyModel(this);
      m_pNearMatchContact->setSourceModel      ( &PersonModel::instance() );
      m_pNearMatchContact->setFilterFixedString( cm->primaryName()        );
   }

   nearMatch->setVisible(cm != nullptr && m_pNearMatchContact->rowCount());

   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), m_pSortedContacts, SLOT(setFilterRegExp(QString))   );

   listView->setModel(m_pSortedContacts);
}

// kate: space-indent on; indent-width 3; replace-tabs on;
