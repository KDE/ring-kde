/***************************************************************************
 *   Copyright (C) 2009-2012 by Savoir-Faire Linux                         *
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
//Base
#include <QtGui/QWidget>
#include <QtCore/QHash>
#include "lib/sflphone_const.h"

//Qt
class QSvgRenderer;
class QToolButton;

//KDE
class KIcon;

//SFLPhone
class ExtendedAction;

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
   CallViewOverlayToolbar(QWidget* parent);

private:
   //Attributes
   QSvgRenderer* m_pRightRender;
   QSvgRenderer* m_pLeftRender ;

   //Buttons
   QToolButton* m_pHold;
   QToolButton* m_pUnhold;
   QToolButton* m_pMute;
   QToolButton* m_pPickup;
   QToolButton* m_pHangup;
   QToolButton* m_pTransfer;
   QToolButton* m_pRecord;
   QToolButton* m_pRefuse;

   QHash<int,QToolButton*> m_hButtons;

   //Helpers
   QToolButton* createButton(ExtendedAction* action);

public slots:
   void updateState(call_state state);

protected:
   void resizeEvent(QResizeEvent* event);
   void paintEvent(QPaintEvent* event);
   void hideEvent(QHideEvent* event);
   void showEvent(QShowEvent* event);

signals:
   void visibilityChanged(bool);

};

#endif