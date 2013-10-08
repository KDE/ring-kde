/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
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
#ifndef SFLPHONEVIEW_H
#define SFLPHONEVIEW_H

#include "ui_SFLPhoneView_base.h"
#include "klib/macromodel.h"
#include "lib/call.h"
#include "lib/callmodel.h"
#include <QtGui/QWidget>

//Qt
class QString;
class QKeyEvent;

//SFLPhone
class Contact;
class CallViewToolbar;
class CallViewOverlay;
class HistoryDelegate;
class ConferenceDelegate;
class AutoCompletion;
class CanvasObjectManager;
class EventManager;

/**
 * This is the main view class for sflphone-client-kde.  Most of the non-menu,
 * non-toolbar, and non-statusbar (e.g., non frame) GUI code should go
 * here.
 * As the state of the view has effects on the window,
 * it emits some signals to ask for changes that the window has
 * to treat.
 *
 * @short Main view
 * @author Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>
 * @version 1.2.3
 */
class SFLPhoneView : public QWidget, public Ui::SFLPhone_view, public MacroListener
{
   Q_OBJECT
   friend class EventManager;

private:
   CallViewToolbar*     m_pCanvasToolbar  ;
   CallViewOverlay*     m_pTransferOverlay;
   ConferenceDelegate*  m_pConfDelegate   ;
   HistoryDelegate*     m_pHistoryDelegate;
   AutoCompletion*      m_pAutoCompletion ;
   CanvasObjectManager* m_pCanvasManager  ;
   EventManager*        m_pEventManager   ;

public:
   //Constructors & Destructors
   /**
    *   This constructor does not load the window as it would
    *   better wait for the parent window to connect to the signals
    *   for updating it (statusMessageChangeAsked...).
    *   You should call the loadWindow() method once
    *   you have constructed the object and connected the
    *   expected signals.
    * @param parent
    */
   explicit SFLPhoneView(QWidget *parent);
   virtual ~SFLPhoneView();



   bool selectCallPhoneNumber(Call** call,Contact* contact);

   //Implement macro key listener
   virtual void addDTMF(const QString& sequence);

   //Getters
   Call* currentCall() const {
      return CallModel::instance()->getCall(m_pView->selectionModel()->currentIndex());
   }

   //Setters
   void setCurrentIndex(const QModelIndex& idx) {
      m_pView->selectionModel()->setCurrentIndex(idx,QItemSelectionModel::SelectCurrent);
   }

private Q_SLOTS:
   /**
    *   Performs the action action on the call call, then updates window.
    *   The call object will handle the action with its "actionPerformed" method.
    *   See the documentation for more details.
    * @param call the call on which to perform the action
    * @param action the code of the action to perform
    */
   void action(Call * call, Call::Action action);

   /**
    * Updates the history's search bar's display according to the current
    * text searched.
    * If empty, hide the search bar.
    */
   void updateRecordButton   ();
   void updateVolumeButton   ();
   void updateRecordBar      (double _value = -1);
   void updateVolumeBar      (double _value = -1);
   void updateVolumeControls ();
   void updateDialpad        ();
   void sendMessage          ();



public Q_SLOTS:
   /**
    * Updates all the display
    * according to the settings.
    */
   void loadWindow();

   /**
    *   Updates the toolbar's actions' display according to the selected
    *   item's state.
    */
   void updateWindowCallState();


   void updateStatusMessage();


//    virtual void keyPressEvent(QKeyEvent *event);

   void displayVolumeControls ( bool checked = true );
   void displayDialpad        ( bool checked = true );
   void displayMessageBox     ( bool checked = true );

   void on_widget_dialpad_typed(QString text);

   void on_slider_recVol_valueChanged ( int value    );
   void on_slider_sndVol_valueChanged ( int value    );
   void on_toolButton_recVol_clicked  ( bool checked );
   void on_toolButton_sndVol_clicked  ( bool checked );

   void on1_incomingCall   ( Call* call                          );
   void on1_voiceMailNotify( const QString &accountID, int count );
   void on1_volumeChanged  ( const QString &device, double value );

   void mute     ( bool value = true);
   void paste();

Q_SIGNALS:
   ///The status need to be updated
   void statusMessageChangeAsked      ( const QString&  message            );
   ///The window title need to be updated
   void windowTitleChangeAsked        ( const QString&  title              );
   ///The toolbar need to be updated
   void enabledActionsChangeAsked     ( const bool*     enabledActions     );
   ///Toolbar/actions icons need to be changed
   void actionIconsChangeAsked        ( const QString*  actionIcons        );
   ///Action string need to be changed
   void actionTextsChangeAsked        ( const QString*  actionTexts        );
   ///Transfer state has changed
   void transferCheckStateChangeAsked ( bool            transferCheckState );
   ///Record state has changed
   void recordCheckStateChangeAsked   ( bool            recordCheckState   );
   ///When a new call is coming
   void incomingCall(const Call * call);
};

#endif // SFLPHONEVIEW_H
