/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
 *   Copyright (C) 2016 by Marat Moustafine                                *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
 *   Author : Marat Moustafine <moustafine@tuta.io>                        *
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
#include "wizard.h"

//std
#include <stdlib.h>

//Qt
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QClipboard>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QtCore/QProcess>
#include <QtCore/QPointer>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QValidator>

//KDE
#include <KUser>
#include <KColorScheme>

//Ring
#include <accountmodel.h>

#include "ui_welcome.h"
#include "ui_accountcreator.h"
#include "ui_share.h"
#include "actioncollection.h"

Wizard::Wizard(QWidget* parent) : QWidget(parent)
{
   if (parent)
      parent->installEventFilter(this);
   if (parent) {
      resize(parent->width(),parent->height());
      move(0,0);
   }

   m_pLayout = new QHBoxLayout(this);

   m_pCurrentPage = new QWidget(this);
   Ui::Welcome* w = new Ui::Welcome();
   w->setupUi(m_pCurrentPage);
   m_pLayout->addWidget(m_pCurrentPage);

   m_pDisplayNameEdit = w->m_pName;
   KUser user;
   w->m_pName->setText(user.property(KUser::FullName).toString());
   w->m_pName->selectAll();
   connect(w->m_pNext, &QPushButton::clicked,
           this, &Wizard::showAccountCreatorWidget);
   connect(w->m_pName, &QLineEdit::returnPressed,
           this, &Wizard::showAccountCreatorWidget);
   setStyleSheet("QLineEdit { background: transparent; }");
}

Wizard::~Wizard()
{
}

void Wizard::showAccountCreatorWidget()
{
   m_pCurrentPage->setVisible(false);

   auto accountCreatorWidget = new Ui::AccountCreatorWidget();
   m_pCurrentPage = new QWidget(this);
   accountCreatorWidget->setupUi(m_pCurrentPage);
   m_pLayout->addWidget(m_pCurrentPage);

   accountCreatorWidget->titleLabel
       ->setPixmap(QIcon::fromTheme(QStringLiteral("preferences-desktop-user"))
                   .pixmap(QSize(128,128)));

   QRegularExpression usernameRegExp("[A-Za-z0-9]+(-?[A-Za-z0-9]+)*");
   QValidator * usernameValidator
       = new QRegularExpressionValidator(usernameRegExp, this);
   accountCreatorWidget->usernameEdit->setValidator(usernameValidator);
   accountCreatorWidget->usernameEdit->setText(qgetenv("USER"));
   accountCreatorWidget->usernameEdit->selectAll();

   accountCreatorWidget->searchingStateLabel->clear();

   accountCreatorWidget->createAccountButton->setEnabled(false);

   accountCreatorWidget->progressBar->hide();

   accountCreatorWidget->signUpCheckBox->hide();

   connect(accountCreatorWidget->usernameEdit, &QLineEdit::textEdited,
           this, &Wizard::validateAccountForm);
   connect(accountCreatorWidget->passwordEdit, &QLineEdit::textEdited,
           this, &Wizard::validateAccountForm);
   connect(accountCreatorWidget->confirmPasswordEdit, &QLineEdit::textEdited,
           this, &Wizard::validateAccountForm);

   connect(accountCreatorWidget->createAccountButton, &QPushButton::clicked,
           this, &Wizard::createAccount);

   return;
}

void Wizard::validateAccountForm(const QString & text)
{
   Q_UNUSED(text);

   m_pCurrentPage->setStyleSheet("");

   auto usernameEdit
       = m_pCurrentPage->findChild<QLineEdit *>("usernameEdit",
                                               Qt::FindDirectChildrenOnly);
   bool validUsername = true;

   KColorScheme colorScheme(QPalette::Active, KColorScheme::View);
   auto warningColor
       = colorScheme.background(KColorScheme::NegativeBackground).color();
   warningColor.setAlpha(191);
   auto warningColorName = warningColor.name(QColor::HexArgb);

   if (usernameEdit->text().isEmpty()) {
      validUsername = false;
      m_pCurrentPage->setStyleSheet("QLineEdit#usernameEdit {"
                                   "background:" + warningColorName
                                   + "}");
   }

   auto passwordEdit
      = m_pCurrentPage->findChild<QLineEdit *>("passwordEdit",
                                               Qt::FindDirectChildrenOnly);
   bool validPassword = true;
   if (passwordEdit->text().isEmpty()) {
      validPassword = false;
      m_pCurrentPage->setStyleSheet(m_pCurrentPage->styleSheet()
                                    + "QLineEdit#passwordEdit {"
                                    "background:" + warningColorName
                                       + "}");
   }

   auto confirmPasswordEdit
         = m_pCurrentPage->findChild<QLineEdit *>("confirmPasswordEdit",
                                               Qt::FindDirectChildrenOnly);
   bool validConfirmPassword = true;
   if (!validPassword
         || passwordEdit->text() != confirmPasswordEdit->text()) {
      validConfirmPassword = false;
      m_pCurrentPage->setStyleSheet(m_pCurrentPage->styleSheet()
                                    + "QLineEdit#confirmPasswordEdit {"
                                       "background:" + warningColorName
                                    + "}");
   }

   auto createAccountButton
         = m_pCurrentPage->findChild<QPushButton *>("createAccountButton",
                                                 Qt::FindDirectChildrenOnly);

   if (!validUsername || !validPassword || !validConfirmPassword) {
      createAccountButton->setEnabled(false);
      return;
   }

   createAccountButton->setEnabled(true);

   return;
}

