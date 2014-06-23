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

#ifndef DLGHOOKS_H
#define DLGHOOKS_H

#include <QWidget>

#include "ui_dlghooksbase.h"

/**
	@author Jérémy Quentin <jeremy.quentin@gmail.com>
*/
class DlgHooks : public QWidget, public Ui_DlgHooksBase
{
Q_OBJECT
public:
   //Constructor
   explicit DlgHooks(QWidget *parent = nullptr);

   //Destructor
   ~DlgHooks();

   bool hasChanged();

private:
   bool m_Changed;

public Q_SLOTS:
   void updateWidgets();
   void updateSettings();

private Q_SLOTS:
   void setChanged();

Q_SIGNALS:
   void updateButtons();

};

#endif
