/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QtWidgets/QSystemTrayIcon>

//KDE
class QAction;

//Qt
class QMenu;
class QIcon;

///SysTray: The old system try, should be totally replaced by a plasmoid some day
class SysTray : public QSystemTrayIcon
{
Q_OBJECT

public:
   //Constructor
   explicit SysTray(const QIcon &icon, QWidget *parent = 0);
   ~SysTray();

   //Mutators
   void addAction(QAction *action);
   void addSeparator();

private:
   //Attributes
   QMenu* m_pTrayIconMenu;

private Q_SLOTS:
   void slotActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // SYSTRAY_H
