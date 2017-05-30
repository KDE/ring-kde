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
class PhoneWindow;

///Group action declaration under the same umbrella
class ActionCollection : public QObject {
   Q_OBJECT
public:

   //Constructor
   explicit ActionCollection(PhoneWindow* parent = nullptr);
   virtual ~ActionCollection();
   static ActionCollection* instance();
   void setupAction(PhoneWindow* mw);

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
   QAction* mailBoxAction               ();
   QAction* showContactDockAction       ();
   QAction* showHistoryDockAction       ();
   QAction* showBookmarkDockAction      ();
   QAction* quitAction                  ();
   QAction* addPerson                   ();
   QAction* focusHistory                ();
   QAction* focusContact                ();
   QAction* focusCall                   ();
   QAction* focusBookmark               ();
   QAction* showWizard                  ();
   QAction* newContact                  ();

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
   QAction * action_mailBox                {nullptr};
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
   QAction * action_editToolBar            {nullptr};
   QAction * action_addPerson              {nullptr};
   QAction * action_raise_client           {nullptr};
   QAction * action_focus_history          {nullptr};
   QAction * action_focus_contact          {nullptr};
   QAction * action_focus_call             {nullptr};
   QAction * action_focus_bookmark         {nullptr};
   QAction * action_show_wizard            {nullptr};

public Q_SLOTS:
   void configureRing         ();
   void raiseClient           ( bool focus = false );

private Q_SLOTS:
   void mailBox  ();
   void showShortCutEditor    ();
   void showNotificationEditor();
   void editToolBar           ();
   void slotAddPerson         ();
   void updateRecordButton    ();
   void updateVolumeButton    ();
   void slotNewContact        ();
   void addMacro              ( const QVariant& newAction );

Q_SIGNALS:
   void windowStateChanged();
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
