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
#ifndef NUMBERCOMPLETIONMODEL_H
#define NUMBERCOMPLETIONMODEL_H

#include <QtCore/QAbstractTableModel>
#include "typedefs.h"
#include "phonedirectorymodel.h"


class PhoneNumber;
class Call;

class LIB_EXPORT NumberCompletionModel : public QAbstractTableModel {
   Q_OBJECT

public:
   NumberCompletionModel();
   virtual ~NumberCompletionModel();

   //Abstract model member
   QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
   int rowCount(const QModelIndex& parent = QModelIndex()             ) const;
   Qt::ItemFlags flags(const QModelIndex& index                       ) const;
   virtual bool setData(const QModelIndex& index, const QVariant &value, int role);
   virtual int columnCount(const QModelIndex& parent = QModelIndex()  ) const;
   virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

   //Setters
   void setCall(Call* call);
   
   //Getters
   Call* call() const;

private:
   enum class Columns {
      CONTENT = 0,
      NAME    = 1,
      ACCOUNT = 2,
      WEIGHT  = 3,
   };


   //Methods
   void updateModel();

   //Helper
   void getRange(QMap<QString,PhoneDirectoryModel::NumberWrapper*> map, const QString& prefix, QSet<PhoneNumber*>& set);
   void locateNameRange  (const QString& prefix, QSet<PhoneNumber*>& set);
   void locateNumberRange(const QString& prefix, QSet<PhoneNumber*>& set);
   uint getWeight(PhoneNumber* number);

   //Attributes
   QMultiMap<int,PhoneNumber*> m_hNumbers;
   QString                     m_Prefix  ;
   Call*                       m_pCall   ;
   bool                        m_Enabled ;

public Q_SLOTS:
   void setPrefix(const QString& str);

Q_SIGNALS:
   void enabled(bool);

};

#endif
