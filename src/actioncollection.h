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
#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <QtCore/QObject>

//Qt
class QActionGroup;

//KDE
class QAction;

// Ring
class KXmlGuiWindow;
class PhoneWindow;
class KActionCollection;

///Group action declaration under the same umbrella
class ActionCollection : public QObject {
   Q_OBJECT
public:

   //Constructor
   explicit ActionCollection(QObject* parent = nullptr);
   virtual ~ActionCollection();
   static ActionCollection* instance();
   void setupAction();
   void setupPhoneAction(PhoneWindow* mw);

   Q_PROPERTY(QAction* holdAction                  READ holdAction                  CONSTANT)
   Q_PROPERTY(QAction* recordAction                READ recordAction                CONSTANT)
   Q_PROPERTY(QAction* muteCaptureAction           READ muteCaptureAction           CONSTANT)
   Q_PROPERTY(QAction* mutePlaybackAction          READ mutePlaybackAction          CONSTANT)
   Q_PROPERTY(QAction* hangupAction                READ hangupAction                CONSTANT)
   Q_PROPERTY(QAction* transferAction              READ transferAction              CONSTANT)
   Q_PROPERTY(QAction* acceptAction                READ acceptAction                CONSTANT)
   Q_PROPERTY(QAction* newCallAction               READ newCallAction               CONSTANT)
   Q_PROPERTY(QAction* displayVolumeControlsAction READ displayVolumeControlsAction CONSTANT)
   Q_PROPERTY(QAction* displayDialpadAction        READ displayDialpadAction        CONSTANT)
   Q_PROPERTY(QAction* displayAccountCbbAction     READ displayAccountCbbAction     CONSTANT)
   Q_PROPERTY(QAction* showContactDockAction       READ showContactDockAction       CONSTANT)
   Q_PROPERTY(QAction* showHistoryDockAction       READ showHistoryDockAction       CONSTANT)
   Q_PROPERTY(QAction* showTimelineDockAction      READ showTimelineDockAction      CONSTANT)
   Q_PROPERTY(QAction* showDialDockAction          READ showDialDockAction          CONSTANT)
   Q_PROPERTY(QAction* showBookmarkDockAction      READ showBookmarkDockAction      CONSTANT)
   Q_PROPERTY(QAction* quitAction                  READ quitAction                  CONSTANT)
   Q_PROPERTY(QAction* addPerson                   READ addPerson                   CONSTANT)
   Q_PROPERTY(QAction* focusHistory                READ focusHistory                CONSTANT)
   Q_PROPERTY(QAction* focusContact                READ focusContact                CONSTANT)
   Q_PROPERTY(QAction* focusCall                   READ focusCall                   CONSTANT)
   Q_PROPERTY(QAction* focusBookmark               READ focusBookmark               CONSTANT)
   Q_PROPERTY(QAction* showWizard                  READ showWizard                  CONSTANT)
   Q_PROPERTY(QAction* showMenu                    READ showMenu                    CONSTANT)
   Q_PROPERTY(QAction* newContact                  READ newContact                  CONSTANT)
   Q_PROPERTY(QAction* configureRing               READ configureRing               CONSTANT)
   Q_PROPERTY(QAction* configureShortcut           READ configureShortcut           CONSTANT)
   Q_PROPERTY(QAction* configureNotification       READ configureNotification       CONSTANT)
   #ifdef ENABLE_VIDEO
   Q_PROPERTY(QAction* videoRotateLeftAction     READ videoRotateLeftAction     CONSTANT)
   Q_PROPERTY(QAction* videoRotateRightAction    READ videoRotateRightAction    CONSTANT)
   Q_PROPERTY(QAction* videoFlipHorizontalAction READ videoFlipHorizontalAction CONSTANT)
   Q_PROPERTY(QAction* videoFlipVerticalAction   READ videoFlipVerticalAction   CONSTANT)
   Q_PROPERTY(QAction* videoMuteAction           READ videoMuteAction           CONSTANT)
   Q_PROPERTY(QAction* videoPreviewAction        READ videoPreviewAction        CONSTANT)
   Q_PROPERTY(QAction* vimw2deoScaleAction          READ videoScaleAction          CONSTANT)
   Q_PROPERTY(QAction* videoFullscreenAction     READ videoFullscreenAction     CONSTANT)
   #endif

   Q_PROPERTY(QObject* fakeMainWindow READ fakeMainWindow CONSTANT)
   Q_PROPERTY(QObject* kactionCollection READ kactionCollection CONSTANT)

