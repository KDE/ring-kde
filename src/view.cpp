/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include "view.h"

//Qt
#include <QtCore/QString>
#include <QtGui/QPalette>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMenu>
#include <QtGui/QClipboard>
#include <QtCore/QMimeData>
#include <QtCore/QSortFilterProxyModel>

//KDE
#include <klocalizedstring.h>

//Ring
#include "icons/icons.h"
#include "mainwindow.h"
#include "widgets/menumodelview.h"
#include "useractionmodel.h"
#include "canvasobjectmanager.h"
#include "widgets/tips/tipcollection.h"
#include "widgets/callviewtoolbar.h"
#include "eventmanager.h"
#include "actioncollection.h"
#include <delegates/conferencedelegate.h>
#include <delegates/historydelegate.h>
#include "widgets/tips/dialpadtip.h"
#include "widgets/kphonenumberselector.h"
#include "widgets/callviewoverlay.h"
#include "widgets/autocompletion.h"
#include "widgets/wizard.h"

//ring library
#include "klib/kcfg_settings.h"
#include "mime.h"
#include <globalinstances.h>
#include "contactmethod.h"
#include "person.h"
#include "media/text.h"
#include "accountmodel.h"
#include "phonedirectorymodel.h"
#include "audio/settings.h"
#include <tip/tipmanager.h>
#include "implementation.h"

class FocusWatcher : public QObject
{
   Q_OBJECT
public:
   explicit FocusWatcher(QObject* parent = nullptr) : QObject(parent)
   {
      if (parent)
         parent->installEventFilter(this);
   }
   virtual bool eventFilter(QObject *obj, QEvent *event) override
   {
      Q_UNUSED(obj)
      if (event->type() == QEvent::FocusIn)
         emit focusChanged(true);
      else if (event->type() == QEvent::FocusOut)
         emit focusChanged(false);

      return false;
   }

Q_SIGNALS:
   void focusChanged(bool in);
};

