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
#ifndef HISTORY_DOCK_H
#define HISTORY_DOCK_H

#include <QtGui/QDockWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtCore/QDate>
#include "../klib/sortabledockcommon.h"
#include "categorizedtreewidget.h"
#include "calltreeitem.h"

//Qt
class QTreeWidgetItem;
class QString;
class QTreeWidget;
class KComboBox;
class QLabel;
class QCheckBox;
class QPushButton;
class QDate;
class QAction;

//KDE
class KLineEdit;
class KDateWidget;
class KAction;

//SFLPhone
class HistoryTreeItem;
class HistoryTree;
class KeyPressEater;
class QNumericTreeWidgetItem;
class CategorizedTreeView;

//Typedef
typedef QList<HistoryTreeItem*> HistoryList;
typedef QHash<QString,QNumericTreeWidgetItem*> GroupHash;

///HistoryDock: Dock to see the previous SFLPhone calls
class HistoryDock : public QDockWidget, public SortableDockCommon<CallTreeItem*,QTreeWidgetItem*> {
   Q_OBJECT

public:
   //Friends
   friend class KeyPressEater;

   //Constructors
   explicit HistoryDock(QWidget* parent = nullptr);
   virtual ~HistoryDock();

private:
   //Attributes
   HistoryTree*         m_pItemView        ;
   KLineEdit*           m_pFilterLE        ;
   KComboBox*           m_pSortByCBB       ;
   QLabel*              m_pSortByL         ;
   QLabel*              m_pFromL           ;
   QLabel*              m_pToL             ;
   CategorizedTreeView* m_pView            ;
   KDateWidget*         m_pFromDW          ;
   KDateWidget*         m_pToDW            ;
   QCheckBox*           m_pAllTimeCB       ;
   QPushButton*         m_pLinkPB          ;
   HistoryList          m_History          ;
   QDate                m_CurrentFromDate  ;
   QDate                m_CurrentToDate    ;
   KeyPressEater*       m_pKeyPressEater   ;
   GroupHash            m_hGroup           ;
   int                  m_LastNewCall      ;
   
   //Menu
    KAction*     m_pCallAgain     ;
    KAction*     m_pAddContact    ;
    KAction*     m_pAddToContact  ;
    KAction*     m_pCopy          ;
    KAction*     m_pEmail         ;
    KAction*     m_pBookmark      ;
    QMenu*       m_pMenu          ;
    Call*        m_pCurrentCall   ;

   //Mutator
   void updateLinkedDate(KDateWidget* item, QDate& prevDate, QDate& newDate);

public Q_SLOTS:
   void enableDateRange(bool disable);
   virtual void keyPressEvent(QKeyEvent* event);

private Q_SLOTS:
   void filter               ( QString text );
   void updateLinkedFromDate ( QDate   date );
   void updateLinkedToDate   ( QDate   date );
   void expandTree           (              );
   
   //Menu
   void slotContextMenu(const QModelIndex& index);
   void slotSendEmail        ();
   void slotCallAgain        ();
   void slotCopy             ();
   void slotAaddContact      ();
   void slotAddToContact     ();
   void slotBookmark         ();
};

///KeyPressEater: Intercept each keypress to manage it globally
class KeyPressEater : public QObject
{
   Q_OBJECT
public:
   explicit KeyPressEater(HistoryDock* parent) : QObject(parent) {
      m_pDock =  parent;
   }
protected:
   bool eventFilter(QObject *obj, QEvent *event);
private:
   HistoryDock* m_pDock;
};

#endif
