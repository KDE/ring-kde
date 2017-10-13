/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#ifndef AUTOCOMPLETION_H
#define AUTOCOMPLETION_H

#include "tips/resizabletip.h"
#include <QtCore/QModelIndex>

//Qt
class QListView;
class QTreeView;
class QLabel;
class QModelIndex;

//Ring
#include <call.h>
class NumberCompletionModel;
class AutoCompletionDelegate2;
class ContactMethod;
class UserActionModel;

class AutoCompletion final : public ResizableTip {
   Q_OBJECT

public:
   explicit AutoCompletion(QTreeView* parent = nullptr);
   virtual ~AutoCompletion();

   //Getters
   Call* call() const;
   ContactMethod* selection() const;

   //Setters
   void setUseUnregisteredAccounts(bool value);

   //Mutator
   void reset();

   int m_Height;

protected:
   virtual void contextMenuEvent ( QContextMenuEvent * e ) override;

private:
   //Attributes
   QListView* m_pView;
   QLabel*    m_pLabel;
   NumberCompletionModel* m_pModel;
   AutoCompletionDelegate2* m_pDelegate;
   UserActionModel* m_pUserActionModel {nullptr};

public Q_SLOTS:
   void moveUp();
   void moveDown();
   void callSelectedNumber();
   void setCall(Call* call);

private Q_SLOTS:
   void selectionChanged(const QModelIndex& idx = QModelIndex());
   void slotLayoutChanged();
   void slotDoubleClicked(const QModelIndex& idx);
   void slotCallStateChanged(Call::State s);

Q_SIGNALS:
   void doubleClicked(ContactMethod* n);
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
