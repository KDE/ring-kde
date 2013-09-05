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
#include "callviewoverlaytoolbar.h"

//Qt
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QBrush>
#include <QtGui/QPen>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QHBoxLayout>
#include <QtGui/QStyle>
#include <KDebug> //TODO remove

//KDE
#include <KStandardDirs>
#include <KIcon>

//SFLPhone
#include "sflphone.h"
#include "extendedaction.h"
#include <klib/tipmanager.h>
#include <lib/call.h>
#include <lib/useractionmodel.h>
#include <lib/callmodel.h>

///Constructor
CallViewOverlayToolbar::CallViewOverlayToolbar(QTreeView* parent) : QWidget(parent),m_pRightRender(nullptr),m_pLeftRender(nullptr),m_pParent(parent)
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
   m_pAccept   = createButton( SFLPhone::app()->getAcceptAction()   );

   m_hButtons[ static_cast<int>(UserActionModel::Action::HOLD)     ] = m_pHold    ;
   m_hButtons[ static_cast<int>(UserActionModel::Action::UNHOLD)   ] = m_pUnhold  ;
   m_hButtons[ static_cast<int>(UserActionModel::Action::PICKUP)   ] = m_pPickup  ;
   m_hButtons[ static_cast<int>(UserActionModel::Action::MUTE)     ] = m_pMute    ;
   m_hButtons[ static_cast<int>(UserActionModel::Action::TRANSFER) ] = m_pTransfer;
   m_hButtons[ static_cast<int>(UserActionModel::Action::RECORD)   ] = m_pRecord  ;
   m_hButtons[ static_cast<int>(UserActionModel::Action::REFUSE)   ] = m_pRefuse  ;
   m_hButtons[ static_cast<int>(UserActionModel::Action::ACCEPT)   ] = m_pAccept  ;
   m_hButtons[ static_cast<int>(UserActionModel::Action::HANGUP)   ] = m_pHangup  ;

   layout->addWidget( m_pHangup   );
   layout->addWidget( m_pTransfer );
   layout->addWidget( m_pMute     );
   layout->addWidget( m_pRecord   );
   layout->addWidget( m_pUnhold   );
   layout->addWidget( m_pHold     );
   layout->addWidget( m_pPickup   );
   layout->addWidget( m_pRefuse   );
   layout->addWidget( m_pAccept   );

   setMinimumSize(100,56);
   if (parent)
      parent->installEventFilter(this);
   setVisible(false);
   hideEvent(nullptr);
} //CallViewOverlayToolbar

CallViewOverlayToolbar::~CallViewOverlayToolbar()
{
   delete m_pRightRender;
   delete m_pLeftRender;
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
   //Use the current style pixel metrics to do as well as possible to guess the right shape
   int margin = style()->pixelMetric(QStyle::PM_FocusFrameHMargin);
   customPainter.setOpacity(0.5);
   customPainter.setBrush (Qt::black);
   customPainter.setPen   (Qt::transparent);
   customPainter.drawRect (QRect(margin,10,width()-2*margin,height()-10-2*margin - margin)               );
   customPainter.drawPie  (QRect(width()-8-margin,height()-10,8,8),270*16,90*16 );
   customPainter.drawPie  (QRect(margin,height()-10,8,8),180*16,90*16           );
   customPainter.drawRect (QRect(4+margin,height()-4-margin,width()-8-2*margin,4)             );

   m_pLeftRender->render (&customPainter,QRect( margin,0,10,10)                 );
   m_pRightRender->render(&customPainter,QRect( width()-10-margin,0,10,10)        );
} //paintEvent

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
} //createButton

///Hide or show the toolbar and select visible actions
void CallViewOverlayToolbar::updateState()
{
   QModelIndex index = m_pParent->selectionModel()->currentIndex();
   if ((!m_pParent->selectionModel()->hasSelection() || !index.isValid()) && CallModel::instance()->rowCount()) {
      m_pParent->selectionModel()->setCurrentIndex(CallModel::instance()->index(0,0),QItemSelectionModel::SelectCurrent);
      index = m_pParent->selectionModel()->currentIndex();
   }
   if (index.isValid() && CallModel::instance()->rowCount()) {
      Call* call = qvariant_cast<Call*>(index.data(Call::Role::Object));
      setVisible(true);
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(53);
      char act_counter = 0;
      for (int i = 0;i<static_cast<int>(UserActionModel::Action::COUNT);i++) {
         try {
            if (call && call->userActionModel()) {
               m_hButtons[ i ]->setVisible(call->userActionModel()->isActionEnabled(static_cast<UserActionModel::Action>(i)));
               act_counter += call->userActionModel()->isActionEnabled( static_cast<UserActionModel::Action>(i));
            }
         }
         catch (Call::State& state) {
            qDebug() << "CallViewOverlayToolbar is out of bound (state)" << state;
         }
         catch (UserActionModel::Action& btn) {
            kDebug() << "CallViewOverlayToolbar is out of bound (Action)" << (int)btn;
         }
         catch (...) {
            kDebug() << "CallViewOverlayToolbar is out of bound (Other)";
         }
      }
      if (!act_counter)
         setVisible(false);
   }
   else {
      setVisible(false);
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(0);
   }
   //Now set the top margin, this doesn't really belong anywhere, so why not here
   const int rows = CallModel::instance()->rowCount(QModelIndex());
   QModelIndex last = CallModel::instance()->index(rows-1,0);
   if (CallModel::instance()->rowCount(last) > 0)
      last = CallModel::instance()->index(CallModel::instance()->rowCount(last)-1,0,last);
   const QRect topMargin =  m_pParent->visualRect(last);
   TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
   manager->setTopMargin(topMargin.y()+topMargin.height());
} //updateState

void CallViewOverlayToolbar::hideEvent(QHideEvent *)
{
   if (parentWidget()->property("tipManager").isValid()) {
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(0);
   }
   emit visibilityChanged(false);
}

void CallViewOverlayToolbar::showEvent(QShowEvent *)
{
   if (parentWidget()->property("tipManager").isValid()) {
      TipManager* manager = qvariant_cast<TipManager*>(parentWidget()->property("tipManager"));
      manager->setBottomMargin(53);
   }
   emit visibilityChanged(true);
}

bool CallViewOverlayToolbar::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize && parentWidget()) {
      resize(parentWidget()->width(),72);
      move(0,parentWidget()->height()-72);
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}
