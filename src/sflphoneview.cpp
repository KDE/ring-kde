/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
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
#include "sflphoneview.h"

//Qt
#include <QtCore/QString>
#include <QtCore/QPointer>
#include <QtGui/QPalette>
#include <QtGui/QWidget>
#include <QtGui/QClipboard>
#include <QtGui/QKeyEvent>
#include <QtGui/QDropEvent>
#include <QtGui/QBitmap>
#include <QtGui/QPainter>
#include <QtGui/QPixmap>
#include <QtCore/QAbstractAnimation>

//KDE
#include <KLocale>
#include <KAction>
#include <KMenu>
#include <KInputDialog>
#include <kabc/addressbook.h>
#include <KMessageBox>
#include <KIcon>
#include <KColorScheme>
#include <KStandardDirs>

//sflphone
#include "accountwizard.h"
#include "sflphone.h"
#include "canvasobjectmanager.h"
#include "widgets/tips/tipcollection.h"
#include "widgets/callviewtoolbar.h"
#include "extendedaction.h"
#include "eventmanager.h"
#include "actioncollection.h"
#include "delegates/conferencedelegate.h"
#include "delegates/historydelegate.h"
#include "delegates/categorizeddelegate.h"
#include "widgets/tips/dialpadtip.h"
#include "widgets/kphonenumberselector.h"
#include "widgets/callviewoverlay.h"
#include "widgets/autocompletion.h"

//sflphone library
#include "klib/kcfg_settings.h"
#include "lib/phonenumber.h"
#include "lib/dbus/callmanager.h"
#include "lib/dbus/instancemanager.h"
#include "lib/sflphone_const.h"
#include "lib/contact.h"
#include "lib/accountlistmodel.h"
#include "lib/phonedirectorymodel.h"
#include "lib/audiosettingsmodel.h"
#include "lib/presencestatusmodel.h"
#include "klib/helperfunctions.h"
#include "klib/tipmanager.h"
#include "implementation.h"

#define IM_ACTIVE m_pMessageTabBox->isVisible()

#define ACTION_LABEL_CALL                 i18n("New call")
#define ACTION_LABEL_PLACE_CALL           i18n("Place call")
#define ACTION_LABEL_HANG_UP              i18n("Hang up")
#define ACTION_LABEL_HOLD                 i18n("Hold on")
#define ACTION_LABEL_TRANSFER             i18n("Transfer")
#define ACTION_LABEL_RECORD               i18n("Record")
#define ACTION_LABEL_ACCEPT               i18n("Pick up")
#define ACTION_LABEL_REFUSE               i18n("Hang up")
#define ACTION_LABEL_UNHOLD               i18n("Hold off")
#define ACTION_LABEL_GIVE_UP_TRANSF       i18n("Give up transfer")
#define ACTION_LABEL_MAILBOX              i18n("Voicemail")

