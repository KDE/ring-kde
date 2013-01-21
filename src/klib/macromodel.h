/****************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#ifndef MACRO_MODEL_H
#define MACRO_MODEL_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include "../lib/typedefs.h"

//KDE
class KAction;

//SFLPhone
class Contact;


#include <QDebug> //TODO remove
class LIB_EXPORT Macro : public QObject {
   Q_OBJECT
public:
   Macro(QObject* parent = nullptr);
   QString name;
   QString description;
   QString sequence;
   QString escaped;
   int     delay;
   QString category;
   KAction* action;
private:
   int position;
public slots:
   void execute();
private slots:
   void nextStep();
};

///Interface to interpret DTMFs instead of using the daemon directly
class LIB_EXPORT MacroListener {
public:
   virtual void addDTMF(const QString& sequence) = 0;
   virtual ~MacroListener() {}
};

///AkonadiBackend: Implement a backend for Akonadi
class LIB_EXPORT MacroModel : public QObject {
   Q_OBJECT
   friend class Macro;
public:
   static MacroModel* getInstance();
   static void addListener(MacroListener* interface);

   enum MacroFields {
      Name        = 100,
      Category    = 101,
      Delay       = 102,
      Description = 103,
      Sequence    = 104
   };

   void addMacro(Macro* macro);

private:
   //Singleton constructor
   MacroModel(QObject* parent);

   //Attributes
   static MacroModel*         m_pInstance  ;

private:
   QHash<QString,Macro*> m_hMacros;
   QList<MacroListener*> m_lListeners;

signals:
   void addAction(KAction*);
};

#endif
