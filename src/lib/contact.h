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
class LIB_EXPORT Contact : public QObject, public CategorizedCompositeNode {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:

   class  PhoneNumbers : public QList<PhoneNumber*>, public CategorizedCompositeNode {
   public:
      virtual QObject* getSelf() __attribute__ ((const));
      explicit PhoneNumbers(Contact* parent);
      PhoneNumbers(Contact* parent, const QList<PhoneNumber*>& list);
      Contact* contact() const;
      time_t lastUsedTimeStamp() const;
   private:
      Contact* m_pParent       ;
   };

   virtual QObject* getSelf();

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
   virtual const PhoneNumbers& phoneNumbers() const;
   virtual const QString& nickName         () const;
   virtual const QString& firstName        () const;
   virtual const QString& secondName       () const;
   virtual const QString& formattedName    () const;
   virtual const QString& organization     () const;
   virtual const QString& uid              () const;
   virtual const QString& preferredEmail   () const;
   virtual const QPixmap* photo            () const;
   virtual const QString& type             () const;
   virtual const QString& group            () const;
   virtual const QString& department       () const;

   //Setters
   virtual void setPhoneNumbers   ( PhoneNumbers        );
   virtual void setFormattedName  ( const QString& name );
   virtual void setNickName       ( const QString& name );
   virtual void setFirstName      ( const QString& name );
   virtual void setFamilyName     ( const QString& name );
   virtual void setOrganization   ( const QString& name );
   virtual void setPreferredEmail ( const QString& name );
   virtual void setGroup          ( const QString& name );
   virtual void setDepartment     ( const QString& name );
   virtual void setUid            ( const QString& id   );
   virtual void setPhoto          ( QPixmap* photo      );

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
