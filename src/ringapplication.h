/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#ifndef RINGAPPLICATION_H
#define RINGAPPLICATION_H

#include <QApplication>

//Qt
class QEvent;

//Ring
class Ring;

///RingApplication: Main application
class RingApplication : public QApplication
{
  Q_OBJECT

public:
   // Constructor
   RingApplication(int & argc, char ** argv);

   // Destructor
   virtual ~RingApplication();

   // Manage new instances
   virtual int newInstance();

   // Exit gracefully
   virtual bool notify (QObject* receiver, QEvent* e);

private:
   //Init
   void initializeMainWindow();
   void initializePaths();

   //Attributes
   static Ring* m_spApp;
};

#endif // RINGAPPLICATION_H
