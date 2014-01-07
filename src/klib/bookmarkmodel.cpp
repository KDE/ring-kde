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
#include "bookmarkmodel.h"

//Qt
#include <QtCore/QMimeData>

//SFLPhone
#include "kcfg_settings.h"
#include "../lib/historymodel.h"
#include "../lib/phonenumber.h"

BookmarkModel* BookmarkModel::m_pSelf = nullptr;

BookmarkModel::BookmarkModel(QObject* parent) : AbstractBookmarkModel(parent)
{
}

BookmarkModel* BookmarkModel::instance()
{
   if (!m_pSelf)
      m_pSelf = new BookmarkModel(nullptr);
   return m_pSelf;
}


void BookmarkModel::addBookmark(PhoneNumber* number, bool trackPresence)
{
   Q_UNUSED(trackPresence)
   if (!number->isBookmarked()) {
      number->setTracked(true);
      number->setBookmarked(true);
      ConfigurationSkeleton::setBookmarkList(ConfigurationSkeleton::bookmarkList() << number->toHash());
      reloadCategories();
   }
}

void BookmarkModel::removeBookmark(PhoneNumber* number)
{
   QStringList bookmarks = ConfigurationSkeleton::bookmarkList();
   kDebug() << "Removing" << number->uri() << "from bookmarks";
   bookmarks.removeAll(number->uid());
   ConfigurationSkeleton::setBookmarkList(bookmarks);
}

bool BookmarkModel::displayFrequentlyUsed() const
{
   return ConfigurationSkeleton::displayPopularAsBookmark();
}

QVector<PhoneNumber*> BookmarkModel::bookmarkList() const
{
   return serialisedToList(ConfigurationSkeleton::bookmarkList());
}
