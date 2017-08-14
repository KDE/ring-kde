/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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

#include "dlgaccessibility.h"
#include "klib/kcfg_settings.h"

//Qt
#include <QtWidgets/QStyledItemDelegate>
#include <QtGui/QIcon>

//KDE
#include <KConfigDialog>
#include <klocalizedstring.h>

//Ring
#include "phonewindow.h"
#include "delegates/categorizeddelegate.h"

///Constructor
DlgAccessibility::DlgAccessibility(KConfigDialog* parent)
 : QWidget(parent),m_Changed(false)
{
   setupUi(this);

#ifdef HAVE_SPEECH
   m_pT2SBG->setEnabled(true);
#endif
}

///Destructor
DlgAccessibility::~DlgAccessibility()
{
}

///Save
void DlgAccessibility::updateSettings()
{
   m_Changed = false;
   emit updateButtons();
}

///Load
void DlgAccessibility::updateWidgets()
{
   
}

void DlgAccessibility::changed()
{
   m_Changed = true;
   emit updateButtons();
}

bool DlgAccessibility::hasChanged()
{
   return m_Changed;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
