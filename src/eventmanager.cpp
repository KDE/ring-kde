/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
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
#include "eventmanager.h"

//Qt
#include <QtGui/QKeyEvent>
#include <QtGui/QDropEvent>

//KDE
#include <KDebug>

//SFLPhone
#include <lib/phonenumber.h>
#include <lib/account.h>
#include <lib/phonedirectorymodel.h>
#include <lib/accountlistmodel.h>
#include <lib/contactmodel.h>
#include <klib/tipmanager.h>
#include "sflphoneview.h"
#include "sflphone.h"
#include "canvasobjectmanager.h"
#include "widgets/kphonenumberselector.h"
#include "widgets/tips/tipcollection.h"
#include "widgets/callviewoverlay.h"
#include "widgets/autocompletion.h"

//This code detect if the window is active, innactive or minimzed
class MainWindowEvent : public QObject {
   Q_OBJECT
public:
   MainWindowEvent(EventManager* ev) : QObject(ev),m_pParent(ev) {
      SFLPhone::app()->installEventFilter(this);
   }
protected:
   virtual bool eventFilter(QObject *obj, QEvent *event) {
      Q_UNUSED(obj)
      if (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut) {
         QFocusEvent* e = static_cast<QFocusEvent*>(event);
         if (e->reason() == Qt::ActiveWindowFocusReason) {
            qDebug() << "ACTIVE WINDOW EVENT";
         }
      }
      else if (event->type() == QEvent::WindowStateChange) {
         QWindowStateChangeEvent* e = static_cast<QWindowStateChangeEvent*>(event);
         switch (SFLPhone::app()->windowState()) {
            case Qt::WindowMinimized:
               emit minimized(true);
               break;
            case Qt::WindowActive:
               qDebug() << "The window is now active";
            case Qt::WindowNoState:
            default:
               if (e->oldState() == Qt::WindowMinimized)
                  emit minimized(false);
               break;
         };
      }
      else if (event->type() == QEvent::KeyPress) {
         m_pParent->viewKeyEvent(static_cast<QKeyEvent*>(event));
      }
      return false;
   }

private:
   EventManager* m_pParent;

Q_SIGNALS:
   void minimized(bool);
};

///Constructor
EventManager::EventManager(SFLPhoneView* parent): QObject(parent),m_pParent(parent),m_pMainWindowEv(new MainWindowEvent(this))
{
   connect(CallModel::instance()    , SIGNAL(callStateChanged(Call*,Call::State)) , this, SLOT(slotCallStateChanged(Call*,Call::State)) );
   connect(CallModel::instance()    , SIGNAL(incomingCall(Call*))                 , this, SLOT(slotIncomingCall(Call*)) );

   connect(m_pMainWindowEv , SIGNAL(minimized(bool)) , m_pParent->m_pCanvasManager, SLOT(slotMinimized(bool)));

   connect(AccountListModel::instance(),SIGNAL(accountRegistrationChanged(Account*,bool)),this,SLOT(slotAccountRegistrationChanged(Account*,bool)));
   connect(AccountListModel::instance(),SIGNAL(badGateway()),this,SLOT(slotNetworkDown()));
   //Listen for macro
   MacroModel::addListener(this);
}

///Destructor
EventManager::~EventManager()
{
   delete m_pMainWindowEv;
}

/*****************************************************************************
 *                                                                           *
 *                              Events filter                                *
 *                                                                           *
 ****************************************************************************/

///Callback when a grag event enter the canvas
bool EventManager::viewDragEnterEvent(const QDragEnterEvent* e)
{
   Q_UNUSED(e)
   if (!e->mimeData()->hasFormat(MIME_CALLID) || CallModel::instance()->hasConference())
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DRAG_ENTER);
   return false;
}

