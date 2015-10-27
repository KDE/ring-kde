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
#include "person.h"

//Qt
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QMimeData>
#include <QtWidgets/QApplication>
#include <QtWidgets/QInputDialog>
#include <QtGui/QClipboard>

//KDE
#include <klocalizedstring.h>
#include <KMessageBox>

//Ring
#include <contactmethod.h>
#include <person.h>
#include <callmodel.h>
#include <availableaccountmodel.h>
#include <numbercategory.h>
#include <categorizedbookmarkmodel.h>
#include <phonedirectorymodel.h>
#include <mime.h>

#include <klib/helperfunctions.h>
#include "../widgets/kphonenumberselector.h"

Menu::Person::Person(QWidget* parent) : QMenu(parent)
{
   if (!m_pCallAgain) {
      m_pCallAgain   = new QAction(this);
      m_pCallAgain->setShortcut   ( Qt::CTRL + Qt::Key_Enter   );
      m_pCallAgain->setText       ( i18n("Call Again")         );
      m_pCallAgain->setIcon       ( QIcon::fromTheme("call-start")        );

      m_pEditPerson = new QAction(this);
      m_pEditPerson->setShortcut ( Qt::CTRL + Qt::Key_E       );
      m_pEditPerson->setText     ( i18n("Edit contact")       );
      m_pEditPerson->setIcon     ( QIcon::fromTheme("contact-new")       );
      m_pEditPerson->setDisabled(true);

      m_pCopy        = new QAction(this);
      m_pCopy->setShortcut        ( Qt::CTRL + Qt::Key_C       );
      m_pCopy->setText            ( i18n("Copy")               );
      m_pCopy->setIcon            ( QIcon::fromTheme("edit-copy")         );

      m_pEmail       = new QAction(this);
      m_pEmail->setShortcut       ( Qt::CTRL + Qt::Key_M       );
      m_pEmail->setText           ( i18n("Send Email")         );
      m_pEmail->setIcon           ( QIcon::fromTheme("mail-message-new")  );
      m_pEmail->setEnabled        ( false                      );

      m_pAddPhone    = new QAction(this);
      m_pAddPhone->setShortcut    ( Qt::CTRL + Qt::Key_N       );
      m_pAddPhone->setText        ( i18n("Add Phone Number")   );
      m_pAddPhone->setIcon        ( QIcon::fromTheme("list-resource-add") );
      m_pAddPhone->setEnabled     ( false                      );

      m_pBookmark    = new QAction(this);
      m_pBookmark->setShortcut    ( Qt::CTRL + Qt::Key_D       );
      m_pBookmark->setText        ( i18n("Bookmark")           );
      m_pBookmark->setIcon        ( QIcon::fromTheme("bookmarks")         );

      m_pRemove      = new QAction(this);
      m_pRemove->setShortcut    ( Qt::CTRL + Qt::Key_Shift + Qt::Key_Delete );
      m_pRemove->setText        ( i18n("Delete")               );
      m_pRemove->setIcon        ( QIcon::fromTheme("edit-delete")         );

      connect(m_pCallAgain    , &QAction::triggered,[this] (){callAgain();}          );
      connect(m_pEditPerson   , &QAction::triggered, this, &Menu::Person::editPerson );
      connect(m_pCopy         , &QAction::triggered, this, &Menu::Person::copy       );
      connect(m_pEmail        , &QAction::triggered, this, &Menu::Person::sendEmail  );
      connect(m_pAddPhone     , &QAction::triggered, this, &Menu::Person::addPhone   );
      connect(m_pBookmark     , &QAction::triggered, this, &Menu::Person::bookmark   );
      connect(m_pRemove       , &QAction::triggered, this, &Menu::Person::slotDelete );

      addAction( m_pCallAgain   );
      addAction( m_pEditPerson );
      addAction( m_pAddPhone    );
      addAction( m_pCopy        );
      addAction( m_pRemove      );
      addAction( m_pEmail       );
      addAction( m_pBookmark    );
   }
}

