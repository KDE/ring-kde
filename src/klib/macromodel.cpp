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

#include "macromodel.h"

//Qt
#include <QtCore/QTimer>

//KDE
#include <KAction>
#include <KLocale>

//SFLPhone
#include "../lib/callmanager_interface_singleton.h"

///Init static attributes
MacroModel*  MacroModel::m_pInstance = nullptr;

Macro::Macro(QObject* parent) : QObject(parent),m_Position(0),m_Delay(0),m_pCat(nullptr),m_pPointer(nullptr)
{
   
}

Macro::Macro(const Macro* macro) : 
QObject(0)                         , m_Position(macro->m_Position), m_Name(macro->m_Name)        ,
m_Description(macro->m_Description), m_Sequence(macro->m_Sequence), m_Escaped(macro->m_Escaped)  ,
m_Id(macro->m_Id)                  , m_Delay(macro->m_Delay)      , m_Category(macro->m_Category),
m_Action(macro->m_Action) {}

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
      if (!MacroModel::getInstance()->m_lListeners.size())
         Q_NOREPLY CallManagerInterfaceSingleton::getInstance().playDTMF(QString(m_Escaped[m_Position]));
      else {
         foreach(MacroListener* l,MacroModel::getInstance()->m_lListeners) {
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

MacroModel::MacroModel(QObject* parent) : QAbstractItemModel(parent),m_pCurrentMacro(nullptr),m_pCurrentMacroMemento(nullptr)
{
}

///Singleton
MacroModel* MacroModel::getInstance()
{
   if (m_pInstance == nullptr) {
      m_pInstance = new MacroModel(0);
   }
   return m_pInstance;
}

void MacroModel::addListener(MacroListener* interface)
{
   MacroModel* m = getInstance();
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
   QString catName = newMacro->m_Category.isEmpty()?i18n("Other"):newMacro->m_Category;
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

//Add a new macro if the current one can be saved
bool MacroModel::newMacro()
{
   m_pCurrentMacro = new Macro();
   KAction* newAction = new KAction(this);
   m_pCurrentMacro->m_Action = newAction;
   m_pCurrentMacro->m_Id = QString::number(QDateTime::currentDateTime().toTime_t());
   m_pCurrentMacro->m_Name = i18n("New");
   m_pCurrentMacro->m_Category = i18n("Other");
   m_pCurrentMacro->m_pModel = this;
   while (m_hMacros[m_pCurrentMacro->m_Id]) {
      m_pCurrentMacro->m_Id += "1";
   }
   m_hMacros[m_pCurrentMacro->m_Id] = m_pCurrentMacro;
   updateTreeModel(m_pCurrentMacro);
   connect(m_pCurrentMacro,SIGNAL(changed(Macro*)),this,SLOT(changed(Macro*)));
   emit dataChanged(index(0,0),index(m_lCategories.size()-1,0));
   emit layoutChanged ();
   emit selectMacro(m_pCurrentMacro);
   newAction->setText(m_pCurrentMacro->m_Name);
   newAction->setIcon(KIcon("view-form-action"));
   connect(newAction, SIGNAL(triggered()), m_pCurrentMacro , SLOT(execute()) );
   emit addAction(newAction);
   return true;
}

//Remove the selected macro
bool MacroModel::removeMacro(QModelIndex idx)
{
   Q_UNUSED(idx)
   return true;
}

void MacroModel::setCurrent(QModelIndex current,QModelIndex previous)
{
   if (!current.isValid())
      return;
   IndexPointer* modelItem = (IndexPointer*)current.internalPointer();
   if (modelItem && modelItem->type == IndexType::MacroIndex) {
      Macro* macro = (Macro*)modelItem->data;
      m_pCurrentMacro = macro;
      emit selectMacro(m_pCurrentMacro);
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
      int idx = m_lCategories.indexOf(((Macro*)modelItem->data)->m_pCat);
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
         if (macro->m_pCat->m_lContent.size() == 1) { //Rename
            int idx = m_lCategories.indexOf(macro->m_pCat);
            macro->m_pCat->m_Name = macro->m_Category;
            emit dataChanged(index(idx,0),index(idx,0));
            return;
         }
         else {
            macro->m_pCat->m_lContent.removeAll(macro);
            macro->m_pCat = nullptr;
         }
         foreach (MacroCategory* cat, m_lCategories) {
            if (cat->m_Name == macro->m_Category) {
               cat->m_lContent << macro;
               macro->m_pCat = cat;
               macro->m_Category = cat->m_Name;
               return;
            }
         }
         MacroCategory* cat = createCategory(macro->m_Category);
         cat->m_lContent << macro;
         macro->m_pCat = cat;
         macro->m_Category = cat->m_Name;
         emit layoutChanged();
      }
   }
}