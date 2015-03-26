/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
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
#include <QHash>
#include <QtWidgets/QDockWidget>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QTreeWidgetItem>

//Ring
#include "categorizedtreeview.h"
class ContactMethod;

//Qt
class QMenu;

//KDE
class QAction;

namespace Akonadi {
   namespace Person {
   }
}

namespace KABC {
}

///Ring
class Person;
class CategorizedContactModel;
class CategorizedDelegate;
class ContactMethodDelegate;
class ContactDelegate;
class KeyPressEaterC;

class PersonSortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit PersonSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {
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
   Person*                     m_pCurrentPerson;
   QString                      m_PreselectedNb  ;
   QSortFilterProxyModel*       m_pProxyModel    ;
   CategorizedContactModel*           m_pSourceModel   ;
   KeyPressEaterC*              m_pKeyPressEater ;

   //Actions
   QAction * m_pCallAgain   ;
   QAction * m_pEditPerson ;
   QAction * m_pCopy        ;
   QAction * m_pEmail       ;
   QAction * m_pAddPhone    ;
   QAction * m_pBookmark    ;
   QAction * m_pRemove      ;

   //Delegates
   CategorizedDelegate* m_pCategoryDelegate;
   ContactMethodDelegate* m_pContactMethodDelegate;
   ContactDelegate*     m_pContactDelegate;

   //Helper
   ContactMethod* showNumberSelector(bool& ok);

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
   void callAgain   (const ContactMethod* n = nullptr);
   void copy        ();
   void editPerson ();
   void addPhone    ();
   void bookmark    ();
   void transferEvent( QMimeData* data   );
   void expandTree  ();
   void expandTreeRows(const QModelIndex& idx);
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
