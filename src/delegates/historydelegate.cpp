/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "historydelegate.h"

//Qt
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>
#include <QtCore/QFile>
#include <QtCore/QDebug>

//KDE
#include <KColorScheme>
#include <KLocale>
#include <KIcon>
#include <KStandardDirs>

//SFLPhone
#include <lib/historymodel.h>
#include <lib/contact.h>
#include <lib/callmodel.h>
#include <lib/phonenumber.h>
#include "klib/kcfg_settings.h"
#include "widgets/playeroverlay.h"
#include "dialpaddelegate.h"
#include "../widgets/tips/ringingtip.h"
#include "klib/tipanimationwrapper.h"
#include "lib/visitors/pixmapmanipulationvisitor.h"
#include "implementation.h"

///Constant
#pragma GCC diagnostic ignored "-Wmissing-braces"
TypedStateMachine< const char* , Call::State > callStateIcons = {{ICON_INCOMING, ICON_RINGING, ICON_CURRENT, ICON_DIALING, ICON_HOLD, ICON_FAILURE, ICON_BUSY, ICON_TRANSFER, ICON_TRANSF_HOLD, "", "", ICON_CONFERENCE}};

HistoryDelegate::HistoryDelegate(QTreeView* parent) : QStyledItemDelegate(parent),m_pParent(parent),m_pDelegatedropoverlay(nullptr),m_AnimationWrapper(nullptr),m_pRingingTip(nullptr)
{
   connect(CallModel::instance(),SIGNAL(callStateChanged(Call*,Call::State)),this,SLOT(slotStopRingingAnimation()));
}

QSize HistoryDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   if (!index.isValid())
      return QSize(-1,-1);
   const QSize sh = QStyledItemDelegate::sizeHint(option, index);
   const QFontMetrics fm(QApplication::font());
   int lineHeight = fm.height()+2;
   int rowCount = 0;
   const Call::State currentState = static_cast<Call::State>(index.data(Call::Role::CallState).toInt());
   int minimumRowHeight = (currentState == Call::State::OVER)?48:(ConfigurationSkeleton::limitMinimumRowHeight()?ConfigurationSkeleton::minimumRowHeight():0);
   if (currentState == Call::State::OVER)
      rowCount = 3;
   else {
      rowCount = ConfigurationSkeleton::displayCallPeer()
      + ConfigurationSkeleton::displayCallNumber       ()
      + ConfigurationSkeleton::displayCallSecure       ()
      + ConfigurationSkeleton::displayCallOrganisation ()
      + ConfigurationSkeleton::displayCallDepartment   ()
      + ConfigurationSkeleton::displayCallEmail        ();
   }
   return QSize(sh.width(),((rowCount*lineHeight)<minimumRowHeight?minimumRowHeight:(rowCount*lineHeight)) + 4);
}

void HistoryDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   const bool isBookmark = index.data(Call::Role::IsBookmark).toBool();

   painter->save();
   int iconHeight = option.rect.height() -4;
   //Paint the "selected" or "hover" backgrounds
      //    if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver) {
      QStyleOptionViewItem opt2 = option;
      QPalette pal = option.palette;
      pal.setBrush(QPalette::Text,QColor(0,0,0,0));
      pal.setBrush(QPalette::HighlightedText,QColor(0,0,0,0));
      opt2.palette = pal;
      QStyledItemDelegate::paint(painter,opt2,index);
      //    }

   painter->setPen(QApplication::palette().color(QPalette::Active,(option.state & QStyle::State_Selected)?QPalette::HighlightedText:QPalette::Text));
   const Call::State currentState = (Call::State) index.data(Call::Role::CallState).toInt();
   const Call::LifeCycleState currentLifeCycleState = static_cast<Call::LifeCycleState>(index.data(Call::Role::CallLifeCycleState).toInt());

   if (currentState == Call::State::HOLD)
      painter->setOpacity(0.70);

   const PhoneNumber* n = qvariant_cast<PhoneNumber*>(index.data(Call::Role::PhoneNu));
   QPixmap pxm = n?PixmapManipulationVisitor::instance()->callPhoto(n,QSize(iconHeight+4,iconHeight+4),isBookmark).value<QPixmap>():QPixmap(QSize(iconHeight+4,iconHeight+4));

   //Handle history with recording
   if (index.data(Call::Role::HasRecording).toBool() && currentState == Call::State::OVER) {
      QObject* obj= qvariant_cast<Call*>(index.data(Call::Role::Object));
      Call* call  = nullptr;
      if (obj)
         call = qobject_cast<Call*>(obj);
      if (call && QFile::exists(call->recordingPath())) {
         QPainter painter(&pxm);
         QPixmap status(KStandardDirs::locate("data","sflphone-client-kde/voicemail.png"));
         status=status.scaled(QSize(24,24));
         painter.drawPixmap(pxm.width()-status.width(),pxm.height()-status.height(),status);
         if (m_pParent && m_pParent->indexWidget(index) == nullptr) {
            auto button = new PlayerOverlay(call,nullptr);
            button->setCall(call);
            m_pParent->setIndexWidget(index,button);
         }
      }
   }
   //Handle history
   else if (!isBookmark && (index.data(Call::Role::Historystate).toInt() != (int)Call::LegacyHistoryState::NONE || currentState != Call::State::OVER) && ConfigurationSkeleton::displayHistoryStatus()) {
      QPainter painter(&pxm);
      QPixmap status((currentState==Call::State::OVER)?
         KDEPixmapManipulation::icnPath   [index.data(Call::Role::Missed    ).toInt()]
                                          [index.data(Call::Role::Direction2).toInt()]
            :callStateIcons[currentState]);
      if (!status.isNull()) {
         const int pxmHeight = option.rect.height()<24?option.rect.height()-2:24;
         status=status.scaled(QSize(pxmHeight,pxmHeight));
         painter.drawPixmap(pxm.width()-status.width(),pxm.height()-status.height(),status);
      }
   }
   if (currentLifeCycleState == Call::LifeCycleState::PROGRESS && index.data(Call::Role::IsRecording).toBool()) {
      const static QPixmap record(KStandardDirs::locate("data","sflphone-client-kde/record.png"));
      time_t curTime;
      ::time(&curTime);
      if (curTime%3)
         painter->drawPixmap(option.rect.x()+option.rect.width()-record.width()-2,option.rect.y()+option.rect.height()-record.height()-2,record);
   }
   int x_offset((iconHeight-pxm.width())/2),y_offset((iconHeight-pxm.height())/2);
   painter->drawPixmap(option.rect.x()+4+x_offset,option.rect.y()+y_offset+(option.rect.height()-iconHeight)/2,pxm);

   QFont font = painter->font();
   QFontMetrics fm(font);
   int currentHeight = option.rect.y()+fm.height()+2;
   //BEGIN history fields
   if (currentState == Call::State::OVER || isBookmark) { //History/Bookmarks
      const QPen textCol = (option.state & QStyle::State_Selected) ? Qt::white : QApplication::palette().color(QPalette::Disabled,QPalette::Text);
      font.setWeight(QFont::DemiBold);
      painter->save();
      painter->setFont(font);
      painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Qt::DisplayRole).toString());
      font.setWeight(QFont::Normal);
      painter->setFont(font);
      painter->setPen(textCol);
      currentHeight +=fm.height();

      //Draw INCOMING/OUTGOING/MISSED
      if (!isBookmark) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::FormattedDate).toString());
         currentHeight +=fm.height();
//          const static QPixmap* callPxm = nullptr;
//          if (!callPxm)
//             callPxm = new QPixmap(KStandardDirs::locate("data","sflphone-client-kde/mini/call.png"));
         QVariant var = index.data(Call::Role::CategoryIcon);
         if (var.type() == QVariant::Pixmap) {
            QPixmap pxm2 = var.value<QPixmap>();
            painter->drawPixmap(option.rect.x()+15+iconHeight,currentHeight-12+(fm.height()-12),pxm2);
         }
      }

// //       if (isTracked) {
// //          if (isPresent)
// //             painter->setPen(presentBrush);
// //          else
// //             painter->setPen(awayBrush);
// //       }

      painter->drawText(option.rect.x()+15+iconHeight+((!isBookmark)?12:0),currentHeight,index.data(Call::Role::Number).toString());