///Callback when something is dropped on the call canvas
bool EventManager::viewDropEvent(QDropEvent* e)
{
   const QModelIndex& idxAt = m_pParent->m_pView->indexAt(e->pos());
   m_pParent->m_pView->cancelHoverState();
   CallModel::instance()->setData(idxAt,-1,Call::Role::DropState);
   e->accept();
   if (!idxAt.isValid()) { //Dropped on empty space
      if (e->mimeData()->hasFormat(MIME_CALLID)) {
         const QByteArray encodedCallId      = e->mimeData()->data( MIME_CALLID      );
         kDebug() << "Call dropped on empty space";
         Call* call =  CallModel::instance()->getCall(encodedCallId);
         if (CallModel::instance()->getIndex(call).parent().isValid()) {
            kDebug() << "Detaching participant";
            CallModel::instance()->detachParticipant(CallModel::instance()->getCall(encodedCallId));
         }
         else
            kDebug() << "The call is not in a conversation (doing nothing)";
      }
      else if (e->mimeData()->hasFormat(MIME_PHONENUMBER)) {
         const QByteArray encodedPhoneNumber = e->mimeData()->data( MIME_PHONENUMBER );
         kDebug() << "Phone number dropped on empty space";
         Call* newCall = CallModel::instance()->dialingCall();
         PhoneNumber* nb = PhoneDirectoryModel::instance()->fromHash(encodedPhoneNumber);
         newCall->setDialNumber(nb);
         if (nb && nb->account())
            newCall->setAccount(nb->account());
         newCall->performAction(Call::Action::ACCEPT);
      }
      else if (e->mimeData()->hasFormat(MIME_CONTACT)) {
         const QByteArray encodedContact     = e->mimeData()->data( MIME_CONTACT     );
         kDebug() << "Contact dropped on empty space";
         const PhoneNumber* number = KPhoneNumberSelector().getNumber(ContactModel::instance()->getContactByUid(encodedContact));
         if (number->uri().isEmpty()) {
            Call* newCall = CallModel::instance()->dialingCall();
            newCall->setDialNumber(number->uri());
            newCall->performAction(Call::Action::ACCEPT);
         }
      }
      else if (e->mimeData()->hasFormat("text/plain")) {
         Call* newCall = CallModel::instance()->dialingCall();
         newCall->setDialNumber(e->mimeData()->data( "text/plain" ));
         newCall->performAction(Call::Action::ACCEPT);
      }
      //Remove unneeded tip
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DRAG_LEAVE);
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DROP);
      return true;
   }
   else {
      //1) Get the right action
      const QPoint position = e->pos();
      const QRect targetRect = m_pParent->m_pView->visualRect(idxAt);
      Call::DropAction act = (position.x() < targetRect.x()+targetRect.width()/2)?Call::DropAction::Conference:Call::DropAction::Transfer;
      QMimeData* data = (QMimeData*)e->mimeData(); //Drop the const
      data->setProperty("dropAction",act);

      //2) Send to the model for processing
      m_pParent->m_pView->model()->dropMimeData(data,Qt::MoveAction,idxAt.row(),idxAt.column(),idxAt.parent());
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DRAG_LEAVE);
   }

   //Remove item overlays
   for (int i = 0;i < m_pParent->m_pView->model()->rowCount();i++) {
      const QModelIndex& idx = m_pParent->m_pView->model()->index(i,0);
      m_pParent->m_pView->model()->setData(idx,-1,Call::Role::DropState);
      for (int j = 0;j < m_pParent->m_pView->model()->rowCount(idx);j++) {
         m_pParent->m_pView->model()->setData(m_pParent->m_pView->model()->index(j,0,idx),-1,Call::Role::DropState);
      }
   }
   return false;
}

