/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
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

#include <QObject>

//Qt
class QActionGroup;

//KDE
class QAction;

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
   QAction* holdAction                  ();
   QAction* recordAction                ();
   QAction* muteCaptureAction           ();
   QAction* mutePlaybackAction          ();
   QAction* hangupAction                ();
   QAction* transferAction              ();
   QAction* acceptAction                ();
   QAction* newCallAction               ();
   QAction *        displayVolumeControlsAction();
   QAction *        displayDialpadAction       ();
   QAction *        displayAccountCbbAction    ();
   QAction *        displayMessageBoxAction    ();
   QAction *        mailBoxAction              ();
   QAction *        showContactDockAction      ();
   QAction *        showHistoryDockAction      ();
   QAction *        showBookmarkDockAction     ();
   QAction *        quitAction                 ();
   QAction *        addPerson                  ();

   //Video actions
   #ifdef ENABLE_VIDEO
   QAction* videoRotateLeftAction     ();
   QAction* videoRotateRightAction    ();
   QAction* videoFlipHorizontalAction ();
   QAction* videoFlipVerticalAction   ();
   QAction* videoMuteAction           ();
   QAction* videoPreviewAction        ();
   QAction* videoScaleAction          ();
   QAction* videoFullscreenAction     ();
   #endif

private:

   //Toolbar actions
   QAction* action_accept        ;
   QAction* action_new_call      ;
   QAction* action_hold          ;
   QAction* action_transfer      ;
   QAction* action_record        ;
   QAction* action_mute_capture  ;
   QAction* action_mute_playback ;
   QAction* action_hangup        ;

   //Video actions
   #ifdef ENABLE_VIDEO
   QAction* action_video_rotate_left    ;
   QAction* action_video_rotate_right   ;
   QAction* action_video_flip_horizontal;
   QAction* action_video_flip_vertical  ;
   QAction* action_video_mute           ;
   QAction* action_video_preview        ;
   QAction* action_video_scale          ;
   QAction* action_video_fullscreen     ;
   #endif

   //Other actions
   QAction * action_mailBox               ;
   QAction * action_close                 ;
   QAction * action_quit                  ;
   QAction * action_displayVolumeControls ;
   QAction * action_displayDialpad        ;
   QAction * action_displayAccountCbb     ;
   QAction * action_displayMessageBox     ;
   QAction * action_configureRing         ;
   QAction * action_configureShortcut     ;
   QAction * action_configureNotifications;
   QAction * action_pastenumber           ;
   QAction * action_showContactDock       ;
   QAction * action_showHistoryDock       ;
   QAction * action_showBookmarkDock      ;
   QAction * action_editToolBar           ;
   QAction * action_addPerson             ;
   QActionGroup* action_screen            ;

   //Singleton
   static ActionCollection* m_spInstance;

public Q_SLOTS:
   void configureRing         ();

private Q_SLOTS:
   void mailBox  ();
   void showShortCutEditor    ();
   void showNotificationEditor();
   void editToolBar           ();
   void slotAddPerson         ();
   void updateRecordButton    ();
   void updateVolumeButton    ();
   void addMacro              ( const QVariant& newAction );

Q_SIGNALS:
   void windowStateChanged();
};

#endif
