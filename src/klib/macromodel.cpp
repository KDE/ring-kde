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

#include "macromodel.h"

//System
#include <time.h>

//Qt
#include <QtCore/QTimer>
#include <QtCore/QFile>

//KDE
#include <KAction>
#include <KLocale>
#include <KStandardDirs>

//SFLPhone
#include "../lib/dbus/callmanager.h"
#include "kcfg_settings.h"
#include "macro.h"

///Init static attributes
MacroModel*  MacroModel::m_pInstance = nullptr;


MacroModel::MacroModel(QObject* parent) : QAbstractItemModel(parent),m_pCurrentMacro(nullptr),m_pCurrentMacroMemento(nullptr)
{
   
}

///Singleton
MacroModel* MacroModel::instance()
{
   if (m_pInstance == nullptr) {
      m_pInstance = new MacroModel(0);
   }
   return m_pInstance;
}

void MacroModel::initMacros()
{
   if (KStandardDirs::exists(KStandardDirs::locateLocal("appdata","")+"macros.txt")) {
      QFile serialized(KStandardDirs::locate( "appdata", "macros.txt" ));
      if (serialized.open(QIODevice::ReadOnly)) {
         QDataStream in(&serialized);
         QList< QMap<QString, QString> > unserialized;
         in >> unserialized;
         serialized.close();
         foreach(const MapStringString& aMacro,unserialized) {
            Macro* nMac = newMacro(aMacro["ID"]);
            nMac->setName(aMacro["Name"]);
            nMac->setSequence(aMacro["Seq"]);
            nMac->setCategory(aMacro["Cat"]);
            nMac->setDelay(aMacro[ "Delay" ].toInt());
            nMac->setDescription(aMacro["Desc"]);
         }
      }
    }
}

void MacroModel::addListener(MacroListener* interface)
{
   MacroModel* m = instance();
   m->m_lListeners << interface;
}

MacroModel::MacroCategory* MacroModel::createCategory(const QString& name)
{
   MacroCategory* cat = new MacroCategory;
   cat->m_Name = name;
   cat->m_pPointer = new IndexPointer(IndexType::CategoryIndex,cat);
   m_lCategories << cat;
   emit dataChanged(MacroModel::index((m_lCategories.size()-2 > 0)? m_lCategories.size()-2:0,0,QModelIndex()),
                    MacroModel::index((m_lCategories.size()-1>0  )? m_lCategories.size()-1:0,0,QModelIndex()));
   emit layoutChanged();
   return cat;
}

void MacroModel::updateTreeModel(Macro* newMacro)
{
   QString catName = newMacro->m_Category.isEmpty()?i18nc("Other category","Other"):newMacro->m_Category;
   foreach (MacroCategory* cat, m_lCategories) {
      if (cat->m_Name == catName) {
         cat->m_lContent << newMacro;
         newMacro->m_pCat = cat;
         newMacro->m_Category = cat->m_Name;
         newMacro->m_pPointer = new IndexPointer(IndexType::MacroIndex,newMacro);
         return;
      }
   }
   MacroCategory* cat = createCategory(catName);
   cat->m_lContent << newMacro;
   newMacro->m_pCat = cat;
   newMacro->m_pPointer = new IndexPointer(IndexType::MacroIndex,newMacro);
}

//Remove the selected macro
bool MacroModel::removeMacro(const QModelIndex& idx)
{
   IndexPointer* modelItem = (IndexPointer*)idx.internalPointer();
   if (modelItem && modelItem->type == IndexType::MacroIndex) {
      Macro* macro = static_cast<Macro*>(modelItem->data);
      MacroCategory* cat = macro->m_pCat;
      cat->m_lContent.removeAll(macro);
      emit layoutChanged();
   }
   else
      kWarning() << "Cannot remove macro: none is selected";
   return true;
}

void MacroModel::setCurrent(const QModelIndex& current, const QModelIndex& previous)
{
   Q_UNUSED(previous)
   if (!current.isValid())
      return;
   IndexPointer* modelItem = (IndexPointer*)current.internalPointer();
   if (modelItem && modelItem->type == IndexType::MacroIndex) {
      Macro* macro = static_cast<Macro*>(modelItem->data);
      m_pCurrentMacro = macro;
      emit selectMacro(m_pCurrentMacro);
   }
}

void MacroModel::save()
{
   QFile macros(KStandardDirs::locateLocal("appdata","")+"macros.txt");
   if (macros.open(QIODevice::WriteOnly)) {
      QDataStream out(&macros);
      QList< QMap<QString, QString> > serialized;
      foreach (MacroCategory* cat, m_lCategories) {
         QMap<QString, QString> serializedMacro;
         foreach(Macro* macro,cat->m_lContent) {
            serializedMacro[ "Name"  ] = macro->m_Name;
            serializedMacro[ "Seq"   ] = macro->m_Sequence;
            serializedMacro[ "Cat"   ] = macro->m_pCat->m_Name;
            serializedMacro[ "Delay" ] = QString::number(macro->m_Delay);
            serializedMacro[ "Desc"  ] = macro->m_Description;
            serializedMacro[ "ID"    ] = macro->m_Id;
            serialized << serializedMacro;
         }
      }
      out << serialized;
      macros.close();
      kDebug() << "Macros correctly saved";
   }
   else {
      kDebug() << "Error saving macros";
   }
}

