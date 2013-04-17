/***************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                         *
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
#ifndef CALL_VIEW_OVERLAY_H
#define CALL_VIEW_OVERLAY_H

//Qt
#include <QtGui/QToolButton>

//Base
#include <QtGui/QWidget>
#include <QtCore/QHash>
#include "lib/sflphone_const.h"

//Qt
class QSvgRenderer;
class QToolButton;

//KDE

//SFLPhone
class ExtendedAction;

class ObserverToolButton : public QToolButton
{
   Q_OBJECT
public:
   explicit ObserverToolButton(QWidget* parent = nullptr) : QToolButton(parent){}
public Q_SLOTS:
   void setNewText(const QString& text) {
      setText(text);
   }
};

enum ActionButton {
   PICKUP   = 0,
   HOLD     = 1,
   UNHOLD   = 2,
   HANGUP   = 3,
   MUTE     = 4,
   TRANSFER = 5,
   RECORD   = 6,
   REFUSE   = 7,
};

class CallViewOverlayToolbar : public QWidget
{
   Q_OBJECT
public:
   explicit CallViewOverlayToolbar(QWidget* parent = nullptr);

private:
   //Attributes
   QSvgRenderer* m_pRightRender;
   QSvgRenderer* m_pLeftRender ;

   //Buttons
   ObserverToolButton* m_pHold    ;
   ObserverToolButton* m_pUnhold  ;
   ObserverToolButton* m_pMute    ;
   ObserverToolButton* m_pPickup  ;
   ObserverToolButton* m_pHangup  ;
   ObserverToolButton* m_pTransfer;
   ObserverToolButton* m_pRecord  ;
   ObserverToolButton* m_pRefuse  ;

   QHash<int,ObserverToolButton*> m_hButtons;

   //Helpers
   ObserverToolButton* createButton(ExtendedAction* action);

public Q_SLOTS:
   void updateState(call_state state);

protected:
   void resizeEvent( QResizeEvent* event);
   void paintEvent ( QPaintEvent*  event);
   void hideEvent  ( QHideEvent*   event);
   void showEvent  ( QShowEvent*   event);
   bool eventFilter( QObject *obj, QEvent *event);

Q_SIGNALS:
   void visibilityChanged(bool);

};

#endif
