/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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

//Parent
#include "ringapplication.h"

//Qt
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtCore/QCommandLineParser>

//KDE
#include <KIconLoader>
#include <QStandardPaths>
#include <KMainWindow>
#include <QDebug>
#include <kmessagebox.h>


//Ring
#include "klib/kcfg_settings.h"
#include "cmd.h"
#include "ring.h"
#include "errormessage.h"
#include "callmodel.h"
#include "implementation.h"

//Other
#include <unistd.h>

Ring* RingApplication::m_spApp = nullptr;

/**
 * The application constructor
 */
RingApplication::RingApplication(int & argc, char ** argv) : QApplication(argc,argv)
{

#ifdef ENABLE_VIDEO
   //Necessary to draw OpenGL from a separated thread
   setAttribute(Qt::AA_X11InitThreads,true);
#endif
}

/**
 * Destructor
 */
RingApplication::~RingApplication()
{
   delete Ring::app();
}

///Parse command line arguments
int RingApplication::newInstance()
{
   static bool init = true;
   //Only call on the first instance
   if (init) {
      init = false;
      m_spApp = new Ring();
      if( ! m_spApp->initialize() ) {
         return 1;
      };

      if (ConfigurationSkeleton::displayOnStart())
         m_spApp->show();
      else
         m_spApp->hide();
   }

   return 0;
}

///Exit gracefully
bool RingApplication::notify (QObject* receiver, QEvent* e)
{
   try {
      return QApplication::notify(receiver,e);
   }
   catch (const Call::State& state) {
      qDebug() << ErrorMessage::GENERIC_ERROR << "CallState" << state;
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   catch (const Call::Action& state) {
      qDebug() << ErrorMessage::GENERIC_ERROR << "Call Action" << state;
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   catch (const QString& errorMessage) {
      KMessageBox::error(Ring::app(),errorMessage);
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   catch (...) {
      qDebug() << ErrorMessage::GENERIC_ERROR;
      KMessageBox::error(Ring::app(),ErrorMessage::GENERIC_ERROR);
      QTimer::singleShot(2500,Ring::app(),SLOT(timeout()));
   }
   return false;
}
#include "ringapplication.moc"
