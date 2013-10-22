/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#ifndef CONTACT_H
#define CONTACT_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

//Qt
class QPixmap;

//KDE
namespace KABC {
   class Addressee   ;
   class Picture     ;
   class PhoneNumber ;
}

//SFLPhone
class PhoneNumber;

#include "typedefs.h"
#include "categorizedcompositenode.h"


///Contact: Abstract version of a contact
class LIB_EXPORT Contact : public QObject {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:

   class  PhoneNumbers : public QVector<PhoneNumber*>, public CategorizedCompositeNode {
   public:
      virtual QObject* getSelf() const __attribute__ ((const));
      explicit PhoneNumbers(Contact* parent);
      PhoneNumbers(Contact* parent, const QVector<PhoneNumber*>& list);
      Contact* contact() const;
      time_t lastUsedTimeStamp() const;
   private:
      Contact* m_pParent2       ;
   };

private:
   QString      m_FirstName      ;
   QString      m_SecondName     ;
   QString      m_NickName       ;
   QPixmap*     m_pPhoto         ;
   QString      m_Type           ;
   QString      m_FormattedName  ;
   QString      m_PreferredEmail ;
   QString      m_Organization   ;
   QString      m_Uid            ;
   QString      m_Group          ;
   QString      m_Department     ;
   bool         m_DisplayPhoto   ;
   PhoneNumbers m_Numbers        ;

public:
   //Constructors & Destructors
   explicit Contact();
   virtual ~Contact();

   //Getters
   const PhoneNumbers& phoneNumbers() const;
   const QString& nickName         () const;
   const QString& firstName        () const;
   const QString& secondName       () const;
   const QString& formattedName    () const;
   const QString& organization     () const;
   const QString& uid              () const;
   const QString& preferredEmail   () const;
   const QPixmap* photo            () const;
   const QString& type             () const;
   const QString& group            () const;
   const QString& department       () const;

   //Number related getters
   bool isPresent() const;
   bool isTracked() const;
   bool supportPresence() const;

   //Setters
   void setPhoneNumbers   ( PhoneNumbers        );
   void setFormattedName  ( const QString& name );
   void setNickName       ( const QString& name );
   void setFirstName      ( const QString& name );
   void setFamilyName     ( const QString& name );
   void setOrganization   ( const QString& name );
   void setPreferredEmail ( const QString& name );
   void setGroup          ( const QString& name );
   void setDepartment     ( const QString& name );
   void setUid            ( const QString& id   );
   void setPhoto          ( QPixmap* photo      );

   //Mutator
   QHash<QString,QVariant> toHash();

Q_SIGNALS:
    void presenceChanged(PhoneNumber*);

protected:
    //Presence secret methods
    void updatePresenceInformations(const QString& uri, bool status, const QString& message);
};

Q_DECLARE_METATYPE(Contact*)

#endif