///Constructor
SFLPhoneView::SFLPhoneView(QWidget *parent)
   : QWidget(parent),m_pTransferOverlay(nullptr),m_pAutoCompletion(nullptr)
{
   new KDEPixmapManipulation();
   setupUi(this);
   KPhoneNumberSelector::init();

   //Enable on-canvas messages
   TipCollection::setManager(new TipManager(m_pView));
   m_pCanvasManager = new CanvasObjectManager();

   //Set global settings
   AudioSettingsModel::instance()->setEnableRoomTone(ConfigurationSkeleton::enableRoomTone());
   PresenceStatusModel::instance()->setPresenceVisitor(new KDEPresenceSerializationVisitor());

   m_pEventManager = new EventManager(this);
   m_pView->setModel(CallModel::instance());
   TipCollection::manager()->changeSize();
   connect(CallModel::instance(),SIGNAL(layoutChanged()),m_pView,SLOT(expandAll()));
   m_pView->expandAll();
   m_pConfDelegate = new ConferenceDelegate(m_pView,palette());
   m_pHistoryDelegate = new HistoryDelegate(m_pView);
   m_pConfDelegate->setCallDelegate(m_pHistoryDelegate);
   m_pView->setItemDelegate(m_pConfDelegate);
   m_pView->viewport()->installEventFilter(m_pEventManager);
   m_pView->installEventFilter(m_pEventManager);
   m_pView->setViewType(CategorizedTreeView::ViewType::Call);


   if (!CallModel::instance()->getCallList().size())
      m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::NO_CALLS);

   QPalette pal = QPalette(palette());
   pal.setColor(QPalette::AlternateBase, Qt::lightGray);
   setPalette(pal);

   m_pColorVisitor = new ColorVisitor(pal);
   AccountListModel::instance()->setColorVisitor(m_pColorVisitor);

   m_pMessageBoxW->setVisible(false);

   //Setup volume
   toolButton_recVol->setDefaultAction(ActionCollection::instance()->muteCaptureAction());
   toolButton_sndVol->setDefaultAction(ActionCollection::instance()->mutePlaybackAction());

   connect(slider_recVol,SIGNAL(valueChanged(int)),AudioSettingsModel::instance(),SLOT(setCaptureVolume(int)));
   connect(slider_sndVol,SIGNAL(valueChanged(int)),AudioSettingsModel::instance(),SLOT(setPlaybackVolume(int)));

   //Setup signals
   //                SENDER                             SIGNAL                              RECEIVER                SLOT                      /
   /**/connect(CallModel::instance()        , SIGNAL(incomingCall(Call*))                   , this   , SLOT(on1_incomingCall(Call*))          );
   /**/connect(AccountListModel::instance() , SIGNAL(voiceMailNotify(Account*,int))         , this   , SLOT(on1_voiceMailNotify(Account*,int)) );
   /**/connect(CallModel::instance()        , SIGNAL(callStateChanged(Call*,Call::State))   , this   , SLOT(updateWindowCallState())          );
   /**/connect(AccountListModel::instance() , SIGNAL(accountListUpdated())                  , this   , SLOT(updateWindowCallState())          );
   /**/connect(m_pSendMessageLE             , SIGNAL(returnPressed())                       , this   , SLOT(sendMessage())                    );
   /**/connect(m_pSendMessagePB             , SIGNAL(clicked())                             , this   , SLOT(sendMessage())                    );
   /**/connect(m_pView                      , SIGNAL(itemDoubleClicked(QModelIndex)), m_pEventManager, SLOT(enter())                          );
   /*                                                                                                                                        */

   //Volume controls
//    connect(AudioSettingsModel::instance(),SIGNAL(playbackVolumeChanged(int)),slider_sndVol,SLOT(setValue(int)));

   //Auto completion
   loadAutoCompletion();

   m_pCanvasToolbar = new CallViewToolbar(m_pView);
   connect(m_pView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)) , m_pCanvasToolbar, SLOT(updateState()));
   connect(CallModel::instance()    , SIGNAL(callStateChanged(Call*,Call::State))     , m_pCanvasToolbar, SLOT(updateState()));
   connect(CallModel::instance()    , SIGNAL(layoutChanged())                         , m_pCanvasToolbar, SLOT(updateState()));
}

///Destructor
SFLPhoneView::~SFLPhoneView()
{
   m_pView->setItemDelegate(nullptr);
   delete m_pConfDelegate;
   delete m_pHistoryDelegate;
   delete m_pCanvasManager;
   if (m_pAutoCompletion)
      delete m_pAutoCompletion;
   delete m_pColorVisitor;
}

///Init main window
void SFLPhoneView::loadWindow()
{
   updateWindowCallState ();
   updateVolumeControls  ();
   loadAutoCompletion    ();
   widget_dialpad->setVisible(ConfigurationSkeleton::displayDialpad());
   AudioSettingsModel::instance()->setEnableRoomTone(ConfigurationSkeleton::enableRoomTone());
}


/*****************************************************************************
 *                                                                           *
 *                                  Setters                                  *
 *                                                                           *
 ****************************************************************************/

///Set the current selection item
void SFLPhoneView::setCurrentIndex(const QModelIndex& idx) const
{
   m_pView->selectionModel()->setCurrentIndex(idx,QItemSelectionModel::SelectCurrent);
}

/*****************************************************************************
 *                                                                           *
 *                                  Getters                                  *
 *                                                                           *
 ****************************************************************************/

///Return the auto completion widget
AutoCompletion* SFLPhoneView::autoCompletion() const
{
   return m_pAutoCompletion;
}

///Return the current (selected) call
Call* SFLPhoneView::currentCall() const
{
   return CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
}