void Wizard::createAccount()
{
   auto progressBar
       = m_pCurrentPage->findChild<QProgressBar *>("progressBar",
                                               Qt::FindDirectChildrenOnly);
   progressBar->show(); // FIXME: Busy animation

   auto usernameEdit
       = m_pCurrentPage->findChild<QLineEdit *>("usernameEdit",
                                                Qt::FindDirectChildrenOnly);
   usernameEdit->setEnabled(false);

   auto passwordEdit
       = m_pCurrentPage->findChild<QLineEdit *>("passwordEdit",
                                                Qt::FindDirectChildrenOnly);
   passwordEdit->setEnabled(false);

   auto confirmPasswordEdit
       = m_pCurrentPage->findChild<QLineEdit *>("confirmPasswordEdit",
                                                Qt::FindDirectChildrenOnly);
   confirmPasswordEdit->setEnabled(false);

   auto createAccountButton
       = m_pCurrentPage->findChild<QPushButton *>("createAccountButton",
                                                Qt::FindDirectChildrenOnly);
   createAccountButton->setEnabled(false);

   m_pAccount = AccountModel::instance().add(usernameEdit->text(),
                                             Account::Protocol::RING);
   if (!m_pDisplayNameEdit->text().isEmpty()) {
      m_pAccount->setDisplayName(m_pDisplayNameEdit->text());
   }
   m_pAccount->setArchivePassword(passwordEdit->text());
   m_pAccount->setUpnpEnabled(true);

   connect(m_pAccount, &Account::stateChanged,
           this, &Wizard::showShareWidget);

   m_pAccount->performAction(Account::EditAction::SAVE);

   return;
}

void Wizard::showShareWidget(Account::RegistrationState state)
{
   switch (state) {
   case Account::RegistrationState::READY:
      m_pAccount->performAction(Account::EditAction::RELOAD);
      break;
      case Account::RegistrationState::UNREGISTERED:
   case Account::RegistrationState::INITIALIZING:
   case Account::RegistrationState::TRYING:
   case Account::RegistrationState::ERROR:
   case Account::RegistrationState::COUNT__:
      return;
   }

   Ui::Share* w = new Ui::Share();
   m_pCurrentPage->setVisible(false);
   m_pCurrentPage = new QWidget(this);
   w->setupUi(m_pCurrentPage);
   m_pLayout->addWidget(m_pCurrentPage);
   w->label->setPixmap(QIcon::fromTheme
                       (QStringLiteral("preferences-system-network-sharing"))
                       .pixmap(QSize(128,128)));
   w->m_pHash->setText(m_pAccount->username());
   w->m_pHash->selectAll();

   connect(w->m_pCopy , &QPushButton::clicked,this,&Wizard::slotCopy     );
   connect(w->m_pEmail, &QPushButton::clicked,this,&Wizard::slotEmail    );
   connect(w->m_pPrint, &QPushButton::clicked,this,&Wizard::slotPrint    );
   connect(w->m_pConf , &QPushButton::clicked,this,&Wizard::slotConfigure);
   connect(w->m_pOk   , &QPushButton::clicked,this,&Wizard::slotComplete );

   return;
}

void Wizard::slotEmail()
{
   qDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   myProcess->start(QStringLiteral("xdg-email"), {"--body",m_pAccount->username(),"--subject",m_pAccount->alias()+" Ring hash"});
}

void Wizard::slotPrint()
{
   QPrinter printer;

   QPointer<QPrintDialog> dialog = new QPrintDialog(&printer);
   if (!dialog)
      return;

   dialog->setWindowTitle(QStringLiteral("Print Hash"));

   //This cause a crash on my system, look like a bug in Qt
   if (dialog->exec() != QDialog::Accepted)
      return;

   QPainter painter;
   painter.begin(&printer);

   painter.drawText(100, 100, 500, 500, Qt::AlignLeft|Qt::AlignTop, m_pAccount->username());

   painter.end();
}

void Wizard::slotCopy()
{
   QApplication::clipboard()->setText(m_pAccount->username());
}

void Wizard::slotConfigure()
{
   ActionCollection::instance()->configureRing();
   slotComplete();
}

void Wizard::slotComplete()
{
  setVisible(false);
}

bool Wizard::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize && parentWidget()) {
      resize(parentWidget()->width(),parentWidget()->height());
//       m_pMainWidget->resize(parentWidget()->width(),parentWidget()->height());
      move(0,0);
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

///How to paint the overlay
void Wizard::paintEvent(QPaintEvent* event) {
   Q_UNUSED(event)

   QPainter painter(this);
   KColorScheme colorScheme(QPalette::Active, KColorScheme::View);
   auto backgroundColor
      = colorScheme.background(KColorScheme::ActiveBackground).color();
   backgroundColor.setAlpha(191);
   painter.fillRect(rect(), backgroundColor);

   return;
}
