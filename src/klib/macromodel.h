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

#ifndef MACRO_MODEL_H
#define MACRO_MODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QHash>
#include "../lib/typedefs.h"

//KDE
class KAction;

//SFLPhone

class Macro;

///Interface to interpret DTMFs instead of using the daemon directly
class LIB_EXPORT MacroListener {
public:
   explicit MacroListener() {}
   virtual void addDTMF(const QString& sequence) = 0;
   virtual ~MacroListener() {}
};

///MacroModel: DTMF emulators model
class LIB_EXPORT MacroModel : public QAbstractItemModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
   friend class Macro;

private:
   
   enum IndexType {
      CategoryIndex = 1,
      MacroIndex = 2
   };
   
   struct IndexPointer {
      IndexPointer(IndexType _type, void* _data) : type(_type),data(_data) {}
      IndexType type;
      void* data;
   };

   struct MacroCategory {
      MacroCategory():m_pPointer(nullptr){}
      ~MacroCategory() { delete m_pPointer; }
      QString m_Name;
      QList<Macro*> m_lContent;
      IndexPointer* m_pPointer;
   };
public:
   static MacroModel* instance();
   static void addListener(MacroListener* interface);

   enum MacroFields {
      Name        = 100,
      Category    = 101,
      Delay       = 102,
      Description = 103,
      Sequence    = 104
   };

   void initMacros();

   //Getters
   Macro* getCurrentMacro();

   //Mutator
   MacroModel::MacroCategory* createCategory(const QString& name);

   //Model implementation
   virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );
   virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
   virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
   virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
   virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
   virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
   virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

private:
   //Singleton constructor
   explicit MacroModel(QObject* parent = nullptr);

   //Attributes
   static MacroModel*         m_pInstance  ;

private:
   void updateTreeModel(Macro* newMacro);
   QHash<QString,Macro*> m_hMacros;
   QList<MacroCategory*> m_lCategories;
   QList<MacroListener*> m_lListeners;
   Macro*                m_pCurrentMacro;
   Macro*                m_pCurrentMacroMemento;

public Q_SLOTS:
   Macro* newMacro(const QString& id = QString());
   bool removeMacro(const QModelIndex& idx);
   void setCurrent(const QModelIndex& current, const QModelIndex& previous);
   void save();

private Q_SLOTS:
   void changed(Macro* macro);

Q_SIGNALS:
   void addAction(KAction*);
   void selectMacro(Macro* macro);
};

#endif
