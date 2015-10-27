/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
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
#include "wizard.h"

//std
#include <stdlib.h>

//Qt
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QClipboard>
#include <QPrinter>
#include <QPrintDialog>
#include <QProcess>

//Ring
#include <accountmodel.h>
#include <account.h>
#include "ui_welcome.h"
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

   const QString user = getenv("USER");
   m_pName = w->m_pName;
   w->m_pName->setText(user);
   w->m_pName->selectAll();
   connect(w->m_pNext, &QPushButton::clicked,this,&Wizard::slotNext);
   connect(w->m_pName, &QLineEdit::returnPressed,this,&Wizard::slotNext);
   setStyleSheet("QLabel{color:white;} QLineEdit{color:white;background:transparent;}");
}

Wizard::~Wizard()
{

}

void Wizard::slotNext()
{
   m_pAccount = AccountModel::instance().add(m_pName->text(),Account::Protocol::RING);
   m_pAccount << Account::EditAction::SAVE;
   m_pAccount << Account::EditAction::RELOAD;

   Ui::Share* w = new Ui::Share();
   m_pCurrentPage->setVisible(false);
   m_pCurrentPage = new QWidget(this);
   w->setupUi(m_pCurrentPage);
   m_pLayout->addWidget(m_pCurrentPage);
   w->label->setPixmap(QIcon::fromTheme("document-share").pixmap(QSize(128,128)));
   w->m_pHash->setText(m_pAccount->username());
   w->m_pHash->selectAll();

   connect(w->m_pCopy , &QPushButton::clicked,this,&Wizard::slotCopy     );
   connect(w->m_pEmail, &QPushButton::clicked,this,&Wizard::slotEmail    );
   connect(w->m_pPrint, &QPushButton::clicked,this,&Wizard::slotPrint    );
   connect(w->m_pConf , &QPushButton::clicked,this,&Wizard::slotConfigure);
   connect(w->m_pOk   , &QPushButton::clicked,this,&Wizard::slotComplete );

}

void Wizard::slotEmail()
{
   qDebug() << "Sending email";
   QProcess *myProcess = new QProcess(this);
   myProcess->start("xdg-email", {"--body",m_pAccount->username(),"--subject",m_pAccount->alias()+" Ring hash"});
}

void Wizard::slotPrint()
{
   QPrinter printer;

   QPrintDialog *dialog = new QPrintDialog(&printer);
   if (!dialog)
      return;

   dialog->setWindowTitle("Print Hash");

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
   QPainter customPainter(this);
   static QColor black(0,0,0,200);
   customPainter.fillRect(rect(),black);
}
