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
#ifndef RINGAPPLICATION_H
#define RINGAPPLICATION_H

#include <QtWidgets/QApplication>

//Qt
class QEvent;
class QQmlApplicationEngine;

//Ring
class MainWindow;

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
   Q_INVOKABLE virtual int newInstance();

   // Exit gracefully
   virtual bool notify (QObject* receiver, QEvent* e) override;

   //Getter
   bool startIconified() const;

   static QQmlApplicationEngine* engine();

   //Setter
   void setIconify(bool iconify);

private:
   //Attributes
   MainWindow*        m_pApp          ;
   bool               m_StartIconified;
};

#endif // RINGAPPLICATION_H

// kate: space-indent on; indent-width 3; replace-tabs on;