   //Actions
   QAction* holdAction                  ();
   QAction* recordAction                ();
   QAction* muteCaptureAction           ();
   QAction* mutePlaybackAction          ();
   QAction* hangupAction                ();
   QAction* transferAction              ();
   QAction* acceptAction                ();
   QAction* newCallAction               ();
   QAction* displayVolumeControlsAction ();
   QAction* displayDialpadAction        ();
   QAction* displayAccountCbbAction     ();
   QAction* showContactDockAction       ();
   QAction* showHistoryDockAction       ();
   QAction* showBookmarkDockAction      ();
   QAction* showTimelineDockAction      ();
   QAction* showDialDockAction          ();
   QAction* quitAction                  ();
   QAction* addPerson                   ();
   QAction* focusHistory                ();
   QAction* focusContact                ();
   QAction* focusCall                   ();
   QAction* focusBookmark               ();
   QAction* showWizard                  ();
   QAction* showMenu                    ();
   QAction* newContact                  ();
   QAction* configureRing               ();
   QAction* configureShortcut           ();
   QAction* configureNotification       ();

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

   //Selection dependant
   QAction* action_accept             {nullptr};
   QAction* action_new_call           {nullptr};
   QAction* action_hold               {nullptr};
   QAction* action_transfer           {nullptr};
   QAction* action_record             {nullptr};
   QAction* action_mute_capture       {nullptr};
   QAction* action_mute_playback      {nullptr};
   QAction* action_hangup             {nullptr};
   QAction* action_mute_video         {nullptr};
   QAction* action_join               {nullptr};
   QAction* action_toggle_video       {nullptr};
   QAction* action_add_contact        {nullptr};
   QAction* action_add_to_contact     {nullptr};
   QAction* action_delete_contact     {nullptr};
   QAction* action_email_contact      {nullptr};
   QAction* action_copy_contact       {nullptr};
   QAction* action_bookmark           {nullptr};
   QAction* action_view_chat_history  {nullptr};
   QAction* action_add_contact_method {nullptr};
   QAction* action_call_contact       {nullptr};
   QAction* action_edit_contact       {nullptr};
   QAction* action_remove_history     {nullptr};

   //Video actions
   #ifdef ENABLE_VIDEO
   QAction* action_video_rotate_left     {nullptr};
   QAction* action_video_rotate_right    {nullptr};
   QAction* action_video_flip_horizontal {nullptr};
   QAction* action_video_flip_vertical   {nullptr};
   QAction* action_video_mute            {nullptr};
   QAction* action_video_preview         {nullptr};
   QAction* action_video_scale           {nullptr};
   QAction* action_video_fullscreen      {nullptr};
   #endif

   //Other actions
   QAction * action_new_contact            {nullptr};
   QAction * action_close_phone            {nullptr};
   QAction * action_close_timeline         {nullptr};
   QAction * action_quit                   {nullptr};
   QAction * action_displayVolumeControls  {nullptr};
   QAction * action_displayDialpad         {nullptr};
   QAction * action_displayAccountCbb      {nullptr};
   QAction * action_configureRing          {nullptr};
   QAction * action_configureShortcut      {nullptr};
   QAction * action_configureNotifications {nullptr};
   QAction * action_pastenumber            {nullptr};
   QAction * action_showContactDock        {nullptr};
   QAction * action_showHistoryDock        {nullptr};
   QAction * action_showBookmarkDock       {nullptr};
   QAction * action_showTimelineDock       {nullptr};
   QAction * action_showDialDock           {nullptr};
   QAction * action_editToolBar            {nullptr};
   QAction * action_addPerson              {nullptr};
   QAction * action_raise_client           {nullptr};
   QAction * action_focus_history          {nullptr};
   QAction * action_focus_contact          {nullptr};
   QAction * action_focus_call             {nullptr};
   QAction * action_focus_bookmark         {nullptr};
   QAction * action_show_wizard            {nullptr};
   QAction * action_show_directory         {nullptr};
   QAction * action_show_menu              {nullptr};

   QObject* fakeMainWindow() const;
   QObject* kactionCollection() const;

public Q_SLOTS:
   void slotConfigureRing         ();
   void slotRaiseClient           ( bool focus = false );

private Q_SLOTS:
   void showShortCutEditor    ();
   void showNotificationEditor();
   void showDirectory         ();
   void editToolBar           ();
   void slotAddPerson         ();
   void updateRecordButton    ();
   void updateVolumeButton    ();
   void slotNewContact        ();
   void slotShowMenubar       (bool s);

Q_SIGNALS:
   void windowStateChanged();
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
