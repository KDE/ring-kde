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
#ifndef BOOKMARKMODEL_H
#define BOOKMARKMODEL_H


//SFLPhone
#include "../lib/typedefs.h"
#include "../lib/contact.h"
#include "../lib/call.h"
#include "../lib/abstractbookmarkmodel.h"


class LIB_EXPORT BookmarkModel :  public AbstractBookmarkModel
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   //Singleton
   static BookmarkModel* instance();

   //Management
   virtual void addBookmark   ( PhoneNumber* number, bool trackPresence = false);
   virtual void removeBookmark( PhoneNumber* number                            );

protected:
   //Pure virtual
   virtual bool                  displayFrequentlyUsed         () const;
   virtual QVector<PhoneNumber*> bookmarkList () const;

   //Constructor
   virtual ~BookmarkModel() {}
   BookmarkModel(QObject* parent);

private:
   //Singleton
   static BookmarkModel* m_pSelf;
};

#endif