///Constructor
View::View(QWidget *parent)
   : QWidget(parent),m_pTransferOverlay(nullptr),m_pAutoCompletion(nullptr)
{
   setupUi(this);
   KPhoneNumberSelector::init();

   //Enable on-canvas messages
   TipCollection::setManager(new TipManager(m_pView));
   m_pCanvasManager = new CanvasObjectManager();

   //Set global settings
   Audio::Settings::instance().setEnableRoomTone(ConfigurationSkeleton::enableRoomTone());
   GlobalInstances::setInterface<KDEPresenceSerializationDelegate>();

   m_pEventManager = new EventManager(this);
   m_pView->setModel(&CallModel::instance());
   m_pView->setSelectionModel(CallModel::instance().selectionModel());
   TipCollection::manager()->changeSize();

   //Create a call already, the app is useless without one anyway
   m_pView->selectionModel()->setCurrentIndex(
      CallModel::instance().rowCount() ?
         CallModel::instance().index(0, 0)
         : CallModel::instance().getIndex(CallModel::instance().dialingCall()),
      QItemSelectionModel::SelectCurrent
   );

   //There is currently way to force a tree to be expanded beyond this
   connect(&CallModel::instance(),&QAbstractItemModel::layoutChanged,m_pView,&QTreeView::expandAll);
   m_pView->expandAll();

   m_pConfDelegate    = new ConferenceDelegate( m_pView,palette() );
   m_pHistoryDelegate = new HistoryDelegate   ( m_pView           );

   m_pConfDelegate->setCallDelegate(m_pHistoryDelegate);
   m_pView->setItemDelegate   (m_pConfDelegate                     );
   m_pView->viewport          (                                    )->installEventFilter(m_pEventManager);
   m_pView->installEventFilter( m_pEventManager                    );
   m_pView->setViewType       ( CategorizedTreeView::ViewType::Call);


   if (!CallModel::instance().getActiveCalls().size())
      m_pCanvasManager->newEvent(CanvasObjectManager::CanvasEvent::NO_CALLS);

   QPalette pal = QPalette(palette());
   pal.setColor(QPalette::AlternateBase, Qt::lightGray);
   setPalette(pal);

   GlobalInstances::setInterface<ColorDelegate>(pal);

   const QModelIndex currentIndex = CallModel::instance().selectionModel()->currentIndex();

   m_pMessageBoxW->setVisible(
      qvariant_cast<Call::LifeCycleState>(currentIndex.data((int)Call::Role::LifeCycleState))
         == Call::LifeCycleState::PROGRESS ||
      (m_pMessageTabBox->isVisible() && m_pMessageTabBox->count())
   );
   connect(new FocusWatcher(m_pSendMessageLE), &FocusWatcher::focusChanged, this, &View::displayHistory);

   //Setup volume
   toolButton_recVol->setDefaultAction(ActionCollection::instance()->muteCaptureAction ());
   toolButton_sndVol->setDefaultAction(ActionCollection::instance()->mutePlaybackAction());

   connect(slider_recVol,&QAbstractSlider::valueChanged,&Audio::Settings::instance(),&Audio::Settings::setCaptureVolume);
   connect(slider_sndVol,&QAbstractSlider::valueChanged,&Audio::Settings::instance(),&Audio::Settings::setPlaybackVolume);

   /*Setup signals                                                                                                                  */
   //                SENDER                             SIGNAL                              RECEIVER                SLOT            */
   /**/connect(&CallModel::instance()       , &CallModel::incomingCall                 , this           , &View::incomingCall       );
   /**/connect(m_pSendMessageLE             , &QLineEdit::returnPressed                , this           , &View::sendMessage        );
   /**/connect(m_pSendMessagePB             , &QAbstractButton::clicked                , this           , &View::sendMessage        );
   /**/connect(m_pView                      , &CategorizedTreeView::itemDoubleClicked  , m_pEventManager, &EventManager::enter      );
   /**/connect(widget_dialpad               , &Dialpad::typed                          , m_pEventManager, &EventManager::typeString );
   /**/connect(m_pView                      , &CategorizedTreeView::contextMenuRequest , this           , &View::slotContextMenu    );
   /*                                                                                                                               */

   connect(m_pView->selectionModel(), &QItemSelectionModel::currentChanged, this, &View::updateTextBoxStatus);

   //Auto completion
   loadAutoCompletion();

   m_pCanvasToolbar = new CallViewToolbar(m_pView);

   if (ConfigurationSkeleton::enableWizard() == true && !AccountModel::instance().isRingSupported()) {
      new Wizard(this);
   }
   ConfigurationSkeleton::setEnableWizard(false);

   setFocus(Qt::OtherFocusReason);

   widget_dialpad->setVisible(ConfigurationSkeleton::displayDialpad());
   Audio::Settings::instance().setEnableRoomTone(ConfigurationSkeleton::enableRoomTone());
}

///Destructor
View::~View()
{
   m_pView->setItemDelegate(nullptr);

   if (m_pAutoCompletion)
      delete m_pAutoCompletion;

   delete m_pConfDelegate   ;
   delete m_pHistoryDelegate;
   delete m_pCanvasManager  ;
}

///Return the auto completion widget
AutoCompletion* View::autoCompletion() const
{
   return m_pAutoCompletion;
}

bool View::messageBoxFocussed() const
{
   return m_pSendMessageLE && m_pSendMessageLE->hasFocus();
}

///Create a call from the clipboard content
void View::paste()
{
   QClipboard* cb = QApplication::clipboard();
   const QMimeData* data = cb->mimeData();
   if (data->hasFormat(RingMimes::PHONENUMBER)) {
      m_pEventManager->typeString(data->data(RingMimes::PHONENUMBER));
   }
   else {
      m_pEventManager->typeString(cb->text());
   }
}

void View::loadAutoCompletion()
{
   if (ConfigurationSkeleton::enableAutoCompletion()) {
      if (!m_pAutoCompletion) {
         m_pAutoCompletion = new AutoCompletion(m_pView);
         PhoneDirectoryModel::instance().setCallWithAccount(ConfigurationSkeleton::autoCompleteUseAccount());
         m_pAutoCompletion->setUseUnregisteredAccounts(ConfigurationSkeleton::autoCompleteMergeNumbers());
         connect(m_pAutoCompletion,&AutoCompletion::doubleClicked,this,&View::slotAutoCompleteClicked);
      }
   }
   else if (m_pAutoCompletion) {
      delete m_pAutoCompletion;
      m_pAutoCompletion = nullptr;
   }
}

