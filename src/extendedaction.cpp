/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
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
#include <QIcon>

ExtendedAction::ExtendedAction(QObject* parent) : QAction(parent)
{
   connect(this,SIGNAL(changed()),this,SLOT(hasChanged()));
}

ExtendedAction::~ExtendedAction()
{
}

const QIcon& ExtendedAction::altIcon()
{
   return m_Icon;
}

void ExtendedAction::setAltIcon(const QString &path)
{
   m_Icon = QIcon::fromTheme(path);
}

void ExtendedAction::setAltIcon(const QIcon &icon)
{
   m_Icon = icon;
}

void ExtendedAction::setText(const QString& newText)
{
   QAction::setText(newText);
   emit textChanged(newText);
}

void ExtendedAction::hasChanged()
{

}
