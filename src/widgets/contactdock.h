/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
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

#include "ui_dockbase.h"

//Qt
#include <QtCore/QHash>
#include <QtGui/QDockWidget>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeWidgetItem>

//SFLPhone
#include "categorizedtreeview.h"

//Qt
class QSplitter;
class QListWidget;
class QTreeWidgetItem;
class QCheckBox;
class QMenu;
class QTreeView;

//KDE
class KLineEdit;
class KComboBox;
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

class ContactSortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit ContactSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}
protected:
   virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
   {
      if (!source_parent.isValid() || source_parent.parent().isValid())
         return true;

      return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
   }
};

///ContactDock: Dock to access contacts
class ContactDock : public QDockWidget, public Ui_DockBase
{
   Q_OBJECT
public:
   //Constructor
   explicit ContactDock(QWidget* parent = nullptr);
   virtual ~ContactDock();

private:
   //Attributes
//    KLineEdit*                   m_pFilterLE      ;
//    QSplitter*                   m_pSplitter      ;
//    CategorizedTreeView*         m_pView          ;
   QListWidget*                 m_pCallView      ;
//    KComboBox*                   m_pSortByCBB     ;
   QCheckBox*                   m_pShowHistoCK   ;
   QMenu*                       m_pMenu          ;
   Contact*                     m_pCurrentContact;
   QString                      m_PreselectedNb  ;
   QSortFilterProxyModel*       m_pProxyModel    ;
   ContactProxyModel*           m_pSourceModel   ;

   //Actions
   KAction* m_pCallAgain   ;
   KAction* m_pEditContact ;
   KAction* m_pCopy        ;
   KAction* m_pEmail       ;
   KAction* m_pAddPhone    ;
   KAction* m_pBookmark    ;

   //Helper
   QString showNumberSelector(bool& ok);
   
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
//    void loadContactHistory ( QTreeWidgetItem* item );
//    void filter             ( const QString& text   );
   void setHistoryVisible  ( bool visible          );
//    void reloadHistoryConst (                       );
   void slotContextMenu    ( QModelIndex index     );

private Q_SLOTS:
   ///Menu actions
   void showContext(const QModelIndex& index);
   void sendEmail   ();
   void callAgain   ();
   void copy        ();
   void editContact ();
   void addPhone    ();
   void bookmark    ();
   void transferEvent( QMimeData* data   );
   void expandTree  ();
   void setCategory (int index);
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
   bool eventFilter(QObject *obj, QEvent *event);

private:
   ContactDock* m_pDock;

};

#endif //CONTACT_DOCK_H
