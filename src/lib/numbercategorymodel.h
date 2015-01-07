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
#ifndef NUMBERCATEGORYMODEL_H
#define NUMBERCATEGORYMODEL_H
#include "typedefs.h"

#include <QtCore/QAbstractListModel>
#include <QtCore/QVector>

class QPixmap;

class NumberCategoryVisitor;
class PhoneNumber;
class NumberCategory;
class NumberCategoryModelPrivate;

class LIB_EXPORT NumberCategoryModel : public QAbstractListModel {
   Q_OBJECT
public:

   enum Role {
      INDEX = 100,
   };

   //Abstract model member
   virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole    ) const override;
   virtual int rowCount(const QModelIndex& parent = QModelIndex()                ) const override;
   virtual Qt::ItemFlags flags(const QModelIndex& index                          ) const override;
   virtual bool setData(const QModelIndex& index, const QVariant &value, int role) override;

   //Mutator
   NumberCategory* addCategory(const QString& name, QPixmap* icon, int index = -1, bool enabled = true);
   void setIcon(int index, QPixmap* icon);
   void save();

   //Singleton
   static NumberCategoryModel* instance();

   //Getter
   QModelIndex nameToIndex(const QString& name) const;
   NumberCategory* getCategory(const QString& type);
   static NumberCategory* other();

   //Mutator
   void registerNumber  (PhoneNumber* number);
   void unregisterNumber(PhoneNumber* number);

private:
   explicit NumberCategoryModel(QObject* parent = nullptr);
   ~NumberCategoryModel();

   NumberCategoryModelPrivate* d_ptr;

   //Singleton
   static NumberCategoryModel* m_spInstance;
};

#endif //NUMBERCATEGORYMODEL_H
