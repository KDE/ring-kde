/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
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
#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <QtCore/QObject>

//Qt
class QActionGroup;

//KDE
class KAction;

//SFLPhonw
class AccountWizard;
class ExtendedAction;

///Group action declaration under the same umbrella
class ActionCollection : public QObject {
   Q_OBJECT
public:

   //Constructor
   explicit ActionCollection(QObject* parent = nullptr);
   virtual ~ActionCollection();
   static ActionCollection* instance();
   void setupAction();

   //Actions
   ExtendedAction* holdAction    ();
   ExtendedAction* recordAction  ();
   ExtendedAction* refuseAction  ();
   ExtendedAction* muteCaptureAction  ();
   ExtendedAction* mutePlaybackAction  ();
   ExtendedAction* hangupAction  ();
   ExtendedAction* unholdAction  ();
   ExtendedAction* transferAction();
   ExtendedAction* pickupAction  ();
   ExtendedAction* acceptAction  ();
   KAction*        displayVolumeControlsAction();
   KAction*        displayDialpadAction       ();
   KAction*        displayMessageBoxAction    ();
   KAction*        mailBoxAction              ();
   KAction*        showContactDockAction      ();
   KAction*        showHistoryDockAction      ();
   KAction*        showBookmarkDockAction     ();
   KAction*        quitAction                 ();
   KAction*        addContact                 ();

   //Video actions
   #ifdef ENABLE_VIDEO
   ExtendedAction* videoRotateLeftAction     ();
   ExtendedAction* videoRotateRightAction    ();
   ExtendedAction* videoFlipHorizontalAction ();
   ExtendedAction* videoFlipVerticalAction   ();
   ExtendedAction* videoMuteAction           ();
   ExtendedAction* videoPreviewAction        ();
   ExtendedAction* videoScaleAction          ();
   ExtendedAction* videoFullscreenAction     ();
   #endif

private:

   //Toolbar actions
   ExtendedAction* action_accept   ;
   ExtendedAction* action_refuse   ;
   ExtendedAction* action_hold     ;
   ExtendedAction* action_transfer ;
   ExtendedAction* action_record   ;
   ExtendedAction* action_mute_capture     ;
   ExtendedAction* action_mute_playback     ;
   ExtendedAction* action_hangup   ;
   ExtendedAction* action_unhold   ;
   ExtendedAction* action_pickup   ;

   //Video actions
   #ifdef ENABLE_VIDEO
   ExtendedAction* action_video_rotate_left    ;
   ExtendedAction* action_video_rotate_right   ;
   ExtendedAction* action_video_flip_horizontal;
   ExtendedAction* action_video_flip_vertical  ;
   ExtendedAction* action_video_mute           ;
   ExtendedAction* action_video_preview        ;
   ExtendedAction* action_video_scale          ;
   ExtendedAction* action_video_fullscreen     ;
   #endif

   //Other actions
   KAction* action_mailBox               ;
   KAction* action_close                 ;
   KAction* action_quit                  ;
   KAction* action_displayVolumeControls ;
   KAction* action_displayDialpad        ;
   KAction* action_displayMessageBox     ;
   KAction* action_configureSflPhone     ;
   KAction* action_configureShortcut     ;
   KAction* action_accountCreationWizard ;
   KAction* action_pastenumber           ;
   KAction* action_showContactDock       ;
   KAction* action_showHistoryDock       ;
   KAction* action_showBookmarkDock      ;
   KAction* action_editToolBar           ;
   KAction* action_addContact            ;
   QActionGroup* action_screen           ;

   AccountWizard*       m_pWizard        ;

   //Singleton
   static ActionCollection* m_spInstance;


private Q_SLOTS:
   void accept   ();
   void hangup   ();
   void refuse   ();
   void hold     ();
   void unhold   ();
   void transfer ();
   void record   ();
   void mailBox  ();
   void configureSflPhone     ();
   void showShortCutEditor    ();
   void accountCreationWizard ();
   void editToolBar           ();
   void addMacro              ( KAction* newAction );
   void slotAddContact();
   void updateRecordButton   ();
   void updateVolumeButton   ();

Q_SIGNALS:
   void windowStateChanged();
};

#endif
