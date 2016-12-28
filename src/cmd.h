/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#ifndef CMD_H
#define CMD_H

#include <QtCore/QObject>

//KDE
class KAboutData;

//Ring
class Call;

class Cmd : public QObject
{
   Q_OBJECT

public:
   virtual ~Cmd(){}
   static Cmd* instance();
   static void parseCmd(int argc, char **argv, KAboutData& about);

private:

   //Static mutators
   static void placeCall(const QString& number);
   static void sendText(const QString& number, const QString& text);
   static void iconify();
   //Private constructor
   explicit Cmd(QObject* parent=nullptr);


   //Attributes
   static Cmd* m_spSelf;

public Q_SLOTS:
   void slotActivateActionRequested (const QString &actionName, const QVariant &parameter);
   void slotActivateRequested (const QStringList &arguments, const QString &workingDirectory);
   void slotOpenRequested (const QList< QUrl > &uris);
};

#endif
