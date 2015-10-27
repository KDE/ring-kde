/***************************************************************************
 *   Copyright (C) 2013-2015 Savoir-Faire Linux                            *
 *   Copyright (C) 2015 Emmanuel Lepage Vallee                             *
 *   Author: Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                         *
 * This library is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Library General Public             *
 * License version 2 as published by the Free Software Foundation.         *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Library General Public License for more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#include "call.h"

//KDE
#include <klocalizedstring.h>

//Qt
#include <QtCore/QProcess>
#include <QtCore/QMimeData>
#include <QtWidgets/QDialog>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>

//Ring
#include <personmodel.h>
#include <callmodel.h>
#include <availableaccountmodel.h>
#include <collectioninterface.h>
#include <phonedirectorymodel.h>
#include <categorizedbookmarkmodel.h>
#include <categorizedhistorymodel.h>
#include <mime.h>

#include <klib/helperfunctions.h>
#include "../conf/account/accountpages/dlgprofiles.h" //TODO remove

Menu::Call::Call(QWidget* parent) : QMenu(parent)
{
   m_pCallAgain = new QAction(this);
   m_pCopy      = new QAction(this);
   m_pEmail     = new QAction(this);
   m_pRemove    = new QAction(this);
   m_pBookmark  = new QAction(this);

   m_pCallAgain->setShortcut    ( Qt::Key_Enter                  );
   m_pCallAgain->setText        ( i18n("Call Again")             );
   m_pCallAgain->setIcon        ( QIcon::fromTheme("call-start")            );

   m_pCopy->setShortcut         ( Qt::CTRL + Qt::Key_C           );
   m_pCopy->setText             ( i18n("Copy")                   );
   m_pCopy->setIcon             ( QIcon::fromTheme("edit-copy")             );

   m_pEmail->setShortcut        ( Qt::CTRL + Qt::Key_M           );
   m_pEmail->setText            ( i18n("Send Email")             );
   m_pEmail->setIcon            ( QIcon::fromTheme("mail-message-new")      );
   m_pEmail->setDisabled        ( true                           );

   m_pRemove->setShortcut       ( Qt::Key_Shift + Qt::Key_Delete );
   m_pRemove->setText           ( i18n("Remove")                 );
   m_pRemove->setIcon           ( QIcon::fromTheme("edit-delete")           );

   m_pBookmark->setShortcut     ( Qt::CTRL + Qt::Key_D           );
   m_pBookmark->setText         ( i18n("Bookmark")               );

   /*if (!idx.data(static_cast<int>(Call::Role::IsBookmark)).toBool()) {
      m_pBookmark->setText      ( i18n("Bookmark")               );
      m_pBookmark->setIcon      ( QIcon::fromTheme("bookmarks")             );
   }
   else {
      m_pBookmark->setText      ( i18n("Remove bookmark")        );
      m_pBookmark->setIcon      ( QIcon::fromTheme("edit-delete")           );
   }*/

   addAction( m_pCallAgain    );

   //Allow to add new contacts
   QMenu* subMenu = nullptr;
   for (CollectionInterface* col : PersonModel::instance().collections(CollectionInterface::SupportedFeatures::ADD | CollectionInterface::SupportedFeatures::MANAGEABLE)) {
      if (!subMenu)
         subMenu = addMenu(QIcon::fromTheme("contact-new"), i18n("Add new contact"));
      QAction* a = new QAction(this);
      a->setText(col->name());
      a->setIcon(qvariant_cast<QIcon>(col->icon()));
      subMenu->addAction(a);
      connect(a, &QAction::triggered, [this,col]() {slotAddPerson(col);});
   }

   addAction( m_pCopy     );
   addAction( m_pEmail    );
   addAction( m_pRemove   );
   addAction( m_pBookmark );

   connect(m_pCallAgain, &QAction::triggered, this, &Menu::Call::slotCallAgain );
   connect(m_pCopy     , &QAction::triggered, this, &Menu::Call::slotCopy      );
   connect(m_pEmail    , &QAction::triggered, this, &Menu::Call::slotSendEmail );
   connect(m_pRemove   , &QAction::triggered, this, &Menu::Call::slotRemove    );
   connect(m_pBookmark , &QAction::triggered, this, &Menu::Call::slotBookmark  );
}

