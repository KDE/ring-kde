/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#ifndef CALL_VIEW_OVERLAY
#define CALL_VIEW_OVERLAY

#include <QtGui/QWidget>
#include "ui_transfer.h"

class Call;

///CallViewOverlay: Display overlay on top of the call tree
class CallViewOverlay : public QWidget, public Ui_Transfer {
   Q_OBJECT

public:
   //Constructor
   explicit CallViewOverlay(QWidget* parent);
   virtual ~CallViewOverlay();

   //Getters
   Call* currentCall();

   //Setters
   void setCornerWidget    ( QWidget* wdg     );
   virtual void setVisible ( bool     enabled );
   void setAccessMessage   (const QString &message );
   void setCurrentCall     ( Call* call       );

protected:
   virtual void paintEvent  (QPaintEvent*  event );
   virtual void resizeEvent (QResizeEvent* e     );

private:
   QWidget* m_pIcon        ;
   uint     m_step         ;
   QTimer*  m_pTimer       ;
   bool     m_enabled      ;
   QColor   m_black        ;
   QString  m_accessMessage;
   QWidget* m_pMainWidget  ;
   Call*    m_pCurrentCall ;

   //Event filter
   bool eventFilter( QObject *obj, QEvent *event);

private Q_SLOTS:
   void changeVisibility();
   void slotLayoutChanged();
   void slotAttendedTransfer();
   void slotTransferClicked();
};

#endif
