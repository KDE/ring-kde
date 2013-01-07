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
#include "callviewoverlaytoolbar.h"

//Qt
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QHBoxLayout>
#include <QDebug> //TODO remove

//KDE
#include <KStandardDirs>
#include <KIcon>

//SFLPhone
#include "sflphone.h"
#include "extendedaction.h"

const bool visibility[8][13] = {              /*ROW = BUTTONS   COLS=STATE*/
            /* INCOMING  RINGING CURRENT DIALING  HOLD FAILURE BUSY  TRANSFERRED TRANSF_HOLD  OVER  ERROR CONFERENCE CONFERENCE_HOLD:*/
 /*PICKUP   */ { true   , true ,  false,  false, false, false, false,   false,     false,    false, false,  false,      false    },
 /*HOLD     */ { false  , false,  true ,  false, false, false, false,   true ,     false,    false, false,  true ,      false    },
 /*UNHOLD   */ { false  , false,  false,  false, true , false, false,   false,     false,    false, false,  false,      false    },
 /*HANGUP   */ { false  , false,  true ,  false, true , true , true ,   true ,     true ,    false, true ,  true ,      true     },
 /*MUTE     */ { false  , true ,  true ,  false, false, false, false,   false,     false,    false, false,  false,      false    },
 /*TRANSFER */ { false  , false,  true ,  false, true , false, false,   false,     false,    false, false,  false,      false    },
 /*RECORD   */ { false  , true ,  true ,  false, true , false, false,   true ,     true ,    false, false,  true ,      true     },
 /*REFUSE   */ { true   , false,  false,  false, false, false, false,   false,     false,    false, false,  false,      false    }
};

///Constructor
CallViewOverlayToolbar::CallViewOverlayToolbar(QWidget* parent) : QWidget(parent),m_pRightRender(0),m_pLeftRender(0)
{
   m_pRightRender = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_right_corner.svg") );
   m_pLeftRender  = new QSvgRenderer( KStandardDirs::locate("data","sflphone-client-kde/overlay_left_corner.svg" ) );

   QHBoxLayout* layout = new QHBoxLayout(this);

   m_pHold     = createButton( SFLPhone::app()->getHoldAction()     );
   m_pUnhold   = createButton( SFLPhone::app()->getUnholdAction()   );
   m_pMute     = createButton( SFLPhone::app()->getMuteAction()     );
   m_pPickup   = createButton( SFLPhone::app()->getPickupAction()   );
   m_pHangup   = createButton( SFLPhone::app()->getHangupAction()   );
   m_pTransfer = createButton( SFLPhone::app()->getTransferAction() );
   m_pRecord   = createButton( SFLPhone::app()->getRecordAction()   );
   m_pRefuse   = createButton( SFLPhone::app()->getRefuseAction()   );

   m_hButtons[ ActionButton::HOLD     ] = m_pHold    ;
   m_hButtons[ ActionButton::UNHOLD   ] = m_pUnhold  ;
   m_hButtons[ ActionButton::PICKUP   ] = m_pPickup  ;
   m_hButtons[ ActionButton::HANGUP   ] = m_pHangup  ;
   m_hButtons[ ActionButton::MUTE     ] = m_pMute    ;
   m_hButtons[ ActionButton::TRANSFER ] = m_pTransfer;
   m_hButtons[ ActionButton::RECORD   ] = m_pRecord  ;
   m_hButtons[ ActionButton::REFUSE   ] = m_pRefuse  ;

   layout->addWidget( m_pHangup   );
   layout->addWidget( m_pTransfer );
   layout->addWidget( m_pMute     );
   layout->addWidget( m_pRecord   );
   layout->addWidget( m_pUnhold   );
   layout->addWidget( m_pHold     );
   layout->addWidget( m_pPickup   );
   layout->addWidget( m_pRefuse   );

   setMinimumSize(100,56);
}

///Resize event
void CallViewOverlayToolbar::resizeEvent(QResizeEvent* event)
{
   QWidget::resizeEvent(event);
}

