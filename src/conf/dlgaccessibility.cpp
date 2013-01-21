/****************************************************************************
 *   Copyright (C) 2009 by Savoir-Faire Linux                               *
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

#include "dlgaccessibility.h"
#include "klib/configurationskeleton.h"

//KDE
#include <KConfigDialog>
#include <KLocale>

///Constructor
DlgAccessibility::DlgAccessibility(KConfigDialog* parent)
 : QWidget(parent),m_Changed(false)
{
   setupUi(this);

   m_pAddTB->setIcon    ( KIcon( "list-add"    ) );
   m_pRemoveTB->setIcon ( KIcon( "list-remove" ) );
   m_pInfoIconL->setPixmap(KIcon("dialog-information").pixmap(QSize(24,24)));
   m_pInfoL->setText(i18n("This page allow to create macros that can then be called using keybooard shortcuts or added to the toolbar. To create one, \
   assign a name and a character sequence. The sequence can be numeric or any character than can be interpretted as one (ex: \"A\" would be interpretted as 2)"));

   connect(m_pNameLE        , SIGNAL(textChanged(QString)) , this,SLOT(changed()) );
   connect(m_pCategoryCBB   , SIGNAL(textChanged(QString)) , this,SLOT(changed()) );
   connect(m_pDelaySB       , SIGNAL(valueChanged(int))    , this,SLOT(changed()) );
   connect(m_pSequenceLE    , SIGNAL(textChanged(QString)) , this,SLOT(changed()) );
   connect(m_pDescriptionLE , SIGNAL(textChanged(QString)) , this,SLOT(changed()) );

   connect(this , SIGNAL(updateButtons()) , parent,SLOT(updateButtons()) );
}

///Destructor
DlgAccessibility::~DlgAccessibility()
{
   
}

///Save
void DlgAccessibility::updateSettings()
{
   
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