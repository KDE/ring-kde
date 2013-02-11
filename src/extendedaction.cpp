/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
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
#include "extendedaction.h"

ExtendedAction::ExtendedAction(QObject* parent) : KAction(parent),m_pIcon(nullptr)
{
   connect(this,SIGNAL(changed()),this,SLOT(hasChanged()));
}

ExtendedAction::~ExtendedAction()
{
   if (m_pIcon) delete m_pIcon;
}

const KIcon& ExtendedAction::altIcon()
{
   return (const KIcon&) *m_pIcon;
}

void ExtendedAction::setAltIcon(QString path)
{
   m_pIcon = new KIcon(path);
}

void ExtendedAction::setText(const QString& newText)
{
   QAction::setText(newText);
   emit textChanged(newText);
}

void ExtendedAction::hasChanged()
{
   
}