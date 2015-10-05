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

//Parent
#include "historydock.h"

//Qt
#include <QtCore/QString>
#include <QtCore/QDate>
#include <QtCore/QProcess>
#include <QtCore/QTimer>
#include <QtCore/QMimeData>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMenu>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>
#include <QtWidgets/QDialog>
#include <QtGui/QKeyEvent>

//KDE
#include <QDebug>
#include <QIcon>
#include <KDatePicker>
#include <klocalizedstring.h>
#include <QAction>
#include <KColorScheme>

//Ring
#include "mainwindow.h"
#include "widgets/categorizedtreeview.h"
#include "widgets/bookmarkdock.h"
#include "klib/kcfg_settings.h"
#include "categorizedhistorymodel.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "callmodel.h"
#include "mime.h"
#include "contactmethod.h"
#include "phonedirectorymodel.h"
#include "collectioninterface.h"
#include "personmodel.h"
#include <conf/account/accountpages/dlgprofiles.h>
#include <conf/account/delegates/categorizeddelegate.h>
#include "../delegates/historydelegate.h"
#include "../menu/call.h"

//Ring library
#include <categorizedbookmarkmodel.h>
#include "klib/helperfunctions.h"


#define CURRENT_SORTING_MODE m_pSortByCBB->currentIndex()

///Event filter allowing to write text on the Tree widget to filter it.
bool KeyPressEater::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::KeyPress) {
      QKeyEvent* e = (QKeyEvent*)event;
      if (e->key() != Qt::Key_Up && e->key() != Qt::Key_Down) {
         m_pDock->keyPressEvent(e);
         return true;
      }
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

///Constructor
HistoryDock::HistoryDock(QWidget* parent) : QDockWidget(parent),m_pMenu(nullptr)
{
   setObjectName("historyDock");
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);
   setMinimumSize(250,0);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

   m_pSortByCBB->bindToModel(
      CategorizedHistoryModel::SortedProxy::instance()->categoryModel         (),
      CategorizedHistoryModel::SortedProxy::instance()->categorySelectionModel()
   );

   CategorizedDelegate* delegate = new CategorizedDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pView->setViewType(CategorizedTreeView::ViewType::History);

   QSortFilterProxyModel* proxy = CategorizedHistoryModel::SortedProxy::instance()->model();

   m_pView->setModel(proxy);
   m_pKeyPressEater = new KeyPressEater(this);
   m_pView->installEventFilter(m_pKeyPressEater);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::DescendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex)), this, SLOT(slotContextMenu(QModelIndex)));
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotDoubleClick(QModelIndex)));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), proxy , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));
   connect(proxy,SIGNAL(modelReset()), this , SLOT(expandTree()));
   connect(CategorizedHistoryModel::instance() ,SIGNAL(layoutChanged()), this , SLOT(expandTree())                );
   expandTree();

   m_pSortByCBB->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);

   setWidget(mainWidget);

   setWindowTitle(i18nc("History tab","History"));

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::historySortMode());

} //HistoryDock

///Destructor
HistoryDock::~HistoryDock()
{
   delete m_pKeyPressEater;

   if (m_pMenu)
      delete m_pMenu;
} //~HistoryDock


/*****************************************************************************
 *                                                                           *
 *                              Keyboard handling                            *
 *                                                                           *
 ****************************************************************************/

///Handle keyboard input and redirect them to the filterbox
void HistoryDock::keyPressEvent(QKeyEvent* event) {
   int key = event->key();
   if(key == Qt::Key_Escape)
      m_pFilterLE->setText(QString());
   else if (key == Qt::Key_Return || key == Qt::Key_Enter) {
      slotDoubleClick(m_pView->selectionModel()->currentIndex());
   }
   else if((key == Qt::Key_Backspace) && (m_pFilterLE->text().size()))
      m_pFilterLE->setText(m_pFilterLE->text().left( m_pFilterLE->text().size()-1 ));
   else if (!event->text().isEmpty() && !(key == Qt::Key_Backspace))
      m_pFilterLE->setText(m_pFilterLE->text()+event->text());
} //keyPressEvent


/*****************************************************************************
 *                                                                           *
 *                                  Helpers                                  *
 *                                                                           *
 ****************************************************************************/

///Expand the tree according to the user preferences
void HistoryDock::expandTree()
{
   m_pView->expandToDepth(1);
}


/*****************************************************************************
 *                                                                           *
 *                                Menu handling                              *
 *                                                                           *
 ****************************************************************************/


void HistoryDock::slotContextMenu(const QModelIndex& index)
{
   QModelIndex idx = (static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::CALL)
      return;
   if (!m_pMenu) {
      m_pMenu = new Menu::Call(this);
   }
   m_pCurrentCall = static_cast<Call*>(static_cast<CategorizedCompositeNode*>(idx.internalPointer())->getSelf());
   m_pMenu->exec(QCursor::pos());
}

void HistoryDock::slotDoubleClick(const QModelIndex& index)
{
   if (!index.isValid())
      return;
   QModelIndex idx = (static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::CALL)
      return;
   m_pCurrentCall = static_cast<Call*>(static_cast<CategorizedCompositeNode*>(idx.internalPointer())->getSelf());

   if (!m_pMenu)
      m_pMenu = new Menu::Call(this);

   m_pMenu->slotCallAgain();
}

