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
#include "dlghooks.h"

#include <hookmanager.h>

///Constructor
DlgHooks::DlgHooks(QWidget *parent)
 : QWidget(parent),m_Changed(false)
{
   setupUi(this);
   updateWidgets();
}

///Destructor
DlgHooks::~DlgHooks()
{
}

void DlgHooks::updateWidgets()
{
   kcfg_enableHooksSIP->blockSignals(true);
   kcfg_hooksCommand->blockSignals(true);
   kcfg_prepend->blockSignals(true);
   kcfg_hooksSIPHeader->blockSignals(true);
   kcfg_addPrefix->blockSignals(true);
   kcfg_enableHooksSIP  ->setChecked(HookManager::instance().isSipEnabled        ());
   kcfg_hooksCommand    ->setText   (HookManager::instance().command             ());
   kcfg_prepend         ->setText   (HookManager::instance().prefix              ());
   kcfg_hooksSIPHeader  ->setText   (HookManager::instance().sipFeild            ());
   kcfg_addPrefix       ->setChecked(HookManager::instance().isContactMethodEnabled());
   kcfg_enableHooksSIP->blockSignals(false);
   kcfg_hooksCommand->blockSignals(false);
   kcfg_prepend->blockSignals(false);
   kcfg_hooksSIPHeader->blockSignals(false);
   kcfg_addPrefix->blockSignals(false);
}

void DlgHooks::updateSettings()
{
   HookManager::instance().setSipEnabled        (kcfg_enableHooksSIP  ->isChecked());
   HookManager::instance().setCommand           (kcfg_hooksCommand    ->text     ());
   HookManager::instance().setPrefix            (kcfg_prepend         ->text     ());
   HookManager::instance().setSipFeild          (kcfg_hooksSIPHeader  ->text     ());
   HookManager::instance().setContactMethodEnabled(kcfg_addPrefix       ->isChecked());
   m_Changed = false;
   emit updateButtons();
}

void DlgHooks::setChanged()
{
   m_Changed = true;
   emit updateButtons();
}

bool DlgHooks::hasChanged()
{
   return m_Changed;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