bool MacroModel::setData( const QModelIndex& index, const QVariant &value, int role)
{
   Q_UNUSED(index)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant MacroModel::data( const QModelIndex& index, int role) const
{
   if (!index.isValid())
      return QVariant();
   if (!index.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(m_lCategories[index.row()]->m_Name);
   }
   else if (index.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      return QVariant(m_lCategories[index.parent().row()]->m_lContent[index.row()]->m_Name);
   }
   return QVariant();
}

QVariant MacroModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant(i18n("Macros"));
   return QVariant();
}

int MacroModel::rowCount( const QModelIndex& parent ) const
{
   if (!parent.isValid()) {
      return m_lCategories.size();
   }
   else if (!parent.parent().isValid() && parent.row() < m_lCategories.size()) {
      return m_lCategories[parent.row()]->m_lContent.size();
   }
   return 0;
}

Qt::ItemFlags MacroModel::flags( const QModelIndex& index ) const
{
   if (!index.isValid())
      return 0;
   return Qt::ItemIsEnabled | ((index.parent().isValid())?Qt::ItemIsSelectable:Qt::ItemIsEnabled);
}

int MacroModel::columnCount ( const QModelIndex& parent) const
{
   Q_UNUSED(parent)
   return 1;
}

QModelIndex MacroModel::parent( const QModelIndex& index) const
{
   if (!index.isValid())
      return QModelIndex();
   IndexPointer* modelItem = (IndexPointer*)index.internalPointer();
   if (modelItem && modelItem->type == IndexType::MacroIndex) {
      const int idx = m_lCategories.indexOf(static_cast<Macro*>(modelItem->data)->m_pCat);
      if (idx != -1) {
         return MacroModel::index(idx,0,QModelIndex());
      }
   }
   return QModelIndex();
}

QModelIndex MacroModel::index( int row, int column, const QModelIndex& parent) const
{
   if (!parent.isValid() && m_lCategories.size() > row) {
      return createIndex(row,column,m_lCategories[row]->m_pPointer);
   }
   else if (parent.isValid() && m_lCategories[parent.row()]->m_lContent.size() > row) {
      return createIndex(row,column,m_lCategories[parent.row()]->m_lContent[row]->m_pPointer);
   }
   return QModelIndex();
}

void MacroModel::changed(Macro* macro)
{
   if (macro && macro->m_pCat) {
      QModelIndex parent = index(m_lCategories.indexOf(macro->m_pCat),0);
      emit dataChanged(index(0,0,parent),index(rowCount(parent),0,parent));
      if (macro->m_pCat->m_Name != macro->m_Category) {
         MacroCategory* newIdx = nullptr;
         foreach (MacroCategory* cat, m_lCategories) {
            if (cat->m_Name == macro->m_Category) {
               newIdx = cat;
               break;
            }
         }
         if (macro->m_pCat->m_lContent.size() == 1 && !newIdx) { //Rename
            int idx = m_lCategories.indexOf(macro->m_pCat);
            macro->m_pCat->m_Name = macro->m_Category;
            emit dataChanged(index(idx,0),index(idx,0));
            return;
         }
         else {
            macro->m_pCat->m_lContent.removeAll(macro);
            if (!macro->m_pCat->m_lContent.size()) {
               m_lCategories.removeAll(macro->m_pCat);
               delete macro->m_pCat;
               emit dataChanged(index(0,0),index(m_lCategories.size()-1,0));
            }
            macro->m_pCat = nullptr;
         }

         if (newIdx) {
            newIdx->m_lContent << macro;
            macro->m_pCat = newIdx;
            macro->m_Category = newIdx->m_Name;
            return;
         }
         MacroCategory* cat = createCategory(macro->m_Category);
         cat->m_lContent << macro;
         macro->m_pCat = cat;
         macro->m_Category = cat->m_Name;
         emit layoutChanged();
      }
   }
}


//Add a new macro if the current one can be saved
Macro* MacroModel::newMacro(const QString& id)
{
   m_pCurrentMacro = new Macro(this);
   KAction* newAction = new KAction(this);
   m_pCurrentMacro->m_Action = newAction;
   m_pCurrentMacro->m_Name = i18nc("New macro","New");
   m_pCurrentMacro->m_Category = i18nc("Other category","Other");
   m_pCurrentMacro->m_pModel = this;
   if (id.isEmpty()) {
      time_t curTime;
      ::time(&curTime);
      m_pCurrentMacro->m_Id = QString::number(curTime);
      while (m_hMacros[m_pCurrentMacro->m_Id]) {
         m_pCurrentMacro->m_Id += '1';
      }
   }
   else
      m_pCurrentMacro->m_Id += id;
   m_hMacros[m_pCurrentMacro->m_Id] = m_pCurrentMacro;
   updateTreeModel(m_pCurrentMacro);
   connect(m_pCurrentMacro,SIGNAL(changed(Macro*)),this,SLOT(changed(Macro*)));
   emit dataChanged(index(0,0),index(m_lCategories.size()-1,0));
   emit layoutChanged ();
   emit selectMacro(m_pCurrentMacro);
   newAction->setText(m_pCurrentMacro->m_Name);
   newAction->setIcon(KIcon("view-form-action"));
   newAction->setObjectName("action_macro"+m_pCurrentMacro->m_Id);
   connect(newAction, SIGNAL(triggered()), m_pCurrentMacro , SLOT(execute()) );
   emit addAction(newAction);
   return m_pCurrentMacro;
}

Macro* MacroModel::getCurrentMacro()
{
   return m_pCurrentMacro;
}
