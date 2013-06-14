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

#include "ui_dockbase.h"
#include <QtGui/QDockWidget>

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QDate>

//Qt
class QString;
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
#include "../lib/historymodel.h"
class KeyPressEater;
class QNumericTreeWidgetItem;
class CategorizedTreeView;

//Typedef

///HistoryDock: Dock to see the previous SFLPhone calls
class HistoryDock : public QDockWidget, public Ui_DockBase {
   Q_OBJECT

public:
   //Friends
   friend class KeyPressEater;

   //Constructors
   explicit HistoryDock(QWidget* parent = nullptr);
   virtual ~HistoryDock();

private:
   QLabel*                m_pFromL           ;
   QLabel*                m_pToL             ;
   KDateWidget*           m_pFromDW          ;
   KDateWidget*           m_pToDW            ;
   QCheckBox*             m_pAllTimeCB       ;
   QPushButton*           m_pLinkPB          ;
   QDate                  m_CurrentFromDate  ;
   QDate                  m_CurrentToDate    ;
   KeyPressEater*         m_pKeyPressEater   ;
   QSortFilterProxyModel* m_pProxyModel      ;
   
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
   
   enum Role {
      Date =0,
      Name,
      Popularity,
      Length
   };

public Q_SLOTS:
   void enableDateRange(bool disable);
   virtual void keyPressEvent(QKeyEvent* event);

private Q_SLOTS:
   void expandTree           (              );
   void slotSetSortRole      ( int          );
   void slotDoubleClick(const QModelIndex& index);
   
   //Menu
   void slotContextMenu(const QModelIndex& index);
   void slotSendEmail        ();
   void slotCallAgain        ();
   void slotCopy             ();
   void slotAaddContact      ();
   void slotAddToContact     ();
   void slotBookmark         ();
};

///Tuned sorting model for the history model
class HistorySortFilterProxyModel : public QSortFilterProxyModel
{
   Q_OBJECT
public:
   explicit HistorySortFilterProxyModel(QObject* parent) : QSortFilterProxyModel(parent) {}
protected:
   virtual bool filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
   {
      if (!source_parent.isValid() ) { //Is a category
         for (int i=0;i<HistoryModel::self()->rowCount(HistoryModel::self()->index(source_row,0,source_parent));i++) {
            if (filterAcceptsRow(i, HistoryModel::self()->index(source_row,0,source_parent)))
               return true;
         }
      }

      return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
   }
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
