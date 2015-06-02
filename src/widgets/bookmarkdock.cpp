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
#include "bookmarkdock.h"

//Qt
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMenu>
#include <QtGui/QClipboard>
#include <QMimeData>
#include <QInputDialog>

//KDE
#include <klocalizedstring.h>
#include <QIcon>
#include <QAction>

//Ring
#include "klib/kcfg_settings.h"
#include "ring.h"
#include "klib/helperfunctions.h"
#include <categorizedbookmarkmodel.h>
#include "categorizedhistorymodel.h"
#include "contactmethod.h"
#include "mime.h"
#include "personmodel.h"
#include "phonedirectorymodel.h"
#include "numbercategory.h"
#include "accountmodel.h"
#include "availableaccountmodel.h"
#include "callmodel.h"
#include "categorizedtreeview.h"
#include "../delegates/categorizeddelegate.h"
#include "../delegates/historydelegate.h"

///Constructor
BookmarkDock::BookmarkDock(QWidget* parent) : QDockWidget(parent), m_pMenu(nullptr),m_pCallAgain(nullptr),
m_pEditPerson(nullptr), m_pCopy(nullptr), m_pEmail(nullptr), m_pAddPhone(nullptr), 
m_pBookmark(nullptr)
{
   setObjectName("bookmarkDock");
   QWidget* mainWidget     = new QWidget   ( this );
   m_pMostUsedCK           = new QCheckBox ( this );
   setupUi(mainWidget);
   m_pSortByCBB   ->setHidden(true);
   setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   setMinimumSize(250,0);

   CategorizedDelegate* delegate = new CategorizedDelegate(m_pView);
   delegate->setChildDelegate(new HistoryDelegate(m_pView));
   m_pView->setDelegate(delegate);
   m_pView->setSortingEnabled(true);
   m_pProxyModel = new BookmarkSortFilterProxyModel(this);
   m_pProxyModel->setSourceModel          ( CategorizedBookmarkModel::instance() );
   m_pProxyModel->setSortRole             ( static_cast<int>(Call::Role::Name  )        );
   m_pProxyModel->setFilterRole           ( static_cast<int>(Call::Role::Filter)        );
   m_pProxyModel->setFilterCaseSensitivity( Qt::CaseInsensitive       );
   m_pProxyModel->setSortCaseSensitivity  ( Qt::CaseInsensitive       );
   m_pProxyModel->setDynamicSortFilter    ( true                      );
   m_pView->setModel(m_pProxyModel);
   m_pProxyModel->sort(0);
   expandTree();

   connect(CategorizedBookmarkModel::instance() ,SIGNAL(layoutChanged()), this , SLOT(expandTree()));
   connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString)), m_pProxyModel , SLOT(setFilterRegExp(QString)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString)), this , SLOT(expandTree()));

   m_pFilterLE->setPlaceholderText(i18n("Filter"));

   m_pMostUsedCK->setChecked(ConfigurationSkeleton::displayPopularAsBookmark());

   setWidget(mainWidget);

   m_pMostUsedCK->setText(i18n("Show most called contacts"));
   m_pMostUsedCK->setVisible(false); //FIXME

   splitter->setStretchFactor(0,7);

   setWindowTitle(i18nc("Bookmark dock","Bookmark"));

//    connect(m_pFilterLE                    , SIGNAL(textChanged(QString))            , this , SLOT(filter(QString))             );
   connect(m_pMostUsedCK                  , SIGNAL(toggled(bool))                   , this , SLOT(reload())                    );
   connect(m_pView                        , SIGNAL(doubleClicked(QModelIndex))      , this , SLOT(slotDoubleClick(QModelIndex)));
   connect(m_pView                        , SIGNAL(contextMenuRequest(QModelIndex)) , this , SLOT(slotContextMenu(QModelIndex)));
   reload();
} //BookmarkDock

