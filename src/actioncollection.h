/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
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

class KAction;
class QActionGroup;
class AccountWizard;
class ExtendedAction;

class ActionCollection : public QObject {
   Q_OBJECT
public:

   //Constructor
   ActionCollection(QObject* parent = nullptr);
   virtual ~ActionCollection();
   static ActionCollection* instance();
   void setupAction();

   //Actions
   ExtendedAction* holdAction    () { return action_hold;     }
   ExtendedAction* recordAction  () { return action_record;   }
   ExtendedAction* refuseAction  () { return action_refuse;   }
   ExtendedAction* muteAction    () { return action_mute;     }
   ExtendedAction* hangupAction  () { return action_hangup;   }
   ExtendedAction* unholdAction  () { return action_unhold;   }
   ExtendedAction* transferAction() { return action_transfer; }
   ExtendedAction* pickupAction  () { return action_pickup;   }
   ExtendedAction* acceptAction  () { return action_accept;   }
   KAction*        displayVolumeControlsAction() { return action_displayVolumeControls; }
   KAction*        displayDialpadAction       () { return action_displayDialpad;        }
   KAction*        displayMessageBoxAction    () { return action_displayMessageBox;     }
   KAction*        mailBoxAction              () { return action_mailBox;               }
   KAction*        showContactDockAction      () { return action_showContactDock;       }
   KAction*        showHistoryDockAction      () { return action_showHistoryDock;       }
   KAction*        showBookmarkDockAction     () { return action_showBookmarkDock;      }
   KAction*        quitAction                 () { return action_quit;                  }

   //Video actions
   #ifdef ENABLE_VIDEO
   ExtendedAction* videoRotateLeftAction     () { return action_video_rotate_left    ;}
   ExtendedAction* videoRotateRightAction    () { return action_video_rotate_right   ;}
   ExtendedAction* videoFlipHorizontalAction () { return action_video_flip_horizontal;}
   ExtendedAction* videoFlipVerticalAction   () { return action_video_flip_vertical  ;}
   ExtendedAction* videoMuteAction           () { return action_video_mute           ;}
   ExtendedAction* videoPreviewAction        () { return action_video_preview        ;}
   #endif

private:

   //Attributes
   ExtendedAction* action_accept         ;
   ExtendedAction* action_refuse         ;
   ExtendedAction* action_hold           ;
   ExtendedAction* action_transfer       ;
   ExtendedAction* action_record         ;
   ExtendedAction* action_mute           ;
   ExtendedAction* action_hangup         ;
   ExtendedAction* action_unhold         ;
   ExtendedAction* action_pickup         ;
   //Video actions
   #ifdef ENABLE_VIDEO
   ExtendedAction* action_video_rotate_left    ;
   ExtendedAction* action_video_rotate_right   ;
   ExtendedAction* action_video_flip_horizontal;
   ExtendedAction* action_video_flip_vertical  ;
   ExtendedAction* action_video_mute           ;
   ExtendedAction* action_video_preview        ;
   #endif
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
   void showShortCutEditor                       (                                      );
   void accountCreationWizard ();
   void editToolBar                              (                                      );
   void addMacro                                 ( KAction* newAction                   );

Q_SIGNALS:
   void windowStateChanged();
};

#endif