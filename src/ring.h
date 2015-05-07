/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
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
#ifndef RING_H
#define RING_H

#include <KXmlGuiWindow>

//Qt
class QString;
class QLabel;
class QToolButton;

//KDE
class QComboBox;

//Ring
class Call;
class ContactDock;
class BookmarkDock;
class VideoDock;
class HistoryDock;
class AvailableAccountModel;
class Account;
class View;
class AutoComboBox;
class SysTray;
namespace Video {
   class Renderer;
}

/**
 * This class represents the Ring main window
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
class Ring : public KXmlGuiWindow
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
   View*          m_pView            ;
   bool           m_pIconChanged     ;
   SysTray*       m_pTrayIcon        ;
   QLabel*        m_pStatusBarWidget ;
   ContactDock*   m_pContactCD       ;
   QDockWidget*   m_pCentralDW       ;
   HistoryDock*   m_pHistoryDW       ;
   BookmarkDock*  m_pBookmarkDW      ;
   AutoComboBox*  m_pAccountStatus   ;
   #ifdef ENABLE_VIDEO
   VideoDock*     m_pVideoDW         ;
   #endif
   QToolButton*   m_pPresent         ;
   QDockWidget*   m_pPresenceDock    ;
   QLabel*        m_pCurAccL         ;

   static Ring*            m_sApp;
   AvailableAccountModel* m_pAccountModel;

   //Setters
   void setObjectNames();

protected:
   virtual bool  queryClose (                )       override;
   virtual QSize sizeHint   (                ) const override;


public:
   explicit Ring(QWidget* parent = nullptr);
   ~Ring();
   bool initialize();

   friend class View;

   static Ring* app  ();
   static View* view ();

   ContactDock*  contactDock ();
   HistoryDock*  historyDock ();
   BookmarkDock* bookmarkDock();

private Q_SLOTS:
   void on_m_pView_windowTitleChangeAsked        ( const QString& message        );
   void on_m_pView_transferCheckStateChangeAsked ( bool  transferCheckState      );
   void on_m_pView_recordCheckStateChangeAsked   ( bool  recordCheckState        );
   void on_m_pView_incomingCall                  ( const Call*    call           );
   void currentPriorAccountChanged               ( Account* newPrior             );
   void quitButton                               (                               );
   void displayAccountCbb                        ( bool checked = true           );
   void updateTabIcons                           (                               );
   void updatePresence                           ( const QString& status         );
   void hidePresenceDock                         (                               );
   void slotPresenceEnabled                      ( bool state                    );
   #ifdef ENABLE_VIDEO
   void displayVideoDock                         ( Call* c, Video::Renderer* r   );
   void hideVideoDock                            ( Call* c, Video::Renderer* r   );
   #endif

public Q_SLOTS:
   void timeout                                  (                               );
   void selectCallTab                            (                               );
};

#endif
