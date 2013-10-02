/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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

#include <QtGui/QWidget>
#include <QtCore/QModelIndex>

//Qt
class QListView;
class QTreeView;
class QLabel;

//SFLPhone
class Call;
class NumberCompletionModel;
class QModelIndex;

class AutoCompletion : public QWidget {
   Q_OBJECT

public:
   AutoCompletion(QTreeView* parent = nullptr);
   virtual ~AutoCompletion();

   //Getters
   Call* call() const;

protected:
   //Virtual events
   void paintEvent ( QPaintEvent*  event);
   bool eventFilter( QObject *obj, QEvent *event);

private:
   //Attributes
   QListView* m_pView;
   QLabel*    m_pLabel;
   NumberCompletionModel* m_pModel;

   //Helpers
   bool brightOrDarkBase();

public Q_SLOTS:
   void moveUp();
   void moveDown();
   void setCall(Call* call);
   
private Q_SLOTS:
   void selectionChanged(const QModelIndex& idx = QModelIndex());
};

#endif