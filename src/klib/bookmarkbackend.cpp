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
#include <QtWidgets/QApplication>
#include <QtCore/QStandardPaths>

//KDE

#include <kmessagebox.h>
#include <klocalizedstring.h>

//SFLPhone
#include <call.h>
#include <account.h>
#include <contactmethod.h>
#include <accountmodel.h>
#include <personmodel.h>
#include <phonedirectorymodel.h>
#include "delegates/pixmapmanipulationdelegate.h"


BookmarkBackend::~BookmarkBackend()
{

}

bool BookmarkEditor::save(const ContactMethod* number)
{
//    if (call->backend() != this)
//       append(call);

   QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/')+"bookmark.ini");
   if ( file.open(QIODevice::WriteOnly | QIODevice::Text) ) {
      QTextStream streamFileOut(&file);
      foreach(const ContactMethod* n, m_lNumbers) {
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

bool BookmarkEditor::remove(const ContactMethod* item)
{
   Q_UNUSED(item)

   if (m_lNumbers.indexOf(const_cast<ContactMethod*>(item)) != -1) {
      m_lNumbers.removeAt(m_lNumbers.indexOf(const_cast<ContactMethod*>(item)));
      mediator()->removeItem(item);
      return save(nullptr);
   }
   return false;
}

bool BookmarkEditor::edit( ContactMethod* item)
{
   Q_UNUSED(item)
   return false;
}

bool BookmarkEditor::addNew(const ContactMethod* item)
{
   Q_UNUSED(item)
   addExisting(item);
   return save(item);
}

bool BookmarkEditor::addExisting(const ContactMethod* item)
{
   m_lNumbers << const_cast<ContactMethod*>(item);
   mediator()->addItem(item);
   return false;
}

QVector<ContactMethod*> BookmarkEditor::items() const
{
   return m_lNumbers;
}

QString BookmarkBackend::name () const
{
   return QObject::tr("Local bookmarks");
}

QString BookmarkBackend::category () const
{
   return QObject::tr("Bookmark");
}

QVariant BookmarkBackend::icon() const
{
   return PixmapManipulationDelegate::instance()->collectionIcon(this,PixmapManipulationDelegate::CollectionIconHint::BOOKMARK);
}

bool BookmarkBackend::isEnabled() const
{
   return true;
}

bool BookmarkBackend::load()
{
   QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') +"bookmark.ini");
   if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
      QMap<QString,QString> hc;
      while (!file.atEnd()) {
         QByteArray line = file.readLine().trimmed();

         if (line.isEmpty() && hc.size()) {
            //Parse the info
            const QByteArray& aid = hc[Call::HistoryMapFields::ACCOUNT_ID].toLatin1();
            Account*          a   = aid.isEmpty()?nullptr:AccountModel::instance()->getById(aid);
            Person*          c   = PersonModel::instance()->getPersonByUid(hc[Call::HistoryMapFields::CONTACT_UID].toLatin1());
            const QString&    uri = hc[Call::HistoryMapFields::PEER_NUMBER];
            ContactMethod*      n   = PhoneDirectoryModel::instance()->getNumber(uri,c,a);

            //Add the number
            editor<ContactMethod>()->addExisting(n);

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
void BookmarkEditor::saveHelper(QTextStream& streamFileOut, const ContactMethod* number)
{
   const Account* a = number->account();
   streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::ACCOUNT_ID      ).arg(a?QString(a->id()):""  );
   streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::PEER_NUMBER     ).arg(number->uri() );
   if (number->contact()) {
      streamFileOut << QString("%1=%2\n").arg(Call::HistoryMapFields::CONTACT_UID  ).arg(
         QString(number->contact()->uid())
      );
   }
   streamFileOut << QString('\n');
}

bool BookmarkEditor::append(const ContactMethod* number)
{
   if (!number->isBookmarked()) {
      const_cast<ContactMethod*>(number)->setTracked(true);
      const_cast<ContactMethod*>(number)->setBookmarked(true);

      //TODO support \r and \n\r end of line
      QFile file(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/')+"bookmark.ini");
      if ( file.open(QIODevice::Append | QIODevice::Text) ) {
         QTextStream streamFileOut(&file);
         saveHelper(streamFileOut,number);
         m_lNumbers << const_cast<ContactMethod*>(number);
         streamFileOut << "\n";
         streamFileOut.flush();
         file.close();
         return true;
      }
      else
         qWarning() << "Unable to save bookmarks";
   }
   else
      qDebug() << number->uri() << "is already bookmarked";
   return false;
}

FlagPack<CollectionInterface::SupportedFeatures> BookmarkBackend::supportedFeatures() const
{
   return
      CollectionInterface::SupportedFeatures::NONE   |
      CollectionInterface::SupportedFeatures::LOAD   |
      CollectionInterface::SupportedFeatures::CLEAR  |
      CollectionInterface::SupportedFeatures::ADD    |
      CollectionInterface::SupportedFeatures::REMOVE ;
}

bool BookmarkBackend::clear()
{
   const int ret = KMessageBox::questionYesNo(static_cast<QApplication*>(QApplication::instance())->activeWindow(), i18n("Are you sure you want to clear history?"), i18n("Clear history"));
   if (ret == KMessageBox::Yes) {
      QFile::remove(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1Char('/') +"bookmark.ini");
      return true;
   }
   return false;
}

QByteArray BookmarkBackend::id() const
{
   return "kbookmark";
}

// QList<ContactMethod*> BookmarkBackend::items() const
// {
//    return m_lNumbers;
// }