///Hide or show the volume control
void View::updateVolumeControls()
{
   toolButton_recVol->setVisible ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );
   toolButton_sndVol->setVisible ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );
   slider_recVol->setVisible     ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );
   slider_sndVol->setVisible     ( ActionCollection::instance()->displayVolumeControlsAction()->isChecked() && ConfigurationSkeleton::displayVolume() );

   //Prevent an infinite update loop
   slider_recVol->blockSignals(true);
   slider_sndVol->blockSignals(true);

   slider_recVol->setValue(Audio::Settings::instance().captureVolume());
   slider_sndVol->setValue(Audio::Settings::instance().playbackVolume());

   slider_recVol->blockSignals(false);
   slider_sndVol->blockSignals(false);
}

///Proxy to hide or show the volume control
void View::displayVolumeControls(bool checked)
{
   ConfigurationSkeleton::setDisplayVolume(checked);
   updateVolumeControls();
}

///Proxy to hide or show the dialpad
void View::displayDialpad(bool checked)
{
   ConfigurationSkeleton::setDisplayDialpad(checked);
   widget_dialpad->setVisible(ConfigurationSkeleton::displayDialpad());
}

///Display a notification popup (freedesktop notification)
void View::displayMessageBox(bool checked)
{
   ConfigurationSkeleton::setDisplayMessageBox(checked);
   Call* call = CallModel::instance().selectedCall();
   m_pMessageBoxW->setVisible(checked
      && ((
        call
        && (call->lifeCycleState() == Call::LifeCycleState::PROGRESS)
      ) ||
      (
         (m_pMessageTabBox->isVisible() && m_pMessageTabBox->count())
      ))
   );
}

///When a call is coming (dbus)
void View::incomingCall(Call* call)
{
   qDebug() << "Signal : Incoming Call ! ID = " << call;

   if (ConfigurationSkeleton::displayOnCalls()) {
      MainWindow::app()->activateWindow(      );
      MainWindow::app()->raise         (      );
      MainWindow::app()->setVisible    ( true );
   }

   const QModelIndex& idx = CallModel::instance().getIndex(call);
   if (idx.isValid() && (call->state() == Call::State::RINGING || call->state() == Call::State::INCOMING)) {
      CallModel::instance().selectCall(call);
   }
}

///Send a text message
void View::sendMessage()
{
   Call* call = CallModel::instance().selectedCall();
   if (call && !m_pSendMessageLE->text().isEmpty()) {
      call->addOutgoingMedia<Media::Text>()->send({{"text/plain",m_pSendMessageLE->text()}});
   }
   m_pSendMessageLE->clear();
}

void View::slotAutoCompleteClicked(ContactMethod* n) //TODO use the new LRC API for this
{
   Call* call = CallModel::instance().selectedCall();
   if (call && call->lifeCycleState() == Call::LifeCycleState::CREATION) {
      call->setDialNumber(n);
      if (n->account())
         call->setAccount(n->account());
      call->performAction(Call::Action::ACCEPT);
      m_pAutoCompletion->reset();
   }
}

void View::slotContextMenu(const QModelIndex& index)
{
   Q_UNUSED(index)

   if (!m_pUserActionModel) {
      m_pUserActionModel = new UserActionModel(m_pView->model(), UserActionModel::Context::ALL);
      m_pUserActionModel->setSelectionModel(m_pView->selectionModel());
   }

   QMenu* m = new MenuModelView(m_pUserActionModel->activeActionModel(), new QItemSelectionModel(m_pUserActionModel->activeActionModel()), this);

   m->exec(QCursor::pos());
}

void View::updateTextBoxStatus()
{
   static bool display = ConfigurationSkeleton::displayMessageBox();
   if (display) {
      Call* call = CallModel::instance().selectedCall();
      m_pMessageBoxW->setVisible((call
         && (call->lifeCycleState() == Call::LifeCycleState::PROGRESS)) ||
         (m_pMessageTabBox->isVisible() && m_pMessageTabBox->count())
      );
   }
}

void View::displayHistory(bool in)
{
   if (!in)
      return;

   Call* call = CallModel::instance().selectedCall();

   m_pMessageBoxW->setVisible((call
      && (call->lifeCycleState() == Call::LifeCycleState::PROGRESS)) ||
      (m_pMessageTabBox->isVisible() && m_pMessageTabBox->count())
   );

   m_pMessageTabBox->clearColor();
}

#include "view.moc"
// kate: space-indent on; indent-width 3; replace-tabs on;
