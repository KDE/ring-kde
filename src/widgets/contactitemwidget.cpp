/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Mathieu Leduc-Hamel mathieu.leduc-hamel@savoirfairelinux.com *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "contactitemwidget.h"

//Qt
#include <QtCore/QMimeData>
#include <QtCore/QProcess>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QGridLayout>
#include <QtGui/QMenu>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>

//KDE
#include <KIcon>
#include <KLocale>
#include <KDebug>
#include <KAction>
#include <KStandardDirs>
#include <KInputDialog>

//System
#include <unistd.h>

//SFLPhone
#include "klib/akonadibackend.h"
#include "widgets/bookmarkdock.h"
#include "klib/configurationskeleton.h"
#include "klib/helperfunctions.h"
#include "widgets/translucentbuttons.h"
#include "sflphone.h"

//SFLPhone library
#include "lib/contact.h"
#include "lib/sflphone_const.h"

///Constructor
ContactItemWidget::ContactItemWidget(QWidget *parent)
   : QWidget(parent),m_pOrganizationL(0),m_pEmailL(0),m_pContactKA(0), m_pIconL(0), m_pContactNameL(0),
   m_pCallNumberL(0)
{
   setContextMenuPolicy(Qt::CustomContextMenu);
   setAcceptDrops(true);

   //Overlay
   m_pBtnTrans = new TranslucentButtons(this);
   m_pBtnTrans->setText   ( i18n("Transfer"));
   m_pBtnTrans->setVisible( false);
   m_pBtnTrans->setPixmap ( new QImage(KStandardDirs::locate("data","sflphone-client-kde/transferarraw.png")));
   connect(m_pBtnTrans,SIGNAL(dataDropped(QMimeData*)),this,SLOT(transferEvent(QMimeData*)));
} //ContactItemWidget

///Destructor
ContactItemWidget::~ContactItemWidget()
{
   if (m_pIconL)         delete m_pIconL        ;
   if (m_pContactNameL)  delete m_pContactNameL ;
   if (m_pCallNumberL)   delete m_pCallNumberL  ;
   if (m_pOrganizationL) delete m_pOrganizationL;
   if (m_pEmailL)        delete m_pEmailL       ;
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Set the contact
void ContactItemWidget::setContact(Contact* contact)
{
   m_pContactKA     = contact;
   m_pIconL         = new QLabel ( this );
   m_pContactNameL  = new QLabel (      );
   m_pCallNumberL   = new QLabel ( this );

   m_pIconL->setMinimumSize(70,48);
   m_pIconL->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

   QSpacerItem* verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

   m_pIconL->setMaximumSize(48,9999);
   m_pIconL->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

   m_pIconL->setPixmap(QPixmap(KIcon("user-identity").pixmap(QSize(48,48))));

   QGridLayout* mainLayout = new QGridLayout(this);
   mainLayout->setContentsMargins(0,0,0,0);
   mainLayout->addWidget( m_pIconL        , 0 , 0 , 4 , 1 );
   mainLayout->addWidget( m_pContactNameL , 0 , 1         );

   uint row = 1;

   if (ConfigurationSkeleton::displayOrganisation() && !contact->getOrganization().isEmpty()) {
      m_pOrganizationL = new QLabel ( this );
      mainLayout->addWidget( m_pOrganizationL, row , 1);
      row++;
   }
   mainLayout->addWidget( m_pCallNumberL  , row , 1       );
   row++;

   if (ConfigurationSkeleton::displayEmail() && !contact->getPreferredEmail().isEmpty()) {
      m_pEmailL        = new QLabel (      );
      mainLayout->addWidget( m_pEmailL       , row , 1    );
      row++;
   }
   
   mainLayout->addItem(verticalSpacer     , row , 1       );

   setLayout(mainLayout);
   setMinimumSize(QSize(50, 30));

   updated();
   connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContext(QPoint)));

   uint height =0;
   if ( m_pContactNameL  ) {
      QFontMetrics fm(m_pContactNameL->font());
      height += fm.height();
   }
   if ( m_pCallNumberL   ) {
      QFontMetrics fm(m_pCallNumberL->font());
      height += fm.height();
   }
   if ( m_pOrganizationL ) {
      QFontMetrics fm(m_pOrganizationL->font());
      height += fm.height();
   }
   if ( m_pEmailL        ) {
      QFontMetrics fm(m_pEmailL->font());
      height += fm.height();
   }

   if (height < 48)
      height = 48;
   m_Size = QSize(0,height+8);


} //setContact

///Set the model index
void ContactItemWidget::setItem(QTreeWidgetItem* item)
{
   m_pItem = item;
}


/*****************************************************************************
 *                                                                           *
 *                                    Slots                                  *
 *                                                                           *
 ****************************************************************************/