///Callback when a drag and drop is in progress
bool EventManager::viewDragMoveEvent(const QDragMoveEvent* e)
{
   const bool isCall = e->mimeData()->hasFormat(MIME_CALLID);
   if (TipCollection::removeConference() != TipCollection::manager()->currentTip() /*&& idxAt.parent().isValid()*/) {
      if (isCall) {
         TipCollection::removeConference()->setText(i18n("Remove the call from the conference, the call will be put on hold"));
      }
      else if (e->mimeData()->hasFormat(MIME_PHONENUMBER)) {
         PhoneNumber* n = PhoneDirectoryModel::instance()->fromHash(e->mimeData()->data(MIME_PHONENUMBER));
         if (n)
            TipCollection::removeConference()->setText(i18n("Call %1 using %2",n->uri(),
               (n->account()?n->account():AccountListModel::instance()->currentAccount())->alias()));
      }
      else if (e->mimeData()->hasFormat(MIME_CONTACT)) {
         Contact* c = ContactModel::instance()->getContactByUid(e->mimeData()->data(MIME_CONTACT));
         if (c) {
            TipCollection::removeConference()->setText(i18n("Call %1",c->formattedName()));
         }
      }
      else if (e->mimeData()->hasFormat("text/plain")) {
         TipCollection::removeConference()->setText(i18n("Call %1",QString(e->mimeData()->data("text/plain"))));
      }
   }
   if (TipCollection::removeConference() == TipCollection::manager()->currentTip()) {
      if (isCall) {
         TipCollection::removeConference()->setText(i18n("Remove the call from the conference, the call will be put on hold"));
      }
      else if (e->mimeData()->hasFormat(MIME_PHONENUMBER)) {
         PhoneNumber* n = PhoneDirectoryModel::instance()->fromHash(e->mimeData()->data(MIME_PHONENUMBER));
         if (n)
            TipCollection::removeConference()->setText(i18n("Call %1 using %2",n->uri(),
               (n->account()?n->account():AccountListModel::instance()->currentAccount())->alias()));
      }
      else if (e->mimeData()->hasFormat(MIME_CONTACT)) {
         Contact* c = ContactModel::instance()->getContactByUid(e->mimeData()->data(MIME_CONTACT));
         if (c) {
            TipCollection::removeConference()->setText(i18n("Call %1",c->formattedName()));
         }
      }
      else if (e->mimeData()->hasFormat("text/plain")) {
         TipCollection::removeConference()->setText(i18n("Call %1",QString(e->mimeData()->data("text/plain"))));
      }
   }
   if (!isCall || CallModel::instance()->hasConference())
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DRAG_MOVE);
   //Just as drop, compute the position
   const QModelIndex& idxAt = m_pParent->m_pView->indexAt(e->pos());
   const QPoint position = e->pos();
   const QRect targetRect = m_pParent->m_pView->visualRect(idxAt);
   Call* source = nullptr;
   if (isCall)
      source = CallModel::instance()->getCall(e->mimeData()->data(MIME_CALLID));
   Call::DropAction act = (position.x() < targetRect.x()+targetRect.width()/2)?Call::DropAction::Conference:Call::DropAction::Transfer;
   CallModel::instance()->setData(idxAt,act,Call::Role::DropPosition);
   if ((!isCall) || CallModel::instance()->getIndex(source) != idxAt)
      m_pParent->m_pView->setHoverState(idxAt);
   else
      m_pParent->m_pView->cancelHoverState();
   return true;
}

///Callback when a drag is cancelled
bool EventManager::viewDragLeaveEvent(const QDragMoveEvent* e)
{
   Q_UNUSED(e)
   m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DRAG_LEAVE);
   return false;
}

///Dispatch call canvas events to the right function
bool EventManager::eventFilter(QObject *obj, QEvent *event)
{
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wswitch"
   switch (event->type()) {
      case QEvent::KeyPress: {
         const int key = static_cast<QKeyEvent*>(event)->key();
         if (key != Qt::Key_Left && key != Qt::Key_Right) {
            if (viewKeyEvent(static_cast<QKeyEvent*>(event))) return true;
         }
         } break;
      case QEvent::Drop:
         if (viewDropEvent(static_cast<QDropEvent*>(event))) return true;
         break;
      case QEvent::DragMove:
         if (viewDragMoveEvent(static_cast<QDragMoveEvent*>(event))) return true;
         break;
      case QEvent::DragLeave:
         m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DRAG_LEAVE);
         break;
   };
   #pragma GCC diagnostic pop
   return QObject::eventFilter(obj, event);
} //eventFilter

///Callback when a key is pressed on the view
bool EventManager::viewKeyEvent(QKeyEvent* event)
{
   switch(event->key()) {
      case Qt::Key_Escape:
         escape();
         m_pParent->setFocus();
         break;
      case Qt::Key_Return:
      case Qt::Key_Enter:
         if (m_pParent->m_pAutoCompletion && m_pParent->m_pAutoCompletion->selection()) {
            PhoneNumber* n = m_pParent->m_pAutoCompletion->selection();
            Call* call = m_pParent->currentCall();
            if (call->state() == Call::State::DIALING) {
               call->setDialNumber(n->uri());
               if (PhoneDirectoryModel::instance()->callWithAccount()
                && n->account() && n->account()->id() != Account::ProtocolName::IP2IP)
                  call->setAccount(n->account());
               m_pParent->m_pAutoCompletion->reset();
               call->performAction(Call::Action::ACCEPT);
            }
         }
         else
            enter();
         break;
      case Qt::Key_Backspace:
         backspace();
         break;
      case Qt::Key_Up:
         if (m_pParent->m_pAutoCompletion && m_pParent->m_pAutoCompletion->isVisible()) {
            m_pParent->m_pAutoCompletion->moveUp();
         }
         else
            return false;
         break;
      case Qt::Key_Down:
         if (m_pParent->m_pAutoCompletion && m_pParent->m_pAutoCompletion->isVisible()) {
            m_pParent->m_pAutoCompletion->moveDown();
         }
         else
            return false;
         break;
      default: {
         const QString& text = event->text();
         if(! text.isEmpty()) {
            typeString(text);
         }
      }
      break;
   };
   return true;
}

