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
#ifndef MACRO_H
#define MACRO_H

#include <QtCore/QObject>
#include <QtCore/QModelIndex>
#include "../lib/typedefs.h"

//Qt
class KAction;

//SFLPhone
#include "macromodel.h"

class LIB_EXPORT Macro : public QObject {
   Q_OBJECT
   friend class MacroModel; //Use factory method

protected:
   MacroModel::MacroCategory* m_pCat;

public:
   Macro(const Macro* macro);

   //Getters
   QString  name();
   QString  description();
   QString  sequence();
   QString  escaped();
   QString  id();
   int      delay();
   QString  category();
   KAction* action();

   QModelIndex index();

   //Setters
   void setName(QString value);
   void setDescription(QString value);
   void setSequence(QString value);
   void setEscaped(QString value);
   void setId(QString value);
   void setDelay(int value);
   void setCategory(QString value);
   
private:
   explicit Macro(QObject* parent = nullptr);
   int         m_Position;
   QString     m_Name;
   QString     m_Description;
   QString     m_Sequence;
   QString     m_Escaped;
   QString     m_Id;
   int         m_Delay;
   QString     m_Category;
   KAction*    m_Action;
   MacroModel* m_pModel;
   MacroModel::IndexPointer* m_pPointer;
public Q_SLOTS:
   void execute();
private Q_SLOTS:
   void nextStep();

Q_SIGNALS:
   void changed(Macro*);
};

#endif