/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
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

//SFLPhone
class PhoneNumber;
class Call;

//Private
class NumberCompletionModelPrivate;

class LIB_EXPORT NumberCompletionModel : public QAbstractTableModel {
   Q_OBJECT

public:

   //Properties
   Q_PROPERTY(QString prefix READ prefix WRITE setPrefix)

   enum Role {
      ALTERNATE_ACCOUNT= 100,
      FORCE_ACCOUNT    = 101,
      ACCOUNT          = 102,
   };

   NumberCompletionModel();
   virtual ~NumberCompletionModel();

   //Abstract model member
   virtual QVariant      data       ( const QModelIndex& index, int role = Qt::DisplayRole                 ) const override;
   virtual int           rowCount   ( const QModelIndex& parent = QModelIndex()                            ) const override;
   virtual Qt::ItemFlags flags      ( const QModelIndex& index                                             ) const override;
   virtual bool          setData    ( const QModelIndex& index, const QVariant &value, int role            )       override;
   virtual int           columnCount( const QModelIndex& parent = QModelIndex()                            ) const override;
   virtual QVariant      headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const override;

   //Setters
   void setCall(Call* call);
   void setUseUnregisteredAccounts(bool value);

   //Getters
   Call* call() const;
   PhoneNumber* number(const QModelIndex& idx) const;
   bool isUsingUnregisteredAccounts();
   QString prefix() const;

private:
   QScopedPointer<NumberCompletionModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(NumberCompletionModel)


public Q_SLOTS:
   void setPrefix(const QString& str);

Q_SIGNALS:
   void enabled(bool);

};

#endif