///Called on keyboard
void EventManager::typeString(const QString& str)
{
   /* There is 5 cases
    * 1) There is no call, then create one
    * 2) There is one or more call and the active call is also selected, then send DTMF to the active call
    * 3) There is multiple call, but the active one is not selected, then create a new call or add to existing dialing call
    * 4) There is only inactive calls, then create a new one or add to existing dialing call
    * 5) There is only FAILURE, BUSY or UNKNOWN calls, then create a new one or add to existing dialing call
    * 
    * When adding to dialing call, select it to give user feedback of where the tone went.
    * 
    * Any other comportment need to be documented here or treated as a bug
    */

   Call* call = CallModel::instance()->getCall(m_pParent->m_pView->selectionModel()->currentIndex());
   Call* currentCall = nullptr;
   Call* candidate   = nullptr;


   //If the selected call is also the current one, then send DTMF and exit
   if(call && call->state() == Call::State::CURRENT) {
      currentCall = call;
      call->playDTMF(str);
      return;
   }

   foreach (Call* call2, CallModel::instance()->getCallList()) {
      if(dynamic_cast<Call*>(call2) && currentCall != call2 && call2->state() == Call::State::CURRENT) {
         call2->performAction(Call::Action::HOLD);
      }
      else if(dynamic_cast<Call*>(call2) && call2->state() == Call::State::DIALING) {
         candidate = call2;
         m_pParent->selectDialingCall();
      }
   }

   if(!currentCall && !candidate) {
      kDebug() << "Typing when no item is selected. Opening an item.";
      candidate = CallModel::instance()->dialingCall();
      m_pParent->selectDialingCall();
      candidate->playDTMF(str);
      const QModelIndex& newCallIdx = CallModel::instance()->getIndex(candidate);
      if (newCallIdx.isValid()) {
         m_pParent->m_pView->selectionModel()->setCurrentIndex(newCallIdx,QItemSelectionModel::SelectCurrent);
      }
      m_pParent->updateWindowCallState();
   }

   if (!candidate) {
      candidate = CallModel::instance()->dialingCall();
      candidate->playDTMF(str);
      m_pParent->updateWindowCallState();
   }
   if(!currentCall && candidate) {
      candidate->playDTMF(str);
      candidate->appendText(str);
   }
} //typeString

///Called when a backspace is detected
void EventManager::backspace()
{
   kDebug() << "backspace";
   Call* call = CallModel::instance()->getCall(m_pParent->m_pView->selectionModel()->currentIndex());
   if(!call) {
      kDebug() << "Error : Backspace on unexisting call.";
   }
   else {
      call->backspaceItemText();
   }
}

///Called when escape is detected
void EventManager::escape()
{
   kDebug() << "escape";
   Call* call = m_pParent->currentCall();
   if (m_pParent->m_pTransferOverlay && m_pParent->m_pTransferOverlay->isVisible()) {
      m_pParent->m_pTransferOverlay->setVisible(false);
      m_pParent->updateWindowCallState();
      return;
   }

   if(!call) {
      kDebug() << "Escape when no item is selected. Doing nothing.";
   }
   else {
      switch (call->state()) {
         case Call::State::TRANSFERRED:
         case Call::State::TRANSF_HOLD:
            call->performAction(Call::Action::TRANSFER);
            break;
         case Call::State::ERROR:
         case Call::State::INCOMING:
         case Call::State::DIALING:
         case Call::State::INITIALIZATION:
         case Call::State::HOLD:
         case Call::State::RINGING:
         case Call::State::CURRENT:
         case Call::State::FAILURE:
         case Call::State::BUSY:
         case Call::State::OVER:
         case Call::State::CONFERENCE:
         case Call::State::CONFERENCE_HOLD:
         case Call::State::__COUNT:
         default:
            call->performAction(Call::Action::REFUSE);
      }
   }
} //escape

