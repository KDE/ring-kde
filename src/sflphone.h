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

#ifndef SFLPHONE_H
#define SFLPHONE_H

#include <KXmlGuiWindow>

//Qt
class QString;
class QLabel;
class QToolButton;

//KDE
class KComboBox;

//SFLPhone
class Call;
class ContactDock;
class BookmarkDock;
class VideoDock;
class SFLPhoneTray;
class SFLPhoneView;
class HistoryDock;
class VideoRenderer;
class AccountListNoCheckProxyModel;
class Account;


/**
 * This class represents the SFLphone main window
 * It implements the methods relative to windowing
 * (status, menus, toolbars, notifications...).
 * It uses a view which implements the real functionning
 * and features of the phone.
 * The display of the window is according to the state of the view,
 * so the view sends some signals to ask for changes on the window
 * that the window has to take into account.
 *
 * @short Main window
 * @author Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>
 * @author Emmanuel Lepage <emmanuel.lepage@savoirfairelinux.com>
 * @version 1.4.1
**/
class SFLPhone : public KXmlGuiWindow
{
Q_OBJECT

public:
enum CallAction {
        Accept            ,
        Refuse            ,
        Hold              ,
        Transfer          ,
        Record            ,
        Mailbox           ,
        NumberOfCallActions
};

private:
   //Attributes
   bool           m_pInitialized     ;
   SFLPhoneView*  m_pView            ;
   bool           m_pIconChanged     ;
   SFLPhoneTray*  m_pTrayIcon        ;
   QLabel*        m_pStatusBarWidget ;
   ContactDock*   m_pContactCD       ;
   QDockWidget*   m_pCentralDW       ;
   HistoryDock*   m_pHistoryDW       ;
   BookmarkDock*  m_pBookmarkDW      ;
   KComboBox*     m_pAccountStatus   ;
   #ifdef ENABLE_VIDEO
   VideoDock*     m_pVideoDW         ;
   #endif
   QToolButton*   m_pPresent         ;
   QDockWidget*   m_pPresenceDock    ;

   static SFLPhone*            m_sApp;
   AccountListNoCheckProxyModel* m_pAccountModel;

   //Setters
   void setObjectNames();

protected:
   virtual bool  queryClose (                )      ;
   virtual QSize sizeHint   (                ) const;


public:
   explicit SFLPhone(QWidget *parent = 0);
   ~SFLPhone                       ();
   bool             initialize     ();

   friend class SFLPhoneView;

   static SFLPhone*            app   ();
   static SFLPhoneView*        view  ();

   ContactDock*  contactDock ();
   HistoryDock*  historyDock ();
   BookmarkDock* bookmarkDock();

private Q_SLOTS:
   void on_m_pView_windowTitleChangeAsked        ( const QString& message               );
   void on_m_pView_enabledActionsChangeAsked     ( const bool*    enabledActions        );
   void on_m_pView_actionIconsChangeAsked        ( const QString* actionIcons           );
   void on_m_pView_actionTextsChangeAsked        ( const QString* actionTexts           );
   void on_m_pView_transferCheckStateChangeAsked ( bool  transferCheckState             );
   void on_m_pView_recordCheckStateChangeAsked   ( bool  recordCheckState               );
   void on_m_pView_incomingCall                  ( const Call*    call                  );
   void currentAccountIndexChanged               ( int newIndex                         );
   void currentPriorAccountChanged               ( Account* newPrior                    );
   void quitButton                               (                                      );
   void updateTabIcons                           (                                      );
   void updatePresence                           ( const QString& status                );
   void hidePresenceDock                         (                                      );
   void slotPresenceEnabled                      ( bool state                           );
   #ifdef ENABLE_VIDEO
   void displayVideoDock                         ( VideoRenderer* r                     );
   #endif

public Q_SLOTS:
   void timeout                                  (                                      );
   void selectCallTab                            (                                      );
};

#endif