///The contact need to be updated
void ContactItemWidget::updated()
{
   m_pContactNameL->setText("<b>"+m_pContactKA->getFormattedName()+"</b>");
   if (m_pOrganizationL && !m_pContactKA->getOrganization().isEmpty()) {
      m_pOrganizationL->setText(m_pContactKA->getOrganization());
   }
   else if (m_pOrganizationL) {
      m_pOrganizationL->setVisible(false);
   }

   if (m_pEmailL && !getEmail().isEmpty()) {
      m_pEmailL->setText(getEmail());
   }
   else if (m_pEmailL) {
      m_pEmailL->setVisible(false);
   }

   Contact::PhoneNumbers numbers = m_pContactKA->getPhoneNumbers();

   if (getCallNumbers().count() == 1)
      m_pCallNumberL->setText(getCallNumbers()[0]->getNumber());
   else
      m_pCallNumberL->setText(i18np("%1 number","%1 numbers",getCallNumbers().count()));

   if (!m_pContactKA->getPhoto()) {

      m_pIconL->setPixmap(QPixmap(KIcon("user-identity").pixmap(QSize(48,48))));
   }
   else {
      QPixmap pxm =*m_pContactKA->getPhoto();
      QRect pxRect = pxm.rect();
      QBitmap mask(pxRect.size());
      QPainter customPainter(&mask);
      customPainter.setRenderHint  (QPainter::Antialiasing, true   );
      customPainter.fillRect       (pxRect                ,"white" );
      customPainter.setBackground  (QColor("black")                );
      customPainter.setBrush       (QColor("black")                );
      customPainter.drawRoundedRect(pxRect,5,5);
      pxm.setMask(mask);
      m_pIconL->setPixmap(pxm);
   }
} //updated


/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Return contact name
const QString& ContactItemWidget::getContactName() const
{
   return m_pContactKA->getFormattedName();
}

///Return call number
Contact::PhoneNumbers ContactItemWidget::getCallNumbers() const
{
   return m_pContactKA->getPhoneNumbers();
}

///Return the organisation
const QString& ContactItemWidget::getOrganization() const
{
   return m_pContactKA->getOrganization();
}

///Return the email address
const QString& ContactItemWidget::getEmail() const
{
   return m_pContactKA->getPreferredEmail();
}

///Return the picture
QPixmap* ContactItemWidget::getPicture() const
{
   return (QPixmap*) m_pContactKA->getPhoto();
}

///Return the model index
QTreeWidgetItem* ContactItemWidget::getItem() const
{
   return m_pItem;
}

///Return the contact object
Contact* ContactItemWidget::getContact() const
{
   return m_pContactKA;
}



///Return precalculated size hint, prevent it from being computed over and over
QSize ContactItemWidget::sizeHint () const
{
   return m_Size;
}

/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/



/*****************************************************************************
 *                                                                           *
 *                                 Drag&Dop                                  *
 *                                                                           *
 ****************************************************************************/

///Called when a drag and drop occur while the item have not been dropped yet
void ContactItemWidget::dragEnterEvent ( QDragEnterEvent *e )
{
   kDebug() << "Drag enter";
   if (e->mimeData()->hasFormat( MIME_CALLID) && m_pBtnTrans) {
      m_pBtnTrans->setHoverState(true);
      m_pBtnTrans->setMinimumSize(width()-16,height()-4);
      m_pBtnTrans->setMaximumSize(width()-16,height()-4);
      m_pBtnTrans->move(8,2);
      m_pBtnTrans->setVisible(true);
      m_pBtnTrans->setHoverState(true);
      e->accept();
   }
   else
      e->ignore();
} //dragEnterEvent

///The cursor move on a potential drag event
void ContactItemWidget::dragMoveEvent  ( QDragMoveEvent  *e )
{
   m_pBtnTrans->setHoverState(true);
   e->accept();
}

///A potential drag event is cancelled
void ContactItemWidget::dragLeaveEvent ( QDragLeaveEvent *e )
{
   m_pBtnTrans->setHoverState(false);
   m_pBtnTrans->setVisible(false);
   kDebug() << "Drag leave";
   e->accept();
}

///On data drop
void ContactItemWidget::dropEvent(QDropEvent *e)
{
   kDebug() << "Drop accepted";
   if (dynamic_cast<const QMimeData*>(e->mimeData()) && e->mimeData()->hasFormat( MIME_CALLID)) {
//       transferEvent((QMimeData*)e->mimeData());
      e->accept();
   }
   else {
      kDebug() << "Invalid drop data";
      e->ignore();
   }
}

///On double click
void ContactItemWidget::mouseDoubleClickEvent(QMouseEvent *e )
{
   Contact::PhoneNumbers numbers = m_pContactKA->getPhoneNumbers();

   if (getCallNumbers().count() == 1) {
      e->accept();
//       callAgain();
   }
   else {
      e->ignore();
   }
}
