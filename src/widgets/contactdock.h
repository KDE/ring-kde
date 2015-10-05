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
#include <QtWidgets/QTreeWidgetItem>

//Ring
#include "categorizedtreeview.h"
class ContactMethod;

//Qt
class QMenu;

//KDE
class QAction;

namespace Menu {
   class Person;
}

///Ring
class Person;
class CategorizedContactModel;
class CategorizedDelegate;
class ContactMethodDelegate;
class ContactDelegate;
class KeyPressEaterC;

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
   Menu::Person*                m_pMenu          ;
   Person*                      m_pCurrentPerson ;
   KeyPressEaterC*              m_pKeyPressEater ;

   //Delegates
   CategorizedDelegate*   m_pCategoryDelegate;
   ContactMethodDelegate* m_pContactMethodDelegate;
   ContactDelegate*       m_pContactDelegate;

public Q_SLOTS:
   virtual void keyPressEvent(QKeyEvent* event) override;

private Q_SLOTS:
   void slotContextMenu    ( QModelIndex index     );

private Q_SLOTS:
   void showContext(const QModelIndex& index);
   void transferEvent( QMimeData* data   );
   void expandTree  ();
   void expandTreeRows(const QModelIndex& idx);
   void slotDoubleClick(const QModelIndex& index);
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
   bool eventFilter(QObject *obj, QEvent *event) override;

private:
   ContactDock* m_pDock;

};

#endif //CONTACT_DOCK_H
