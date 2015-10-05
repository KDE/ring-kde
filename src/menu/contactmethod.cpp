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
#include "contactmethod.h"

//Qt
#include <QtCore/QMimeData>
#include <QtWidgets/QApplication>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLineEdit>
#include <QtGui/QClipboard>

//KDE
#include <klocalizedstring.h>

//Ring
#include <mime.h>
#include <person.h>
#include <call.h>
#include <callmodel.h>
#include <personmodel.h>
#include <numbercategory.h>
#include <categorizedbookmarkmodel.h>

#include <klib/helperfunctions.h>

Menu::ContactMethod::ContactMethod(QWidget* parent) : QMenu(parent)
{
   m_pCallAgain   = new QAction(this);
   m_pCallAgain->setText       ( i18n("Call Again")         );
   m_pCallAgain->setIcon       ( QIcon::fromTheme("call-start")        );

   m_pEditPerson = new QAction(this);
   m_pEditPerson->setText     ( i18n("Edit contact")       );
   m_pEditPerson->setIcon     ( QIcon::fromTheme("contact-new")       );

   m_pCopy       = new QAction(this);
   m_pCopy->setText            ( i18n("Copy")               );
   m_pCopy->setIcon            ( QIcon::fromTheme("edit-copy")         );

   m_pEmail      = new QAction(this);
   m_pEmail->setText           ( i18n("Send Email")         );
   m_pEmail->setIcon           ( QIcon::fromTheme("mail-message-new")  );
   m_pEmail->setEnabled        ( false                      );

   m_pAddPhone   = new QAction(this);
   m_pAddPhone->setText        ( i18n("Add Phone Number")   );
   m_pAddPhone->setIcon        ( QIcon::fromTheme("list-resource-add") );
   m_pEmail->setEnabled        ( false                      );

   m_pBookmark   = new QAction(this);
   m_pBookmark->setText        ( i18n("Remove Bookmark")    );
   m_pBookmark->setIcon        ( QIcon::fromTheme("list-remove")       );

   connect(m_pCallAgain   , SIGNAL(triggered()) , this,SLOT(callAgain())  );
   connect(m_pEditPerson  , SIGNAL(triggered()) , this,SLOT(editPerson()));
   connect(m_pCopy        , SIGNAL(triggered()) , this,SLOT(copy())       );
   connect(m_pEmail       , SIGNAL(triggered()) , this,SLOT(sendEmail())  );
   connect(m_pAddPhone    , SIGNAL(triggered()) , this,SLOT(addPhone())   );
   connect(m_pBookmark    , SIGNAL(triggered()) , this,SLOT(removeBookmark())   );

   addAction( m_pCallAgain   );
   addAction( m_pEditPerson );
   addAction( m_pAddPhone    );
   addAction( m_pCopy        );
   addAction( m_pEmail       );
   addAction( m_pBookmark    );
}

void Menu::ContactMethod::setContactMethod(::ContactMethod* p)
{
   m_pCurrentCM = p;
}

void Menu::ContactMethod::removeBookmark()
{
//    if (m_CurrentIndex.isValid()) {
//       CategorizedBookmarkModel::instance()->remove(m_CurrentIndex);
//       m_CurrentIndex = QModelIndex();
//       //expandTree();
//    }
}


///Copy contact to clipboard
void Menu::ContactMethod::copy()
{
   qDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();

   if (m_pCurrentCM) {
      Person* c = m_pCurrentCM->contact();

      //A bookmark can exist without a contact
      if (c) {
         mimeData->setData(RingMimes::CONTACT, c->uid());
         QString numbers(c->formattedName()+": ");
         QString numbersHtml("<b>"+c->formattedName()+"</b><br />\n");
         foreach (::ContactMethod* number, c->phoneNumbers()) {
            numbers     += number->uri()+" ("+number->category()->name()+")  ";
            numbersHtml += number->uri()+" ("+number->category()->name()+")  <br />\n";
         }
         mimeData->setData("text/plain", numbers.toUtf8());
         mimeData->setData("text/html", numbersHtml.toUtf8());
      }
      else {
         mimeData->setData("text/plain", QString(m_pCurrentCM->primaryName()+'\n'+m_pCurrentCM->uri()).toUtf8());
         mimeData->setData("text/html", QString("<b>"+m_pCurrentCM->primaryName()+"</b>\n<br>"+m_pCurrentCM->uri()).toUtf8());
      }

      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setMimeData(mimeData);
   }
}

///Call the same number again
void Menu::ContactMethod::callAgain()
{
   if ( m_pCurrentCM ) {
      const QString name = m_pCurrentCM->contact()?m_pCurrentCM->contact()->formattedName() : m_pCurrentCM->primaryName();
      Call* call = CallModel::instance()->dialingCall(m_pCurrentCM);
      if (call) {
         call->performAction(Call::Action::ACCEPT);
      }
      else {
         HelperFunctions::displayNoAccountMessageBox(this);
      }
   }
}

///Edit this contact
void Menu::ContactMethod::editPerson()
{
   qDebug() << "Edit contact";

   if (m_pCurrentCM) {
      if (m_pCurrentCM->contact())
         m_pCurrentCM->contact()->edit();
      else {
         //Add a contact
         Person* aPerson = new Person();
         aPerson->setContactMethods({m_pCurrentCM});
         aPerson->setFormattedName(m_pCurrentCM->primaryName());
         PersonModel::instance()->addNewPerson(aPerson);
      }
   }
}

///Add a new phone number for this contact
//TODO
void Menu::ContactMethod::addPhone()
{
   qDebug() << "Adding to contact";

   if (m_pCurrentCM) {
      if (m_pCurrentCM->contact()) {
         bool ok;
         const QString text = QInputDialog::getText(this, i18n("Enter a new number"), i18n("New number:"),QLineEdit::Normal, QString(), &ok);
         if (ok && !text.isEmpty()) {
//             ContactMethod* n = PhoneDirectoryModel::instance()->getNumber(text,"work");
//             m_pCurrentCM->contact()->addContactMethod(n); //FIXME
         }
      }
      else {
         //Better use the full dialog for this
         editPerson();
      }
   }
}

void Menu::ContactMethod::sendEmail() {
   
}