/*****************************************************************************
 *                                                                           *
 *                              Keyboard input                               *
 *                                                                           *
 ****************************************************************************/

///Create a call from the clipboard content
void SFLPhoneView::paste()
{
   QClipboard* cb = QApplication::clipboard();
   const QMimeData* data = cb->mimeData();
   if (data->hasFormat(MIME_PHONENUMBER)) {
      m_pEventManager->typeString(data->data(MIME_PHONENUMBER));
   }
   else {
      m_pEventManager->typeString(cb->text());
   }
}


/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/

void SFLPhoneView::selectDialingCall() const
{
   foreach(Call* call,CallModel::instance()->getCallList()) {
      if (call->state() == Call::State::DIALING) {
         const QModelIndex idx = CallModel::instance()->getIndex(call);
         setCurrentIndex(idx);

         //Focus editor widget
         //m_pView->openPersistentEditor(idx);
      }
   }
}


/*****************************************************************************
 *                                                                           *
 *                       Update display related code                         *
 *                                                                           *
 ****************************************************************************/

///Change GUI icons
void SFLPhoneView::updateWindowCallState()
{
   kDebug() << "Call state changed";
   bool    enabledActions [6] = { true             , true                , true             , true                 , true               , true                 };
   QString buttonIconFiles[6] = { ICON_CALL        , ICON_HANGUP         , ICON_HOLD        , ICON_TRANSFER        , ICON_REC_DEL_OFF   , ICON_MAILBOX         };
   QString actionTexts    [6] = { ACTION_LABEL_CALL, ACTION_LABEL_HANG_UP, ACTION_LABEL_HOLD, ACTION_LABEL_TRANSFER, ACTION_LABEL_RECORD, ACTION_LABEL_MAILBOX };

   Call* call = 0;

   bool transfer(false),recordActivated(false);

   enabledActions[SFLPhone::CallAction::Mailbox] = AccountListModel::currentAccount() && ! AccountListModel::currentAccount()->mailbox().isEmpty();

   call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if (!call) {
      kDebug() << "No item selected.";
      enabledActions[ SFLPhone::CallAction::Refuse   ] = false;
      enabledActions[ SFLPhone::CallAction::Hold     ] = false;
      enabledActions[ SFLPhone::CallAction::Transfer ] = false;
      enabledActions[ SFLPhone::CallAction::Record   ] = false;
      m_pMessageBoxW->setVisible(false);
   }
   else if (call->type() == Call::Type::CONFERENCE) {
      //TODO Something to do?
   }
   else {
      Call::State state = call->state();
      recordActivated = call->isRecording();

      switch (state) {
         case Call::State::INCOMING:
            buttonIconFiles [ SFLPhone::CallAction::Accept   ] = ICON_ACCEPT                 ;
            buttonIconFiles [ SFLPhone::CallAction::Refuse   ] = ICON_REFUSE                 ;
            actionTexts     [ SFLPhone::CallAction::Accept   ] = ACTION_LABEL_ACCEPT         ;
            actionTexts     [ SFLPhone::CallAction::Refuse   ] = ACTION_LABEL_REFUSE         ;
            m_pMessageBoxW->setVisible(false || IM_ACTIVE)   ;
            break;

         case Call::State::RINGING:
         case Call::State::INITIALIZATION:
            enabledActions  [ SFLPhone::CallAction::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::CallAction::Transfer ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::CURRENT:
            buttonIconFiles [ SFLPhone::CallAction::Record   ] = ICON_REC_DEL_ON             ;
            m_pMessageBoxW->setVisible((true && ConfigurationSkeleton::displayMessageBox()) || IM_ACTIVE);
            break;

         case Call::State::DIALING:
            enabledActions  [ SFLPhone::CallAction::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::CallAction::Transfer ] = false                       ;
            enabledActions  [ SFLPhone::CallAction::Record   ] = false                       ;
            actionTexts     [ SFLPhone::CallAction::Accept   ] = ACTION_LABEL_PLACE_CALL     ;
            buttonIconFiles [ SFLPhone::CallAction::Accept   ] = ICON_ACCEPT                 ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::HOLD:
            buttonIconFiles [ SFLPhone::CallAction::Hold     ] = ICON_UNHOLD                 ;
            actionTexts     [ SFLPhone::CallAction::Hold     ] = ACTION_LABEL_UNHOLD         ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::FAILURE:
            //enabledActions  [ SFLPhone::Accept   ] = false                     ;
            enabledActions  [ SFLPhone::CallAction::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::CallAction::Transfer ] = false                       ;
            enabledActions  [ SFLPhone::CallAction::Record   ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::BUSY:
            //enabledActions  [ SFLPhone::Accept   ] = false                     ;
            enabledActions  [ SFLPhone::CallAction::Hold     ] = false                       ;
            enabledActions  [ SFLPhone::CallAction::Transfer ] = false                       ;
            enabledActions  [ SFLPhone::CallAction::Record   ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::TRANSFERRED:
            buttonIconFiles [ SFLPhone::CallAction::Accept   ] = ICON_EXEC_TRANSF            ;
            actionTexts     [ SFLPhone::CallAction::Transfer ] = ACTION_LABEL_GIVE_UP_TRANSF ;
            buttonIconFiles [ SFLPhone::CallAction::Record   ] = ICON_REC_DEL_ON             ;
            m_pMessageBoxW->setVisible(false || IM_ACTIVE)                       ;
            if (!m_pTransferOverlay) {
               m_pTransferOverlay = new CallViewOverlay(m_pView);
            }
            m_pTransferOverlay->setCurrentCall(call);
            m_pTransferOverlay->setVisible(true);
            transfer = true;
            break;

         case Call::State::TRANSF_HOLD:
            buttonIconFiles [ SFLPhone::CallAction::Accept   ] = ICON_EXEC_TRANSF            ;
            buttonIconFiles [ SFLPhone::CallAction::Hold     ] = ICON_UNHOLD                 ;
            actionTexts     [ SFLPhone::CallAction::Transfer ] = ACTION_LABEL_GIVE_UP_TRANSF ;
            actionTexts     [ SFLPhone::CallAction::Hold     ] = ACTION_LABEL_UNHOLD         ;
            m_pMessageBoxW->setVisible(false)                                    ;
            transfer = true;
            break;

         case Call::State::OVER:
            kDebug() << "Error : Reached CALL_STATE_OVER with call "  << call->id() << "!";
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::ERROR:
            kDebug() << "Error : Reached CALL_STATE_ERROR with call " << call->id() << "!";
            m_pMessageBoxW->setVisible(false)                                    ;
            break;

         case Call::State::CONFERENCE:
            enabledActions  [ SFLPhone::CallAction::Transfer ] = false                       ;
            m_pMessageBoxW->setVisible(false || IM_ACTIVE)                       ;
            break;

         case Call::State::CONFERENCE_HOLD:
            enabledActions  [ SFLPhone::CallAction::Transfer ] = false                       ;
            m_pMessageBoxW->setVisible(false)                                    ;
            break;
         case Call::State::__COUNT:
         default:
            kDebug() << "Error : Reached unexisting state for call "  << call->id() << "(" << call->state() << "!";
            break;

      }

      if (TipCollection::dragAndDrop()) {
         int activeCallCounter=0;
         foreach (Call* call2, CallModel::instance()->getCallList()) {
            if (dynamic_cast<Call*>(call2)) {
               activeCallCounter += (call2->lifeCycleState() == Call::LifeCycleState::PROGRESS)?1:0;
               activeCallCounter -= (call2->lifeCycleState() == Call::LifeCycleState::INITIALIZATION)*1000;
            }
         }
         if (activeCallCounter >= 2 && !CallModel::instance()->getConferenceList().size()) {
            m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::CALL_COUNT_CHANGED);
         }
      }
   }

   emit enabledActionsChangeAsked     ( enabledActions  );
   emit actionIconsChangeAsked        ( buttonIconFiles );
   emit actionTextsChangeAsked        ( actionTexts     );
   emit transferCheckStateChangeAsked ( transfer        );
   emit recordCheckStateChangeAsked   ( recordActivated );

} //updateWindowCallState

void SFLPhoneView::loadAutoCompletion()
{
   if (ConfigurationSkeleton::enableAutoCompletion()) {
      if (!m_pAutoCompletion) {
         m_pAutoCompletion = new AutoCompletion(m_pView);
         PhoneDirectoryModel::instance()->setCallWithAccount(ConfigurationSkeleton::autoCompleteUseAccount());
         m_pAutoCompletion->setUseUnregisteredAccounts(ConfigurationSkeleton::autoCompleteMergeNumbers());
         connect(m_pAutoCompletion, SIGNAL(requestVisibility(bool,bool)), m_pEventManager, SLOT(slotAutoCompletionVisibility(bool,bool)));
         connect(m_pAutoCompletion,SIGNAL(doubleClicked(PhoneNumber*)),this,SLOT(slotAutoCompleteClicked(PhoneNumber*)));
      }
   }
   else if (m_pAutoCompletion) {
      delete m_pAutoCompletion;
      m_pAutoCompletion = nullptr;
   }
}

///Hide or show the volume control
void SFLPhoneView::updateVolumeControls()
{
   toolButton_recVol->setVisible ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );
   toolButton_sndVol->setVisible ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );
   slider_recVol->setVisible     ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );
   slider_sndVol->setVisible     ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );

   //Prevent an infinite update loop
   slider_recVol->blockSignals(true);
   slider_sndVol->blockSignals(true);

   slider_recVol->setValue(AudioSettingsModel::instance()->captureVolume());
   slider_sndVol->setValue(AudioSettingsModel::instance()->playbackVolume());

   slider_recVol->blockSignals(false);
   slider_sndVol->blockSignals(false);
}


