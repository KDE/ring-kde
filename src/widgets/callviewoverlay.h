/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
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

///CallViewOverlay: Display overlay on top of the call tree
class CallViewOverlay : public QWidget {
   Q_OBJECT

public:
   //Constructor
   CallViewOverlay(QWidget* parent);
   ~CallViewOverlay();

   //Setters
   void setCornerWidget  ( QWidget* wdg     );
   void setVisible       ( bool     enabled );
   void setAccessMessage ( QString  message );

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

private slots:
   void changeVisibility();
};

#endif