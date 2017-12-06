/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#include <klocalizedstring.h>

//LRC
#include <categorizedcontactmodel.h>

//Ring
#include "phonewindow.h"

///Constructor
DlgDisplay::DlgDisplay(KConfigDialog *parent)
 : QWidget(parent),m_HasChanged(false)
{
   setupUi(this);

   connect(this,SIGNAL(updateButtons()), parent , SLOT(updateButtons()));

//FIXME DROP QTWIDGET
//    PhoneWindow::app()->isAutoStart();
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
//FIXME DROP QTWIDGET
//    kcfg_autoStart->setChecked(PhoneWindow::app()->isAutoStart());
}

///Save current settings
void DlgDisplay::updateSettings()
{
   if (ConfigurationSkeleton::autoStart() != kcfg_autoStart->isChecked()) {
      ConfigurationSkeleton::setAutoStartOverride(true);
   }

   if (ConfigurationSkeleton::hideUnreachable() != kcfg_hideUnreachable->isChecked())
      CategorizedContactModel::instance().setUnreachableHidden(kcfg_hideUnreachable->isChecked());

//FIXME DROP QTWIDGET
//    PhoneWindow::app()->setAutoStart(kcfg_autoStart->isChecked());

   m_HasChanged = false;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
