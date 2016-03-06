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
#ifndef ACCESSIBILITY_H
#define ACCESSIBILITY_H

#include <QtCore/QList>
#include <QAction>
class QTextToSpeech;

///Accessibility: Functions to improve accessibility for disabled peoples
class Accessibility : public QObject, public QList<QAction *>
{
   Q_OBJECT
public:
   //Singleton
   static Accessibility* instance();
   virtual ~Accessibility();
private:
   //Constructor
   explicit Accessibility();
   QString numberToDigit(const QString &number);

   //Attributes
   QTextToSpeech* m_Speech;

   //Static attributes
   static Accessibility* m_pInstance;

public Q_SLOTS:
   void listCall();
   void currentCallDetails();
   void say(const QString &message);
};

#endif