///Called when enter is detected
void EventManager::enter()
{
   Call* call = m_pParent->currentCall();
   if(!call) {
      kDebug() << "Error : Enter on unexisting call.";
   }
   else {
      switch (call->state()) {
         case Call::State::DIALING:
            //Change account if it changed
            call->setAccount(AccountListModel::instance()->currentAccount());
         case Call::State::INCOMING:
         case Call::State::TRANSFERRED:
         case Call::State::TRANSF_HOLD:
            call->performAction(Call::Action::ACCEPT);
            break;
         case Call::State::HOLD:
            call->performAction(Call::Action::HOLD);
            break;
         case Call::State::INITIALIZATION:
         case Call::State::RINGING:
         case Call::State::CURRENT:
         case Call::State::FAILURE:
         case Call::State::BUSY:
         case Call::State::OVER:
         case Call::State::ERROR:
         case Call::State::CONFERENCE:
         case Call::State::CONFERENCE_HOLD:
         case Call::State::__COUNT:
         default:
            kDebug() << "Enter when call selected not in appropriate state. Doing nothing.";
      }
   }
}

///Macros needs to be executed at high level so the animations kicks in
void EventManager::addDTMF(const QString& sequence)
{
   if (sequence == "\n")
      enter();
   else
      typeString(sequence);
}

/*****************************************************************************
 *                                                                           *
 *                                  Slots                                    *
 *                                                                           *
 ****************************************************************************/
void EventManager::slotCallStateChanged(Call* call, Call::State previousState)
{
   Q_UNUSED(call)
   Q_UNUSED(previousState)
   switch (call->state()) {
      case Call::State::RINGING:
         m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_RINGING);
         SFLPhone::app()->selectCallTab();
         break;
      case Call::State::DIALING:
      case Call::State::INCOMING:
         break; //Handled elsewhere
      case Call::State::OVER:
         if (previousState == Call::State::DIALING || previousState == Call::State::OVER) {
            if (call->historyState() == Call::LegacyHistoryState::MISSED)
               m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_ENDED,i18n("Missed"));
            else
               m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_ENDED,i18n("Cancelled"));
         }
         else
            m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_ENDED,i18n("Call ended"));
         break;
      case Call::State::FAILURE:
      case Call::State::BUSY:
         m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_BUSY);
         SFLPhone::view()->updateWindowCallState();
         break;
      case Call::State::INITIALIZATION:
      case Call::State::TRANSFERRED:
      case Call::State::TRANSF_HOLD:
      case Call::State::HOLD:
      case Call::State::CURRENT:
      case Call::State::ERROR:
      case Call::State::CONFERENCE:
      case Call::State::CONFERENCE_HOLD:
      case Call::State::__COUNT:
      default:
         m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_STATE_CHANGED);
         kDebug() << "Enter when call selected not in appropriate state. Doing nothing.";
   }
}

void EventManager::slotIncomingCall(Call* call)
{
   Q_UNUSED(call)
   if (call->state() == Call::State::INCOMING || call->state() == Call::State::RINGING) {
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_RINGING);
      SFLPhone::app()->selectCallTab();
   }
}

void EventManager::slotAutoCompletionVisibility(bool visible,bool hasCall)
{
   Q_UNUSED(visible)
   Q_UNUSED(hasCall)
   //Show/Hide the autocompletion widget. This does this way due to legacy reasons
   if (visible)
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_DIALING_CHANGED);
   else if (!hasCall)
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::USER_CANCEL);
   //The other is handled by call state changed
}

void EventManager::slotAccountRegistrationChanged(Account* a,bool reg)
{
   Q_UNUSED(a)
   if (a && !reg)
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::UNREGISTERED_ACCOUNT,i18n("%1 is not unregistered",a->alias()));
   else
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::REGISTERED_ACCOUNT);
}

void EventManager::slotNetworkDown()
{
   m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::NETWORK_ERROR);
}

#include "moc_eventmanager.cpp"
#include "eventmanager.moc"
