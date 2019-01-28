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
class QAction;

// Ring
class QmlAction;
class KXmlGuiWindow;
class PhoneWindow;
class KActionCollection;

///Group action declaration under the same umbrella
class Q_DECL_EXPORT ActionCollection : public QObject
{
   Q_OBJECT
public:

   //Constructor
   explicit ActionCollection(QObject* parent = nullptr);
   virtual ~ActionCollection();
   static ActionCollection* instance();
   void setupAction();

   Q_PROPERTY(QmlAction* holdAction                  READ holdAction                  CONSTANT)
   Q_PROPERTY(QmlAction* recordAction                READ recordAction                CONSTANT)
   Q_PROPERTY(QmlAction* muteCaptureAction           READ muteCaptureAction           CONSTANT)
   Q_PROPERTY(QmlAction* mutePlaybackAction          READ mutePlaybackAction          CONSTANT)
   Q_PROPERTY(QmlAction* hangupAction                READ hangupAction                CONSTANT)
   Q_PROPERTY(QmlAction* transferAction              READ transferAction              CONSTANT)
   Q_PROPERTY(QmlAction* acceptAction                READ acceptAction                CONSTANT)
   Q_PROPERTY(QmlAction* newCallAction               READ newCallAction               CONSTANT)
   Q_PROPERTY(QmlAction* displayVolumeControlsAction READ displayVolumeControlsAction CONSTANT)
   Q_PROPERTY(QmlAction* displayDialpadAction        READ displayDialpadAction        CONSTANT)
   Q_PROPERTY(QmlAction* displayAccountCbbAction     READ displayAccountCbbAction     CONSTANT)
   Q_PROPERTY(QmlAction* showContactDockAction       READ showContactDockAction       CONSTANT)
   Q_PROPERTY(QmlAction* showHistoryDockAction       READ showHistoryDockAction       CONSTANT)
   Q_PROPERTY(QmlAction* showTimelineDockAction      READ showTimelineDockAction      CONSTANT)
   Q_PROPERTY(QmlAction* showDialDockAction          READ showDialDockAction          CONSTANT)
   Q_PROPERTY(QmlAction* showBookmarkDockAction      READ showBookmarkDockAction      CONSTANT)
   Q_PROPERTY(QmlAction* quitAction                  READ quitAction                  CONSTANT)
   Q_PROPERTY(QmlAction* closeAction                 READ closeAction                 CONSTANT)
   Q_PROPERTY(QmlAction* addPerson                   READ addPerson                   CONSTANT)
   Q_PROPERTY(QmlAction* focusHistory                READ focusHistory                CONSTANT)
   Q_PROPERTY(QmlAction* focusContact                READ focusContact                CONSTANT)
   Q_PROPERTY(QmlAction* focusCall                   READ focusCall                   CONSTANT)
   Q_PROPERTY(QmlAction* focusBookmark               READ focusBookmark               CONSTANT)
   Q_PROPERTY(QmlAction* showWizard                  READ showWizard                  CONSTANT)
   Q_PROPERTY(QmlAction* showMenu                    READ showMenu                    CONSTANT)
   Q_PROPERTY(QmlAction* newContact                  READ newContact                  CONSTANT)
   Q_PROPERTY(QmlAction* configureShortcut           READ configureShortcut           CONSTANT)
   Q_PROPERTY(QmlAction* configureNotification       READ configureNotification       CONSTANT)
   Q_PROPERTY(QmlAction* configureAccount            READ configureAccount            CONSTANT)
   Q_PROPERTY(QmlAction* videoRotateLeftAction     READ videoRotateLeftAction     CONSTANT)
   Q_PROPERTY(QmlAction* videoRotateRightAction    READ videoRotateRightAction    CONSTANT)
   Q_PROPERTY(QmlAction* videoFlipHorizontalAction READ videoFlipHorizontalAction CONSTANT)
   Q_PROPERTY(QmlAction* videoFlipVerticalAction   READ videoFlipVerticalAction   CONSTANT)
   Q_PROPERTY(QmlAction* videoMuteAction           READ videoMuteAction           CONSTANT)
   Q_PROPERTY(QmlAction* videoPreviewAction        READ videoPreviewAction        CONSTANT)
   Q_PROPERTY(QmlAction* vimw2deoScaleAction          READ videoScaleAction          CONSTANT)
   Q_PROPERTY(QmlAction* videoFullscreenAction     READ videoFullscreenAction     CONSTANT)

   Q_PROPERTY(QObject* fakeMainWindow READ fakeMainWindow CONSTANT)
   Q_PROPERTY(QObject* kactionCollection READ kactionCollection CONSTANT)

