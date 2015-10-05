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
#include "contactdock.h"

//Qt
#include <QProcess>
#include <QtCore/QMimeData>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMenu>
#include <QtGui/QClipboard>
#include <QtWidgets/QListWidget>
#include <QtGui/QKeyEvent>
#include <QInputDialog>

//KDE
#include <QDebug>
#include <klocalizedstring.h>
#include <QIcon>
#include <QAction>
#include <kmessagebox.h>

//Ring
#include "mainwindow.h"
#include "view.h"
#include "bookmarkdock.h"/*
#include "kphonenumberselector.h"*/

//Ring library
#include <categorizedbookmarkmodel.h>
#include "categorizedtreeview.h"
#include "categorizedhistorymodel.h"
#include "call.h"
#include "person.h"
#include "mime.h"
#include "personmodel.h"
#include "numbercategory.h"
#include "phonedirectorymodel.h"
#include "contactmethod.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "klib/helperfunctions.h"
#include "klib/kcfg_settings.h"
#include "categorizedcontactmodel.h"
#include <conf/account/delegates/categorizeddelegate.h>
#include "../delegates/contactdelegate.h"
#include "../delegates/phonenumberdelegate.h"
#include "../menu/person.h"

///Forward keypresses to the filter line edit
bool KeyPressEaterC::eventFilter(QObject *obj, QEvent *event)
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
ContactDock::ContactDock(QWidget* parent) : QDockWidget(parent),m_pMenu(nullptr)
{
   setObjectName("contactDock");
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);

   m_pSortByCBB->bindToModel(CategorizedContactModel::SortedProxy::instance()->categoryModel(),CategorizedContactModel::SortedProxy::instance()->categorySelectionModel());

   setWidget(mainWidget);
   m_pKeyPressEater = new KeyPressEaterC( this );


   m_pCategoryDelegate = new CategorizedDelegate(m_pView);
   m_pContactMethodDelegate = new ContactMethodDelegate();
   m_pContactDelegate = new ContactDelegate(m_pView);
   m_pContactMethodDelegate->setView(m_pView);
   m_pContactDelegate->setChildDelegate(m_pContactMethodDelegate);
   m_pCategoryDelegate->setChildDelegate(m_pContactDelegate);
   m_pCategoryDelegate->setChildChildDelegate(m_pContactMethodDelegate);
   m_pView->setDelegate(m_pCategoryDelegate);
   m_pView->setViewType(CategorizedTreeView::ViewType::Person);

   CategorizedContactModel::instance()->setUnreachableHidden(ConfigurationSkeleton::hidePersonWithoutPhone());
   QSortFilterProxyModel* proxy = CategorizedContactModel::SortedProxy::instance()->model();
   m_pView->setModel(proxy);
   m_pView->installEventFilter(m_pKeyPressEater);
   m_pView->setSortingEnabled(true);
   m_pView->sortByColumn(0,Qt::AscendingOrder);
   connect(m_pView,SIGNAL(contextMenuRequest(QModelIndex))     , this , SLOT(slotContextMenu(QModelIndex)));
   connect(proxy ,SIGNAL(layoutChanged()), this , SLOT(expandTree()));
   connect(proxy ,SIGNAL(rowsInserted(QModelIndex,int,int)), this , SLOT(expandTreeRows(QModelIndex)));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), proxy , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));

   m_pSortByCBB->setCurrentIndex(ConfigurationSkeleton::contactSortMode());

   connect(m_pView                       ,SIGNAL(doubleClicked(QModelIndex)),                           this, SLOT(slotDoubleClick(QModelIndex))        );
   setWindowTitle(i18nc("Contact tab","Contact"));
   expandTree();
} //ContactDock

///Destructor
ContactDock::~ContactDock()
{
   if (m_pMenu) {
      delete m_pMenu        ;
   }
   //Delegates
   delete m_pCategoryDelegate;

   //Models
   delete m_pKeyPressEater;

}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Called when someone right click on the 'index'
void ContactDock::slotContextMenu(QModelIndex index)
{
   showContext(index);
}

void ContactDock::slotDoubleClick(const QModelIndex& index)
{
   const QModelIndex idx = (static_cast<const QSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   const CategorizedCompositeNode* modelItem = static_cast<CategorizedCompositeNode*>(idx.internalPointer());
   if (modelItem->type() == CategorizedCompositeNode::Type::NUMBER) {
      const ContactMethod* nb  = qvariant_cast<ContactMethod*>(index.data((int)ContactMethod::Role::Object));
      m_pCurrentPerson = nullptr;
      if (nb) {
         if (!m_pMenu)
            m_pMenu = new Menu::Person(this);

         m_pMenu->callAgain(nb);
      }
   }
   else if (modelItem->type() == CategorizedCompositeNode::Type::CONTACT) {
      m_pCurrentPerson = static_cast<Person*>((modelItem)->getSelf());

      if (!m_pMenu)
         m_pMenu = new Menu::Person(this);

      m_pMenu->callAgain();
   }
}


///Show the context menu
void ContactDock::showContext(const QModelIndex& index)
{
   if (!index.parent().isValid())
      return;

   if (index.parent().isValid()  && !index.parent().parent().isValid()) {
      const QString& email = index.data((int)Person::Role::PreferredEmail).toString();
      Person* ct = qvariant_cast<Person*>(index.data((int)Person::Role::Object));
      if (ct) {
         m_pMenu->setPerson(ct);

         m_pCurrentPerson = ct;
         Person::ContactMethods numbers = ct->phoneNumbers();
      }
   }
   else if (index.parent().parent().isValid()) {
      m_pCurrentPerson = qvariant_cast<Person*>(index.data((int)Person::Role::Object));;
   }
   else {
      m_pCurrentPerson = nullptr;
   }
   if (!m_pMenu) {
      m_pMenu = new Menu::Person(this);
   }
   m_pMenu->exec(QCursor::pos());
} //showContext

///Called when a call is dropped on transfer
void ContactDock::transferEvent(QMimeData* data)
{
   if (data->hasFormat( RingMimes::CALLID)) {
      bool ok = false;

      if (!m_pMenu)
         m_pMenu = new Menu::Person(this);

      const ::ContactMethod* result = m_pMenu->showNumberSelector(ok);

      if (ok && result) {
         Call* call = CallModel::instance()->fromMime(data->data(RingMimes::CALLID));
         if (dynamic_cast<Call*>(call)) {
            CallModel::instance()->transfer(call, result);
         }
      }
   }
   else
      qDebug() << "Invalid mime data";
}


/*****************************************************************************
 *                                                                           *
 *                             Keyboard handling                             *
 *                                                                           *
 ****************************************************************************/

///Handle keypresses ont the dock
void ContactDock::keyPressEvent(QKeyEvent* event) {
   int key = event->key();
   if(key == Qt::Key_Escape)
      m_pFilterLE->setText(QString());
   else if(key == Qt::Key_Return || key == Qt::Key_Enter) {

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
void ContactDock::expandTree()
{
   m_pView->expandToDepth( 2 );
}

void ContactDock::expandTreeRows(const QModelIndex& idx)
{
   if (!idx.isValid()) //Only top level
      m_pView->expandToDepth( 2 );
}
