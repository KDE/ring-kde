/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
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
#include "macro.h"

//Qt
#include <QtCore/QTimer>

//KDE
#include <KAction>
#include <KLocale>
#include <KIcon>

//SFLPhone
#include "../lib/dbus/callmanager.h"

Macro::Macro(QObject* parent) : QObject(parent),m_Position(0),m_Delay(0),m_pCat(nullptr),m_pPointer(nullptr),
m_Action(nullptr),m_pModel(nullptr)
{
}

Macro::Macro(const Macro* macro) : 
QObject(0)                         , m_Position(macro->m_Position), m_Name(macro->m_Name)        ,
m_Description(macro->m_Description), m_Sequence(macro->m_Sequence), m_Escaped(macro->m_Escaped)  ,
m_Id(macro->m_Id)                  , m_Delay(macro->m_Delay)      , m_Category(macro->m_Category),
m_Action(macro->m_Action)          , m_pCat(macro->m_pCat)        , m_pModel(macro->m_pModel)    ,
m_pPointer(macro->m_pPointer)
{}

void Macro::execute() {
   m_Escaped = m_Sequence;
   while (m_Escaped.indexOf("\\n") != -1) {
      m_Escaped = m_Escaped.replace("\\n","\n");
   }
   nextStep();
}

void Macro::nextStep()
{
   if (m_Position < m_Escaped.size()) {
      if (!MacroModel::instance()->m_lListeners.size())
         Q_NOREPLY DBus::CallManager::instance().playDTMF(QString(m_Escaped[m_Position]));
      else {
         foreach(MacroListener* l,MacroModel::instance()->m_lListeners) {
            l->addDTMF(QString(m_Escaped[m_Position]));
         }
      }
      m_Position++;
      QTimer::singleShot(m_Delay?m_Delay:100,this,SLOT(nextStep()));
   }
   else {
      m_Position = 0;
   }
}

QModelIndex Macro::index()
{
   QModelIndex parent = m_pModel->index(m_pModel->m_lCategories.indexOf(m_pCat),0,QModelIndex());
   return  m_pModel->index(m_pCat->m_lContent.indexOf(this),0,parent);
}

void Macro::setName(const QString &value)
{
   m_Name = value;
   emit changed(this);
   m_Action->setText(m_Name);
}

void Macro::setDescription(const QString &value)
{
   m_Description = value;emit changed(this);
}
void Macro::setSequence(const QString &value)
{
   m_Sequence = value;emit changed(this);
}

void Macro::setEscaped(const QString &value)
{
   m_Escaped = value;emit changed(this);
}

void Macro::setId(const QString &value)
{
   m_Id = value;emit changed(this);
}

void Macro::setDelay(int value)
{
   m_Delay = value;emit changed(this);
}

void Macro::setCategory(const QString &value)
{
   m_Category = value;emit changed(this);
}

QString Macro::name() const
{
   return m_Name;
}

QString Macro::description() const
{
   return m_Description;
}

QString Macro::sequence() const
{
   return m_Sequence;
}

QString Macro::escaped() const
{
   return m_Escaped;
}

QString Macro::id() const
{
   return m_Id;
}

int Macro::delay() const
{
   return m_Delay;
}

QString  Macro::category() const
{
   return m_Category;
}

KAction* Macro::action()
{
   return m_Action;
}
