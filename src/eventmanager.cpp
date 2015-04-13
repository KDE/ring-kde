/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include <QtCore/QMimeData>
#include <QtCore/QLocale>
#include <QtCore/QMutex>

//KDE
#include <klocalizedstring.h>

//Ring
#include <mime.h>
#include <contactmethod.h>
#include <account.h>
#include <phonedirectorymodel.h>
#include <accountmodel.h>
#include <availableaccountmodel.h>
#include <availableaccountmodel.h>
#include <personmodel.h>
#include <klib/tipmanager.h>
#include "view.h"
#include "ring.h"
#include "useractionmodel.h"
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
      Ring::app()->installEventFilter(this);
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
         switch (Ring::app()->windowState()) {
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
EventManager::EventManager(View* parent): QObject(parent),m_pParent(parent),m_pMainWindowEv(new MainWindowEvent(this))
{
   connect(CallModel::instance()    , SIGNAL(callStateChanged(Call*,Call::State)) , this, SLOT(slotCallStateChanged(Call*,Call::State)) );
   connect(CallModel::instance()    , SIGNAL(incomingCall(Call*))                 , this, SLOT(slotIncomingCall(Call*)) );

   connect(m_pMainWindowEv , SIGNAL(minimized(bool)) , m_pParent->m_pCanvasManager, SLOT(slotMinimized(bool)));

   connect(AccountModel::instance(),SIGNAL(registrationChanged(Account*,bool)),this,SLOT(slotregistrationChanged(Account*,bool)));
   connect(AccountModel::instance(),SIGNAL(badGateway()),this,SLOT(slotNetworkDown()));
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
   if (!e->mimeData()->hasFormat(RingMimes::CALLID) || CallModel::instance()->hasConference())
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::DRAG_ENTER);
   return false;
}

