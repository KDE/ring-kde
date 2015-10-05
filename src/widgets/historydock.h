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
#ifndef HISTORY_DOCK_H
#define HISTORY_DOCK_H

#include "ui_dockbase.h"
#include <QtWidgets/QDockWidget>

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>
#include <QtCore/QSortFilterProxyModel>
#include <QDate>

//Qt
class QLabel;
class QCheckBox;
class QPushButton;
class QDate;

//KDE
class KDatePicker;
class QAction;

//Ring
#include "categorizedhistorymodel.h"
class KeyPressEater;
class CollectionInterface;
namespace Menu {
   class Call;
}

//Typedef

///HistoryDock: Dock to see the previous Ring calls
class HistoryDock : public QDockWidget, public Ui_DockBase {
   Q_OBJECT

public:
   //Friends
   friend class KeyPressEater;

   //Constructors
   explicit HistoryDock(QWidget* parent = nullptr);
   virtual ~HistoryDock();

   //Getters
   time_t stopTime () const;
   time_t startTime() const;

private:
   //Attributes
   KeyPressEater*         m_pKeyPressEater   ;

   //Menu
   Menu::Call*        m_pMenu          ;
   Call*         m_pCurrentCall   ;

public Q_SLOTS:
   virtual void keyPressEvent(QKeyEvent* event) override;

private Q_SLOTS:
   void expandTree           (              );
   void slotDoubleClick(const QModelIndex& index);

   //Menu
   void slotContextMenu(const QModelIndex& index);
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
   bool eventFilter(QObject *obj, QEvent *event) override;
private:
   HistoryDock* m_pDock;
};

#endif
