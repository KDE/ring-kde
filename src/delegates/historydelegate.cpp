/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
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

#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtGui/QBitmap>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QTreeView>
#include <QtCore/QFile>

#include <QtCore/QDebug>

#include <KLocale>
#include <KIcon>
#include <KStandardDirs>

#include <lib/historymodel.h>
#include <lib/contact.h>
#include "klib/configurationskeleton.h"
#include "widgets/playeroverlay.h"
#include "delegatedropoverlay.h"

static const char* icnPath[4] = {
/* INCOMING */ ICON_HISTORY_INCOMING,
/* OUTGOING */ ICON_HISTORY_OUTGOING,
/* MISSED   */ ICON_HISTORY_MISSED  ,
/* NONE     */ ""                   ,
};

///Constant
TypedStateMachine< const char* , Call::State, Call::State::COUNT > callStateIcons = {ICON_INCOMING, ICON_RINGING, ICON_CURRENT, ICON_DIALING, ICON_HOLD, ICON_FAILURE, ICON_BUSY, ICON_TRANSFER, ICON_TRANSF_HOLD, "", "", ICON_CONFERENCE};

HistoryDelegate::HistoryDelegate(QTreeView* parent) : QStyledItemDelegate(parent),m_pParent(parent),m_pDelegatedropoverlay(nullptr)
{
}

QSize HistoryDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
   QSize sh = QStyledItemDelegate::sizeHint(option, index);
   QFontMetrics fm(QApplication::font());
   int lineHeight = fm.height()+2;
   int rowCount = 0;
   Call::State currentState = static_cast<Call::State>(index.data(Call::Role::CallState).toInt());
   int minimumRowHeight = (currentState == Call::State::OVER)?48:(ConfigurationSkeleton::limitMinimumRowHeight()?ConfigurationSkeleton::minimumRowHeight():0);
   if (currentState == Call::State::OVER)
      rowCount = 3;
   else {
      rowCount = ConfigurationSkeleton::displayCallPeer()
      + ConfigurationSkeleton::displayCallNumber       ()
      + ConfigurationSkeleton::displayCallSecure       ()
      + ConfigurationSkeleton::displayCallCodec        ()
      + ConfigurationSkeleton::displayCallOrganisation ()
      + ConfigurationSkeleton::displayCallDepartment   ()
      + ConfigurationSkeleton::displayCallEmail        ();
   }
   return QSize(sh.rwidth(),((rowCount*lineHeight)<minimumRowHeight?minimumRowHeight:(rowCount*lineHeight)) + 4);
}

void HistoryDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());
   painter->save();
   int iconHeight = option.rect.height() -4;
   if (option.state & QStyle::State_Selected || option.state & QStyle::State_MouseOver) {
      QStyleOptionViewItem opt2 = option;
      QPalette pal = option.palette;
      pal.setBrush(QPalette::Text,QColor(0,0,0,0));
      pal.setBrush(QPalette::HighlightedText,QColor(0,0,0,0));
      opt2.palette = pal;
      QStyledItemDelegate::paint(painter,opt2,index);
   }

   painter->setPen(QApplication::palette().color(QPalette::Active,(option.state & QStyle::State_Selected)?QPalette::HighlightedText:QPalette::Text));
   QObject* obj= qvariant_cast<Call*>(index.data(Call::Role::Object));
   QPixmap* pxmPtr=  (QPixmap*)qvariant_cast<void*>(index.data(Call::Role::PhotoPtr));
   Call* call  = nullptr;
   if (obj)
      call = qobject_cast<Call*>(obj);
   Call::State currentState = (Call::State) index.data(Call::Role::CallState).toInt();
   if (currentState == Call::State::HOLD)
      painter->setOpacity(0.70);

   QPixmap pxm;
   if (pxmPtr) {
      pxm = (*pxmPtr).scaledToWidth(iconHeight);
      QRect pxRect = pxm.rect();
      QBitmap mask(pxRect.size());
      QPainter customPainter(&mask);
      customPainter.setRenderHint  (QPainter::Antialiasing, true   );
      customPainter.fillRect       (pxRect                ,"white" );
      customPainter.setBackground  (QColor("black")                );
      customPainter.setBrush       (QColor("black")                );
      customPainter.drawRoundedRect(pxRect,5,5);
      pxm.setMask(mask);
   }
   else {
      if (currentState == Call::State::OVER)
         pxm = QPixmap(KIcon("user-identity").pixmap(QSize(iconHeight,iconHeight)));
      else
         pxm = QPixmap(callStateIcons[currentState]).scaledToWidth(iconHeight);
   }

   if (index.data(Call::Role::HasRecording).toBool() && call && QFile::exists(call->getRecordingPath())) {
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
   else if ((index.data(Call::Role::HistoryState).toInt() != history_state::NONE || currentState != Call::State::OVER) && ConfigurationSkeleton::displayHistoryStatus()) {
      QPainter painter(&pxm);
      QPixmap status((currentState==Call::State::OVER)?icnPath[index.data(Call::Role::HistoryState).toInt()]:callStateIcons[currentState]);
      status=status.scaled(QSize(24,24));
      painter.drawPixmap(pxm.width()-status.width(),pxm.height()-status.height(),status);
   }
   painter->drawPixmap(option.rect.x()+4,option.rect.y()+(option.rect.height()-iconHeight)/2,pxm);

   QFont font = painter->font();
   QFontMetrics fm(font);
   int currentHeight = option.rect.y()+fm.height()+2;
   if (currentState == Call::State::OVER) { //History/Bookmarks
      font.setBold(true);
      painter->setFont(font);
      painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Qt::DisplayRole).toString());
      font.setBold(false);
      painter->setFont(font);
      currentHeight +=fm.height();
      painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::FormattedDate).toString());
      currentHeight +=fm.height();
      painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Number).toString());
      currentHeight +=fm.height();
   }
   else { //Active calls
      if (ConfigurationSkeleton::displayCallIcon()) {
//          m_pIconL = new QLabel(" ");
//          mainLayout->addWidget(m_pIconL);
      }

      if(ConfigurationSkeleton::displayCallPeer()) {
         font.setBold(true);
         painter->setFont(font);
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Qt::DisplayRole).toString());
         font.setBold(false);
         painter->setFont(font);
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallNumber()) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Number).toString());
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallSecure()) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Security).toString());
         currentHeight +=fm.height();
      }

      if (ConfigurationSkeleton::displayCallCodec()) {
         painter->drawText(option.rect.x()+15+iconHeight,currentHeight,index.data(Call::Role::Codec).toString());
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
         currentHeight +=fm.height();
      }
   }

   if (!index.parent().isValid()){
      QString length = index.data(Call::Role::Length).toString();
      if (!length.isEmpty()) {
         painter->drawText(option.rect.x()+option.rect.width()-fm.width(length)-4,option.rect.y()+(option.rect.height()/2),length);
      }
   }

   static QMap<QString,QImage*> historyMap,callMap;
   //BEGIN overlay path
   if (index.data(Call::Role::DropState).toInt() != 0) {
      /*static*/ if (!m_pDelegatedropoverlay) {
         ((HistoryDelegate*)this)->m_pDelegatedropoverlay = new DelegateDropOverlay((QObject*)this);
         callMap.insert(i18n("Conference")   ,new QImage(KStandardDirs::locate("data","sflphone-client-kde/confBlackWhite.png")));
         callMap.insert(i18n("Transfer")     ,new QImage(KStandardDirs::locate("data","sflphone-client-kde/transferarraw.png")));
         historyMap.insert(i18n("Transfer")  ,new QImage(KStandardDirs::locate("data","sflphone-client-kde/transferarraw.png")));
      }

      if (currentState == Call::State::OVER)
         m_pDelegatedropoverlay->setButtons(&historyMap);
      else
         m_pDelegatedropoverlay->setButtons(&callMap);
      m_pDelegatedropoverlay->paintEvent(painter, option, index);
   }
   //END overlay path
   painter->restore();
}
