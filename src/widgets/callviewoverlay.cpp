/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include <QTimer>
#include <QEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

//KDE
#include <QPushButton>
#include <klocalizedstring.h>
#include <QStandardPaths>


//Ring 
#include "../accessibility.h"
#include "klib/kcfg_settings.h"
#include "callmodel.h"
#include "call.h"
#include "phonedirectorymodel.h"

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

   m_pMainWidget = new QWidget(this);
   if (parent)
      m_pMainWidget->resize(parent->width(),parent->height());
   setupUi(m_pMainWidget);

   connect(&CallModel::instance(),SIGNAL(layoutChanged()),this,SLOT(slotLayoutChanged()));
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
      Accessibility::instance()->say(m_accessMessage);
   }
} //setVisible

///How to paint the overlay
void CallViewOverlay::paintEvent(QPaintEvent* event) {
   Q_UNUSED(event)
   QPainter customPainter(this);
   customPainter.fillRect(rect(),m_black);
   customPainter.drawPixmap(rect().width()-125-10,rect().height()-50-10,QPixmap(":/gui/icons/transferarrow.svg"));
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
   if (CallModel::instance().getActiveCalls().size()-1 > 0) {
      foreach(Call* call, CallModel::instance().getActiveCalls()) {
         if (call != m_pCurrentCall) {
            QPushButton* btn = new QPushButton(call->roleData(Qt::DisplayRole).toString(),this);
            btn->setStyleSheet(m_pTransferB->styleSheet());
            m_pAttTransferGB->layout()->addWidget(btn);
            btn->setProperty("callId",call->historyId());
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
   const QByteArray callId = QObject::sender()->property("callId").toByteArray();
   if (!callId.isEmpty()) {
      CallModel::instance().attendedTransfer(m_pCurrentCall,CallModel::instance().fromMime(callId));
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
   CallModel::instance().transfer(m_pCurrentCall,PhoneDirectoryModel::instance().getNumber(m_pNumberLE->text()));
   setVisible(false);
}
