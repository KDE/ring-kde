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

#include <QtCore/QHash>
#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidgetItem>
#include "categorizedtreewidget.h"
#include "categorizedtreeview.h"
// #include "../klib/sortabledockcommon.h"
#include "../klib/contactproxymodel.h"
#include "calltreeitem.h"

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
class ContactItemWidget;
class Contact;

class ContactSortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   ContactSortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}
protected:
   virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
   {
      if (!source_parent.isValid() || source_parent.parent().isValid())
         return true;

      return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
   }
};

///ContactDock: Dock to access contacts
class ContactDock : public QDockWidget/*, public SortableDockCommon<CallTreeItem*,QTreeWidgetItem*>*/
{
   Q_OBJECT
public:
   //Constructor
   explicit ContactDock(QWidget* parent = nullptr);
   virtual ~ContactDock();

private:
   //Attributes
   KLineEdit*                   m_pFilterLE      ;
   QSplitter*                   m_pSplitter      ;
   QTreeView*                   m_pView          ;
   QListWidget*                 m_pCallView      ;
   KComboBox*                   m_pSortByCBB     ;
   QCheckBox*                   m_pShowHistoCK   ;
   QList<ContactItemWidget*>    m_Contacts       ;
   QMenu*                       m_pMenu          ;
   Contact*                     m_pCurrentContact;
   QString                      m_PreselectedNb  ;
   QSortFilterProxyModel*       m_pProxyModel    ;
   ContactByNameProxyModel*     m_pSourceModel   ;

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
};

///ContactTree: tree view with additinal drag and drop
class ContactTree : public CategorizedTreeWidget {
   Q_OBJECT
public:
   ///Constructor
   explicit ContactTree(QWidget* parent) : CategorizedTreeWidget(parent) {setUniformRowHeights(false);}
//    virtual QMimeData* mimeData( const QList<QTreeWidgetItem *> items) const;
//    bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);
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