///Repaint event
void CallViewOverlayToolbar::paintEvent(QPaintEvent* event)
{
   Q_UNUSED(event);
   QPainter customPainter(this);
   customPainter.setRenderHint(QPainter::Antialiasing);
   QBrush b = customPainter.brush();
   QPen   p = customPainter.pen();

   b.setColor("black");
   customPainter.setOpacity(0.5);

   customPainter.setBrush (Qt::black);
   customPainter.setPen   (Qt::transparent);
   customPainter.drawRect (QRect(3,10,width()-6,height()-16)               );
   customPainter.drawPie  (QRect(width()-8-3,height()-10,8,8),270*16,90*16 );
   customPainter.drawPie  (QRect(3,height()-10,8,8),180*16,90*16           );
   customPainter.drawRect (QRect(5+2,height()-6,width()-8-6,3)             );

   m_pLeftRender->render (&customPainter,QRect( 3,0,10,10)                 );
   m_pRightRender->render(&customPainter,QRect( width()-13,0,10,10)        );
}

///Create a toolbar button
ObserverToolButton* CallViewOverlayToolbar::createButton(ExtendedAction* action)
{
   ObserverToolButton* b = new ObserverToolButton(this);
   b->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
   b->setIconSize(QSize(28,28));
   b->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
   b->setText(action->text());
   b->setShortcut(0);
   b->setStyleSheet("margin-bottom:0px;margin-top:7px;font-weight:bold;background-color:transparent;color:#DDDDDD;padding-top:5px;");
   b->setIcon(action->altIcon());
   connect(action,SIGNAL(textChanged(QString)),b,SLOT(setNewText(QString)));
   connect(b,SIGNAL(clicked()),action,SLOT(trigger()));
   return b;
}

///Hide or show the toolbar and select visible actions
void CallViewOverlayToolbar::updateState(call_state state)
{
   m_hButtons[ ActionButton::HOLD     ]->setVisible(visibility[ ActionButton::HOLD     ][state]);
   m_hButtons[ ActionButton::UNHOLD   ]->setVisible(visibility[ ActionButton::UNHOLD   ][state]);
   m_hButtons[ ActionButton::PICKUP   ]->setVisible(visibility[ ActionButton::PICKUP   ][state]);
   m_hButtons[ ActionButton::HANGUP   ]->setVisible(visibility[ ActionButton::HANGUP   ][state]);
   m_hButtons[ ActionButton::MUTE     ]->setVisible(visibility[ ActionButton::MUTE     ][state]);
   m_hButtons[ ActionButton::TRANSFER ]->setVisible(visibility[ ActionButton::TRANSFER ][state]);
   m_hButtons[ ActionButton::RECORD   ]->setVisible(visibility[ ActionButton::RECORD   ][state]);
   m_hButtons[ ActionButton::REFUSE   ]->setVisible(visibility[ ActionButton::REFUSE   ][state]);
   /*switch (state) {
      case CALL_STATE_INCOMING:
         
         break;

      case CALL_STATE_RINGING:
         
         break;

      case CALL_STATE_CURRENT:
         
         break;

      case CALL_STATE_DIALING:
         
         break;

      case CALL_STATE_HOLD:
         
         break;

      case CALL_STATE_FAILURE:
         
         break;

      case CALL_STATE_BUSY:
         
         break;

      case CALL_STATE_TRANSFERRED:
         
         break;

      case CALL_STATE_TRANSF_HOLD:
         
         break;

      case CALL_STATE_OVER:
         
         break;

      case CALL_STATE_ERROR:
         
         break;

      case CALL_STATE_CONFERENCE:
         
         break;

      case CALL_STATE_CONFERENCE_HOLD:
         
         break;

      default:
         
         break;

   }*/
}

void CallViewOverlayToolbar::hideEvent(QHideEvent *)
{
    emit visibilityChanged(false);
}

void CallViewOverlayToolbar::showEvent(QShowEvent *)
{
    emit visibilityChanged(true);
}