//       if (isTracked)
//          painter->setPen(textCol);

      currentHeight +=fm.height();
      painter->restore();
   }
   //END history fields
   else { //Active calls
      if (ConfigurationSkeleton::displayCallIcon()) {
         //TODO dead code
      }
      if(ConfigurationSkeleton::displayCallPeer() && !(currentState == Call::State::DIALING || (option.state & QStyle::State_Editing))) {
         font.setBold(true);
         painter->setFont(font);
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Qt::DisplayRole).toString());
         font.setBold(false);
         painter->setFont(font);
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallNumber() && !(currentState == Call::State::DIALING || (option.state & QStyle::State_Editing))) {
//          if (isTracked) {
//             if (isPresent)
//                painter->setPen(presentBrush);
//             else
//                painter->setPen(awayBrush);
//          }
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Number).toString());
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallSecure()) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Security).toString());
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallOrganisation()) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Organisation).toString());
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallDepartment()) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Department).toString());
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallEmail()) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Email).toString());
         //currentHeight +=fm.height();
      }
   }

   if (!index.parent().isValid() && currentLifeCycleState == Call::LifeCycleState::PROGRESS){
      const QString length = index.data(Call::Role::Length).toString();
      const int lenLen = fm.width(length);
      if (!length.isEmpty()) {
         painter->drawText(option.rect.x()+option.rect.width()-lenLen-4,option.rect.y()+(option.rect.height()/2)+(fm.height()/3),length);
      }
      DialpadDelegate::paint(painter,option,index,lenLen);
   }
   else if (!isBookmark && ((currentState == Call::State::RINGING || currentState == Call::State::INCOMING) && index.model()->rowCount() > 1)) {
      if (!m_AnimationWrapper) {
         const_cast<HistoryDelegate*>(this)->m_AnimationWrapper = new TipAnimationWrapper();
         const_cast<HistoryDelegate*>(this)->m_pRingingTip = new RingingTip();
         m_AnimationWrapper->setTip(m_pRingingTip);
      }
      if (!m_pRingingTip->isVisible())
         m_pRingingTip->setVisible(true);
      painter->save();
      painter->setRenderHint  (QPainter::Antialiasing, true   );
      painter->setOpacity(0.066);
      painter->drawImage(QRect(option.rect.x()+option.rect.width()-option.rect.height()-8,option.rect.y()+4,option.rect.height()-8,option.rect.height()-8),m_AnimationWrapper->currentImage());
      painter->restore();
   }
   else {
      //Display the dialpad if necesary
      DialpadDelegate::paint(painter,option,index);
   }

   //BEGIN overlay path
   if (index.data(Call::Role::DropState).toInt() != 0) {
      /*static*/ if (!m_pDelegatedropoverlay) {
         const_cast<HistoryDelegate*>(this)->m_pDelegatedropoverlay = new DelegateDropOverlay((QObject*)this);
         const_cast<HistoryDelegate*>(this)->callMap.insert(i18n("Conference")   ,new DelegateDropOverlay::OverlayButton(new QImage(KStandardDirs::locate("data","sflphone-client-kde/confBlackWhite.png")),Call::DropAction::Conference));
         const_cast<HistoryDelegate*>(this)->callMap.insert(i18n("Transfer")     ,new DelegateDropOverlay::OverlayButton(new QImage(KStandardDirs::locate("data","sflphone-client-kde/transferarrow.png")),Call::DropAction::Transfer));
         const_cast<HistoryDelegate*>(this)->historyMap.insert(i18n("Transfer")  ,new DelegateDropOverlay::OverlayButton(new QImage(KStandardDirs::locate("data","sflphone-client-kde/transferarrow.png")),Call::DropAction::Transfer));
      }

      if (currentState == Call::State::OVER)
         const_cast<HistoryDelegate*>(this)->m_pDelegatedropoverlay->setButtons(&const_cast<HistoryDelegate*>(this)->historyMap);
      else
         const_cast<HistoryDelegate*>(this)->m_pDelegatedropoverlay->setButtons(&const_cast<HistoryDelegate*>(this)->callMap);
      m_pDelegatedropoverlay->paintEvent(painter, option, index);
   }
   //END overlay path

   //BEGIN Item editor
   if (currentState == Call::State::DIALING) {
      m_pParent->edit(index);
   }
   //END item editor
   painter->restore();
}

void HistoryDelegate::slotStopRingingAnimation()
{
   if (m_pRingingTip && m_pRingingTip->isVisible()) {
      bool found = false;
      foreach(const Call* call,CallModel::instance()->getCallList()) {
         found = (call->lifeCycleState() == Call::LifeCycleState::INITIALIZATION);
         if (found)
            break;
      }

      if (!found)
         m_pRingingTip->setVisible(false);
   }
}

HistoryDelegate::~HistoryDelegate()
{
   if (m_AnimationWrapper)
      delete m_AnimationWrapper;
   if (m_pRingingTip)
      delete m_pRingingTip;
   if (m_pDelegatedropoverlay) {
      delete m_pDelegatedropoverlay;
      foreach (DelegateDropOverlay::OverlayButton* b, historyMap)
         delete b;
      foreach (DelegateDropOverlay::OverlayButton* b, callMap)
         delete b;
   }
}