///Callback when something is dropped on the call canvas
bool EventManager::viewDropEvent(QDropEvent* e)
{
   const QModelIndex& idxAt = m_pParent->m_pView->indexAt(e->pos());
   m_pParent->m_pView->cancelHoverState();
   CallModel::instance()->setData(idxAt,-1,static_cast<int>(Call::Role::DropState));
   e->accept();
   if (!idxAt.isValid()) { //Dropped on empty space
      if (e->mimeData()->hasFormat(RingMimes::CALLID)) {
         const QByteArray encodedCallId      = e->mimeData()->data( RingMimes::CALLID      );
         qDebug() << "Call dropped on empty space";
         Call* call =  CallModel::instance()->fromMime(encodedCallId);
         if (CallModel::instance()->getIndex(call).parent().isValid()) {
            qDebug() << "Detaching participant";
            CallModel::instance()->detachParticipant(CallModel::instance()->fromMime(encodedCallId));
         }
         else
            qDebug() << "The call is not in a conversation (doing nothing)";
      }
      else if (e->mimeData()->hasFormat(RingMimes::PHONENUMBER)) {
         const QByteArray encodedContactMethod = e->mimeData()->data( RingMimes::PHONENUMBER );
         qDebug() << "Phone number dropped on empty space";
         Call* newCall = CallModel::instance()->dialingCall();
         ContactMethod* nb = PhoneDirectoryModel::instance()->fromHash(encodedContactMethod);
         newCall->setDialNumber(nb);
         if (nb && nb->account())
            newCall->setAccount(nb->account());
         newCall->performAction(Call::Action::ACCEPT);
      }
      else if (e->mimeData()->hasFormat(RingMimes::CONTACT)) {
         const QByteArray encodedPerson     = e->mimeData()->data( RingMimes::CONTACT     );
         qDebug() << "Person dropped on empty space";
         const ContactMethod* number = KPhoneNumberSelector().getNumber(PersonModel::instance()->getPersonByUid(encodedPerson));
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
      m_pParent->m_pView->model()->setData(idx,-1,static_cast<int>(Call::Role::DropState));
      for (int j = 0;j < m_pParent->m_pView->model()->rowCount(idx);j++) {
         m_pParent->m_pView->model()->setData(m_pParent->m_pView->model()->index(j,0,idx),-1,static_cast<int>(Call::Role::DropState));
      }
   }
   return false;
}

///Callback when a drag and drop is in progress
bool EventManager::viewDragMoveEvent(const QDragMoveEvent* e)
{
   const bool isCall = e->mimeData()->hasFormat(RingMimes::CALLID);
   if (TipCollection::removeConference() != TipCollection::manager()->currentTip() /*&& idxAt.parent().isValid()*/) {
      if (isCall) {
         TipCollection::removeConference()->setText(i18n("Remove the call from the conference, the call will be put on hold"));
      }
      else if (e->mimeData()->hasFormat(RingMimes::PHONENUMBER)) {
         ContactMethod* n = PhoneDirectoryModel::instance()->fromHash(e->mimeData()->data(RingMimes::PHONENUMBER));
         if (n)
            TipCollection::removeConference()->setText(i18n("Call %1 using %2",n->uri(),
               (n->account()?n->account():AvailableAccountModel::instance()->currentDefaultAccount())->alias()));
      }
      else if (e->mimeData()->hasFormat(RingMimes::CONTACT)) {
         Person* c = PersonModel::instance()->getPersonByUid(e->mimeData()->data(RingMimes::CONTACT));
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
      else if (e->mimeData()->hasFormat(RingMimes::PHONENUMBER)) {
         ContactMethod* n = PhoneDirectoryModel::instance()->fromHash(e->mimeData()->data(RingMimes::PHONENUMBER));
         if (n)
            TipCollection::removeConference()->setText(i18n("Call %1 using %2",n->uri(),
               (n->account()?n->account():AvailableAccountModel::instance()->currentDefaultAccount())->alias()));
      }
      else if (e->mimeData()->hasFormat(RingMimes::CONTACT)) {
         Person* c = PersonModel::instance()->getPersonByUid(e->mimeData()->data(RingMimes::CONTACT));
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
      source = CallModel::instance()->fromMime(e->mimeData()->data(RingMimes::CALLID));
   Call::DropAction act = (position.x() < targetRect.x()+targetRect.width()/2)?Call::DropAction::Conference:Call::DropAction::Transfer;
   CallModel::instance()->setData(idxAt,act,static_cast<int>(Call::Role::DropPosition));
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
            ContactMethod* n = m_pParent->m_pAutoCompletion->selection();
            Call* call = m_pParent->currentCall();
            if (call->state() == Call::State::DIALING) {
               call->setDialNumber(n->uri());
               if (PhoneDirectoryModel::instance()->callWithAccount()
                && n->account() && n->account()->id() != "IP2IP") //FIXME do not hardcode this
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

   foreach (Call* call2, CallModel::instance()->getActiveCalls()) {
      if(call2 && currentCall != call2 && call2->state() == Call::State::CURRENT) {
         call2->performAction(Call::Action::HOLD);
      }
      else if(call2 && (call2->lifeCycleState() == Call::LifeCycleState::CREATION)) {
         candidate = call2;
         m_pParent->selectDialingCall();
      }
   }

   if(!currentCall && !candidate) {
      qDebug() << "Typing when no item is selected. Opening an item.";
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
   qDebug() << "backspace";
   Call* call = CallModel::instance()->getCall(m_pParent->m_pView->selectionModel()->currentIndex());
   if(!call) {
      qDebug() << "Error : Backspace on unexisting call.";
   }
   else {
      call->backspaceItemText();
   }
}

///Called when escape is detected
void EventManager::escape()
{
   qDebug() << "escape";
   Call* call = m_pParent->currentCall();
   if (m_pParent->m_pTransferOverlay && m_pParent->m_pTransferOverlay->isVisible()) {
      m_pParent->m_pTransferOverlay->setVisible(false);
      m_pParent->updateWindowCallState();
      return;
   }

   if(!call) {
      qDebug() << "Escape when no item is selected. Doing nothing.";
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
         case Call::State::NEW:
         case Call::State::ABORTED:
         case Call::State::OVER:
         case Call::State::CONFERENCE:
         case Call::State::CONFERENCE_HOLD:
         case Call::State::COUNT__:
         default:
            CallModel::instance()->userActionModel() << UserActionModel::Action::HANGUP;
      }
   }
} //escape

///Called when enter is detected
void EventManager::enter()
{
   Call* call = m_pParent->currentCall();
   if(!call) {
      qDebug() << "Error : Enter on unexisting call.";
   }
   else {
      switch (call->state()) {
         case Call::State::CONFERENCE_HOLD:
         case Call::State::HOLD:
            CallModel::instance()->userActionModel() << UserActionModel::Action::HOLD;
            break;
         case Call::State::FAILURE:
         case Call::State::BUSY:
         case Call::State::OVER:
         case Call::State::ABORTED:
         case Call::State::ERROR:
            CallModel::instance()->userActionModel() << UserActionModel::Action::HANGUP;
            break;
         case Call::State::DIALING:
         case Call::State::INCOMING:
         case Call::State::TRANSFERRED:
         case Call::State::TRANSF_HOLD:
         case Call::State::INITIALIZATION:
         case Call::State::RINGING:
         case Call::State::CURRENT:
         case Call::State::CONFERENCE:
            CallModel::instance()->userActionModel() << UserActionModel::Action::ACCEPT;
            break;
         case Call::State::COUNT__:
         case Call::State::NEW:
         default:
            qDebug() << "Enter when call selected not in appropriate state. Doing nothing.";
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
         Ring::app()->selectCallTab();
         break;
      case Call::State::DIALING:
      case Call::State::NEW:
      case Call::State::INCOMING:
         break; //Handled elsewhere
      case Call::State::OVER:
         if (previousState == Call::State::DIALING || previousState == Call::State::NEW || previousState == Call::State::OVER) {
            if (call->isMissed())
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
         Ring::view()->updateWindowCallState();
         break;
      case Call::State::INITIALIZATION:
      case Call::State::TRANSFERRED:
      case Call::State::TRANSF_HOLD:
      case Call::State::HOLD:
      case Call::State::ABORTED:
      case Call::State::CURRENT:
      case Call::State::ERROR:
      case Call::State::CONFERENCE:
      case Call::State::CONFERENCE_HOLD:
      case Call::State::COUNT__:
      default:
         m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_STATE_CHANGED);
         qDebug() << "Enter when call selected not in appropriate state. Doing nothing.";
   }
}

void EventManager::slotIncomingCall(Call* call)
{
   Q_UNUSED(call)
   if (call->state() == Call::State::INCOMING || call->state() == Call::State::RINGING) {
      m_pParent->m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_RINGING);
      Ring::app()->selectCallTab();
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

void EventManager::slotregistrationChanged(Account* a,bool reg)
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