void Menu::Call::setCall(::Call* p)
{
   m_pCurrentCall = p;

   m_pRemove->setEnabled(m_pCurrentCall && m_pCurrentCall->collection() && m_pCurrentCall->collection()->supportedFeatures() & CollectionInterface::SupportedFeatures::REMOVE);
}

void Menu::Call::slotSendEmail()
{
   if (!m_pCurrentCall) return;
   qDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   Person* ct = m_pCurrentCall->peerContactMethod()->contact();
   if (ct)
      myProcess->start("xdg-email", (arguments << ct->preferredEmail()));
}

void Menu::Call::slotRemove()
{
   if (m_pCurrentCall && m_pCurrentCall->collection()->supportedFeatures() & CollectionInterface::SupportedFeatures::REMOVE) {
      CategorizedHistoryModel::instance().deleteItem(m_pCurrentCall); //TODO add add and remove to the manager
   }
}

void Menu::Call::slotCallAgain()
{
   if (!m_pCurrentCall) return;
   qDebug() << "Calling "<< m_pCurrentCall->peerContactMethod();
   ::Call* call = CallModel::instance().dialingCall(m_pCurrentCall->peerContactMethod());
   if (call) {
      call->performAction  ( ::Call::Action::ACCEPT  );
   }
   else {
      HelperFunctions::displayNoAccountMessageBox(this);
   }
}

void Menu::Call::slotCopy()
{
   if (!m_pCurrentCall) {
      qDebug() << "No call to copy";
      return;
   }

   qDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   mimeData->setData(RingMimes::CALLID, CallModel::instance().getMime(m_pCurrentCall));

   mimeData->setData(RingMimes::PHONENUMBER, m_pCurrentCall->peerContactMethod()->uri().toUtf8());

   QString numbers,numbersHtml;
   const Person* ct = m_pCurrentCall->peerContactMethod()->contact();

   if (ct) {
      numbers     = ct->formattedName()+": "+m_pCurrentCall->peerContactMethod()->uri();
      numbersHtml = "<b>"+ct->formattedName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->peerContactMethod()->uri());
   }
   else {
      numbers     = m_pCurrentCall->peerName()+": "+m_pCurrentCall->peerContactMethod()->uri();
      numbersHtml = "<b>"+m_pCurrentCall->peerName()+"</b><br />"+HelperFunctions::escapeHtmlEntities(m_pCurrentCall->peerContactMethod()->uri());
   }

   mimeData->setData("text/plain", numbers.toUtf8()    );
   mimeData->setData("text/html",  numbersHtml.toUtf8());

   QApplication::clipboard()->setMimeData(mimeData);
}

void Menu::Call::slotAddPerson(CollectionInterface* col)
{
   qDebug() << "Adding contact";
   QDialog* d = new QDialog();
   DlgProfiles* p = new DlgProfiles(this,m_pCurrentCall->peerName(),m_pCurrentCall->peerContactMethod()->uri());

   QHBoxLayout* l = new QHBoxLayout(d);
   l->addWidget(p);
   connect(p,&DlgProfiles::requestSave,[p,col,this,d]() {
      Person* aPerson = p->create(col);

      aPerson->setContactMethods({PhoneDirectoryModel::instance().getNumber(m_pCurrentCall->peerContactMethod()->uri(),aPerson,nullptr, "Home")});

      PersonModel::instance().addNewPerson(aPerson,col);
      d->close();
   });
   connect(p,&DlgProfiles::requestCancel,[col,d]() {
      d->close();
   });

   d->exec();

}

void Menu::Call::slotBookmark()
{
   CategorizedBookmarkModel::instance().addBookmark(m_pCurrentCall->peerContactMethod());
}