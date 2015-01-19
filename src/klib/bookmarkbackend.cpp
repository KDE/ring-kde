/************************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                                       *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>         *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "bookmarkbackend.h"

//Qt
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtGui/QApplication>

//KDE
#include <KStandardDirs>
#include <KMessageBox>
#include <KLocale>

//SFLPhone
#include <call.h>
#include <account.h>
#include <phonenumber.h>
#include <accountmodel.h>
#include <contactmodel.h>
#include <phonedirectorymodel.h>

BookmarkBackend::BookmarkBackend(QObject* parent) : AbstractBookmarkBackend(nullptr,parent)
{
   setObjectName("BookmarkBackend");
   load();
}

BookmarkBackend::~BookmarkBackend()
{
   
}

QString BookmarkBackend::name () const
{
   return "Minimal history backend";
}

QVariant BookmarkBackend::icon() const
{
   return QVariant();
}

bool BookmarkBackend::isEnabled() const
{
   return true;
}

bool BookmarkBackend::load()
{
   QFile file(KStandardDirs::locateLocal("appdata","")+"bookmark.ini");
   if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QMap<QString,QString> hc;
      while (!file.atEnd()) {
         QByteArray line = file.readLine().trimmed();

         if (line.isEmpty() && hc.size()) {
            //Parse the info
            const QString& aid = hc[Call::HistoryMapFields::ACCOUNT_ID];
            Account*       a   = aid.isEmpty()?nullptr:AccountModel::instance()->getById(aid);
            Contact*       c   = ContactModel::instance()->getContactByUid(hc[Call::HistoryMapFields::CONTACT_UID].toAscii());
            const QString& uri = hc[Call::HistoryMapFields::PEER_NUMBER];
            PhoneNumber*   n   = PhoneDirectoryModel::instance()->getNumber(uri,c,a);

            //Add the number
            m_lNumbers << n;
            emit newBookmarkAdded(n);

            //Reset
            hc.clear();
         }
         else {
            const QStringList elem = QString(line).split('=');
            if (elem.size() == 2) {
               hc[elem[0]] = elem[1];
            }
            else
               qDebug() << "Invalid bookmark element";
         }
      }
      return true;
   }
   else
      qWarning() << "Bookmarks doesn't exist or is not readable";
   return false;
}

bool BookmarkBackend::reload()
{
   return false;
}

///Save a single item
void BookmarkBackend::saveHelper(QTextStream& streamFileOut, const PhoneNumber* number)
{
   const Account* a = number->account();
   streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::ACCOUNT_ID      ).arg(a?a->id():""  );
   streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::PEER_NUMBER     ).arg(number->uri() );
   if (number->contact()) {
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CONTACT_UID  ).arg(
         QString(number->contact()->uid())
      );
   }
}

bool BookmarkBackend::append(const PhoneNumber* number)
{
   if (!number->isBookmarked()) {
      const_cast<PhoneNumber*>(number)->setTracked(true);
      const_cast<PhoneNumber*>(number)->setBookmarked(true);

      //TODO support \r and \n\r end of line
      QFile file(KStandardDirs::locateLocal("appdata","")+"bookmark.ini");
      if ( file.open(QIODevice::Append | QIODevice::Text) ) {
         QTextStream streamFileOut(&file);
         saveHelper(streamFileOut,number);
         m_lNumbers << const_cast<PhoneNumber*>(number);
         streamFileOut << "\n";
         streamFileOut.flush();
         file.close();
         emit newBookmarkAdded(const_cast<PhoneNumber*>(number));
         return true;
      }
      else
         qWarning() << "Unable to save bookmarks";
   }
   else
      qDebug() << number->uri() << "is already bookmarked";
   return false;
}

/** Rewrite the file from scratch
 * @todo Eventually check if it is necessary, it will be faster
 */
bool BookmarkBackend::save(const PhoneNumber* number)
{
   Q_UNUSED(number)
//    if (call->backend() != this)
//       append(call);

   QFile file(KStandardDirs::locateLocal("appdata","")+"bookmark.ini");
   if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
      QTextStream streamFileOut(&file);
      foreach(const PhoneNumber* n, m_lNumbers) {
         saveHelper(streamFileOut,n);
      }
      streamFileOut << "\n";
      streamFileOut.flush();
      file.close();
      return true;
   }
   else
      qWarning() << "Unable to save bookmarks";

   return false;
}

///Remove a bookmark and rewrite the file
bool BookmarkBackend::remove(PhoneNumber* number)
{
   m_lNumbers.removeAll(number);
   save(number);
   return true;
}

AbstractBookmarkBackend::SupportedFeatures BookmarkBackend::supportedFeatures() const
{
   return (AbstractBookmarkBackend::SupportedFeatures)   (
      AbstractBookmarkBackend::SupportedFeatures::NONE   |
      AbstractBookmarkBackend::SupportedFeatures::LOAD   |
      AbstractBookmarkBackend::SupportedFeatures::CLEAR  |
      AbstractBookmarkBackend::SupportedFeatures::ADD    |
      AbstractBookmarkBackend::SupportedFeatures::REMOVE );
}

///Edit 'item', the implementation may be a GUI or somehting else
bool BookmarkBackend::edit( PhoneNumber* number)
{
   Q_UNUSED(number)
   return false;
}
///Add a new item to the backend
bool BookmarkBackend::addNew( PhoneNumber* number)
{
   Q_UNUSED(number)
   return true;
}

bool BookmarkBackend::clear()
{
   const int ret = KMessageBox::questionYesNo(static_cast<QApplication*>(QApplication::instance())->activeWindow(), i18n("Are you sure you want to clear history?"), i18n("Clear history"));
   if (ret == KMessageBox::Yes) {
      QFile::remove(KStandardDirs::locateLocal("appdata","")+"bookmark.ini");
      return true;
   }
   return false;
}

QByteArray BookmarkBackend::id() const
{
   return "kbookmark";
}

QList<PhoneNumber*> BookmarkBackend::items() const
{
   return m_lNumbers;
}

