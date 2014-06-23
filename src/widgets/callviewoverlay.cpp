/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
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
#include "callviewoverlay.h"

//Qt
#include <QtCore/QTimer>
#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <QtGui/QGraphicsBlurEffect>
#include <QtGui/QGraphicsOpacityEffect>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

//KDE
#include <KPushButton>
#include <KLineEdit>
#include <KLocale>
#include <KStandardDirs>

//SFLPhone 
#include "../sflphoneaccessibility.h"
#include "klib/kcfg_settings.h"
#include "lib/callmodel.h"
#include "lib/call.h"
#include "lib/phonedirectorymodel.h"

/*****************************************************************************
 *                                                                           *
 *                                 Overlay                                   *
 *                                                                           *
 ****************************************************************************/

///Constructor
CallViewOverlay::CallViewOverlay(QWidget* parent) : QWidget(parent),m_pIcon(0),m_pTimer(0),m_enabled(true),m_black("black"),
m_step(0),m_pCurrentCall(nullptr)
{
   m_black.setAlpha(200);
   if (parent)
      parent->installEventFilter(this);
   if (parent) {
      resize(parent->width(),parent->height());
      move(0,0);
   }

//    QGraphicsOpacityEffect* eff2 = new QGraphicsOpacityEffect(m_pBackground);
//    eff2->setOpacity(0.7);
//    m_pBackground->setGraphicsEffect(eff2);

   m_pMainWidget = new QWidget(this);
   if (parent)
      m_pMainWidget->resize(parent->width(),parent->height());
   setupUi(m_pMainWidget);

   connect(CallModel::instance(),SIGNAL(layoutChanged()),this,SLOT(slotLayoutChanged()));
   connect(m_pNumberLE,SIGNAL(returnPressed()),this,SLOT(slotTransferClicked()));
   connect(m_pTransferB,SIGNAL(clicked()),this,SLOT(slotTransferClicked()));
}

///Destructor
CallViewOverlay::~CallViewOverlay()
{

}

///Add a widget (usually an icon) in the corner
void CallViewOverlay::setCornerWidget(QWidget* wdg) {
   wdg->setParent      ( this                        );
   wdg->setMinimumSize ( 100         , 100           );
   wdg->resize         ( 100         , 100           );
   wdg->move           ( width()/2-50 , height()-175 );
   m_pIcon = wdg;
}

///Overload the setVisible method to trigger the timer
void CallViewOverlay::setVisible(bool enabled) {
   if (m_enabled != enabled || m_step == 0) {
      if (m_pTimer) {
         m_pTimer->stop();
         disconnect(m_pTimer);
         delete m_pTimer;
         m_pTimer = nullptr;
      }
      m_pTimer = new QTimer(this);
      connect(m_pTimer, SIGNAL(timeout()), this, SLOT(changeVisibility()));
      m_step = 0;
      m_black.setAlpha(0);
      repaint();
      m_pTimer->start(10);
   }
   m_enabled = enabled;
   QWidget::setVisible(enabled);
   if (enabled) {
      slotLayoutChanged();
   }
   else {
      if (m_pCurrentCall && m_pCurrentCall->state() == Call::State::TRANSFERRED) {
         m_pCurrentCall->performAction(Call::Action::TRANSFER);
      }
      m_pCurrentCall = nullptr;
   }
   if (!m_accessMessage.isEmpty() && enabled == true && ConfigurationSkeleton::enableReadLabel()) {
      SFLPhoneAccessibility::instance()->say(m_accessMessage);
   }
} //setVisible

///How to paint the overlay
void CallViewOverlay::paintEvent(QPaintEvent* event) {
   Q_UNUSED(event)
   QPainter customPainter(this);
   customPainter.fillRect(rect(),m_black);
   customPainter.drawPixmap(rect().width()-125-10,rect().height()-50-10,QPixmap(KStandardDirs::locate("data" , "sflphone-client-kde/transferarrow.png")));
}

///Be sure the event is always the right size
void CallViewOverlay::resizeEvent(QResizeEvent *e) {
   Q_UNUSED(e)
   if (m_pIcon) {
      m_pIcon->setMinimumSize(100,100);
      m_pIcon->move(width()/2-50,height()-175);
   }
}

///Step by step animation to fade in/out
void CallViewOverlay::changeVisibility() {
   m_step++;
   m_black.setAlpha(0.16*m_step*m_step);
   repaint();
   if (m_step >= 35)
      m_pTimer->stop();
}

///Set accessibility message
void CallViewOverlay::setAccessMessage(const QString &message)
{
   m_accessMessage = message;
}

bool CallViewOverlay::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize && parentWidget()) {
      resize(parentWidget()->width(),parentWidget()->height());
      m_pMainWidget->resize(parentWidget()->width(),parentWidget()->height());
      move(0,0);
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

void CallViewOverlay::slotLayoutChanged()
{
   //Clean the list
   while (m_pAttTransferGB->layout()->count()) {
      QWidget* w = m_pAttTransferGB->layout()->itemAt(0)->widget();
      if (w) {
         m_pAttTransferGB->layout()->removeWidget(w);
         delete w;
      }
   }
   //Fill the list
   if (CallModel::instance()->getCallList().size()-1 > 0) {
      foreach(Call* call, CallModel::instance()->getCallList()) {
         if (call != m_pCurrentCall) {
            KPushButton* btn = new KPushButton(call->roleData(Qt::DisplayRole).toString(),this);
            btn->setStyleSheet(m_pTransferB->styleSheet());
            m_pAttTransferGB->layout()->addWidget(btn);
            btn->setProperty("callId",call->id());
            connect(btn,SIGNAL(clicked()),this,SLOT(slotAttendedTransfer()));
         }
      }
      m_pAttTransferGB->setVisible(true);
   }
   else {
      m_pAttTransferGB->setVisible(false);
   }
}

void CallViewOverlay::slotAttendedTransfer()
{
   const QString callId = QObject::sender()->property("callId").toString();
   if (!callId.isEmpty()) {
      CallModel::instance()->attendedTransfer(m_pCurrentCall,CallModel::instance()->getCall(callId));
      setVisible(false);
   }
}

void CallViewOverlay::setCurrentCall( Call* call )
{
   m_pCurrentCall = call;
   if (call)
      m_pTitleL->setText("<center><h1>"+m_pCurrentCall->roleData(Qt::DisplayRole).toString().replace('<',QString()).replace('>',QString())+"</h1>"+m_pCurrentCall->roleData(Call::Role::Number).toString().replace('<',QString()).replace('>',QString())+"<br></center>");
}

Call* CallViewOverlay::currentCall()
{
   return m_pCurrentCall;
}

void CallViewOverlay::slotTransferClicked()
{
   if (!m_pCurrentCall) return;
   CallModel::instance()->transfer(m_pCurrentCall,PhoneDirectoryModel::instance()->getNumber(m_pNumberLE->text()));
   setVisible(false);
}
