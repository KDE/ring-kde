/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#include "dlgaddressbook.h"

#include "klib/kcfg_settings.h"
#include "lib/numbercategorymodel.h"
#include "delegates/autocompletiondelegate.h"

///Constructor
DlgAddressBook::DlgAddressBook(KConfigDialog* parent)
 : QWidget(parent),m_HasChanged(false)
{
   setupUi(this);
   m_pPhoneTypeList->setModel(NumberCategoryModel::instance());
   m_pDelegate = new AutoCompletionDelegate();
   m_pPhoneTypeList->setItemDelegate(m_pDelegate);
   connect(m_pPhoneTypeList, SIGNAL(itemChanged(QListWidgetItem*)), this   , SLOT(changed())      );
   connect(this            , SIGNAL(updateButtons())              , parent , SLOT(updateButtons()));
} //DlgAddressBook

///Destructor
DlgAddressBook::~DlgAddressBook()
{
   m_pPhoneTypeList->setItemDelegate(nullptr);
   delete m_pDelegate;
}

///Reload the widget
void DlgAddressBook::updateWidgets()
{
   
}

///Save the settings
void DlgAddressBook::updateSettings()
{
   NumberCategoryModel::instance()->save();
   m_HasChanged = false;
}

///Tag this dialog as changed
void DlgAddressBook::changed()
{
   m_HasChanged = true;
   emit updateButtons();
}

///If the "Apply" button need to be enabled
bool DlgAddressBook::hasChanged()
{
   return m_HasChanged;
}