   //Actions
   QmlAction* holdAction                  ();
   QmlAction* recordAction                ();
   QmlAction* muteCaptureAction           ();
   QmlAction* mutePlaybackAction          ();
   QmlAction* hangupAction                ();
   QmlAction* transferAction              ();
   QmlAction* acceptAction                ();
   QmlAction* newCallAction               ();
   QmlAction* displayVolumeControlsAction ();
   QmlAction* displayDialpadAction        ();
   QmlAction* displayAccountCbbAction     ();
   QmlAction* showContactDockAction       ();
   QmlAction* showHistoryDockAction       ();
   QmlAction* showBookmarkDockAction      ();
   QmlAction* showTimelineDockAction      ();
   QmlAction* showDialDockAction          ();
   QmlAction* quitAction                  ();
   QmlAction* closeAction                 ();
   QmlAction* addPerson                   ();
   QmlAction* focusHistory                ();
   QmlAction* focusContact                ();
   QmlAction* focusCall                   ();
   QmlAction* focusBookmark               ();
   QmlAction* showWizard                  ();
   QmlAction* showMenu                    ();
   QmlAction* newContact                  ();
   QmlAction* configureShortcut           ();
   QmlAction* configureNotification       ();
   QmlAction* configureAccount            ();

   //Video actions
   QmlAction* videoRotateLeftAction     ();
   QmlAction* videoRotateRightAction    ();
   QmlAction* videoFlipHorizontalAction ();
   QmlAction* videoFlipVerticalAction   ();
   QmlAction* videoMuteAction           ();
   QmlAction* videoPreviewAction        ();
   QmlAction* videoScaleAction          ();
   QmlAction* videoFullscreenAction     ();

private:

   //Selection dependant
   QmlAction* action_accept             {nullptr};
   QmlAction* action_new_call           {nullptr};
   QmlAction* action_hold               {nullptr};
   QmlAction* action_transfer           {nullptr};
   QmlAction* action_record             {nullptr};
   QmlAction* action_mute_capture       {nullptr};
   QmlAction* action_mute_playback      {nullptr};
   QmlAction* action_hangup             {nullptr};
   QmlAction* action_mute_video         {nullptr};
   QmlAction* action_join               {nullptr};
   QmlAction* action_toggle_video       {nullptr};
   QmlAction* action_add_contact        {nullptr};
   QmlAction* action_add_to_contact     {nullptr};
   QmlAction* action_delete_contact     {nullptr};
   QmlAction* action_email_contact      {nullptr};
   QmlAction* action_copy_contact       {nullptr};
   QmlAction* action_bookmark           {nullptr};
   QmlAction* action_view_chat_history  {nullptr};
   QmlAction* action_add_contact_method {nullptr};
   QmlAction* action_call_contact       {nullptr};
   QmlAction* action_edit_contact       {nullptr};
   QmlAction* action_remove_history     {nullptr};

   //Video actions
   QmlAction* action_video_rotate_left     {nullptr};
   QmlAction* action_video_rotate_right    {nullptr};
   QmlAction* action_video_flip_horizontal {nullptr};
   QmlAction* action_video_flip_vertical   {nullptr};
   QmlAction* action_video_mute            {nullptr};
   QmlAction* action_video_preview         {nullptr};
   QmlAction* action_video_scale           {nullptr};
   QmlAction* action_video_fullscreen      {nullptr};

   //Other actions
   QmlAction* action_new_contact            {nullptr};
   QmlAction* action_quit                   {nullptr};
   QmlAction* action_close                  {nullptr};
   QmlAction* action_displayVolumeControls  {nullptr};
   QmlAction* action_displayDialpad         {nullptr};
   QmlAction* action_displayAccountCbb      {nullptr};
   QmlAction* action_configureShortcut      {nullptr};
   QmlAction* action_configureNotifications {nullptr};
   QmlAction* action_configureAccount       {nullptr};
   QmlAction* action_pastenumber            {nullptr};
   QmlAction* action_showContactDock        {nullptr};
   QmlAction* action_showHistoryDock        {nullptr};
   QmlAction* action_showBookmarkDock       {nullptr};
   QmlAction* action_showTimelineDock       {nullptr};
   QmlAction* action_showDialDock           {nullptr};
   QmlAction* action_addPerson              {nullptr};
   QmlAction* action_raise_client           {nullptr};
   QmlAction* action_focus_history          {nullptr};
   QmlAction* action_focus_contact          {nullptr};
   QmlAction* action_focus_call             {nullptr};
   QmlAction* action_focus_bookmark         {nullptr};
   QmlAction* action_show_wizard            {nullptr};
   QmlAction* action_show_directory         {nullptr};
   QmlAction* action_show_menu              {nullptr};

   QObject* fakeMainWindow() const;
   QObject* kactionCollection() const;

public Q_SLOTS:
   void slotRaiseClient           ( bool focus = false );

private Q_SLOTS:
   void showShortCutEditor    ();
   void showNotificationEditor();
   void slotAddPerson         ();
   void updateRecordButton    ();
   void updateVolumeButton    ();
   void slotNewContact        ();
   void slotClose             ();
   void slotShowMenubar       (bool s);

Q_SIGNALS:
   void windowStateChanged();
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
