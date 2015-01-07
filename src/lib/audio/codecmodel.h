/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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
#ifndef CODEC_MODEL_H
#define CODEC_MODEL_H

#include <QtCore/QAbstractListModel>

//Qt
#include <QtCore/QString>

//SFLPhone
#include "../typedefs.h"

class Account;
class CodecModelPrivate;

namespace Audio {

///AudioCodecModel: A model for account audio codec
class LIB_EXPORT CodecModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   //friend class Account;
   //Roles
   enum Role {
      ID         = 103,
      NAME       = 100,
      BITRATE    = 101,
      SAMPLERATE = 102,
   };

   //Constructor
   explicit CodecModel(Account* account);
   virtual ~CodecModel();

   //Abstract model member
   QVariant data        (const QModelIndex& index, int role = Qt::DisplayRole      ) const override;
   int rowCount         (const QModelIndex& parent = QModelIndex()                 ) const override;
   Qt::ItemFlags flags  (const QModelIndex& index                                  ) const override;
   virtual bool setData (const QModelIndex& index, const QVariant &value, int role )       override;

   //Mutator
   QModelIndex add();
   Q_INVOKABLE void remove   ( const QModelIndex& idx );
   Q_INVOKABLE bool moveUp   ( const QModelIndex& idx );
   Q_INVOKABLE bool moveDown ( const QModelIndex& idx );
   Q_INVOKABLE void clear    (                        );
   Q_INVOKABLE void reload   (                        );
   Q_INVOKABLE void save     (                        );

private:
   QScopedPointer<CodecModelPrivate> d_ptr;
   Q_DECLARE_PRIVATE(CodecModel)
};

}
Q_DECLARE_METATYPE(Audio::CodecModel*)

#endif
