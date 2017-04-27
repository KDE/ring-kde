/***************************************************************************
 *   Copyright (C)  2013 by Savoir-Faire Linux                             *
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
#include "callviewtoolbar.h"

//Qt
#include <QtWidgets/QTreeView>
#include <QtWidgets/QTableView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtCore/QDebug>
#include <QtCore/QSortFilterProxyModel>

//Ring
#include "mainwindow.h"
#include "actioncollection.h"
#include <tip/tipmanager.h>
#include <proxies/simplerotateproxy.h>
#include <call.h>
#include <useractionmodel.h>
#include <callmodel.h>
#include "delegates/toolbardelegate.h"

CallViewToolbar::CallViewToolbar(QTreeView* parent) : OverlayToolbar(parent),m_pParent(parent)
{
   m_pContent = new QTableView(this);
   m_pContent->setShowGrid(false);
   m_pContent->setStyleSheet("QTableView::item{border-width:0px;border-style:none;}"
   "QTableView{"
      "margin-top:7px;"
      "padding:0px;"
      "background-color:transparent;"
      "border-width:0;"
      "border-style:none;"
   "}");

   m_pContent->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   m_pContent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

   if (m_pContent->verticalHeader())
      m_pContent->verticalHeader()->setVisible(false);

   if (m_pContent->horizontalHeader())
      m_pContent->horizontalHeader()->setVisible(false);

   SimpleRotateProxy* pm = new SimpleRotateProxy(this);
   pm->setSourceModel(CallModel::instance().userActionModel()->activeActionModel());

   m_pContent->setItemDelegate(new ToolbarDelegate(m_pContent));
   m_pContent->setModel(pm);

   m_pContent->setRowHeight(0,55); //FIXME don't hardcode

   addWidget(m_pContent);
   setVisible(true);

   auto lambda = [this,pm]() {
      const int count = pm->columnCount(QModelIndex());
      for (int i =0; i<count;i++)
         m_pContent->setColumnWidth(i,(width()-14)/count);
   };

   connect(pm,&SimpleRotateProxy::layoutChanged,lambda);
   connect(this,&CallViewToolbar::resized,lambda);
   connect(m_pContent,&QTableView::clicked,[](const QModelIndex & index ) {

      CallModel::instance().userActionModel()->execute(index);
   });
}

CallViewToolbar::~CallViewToolbar()
{
}

void CallViewToolbar::updateState()
{
   setVisible(true);
} //updateState

// kate: space-indent on; indent-width 3; replace-tabs on;
