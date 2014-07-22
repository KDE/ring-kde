/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
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

#include "dlgdisplay.h"
#include "klib/kcfg_settings.h"

//KDE
#include <KConfigDialog>
#include <KLocale>

///Constructor
DlgDisplay::DlgDisplay(KConfigDialog *parent)
 : QWidget(parent),m_HasChanged(false)
{
   setupUi(this);
   kcfg_minimumRowHeight->setEnabled(ConfigurationSkeleton::limitMinimumRowHeight());


   //Need to be ordered
   m_lCallDetails[ i18n("Display Icon")         ] = "displayCallIcon"        ;
   m_lCallDetails[ i18n("Display Security")     ] = "displayCallSecure"      ;
   m_lCallDetails[ i18n("Display Call Number")  ] = "displayCallNumber"      ;
   m_lCallDetails[ i18n("Display Peer Name")    ] = "displayCallPeer"        ;
   m_lCallDetails[ i18n("Display organisation") ] = "displayCallOrganisation";
   m_lCallDetails[ i18n("Display department")   ] = "displayCallDepartment"  ;
   m_lCallDetails[ i18n("Display e-mail")       ] = "displayCallEmail"       ;

   QMutableMapIterator<QString, QString> iter(m_lCallDetails);
   while (iter.hasNext()) {
      iter.next();
      const bool checked = ConfigurationSkeleton::self()->findItem(iter.value())->isEqual(true);
      QListWidgetItem* i = new QListWidgetItem(i18n(iter.key().toAscii()));
      i->setFlags      (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
      i->setCheckState ((checked)?Qt::Checked:Qt::Unchecked        );
      m_pDetailsList->addItem(m_lItemList[iter.value()] = i);
   }
   kcfg_minimumRowHeight->setEnabled(ConfigurationSkeleton::limitMinimumRowHeight());
   connect(m_pDetailsList   , SIGNAL(itemChanged(QListWidgetItem*))  , this  , SLOT(changed())      );
   connect(this,SIGNAL(updateButtons()), parent , SLOT(updateButtons()));
}

///Destructor
DlgDisplay::~DlgDisplay()
{
}

///Have this dialog changed
bool DlgDisplay::hasChanged()
{
   return m_HasChanged;
}

///Tag this dialog as changed
void DlgDisplay::changed()
{
   m_HasChanged = true;
   emit updateButtons();
}

///Update all widgets
void DlgDisplay::updateWidgets()
{
}

///Save current settings
void DlgDisplay::updateSettings()
{
   QMutableMapIterator<QString, QString> iter(m_lCallDetails);
   while (iter.hasNext()) {
      iter.next();
      ConfigurationSkeleton::self()->findItem(iter.value())->setProperty(m_lItemList[iter.value()]->checkState() == Qt::Checked);
   }

   m_HasChanged = false;
}