///Destructor
BookmarkDock::~BookmarkDock()
{
   delete m_pMostUsedCK;
   delete m_pProxyModel;
   if (m_pMenu) delete m_pMenu;
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

///Show the most popular items
void BookmarkDock::reload()
{
   ConfigurationSkeleton::setDisplayPopularAsBookmark(m_pMostUsedCK->isChecked());
   CategorizedBookmarkModel::instance()->reloadCategories();
} //reload

///Expand the tree according to the user preferences
void BookmarkDock::expandTree()
{
   m_pView->expandToDepth(0);
}

void BookmarkDock::slotDoubleClick(const QModelIndex& index)
{
   QModelIndex idx = (static_cast<const BookmarkSortFilterProxyModel*>(index.model()))->mapToSource(index);
   if (!idx.isValid() || !idx.parent().isValid())
      return;
   if (((CategorizedCompositeNode*)idx.internalPointer())->type() != CategorizedCompositeNode::Type::BOOKMARK)
      return;
   Call* call2 = CallModel::instance()->dialingCall(idx.model()->data(idx,static_cast<int>(Call::Role::Number)).toString(), AvailableAccountModel::currentDefaultAccount());
   call2->setDialNumber  ( idx.model()->data(idx,static_cast<int>(Call::Role::Number)).toString() );
   call2->performAction( Call::Action::ACCEPT   );
}

void BookmarkDock::slotContextMenu( const QModelIndex& index )
{
   if (!index.parent().isValid())
      return;
   if (!m_pCallAgain) {
      m_pCallAgain   = new QAction(this);
      m_pCallAgain->setText       ( i18n("Call Again")         );
      m_pCallAgain->setIcon       ( QIcon::fromTheme("call-start")        );

      m_pEditPerson = new QAction(this);
      m_pEditPerson->setText     ( i18n("Edit contact")       );
      m_pEditPerson->setIcon     ( QIcon::fromTheme("contact-new")       );

      m_pCopy        = new QAction(this);
      m_pCopy->setText            ( i18n("Copy")               );
      m_pCopy->setIcon            ( QIcon::fromTheme("edit-copy")         );

      m_pEmail       = new QAction(this);
      m_pEmail->setText           ( i18n("Send Email")         );
      m_pEmail->setIcon           ( QIcon::fromTheme("mail-message-new")  );
      m_pEmail->setEnabled        ( false                      );

      m_pAddPhone    = new QAction(this);
      m_pAddPhone->setText        ( i18n("Add Phone Number")   );
      m_pAddPhone->setIcon        ( QIcon::fromTheme("list-resource-add") );
      m_pEmail->setEnabled        ( false                      );

      m_pBookmark    = new QAction(this);
      m_pBookmark->setText        ( i18n("Remove Bookmark")    );
      m_pBookmark->setIcon        ( QIcon::fromTheme("list-remove")       );

      connect(m_pCallAgain   , SIGNAL(triggered()) , this,SLOT(callAgain())  );
      connect(m_pEditPerson  , SIGNAL(triggered()) , this,SLOT(editPerson()));
      connect(m_pCopy        , SIGNAL(triggered()) , this,SLOT(copy())       );
      connect(m_pEmail       , SIGNAL(triggered()) , this,SLOT(sendEmail())  );
      connect(m_pAddPhone    , SIGNAL(triggered()) , this,SLOT(addPhone())   );
      connect(m_pBookmark    , SIGNAL(triggered()) , this,SLOT(removeBookmark())   );
   }
   if (!m_pMenu) {
      m_pMenu = new QMenu( this          );
      m_pMenu->addAction( m_pCallAgain   );
      m_pMenu->addAction( m_pEditPerson );
      m_pMenu->addAction( m_pAddPhone    );
      m_pMenu->addAction( m_pCopy        );
      m_pMenu->addAction( m_pEmail       );
      m_pMenu->addAction( m_pBookmark    );
   }
   //TODO Qt5 use lambdas for this
   m_CurrentIndex = m_pProxyModel->mapToSource(index);
   m_pMenu->exec(QCursor::pos());
}

void BookmarkDock::removeBookmark()
{
   if (m_CurrentIndex.isValid()) {
      CategorizedBookmarkModel::instance()->remove(m_CurrentIndex);
      m_CurrentIndex = QModelIndex();
      expandTree();
   }
}


///Copy contact to clipboard
void BookmarkDock::copy()
{
   qDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   ContactMethod* nb = CategorizedBookmarkModel::instance()->getNumber(m_CurrentIndex);

   if (nb) {
      Person* c = nb->contact();

      //A bookmark can exist without a contact
      if (c) {
         mimeData->setData(RingMimes::CONTACT, c->uid());
         QString numbers(c->formattedName()+": ");
         QString numbersHtml("<b>"+c->formattedName()+"</b><br />\n");
         foreach (ContactMethod* number, c->phoneNumbers()) {
            numbers     += number->uri()+" ("+number->category()->name()+")  ";
            numbersHtml += number->uri()+" ("+number->category()->name()+")  <br />\n";
         }
         mimeData->setData("text/plain", numbers.toUtf8());
         mimeData->setData("text/html", numbersHtml.toUtf8());
      }
      else {
         mimeData->setData("text/plain", QString(nb->primaryName()+'\n'+nb->uri()).toUtf8());
         mimeData->setData("text/html", QString("<b>"+nb->primaryName()+"</b>\n<br>"+nb->uri()).toUtf8());
      }

      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setMimeData(mimeData);
   }
}

///Call the same number again
void BookmarkDock::callAgain()
{
   qDebug() << "Calling ";
   ContactMethod* n = CategorizedBookmarkModel::instance()->getNumber(m_CurrentIndex);
   if ( n ) {
      const QString name = n->contact()?n->contact()->formattedName() : n->primaryName();
      Call* call = CallModel::instance()->dialingCall(name, AvailableAccountModel::currentDefaultAccount());
      if (call) {
         call->setDialNumber(n);
         call->setAccount(n->account());
         call->setPeerName(name);
         call->performAction(Call::Action::ACCEPT);
      }
      else {
         HelperFunctions::displayNoAccountMessageBox(this);
      }
   }
}

///Edit this contact
void BookmarkDock::editPerson()
{
   qDebug() << "Edit contact";

   ContactMethod* nb = CategorizedBookmarkModel::instance()->getNumber(m_CurrentIndex);
   if (nb) {
      if (nb->contact())
         nb->contact()->edit();
      else {
         //Add a contact
         Person* aPerson = new Person();
         aPerson->setContactMethods({nb});
         aPerson->setFormattedName(nb->primaryName());
         PersonModel::instance()->addNewPerson(aPerson);
      }
   }
}

///Add a new phone number for this contact
//TODO
void BookmarkDock::addPhone()
{
   qDebug() << "Adding to contact";
   ContactMethod* nb = CategorizedBookmarkModel::instance()->getNumber(m_CurrentIndex);
   if (nb) {
      if (nb->contact()) {
         bool ok;
         const QString text = QInputDialog::getText(this, i18n("Enter a new number"), i18n("New number:"),QLineEdit::Normal, QString(), &ok);
         if (ok && !text.isEmpty()) {
//             ContactMethod* n = PhoneDirectoryModel::instance()->getNumber(text,"work");
//             nb->contact()->addContactMethod(n); //FIXME
         }
      }
      else {
         //Better use the full dialog for this
         editPerson();
      }
   }
}

void BookmarkDock::sendEmail() {
   
}