/*****************************************************************************
 *                                                                           *
 *                                    Slots                                  *
 *                                                                           *
 ****************************************************************************/

///Proxy to hide or show the volume control
void SFLPhoneView::displayVolumeControls(bool checked)
{
   ConfigurationSkeleton::setDisplayVolume(checked);
   updateVolumeControls();
}

///Proxy to hide or show the dialpad
void SFLPhoneView::displayDialpad(bool checked)
{
   ConfigurationSkeleton::setDisplayDialpad(checked);
   widget_dialpad->setVisible(ConfigurationSkeleton::displayDialpad());
}

///Display a notification popup (freedesktop notification)
void SFLPhoneView::displayMessageBox(bool checked)
{
   ConfigurationSkeleton::setDisplayMessageBox(checked);
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   m_pMessageBoxW->setVisible(checked
      && call
      && (call->state()   == Call::State::CURRENT
         || call->state() == Call::State::HOLD
      )
   );
}

///Input grabber
void SFLPhoneView::on_widget_dialpad_typed(QString text)
{
   m_pEventManager->typeString(text);
}

///When a call is coming (dbus)
void SFLPhoneView::on1_incomingCall(Call* call)
{
   kDebug() << "Signal : Incoming Call ! ID = " << call->id();

   updateWindowCallState();

   if (ConfigurationSkeleton::displayOnCalls()) {
      SFLPhone::app()->activateWindow(      );
      SFLPhone::app()->raise         (      );
      SFLPhone::app()->setVisible    ( true );
   }

   const QModelIndex& idx = CallModel::instance()->getIndex(call);
   if (idx.isValid() && (call->state() == Call::State::RINGING || call->state() == Call::State::INCOMING)) {
      m_pView->selectionModel()->clearSelection();
      m_pView->selectionModel()->setCurrentIndex(idx,QItemSelectionModel::SelectCurrent);
   }

   emit incomingCall(call);
}

///When a new voice mail is coming
void SFLPhoneView::on1_voiceMailNotify(Account* a, int count)
{
   kDebug() << "Signal : VoiceMail Notify ! " << count << " new voice mails for account " << a->alias();
}


///Send a text message
void SFLPhoneView::sendMessage()
{
   Call* call = CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   if (dynamic_cast<Call*>(call) && !m_pSendMessageLE->text().isEmpty()) {
      call->sendTextMessage(m_pSendMessageLE->text());
   }
   m_pSendMessageLE->clear();
}

void SFLPhoneView::slotAutoCompleteClicked(PhoneNumber* n)
{
   Call* call = currentCall();
   if (call->state() == Call::State::DIALING) {
      call->setDialNumber(n);
      if (n->account())
         call->setAccount(n->account());
      call->performAction(Call::Action::ACCEPT);
      m_pAutoCompletion->reset();
   }
}

#undef IM_ACTIVE

#include "sflphoneview.moc"
