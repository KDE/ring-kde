/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "accountstatusviewer.h"

//Qt
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>

//Ring
#include <account.h>
#include <accountstatusmodel.h>

AccountStatusViewer::AccountStatusViewer(Account* a, QWidget* parent) : QDialog(parent),
m_pView(new QTableView(this))
{
   QVBoxLayout* l = new QVBoxLayout(this);
   l->addWidget(m_pView);
   m_pView->setWordWrap(true);
   m_pView->setModel(a->statusModel());

   if (m_pView->horizontalHeader()) {
      m_pView->horizontalHeader()->setSectionResizeMode (0,QHeaderView::Stretch         );
      m_pView->horizontalHeader()->setSectionResizeMode (1,QHeaderView::ResizeToContents);
      m_pView->horizontalHeader()->setSectionResizeMode (2,QHeaderView::ResizeToContents);
      m_pView->horizontalHeader()->setSectionResizeMode (3,QHeaderView::ResizeToContents);
   }

   resize(800,600);
}

AccountStatusViewer::~AccountStatusViewer()
{

}