void Menu::Person::setPerson(::Person* p)
{
   m_pCurrentPerson = p;
   m_pEmail->setEnabled(p && !p->preferredEmail().isEmpty());
   m_pRemove->setEnabled(p->collection() && p->collection()->supportedFeatures() & CollectionInterface::SupportedFeatures::REMOVE);
   m_pBookmark->setEnabled(p->phoneNumbers().count() == 1);
}

///Send an email
void Menu::Person::sendEmail()
{
   qDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   QStringList arguments;
   myProcess->start("xdg-email", (arguments << m_pCurrentPerson->preferredEmail()));
}

///Call the same number again
void Menu::Person::callAgain(const ContactMethod* n)
{
   bool ok = false;
   const ContactMethod* number = n?n:showNumberSelector(ok);
   if ( (n || ok) && number) {
      if (auto call = CallModel::instance().dialingCall(const_cast<ContactMethod*>(number)))
         call->performAction(Call::Action::ACCEPT);
      else
         HelperFunctions::displayNoAccountMessageBox(nullptr);
   }
}

///Copy contact to clipboard
void Menu::Person::copy()
{
   qDebug() << "Copying contact";
   QMimeData* mimeData = new QMimeData();
   mimeData->setData(RingMimes::CONTACT, m_pCurrentPerson->uid());
   QString numbers(m_pCurrentPerson->formattedName()+": ");
   QString numbersHtml("<b>"+m_pCurrentPerson->formattedName()+"</b><br />");
   foreach (ContactMethod* number, m_pCurrentPerson->phoneNumbers()) {
      numbers     += number->uri()+" ("+number->category()->name()+")  ";
      numbersHtml += number->uri()+" ("+number->category()->name()+")  <br />";
   }
   mimeData->setData("text/plain", numbers.toUtf8());
   mimeData->setData("text/html", numbersHtml.toUtf8());
   QClipboard* clipboard = QApplication::clipboard();
   clipboard->setMimeData(mimeData);
}

///Edit this contact
void Menu::Person::editPerson()
{
   qDebug() << "Edit contact";
   m_pCurrentPerson->edit();
}

///Add a new phone number for this contact
//TODO
void Menu::Person::addPhone()
{
   qDebug() << "Adding to contact";
   bool ok = false;
   const QString text = QInputDialog::getText(this, i18n("Enter a new number"), i18n("New number:"), QLineEdit::Normal, QString(), &ok);
   if (ok && !text.isEmpty()) {
//       ContactMethod* n = PhoneDirectoryModel::instance().getNumber(text,"work");
//       m_pCurrentPerson->addContactMethod(n); //TODO fixme
   }
}

///Add this contact to the bookmark list
void Menu::Person::bookmark()
{
   const ::Person::ContactMethods numbers = m_pCurrentPerson->phoneNumbers();
   if (numbers.count() == 1) {
      CategorizedBookmarkModel::instance().addBookmark(numbers[0]);
   }
}

void Menu::Person::slotDelete()
{
   if (!m_pCurrentPerson)
      return;
   const int ret = KMessageBox::questionYesNo(this, i18n("Are you sure you want to permanently delete %1?",
      m_pCurrentPerson->formattedName()), i18n("Delete contact"));
   if (ret == KMessageBox::Yes) {
      m_pCurrentPerson->remove();
   }
}


///Select a number
ContactMethod* Menu::Person::showNumberSelector(bool& ok)
{
   if (m_pCurrentPerson && m_pCurrentPerson->phoneNumbers().size() > 1) {
      ContactMethod* number = KPhoneNumberSelector().number(m_pCurrentPerson);
      if (number->uri().isEmpty()) {
         qDebug() << "Operation cancelled";
      }
      return number;
   }
   else if (m_pCurrentPerson&& m_pCurrentPerson->phoneNumbers().size() == 1) {
      ok = true;
      return m_pCurrentPerson->phoneNumbers()[0];
   }
   else {
      ok = false;
      return nullptr;
   }

   return nullptr;
}