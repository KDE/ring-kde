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

//Ring
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
   ExtendedAction* holdAction                  ();
   ExtendedAction* recordAction                ();
   ExtendedAction* muteCaptureAction           ();
   ExtendedAction* mutePlaybackAction          ();
   ExtendedAction* hangupAction                ();
   ExtendedAction* transferAction              ();
   ExtendedAction* acceptAction                ();
   QAction *        displayVolumeControlsAction();
   QAction *        displayDialpadAction       ();
   QAction *        displayMessageBoxAction    ();
   QAction *        mailBoxAction              ();
   QAction *        showContactDockAction      ();
   QAction *        showHistoryDockAction      ();
   QAction *        showBookmarkDockAction     ();
   QAction *        quitAction                 ();
   QAction *        addPerson                  ();

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
   ExtendedAction* action_accept        ;
   ExtendedAction* action_hold          ;
   ExtendedAction* action_transfer      ;
   ExtendedAction* action_record        ;
   ExtendedAction* action_mute_capture  ;
   ExtendedAction* action_mute_playback ;
   ExtendedAction* action_hangup        ;

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
   QAction * action_mailBox               ;
   QAction * action_close                 ;
   QAction * action_quit                  ;
   QAction * action_displayVolumeControls ;
   QAction * action_displayDialpad        ;
   QAction * action_displayMessageBox     ;
   QAction * action_configureRing         ;
   QAction * action_configureShortcut     ;
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
   void editToolBar           ();
   void slotAddPerson         ();
   void updateRecordButton    ();
   void updateVolumeButton    ();
   void addMacro              ( QAction * newAction );

Q_SIGNALS:
   void windowStateChanged();
};

#endif
