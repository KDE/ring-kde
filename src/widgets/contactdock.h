/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#ifndef CONTACT_DOCK_H
#define CONTACT_DOCK_H

#include "ui_contactdock.h"

//Qt
#include <QtCore/QHash>
#include <QtGui/QDockWidget>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeWidgetItem>

//SFLPhone
#include "categorizedtreeview.h"
class PhoneNumber;

//Qt
class QMenu;

//KDE
class KAction;

namespace Akonadi {
   namespace Contact {
   }
}

namespace KABC {
}

///SFLPhone
class Contact;
class ContactProxyModel;
class CategorizedDelegate;
class PhoneNumberDelegate;
class ContactDelegate;
class KeyPressEaterC;

class ContactSortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit ContactSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {
      setDynamicSortFilter(true);
   }
protected:
   virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const;
};

///ContactDock: Dock to access contacts
class ContactDock : public QDockWidget, public Ui_ContactDock
{
   Q_OBJECT
public:
   //Constructor
   explicit ContactDock(QWidget* parent = nullptr);
   virtual ~ContactDock();

private:
   //Attributes
   QMenu*                       m_pMenu          ;
   Contact*                     m_pCurrentContact;
   QString                      m_PreselectedNb  ;
   QSortFilterProxyModel*       m_pProxyModel    ;
   ContactProxyModel*           m_pSourceModel   ;
   KeyPressEaterC*              m_pKeyPressEater ;

   //Actions
   KAction* m_pCallAgain   ;
   KAction* m_pEditContact ;
   KAction* m_pCopy        ;
   KAction* m_pEmail       ;
   KAction* m_pAddPhone    ;
   KAction* m_pBookmark    ;
   KAction* m_pRemove      ;

   //Delegates
   CategorizedDelegate* m_pCategoryDelegate;
   PhoneNumberDelegate* m_pPhoneNumberDelegate;
   ContactDelegate*     m_pContactDelegate;

   //Helper
   PhoneNumber* showNumberSelector(bool& ok);

   enum SortingCategory {
      Name,
      Organization,
      RecentlyUsed,
      Group,
      Department
   };

public Q_SLOTS:
   virtual void keyPressEvent(QKeyEvent* event);

private Q_SLOTS:
   void setHistoryVisible  ( bool visible          );
   void slotContextMenu    ( QModelIndex index     );

private Q_SLOTS:
   ///Menu actions
   void showContext(const QModelIndex& index);
   void sendEmail   ();
   void callAgain   (const PhoneNumber* n = nullptr);
   void copy        ();
   void editContact ();
   void addPhone    ();
   void bookmark    ();
   void transferEvent( QMimeData* data   );
   void expandTree  ();
   void setCategory (int index);
   void slotDoubleClick(const QModelIndex& index);
   void slotDelete();
};

///KeyPressEaterC: keygrabber
class KeyPressEaterC : public QObject
{
   Q_OBJECT
public:
   explicit KeyPressEaterC(ContactDock* parent) : QObject(parent) {
      m_pDock = parent;
   }

protected:
   bool eventFilter(QObject *obj, QEvent *event);

private:
   ContactDock* m_pDock;

};

#endif //CONTACT_DOCK_H
