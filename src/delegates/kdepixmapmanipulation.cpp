/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
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
#include "kdepixmapmanipulation.h"

//Ring
#include "lib/contact.h"
#include "lib/phonenumber.h"
#include "lib/presencestatusmodel.h"
#include "lib/securityvalidationmodel.h"
#include "lib/abstractitembackend.h"

//Qt
#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QApplication>
#include <QtCore/QDebug>
#include <QtCore/QSize>
#include <QImage>
#include <QtGui/QPalette>
#include <QtCore/QBuffer>

//KDE
#include <KIcon>
#include <KColorScheme>
#include <KLocale>
#include <KStandardDirs>


const TypedStateMachine< const char* , Call::State > KDEPixmapManipulation::callStateIcons = {
   {  ICON_INCOMING   ,
      ICON_RINGING    ,
      ICON_CURRENT    ,
      ICON_DIALING    ,
      ICON_HOLD       ,
      ICON_FAILURE    ,
      ICON_BUSY       ,
      ICON_TRANSFER   ,
      ICON_TRANSF_HOLD,
      ""              ,
      ""              ,
      ICON_CONFERENCE}};

KDEPixmapManipulation::KDEPixmapManipulation() : QObject(),PixmapManipulationVisitor()
{

}

///When the Contact is rebased (use a new data source), everything is now invalid
void KDEPixmapManipulation::clearCache()
{
   Contact* c = qobject_cast<Contact*>(sender());
   if (!c) return;

   //Hopefully it wont happen often
   foreach (const QByteArray& name, c->dynamicPropertyNames()) {
      if (name.left(5) == "photo")
         c->setProperty(name,QVariant());
   }
}

QVariant KDEPixmapManipulation::contactPhoto(Contact* c, const QSize& size, bool displayPresence) {
   const QString hash = QString("photo2%1%2%3").arg(size.width()).arg(size.height()).arg(c->isPresent());
   QVariant preRendered = c->property(hash.toAscii());
   if (preRendered.isValid())
      return preRendered;
   else
      connect(c,SIGNAL(rebased(Contact*)),this,SLOT(clearCache()));
   const int radius = (size.height() > 35) ? 7 : 5;
   //const QPixmap pxmPtr = qvariant_cast<QPixmap>(c->photo());
   bool isTracked = displayPresence && c->isTracked();
   bool isPresent = displayPresence && c->isPresent();
   static QColor presentBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::PositiveText ).brush(QPalette::Normal).color();
   static QColor awayBrush    = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();

   QPixmap pxm;
   if (c->photo().isValid()) {
      QPixmap contactPhoto((qvariant_cast<QPixmap>(c->photo())).scaledToWidth(size.height()-6));
      pxm = QPixmap(size);
      pxm.fill(Qt::transparent);
      QPainter painter(&pxm);

      //Clear the pixmap
      painter.setCompositionMode(QPainter::CompositionMode_Clear);
      painter.fillRect(0,0,size.width(),size.height(),QBrush(Qt::white));
      painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

      //Add corner radius to the Pixmap
      QRect pxRect = contactPhoto.rect();
      QBitmap mask(pxRect.size());
      QPainter customPainter(&mask);
      customPainter.setRenderHint  (QPainter::Antialiasing, true      );
      customPainter.fillRect       (pxRect                , Qt::white );
      customPainter.setBackground  (Qt::black                         );
      customPainter.setBrush       (Qt::black                         );
      customPainter.drawRoundedRect(pxRect,radius,radius);
      contactPhoto.setMask(mask);
      painter.drawPixmap(3,3,contactPhoto);
      painter.setBrush(Qt::NoBrush);
      QPen pen(QApplication::palette().color(QPalette::Disabled,QPalette::Text));
      pen.setWidth(isTracked?1:2);
      painter.setPen(pen);
      painter.setRenderHint  (QPainter::Antialiasing, true   );
      painter.drawRoundedRect(3,3,pxm.height()-6,pxm.height()-6,radius,radius);

      //Draw the glow around pixmaps
      if (isTracked) {
         if (isPresent)
            pen.setColor(presentBrush);
         else
            pen.setColor(awayBrush);
         for (int i=2;i<=7;i+=2) {
            pen.setWidth(i);
            painter.setPen(pen);
            painter.setOpacity(0.3f-(((i-2)*0.8f)/10.0f));
            painter.drawRoundedRect(3,3,pxm.height()-6,pxm.height()-6,radius,radius);
         }
      }
   }
   else {
      pxm = drawDefaultUserPixmap(size,isTracked,isPresent);
   }

   c->setProperty(hash.toAscii(),pxm);
   return pxm;
}

QVariant KDEPixmapManipulation::callPhoto(const PhoneNumber* n, const QSize& size, bool displayPresence) {
   if (n->contact()) {
      return contactPhoto(n->contact(),size,displayPresence);
   }
   else {
      bool isTracked = displayPresence && n->isTracked();
      bool isPresent = displayPresence && n->isPresent();

      return drawDefaultUserPixmap(size,isTracked,isPresent);
   }
}

QVariant KDEPixmapManipulation::callPhoto(Call* c, const QSize& size, bool displayPresence)
{
   if (c->peerPhoneNumber()->contact()) {
      return contactPhoto(c->peerPhoneNumber()->contact(),size,displayPresence);
   }
   else
      return QPixmap(callStateIcons[c->state()]);
}

QVariant KDEPixmapManipulation::numberCategoryIcon(const QVariant p, const QSize& size, bool displayPresence, bool isPresent)
{
   Q_UNUSED(size);
   if (displayPresence) {
      QPixmap pxm;
      if(p.isValid())
         pxm = qvariant_cast<QPixmap>(p);
      else
         pxm = QPixmap(KStandardDirs::locate("data" , "sflphone-client-kde/mini/call.png"));
      QPainter painter(&pxm);
      painter.setOpacity(0.3);
      painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
      painter.fillRect(pxm.rect(),isPresent?Qt::green:Qt::red);
      return pxm;
   }
   if (p.isValid())
      return qvariant_cast<QPixmap>(p);
   return QPixmap(KStandardDirs::locate("data" , "sflphone-client-kde/mini/call.png"));
}

QVariant KDEPixmapManipulation::serurityIssueIcon(const QModelIndex& index)
{
   SecurityValidationModel::Severity sev = static_cast<SecurityValidationModel::Severity>(index.data(SecurityValidationModel::Role::SeverityRole).toInt());
   switch(sev) {
      case SecurityValidationModel::Severity::INFORMATION:
         return KIcon("dialog-information");
      case SecurityValidationModel::Severity::WARNING:
         return KIcon("dialog-warning");
      case SecurityValidationModel::Severity::ISSUE:
      case SecurityValidationModel::Severity::FATAL_WARNING:
         return KIcon("task-attempt");
      case SecurityValidationModel::Severity::ERROR:
         return KIcon("dialog-error");
   }
   return QVariant();
}

QByteArray KDEPixmapManipulation::toByteArray(const QVariant pxm)
{
   //Preparation of our QPixmap
   QByteArray bArray;
   QBuffer buffer(&bArray);
   buffer.open(QIODevice::WriteOnly);

   //PNG ?
   (qvariant_cast<QPixmap>(pxm)).save(&buffer, "PNG");
   return bArray;
}

QVariant KDEPixmapManipulation::profilePhoto(const QByteArray& data)
{
   QImage image;
   //For now, ENCODING is only base64 and image type PNG
   image.loadFromData(QByteArray::fromBase64(data), "PNG");
   return QPixmap::fromImage(image);
}

QPixmap KDEPixmapManipulation::drawDefaultUserPixmap(const QSize& size, bool displayPresence, bool isPresent) {
   //Load KDE default user pixmap
   QPixmap pxm(size);
   pxm.fill(Qt::transparent);
   QPainter painter(&pxm);

   painter.drawPixmap(3,3,KIcon("user-identity").pixmap(QSize(size.height()-6,size.width()-6)));

   //Create a region where the pixmap is not fully transparent
   if (displayPresence) {
      static QHash<int,QRegion> r,ri;
      static QHash<int,bool> init;
      const int width = size.width();
      if (!init[width]) {
         r[width] = QRegion(QPixmap::fromImage(pxm.toImage().createAlphaMask()));
         ri[width] = r[width].xored(pxm.rect());
         init[width] = true;
      }

      static QColor presentBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::PositiveText ).brush(QPalette::Normal).color();
      static QColor awayBrush    = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();


      painter.setOpacity(0.05);
      painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
      painter.fillRect(pxm.rect(),isPresent?Qt::green:Qt::red);
      painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

      //Paint only outside of the pixmap, it looks better
      painter.setClipRegion(ri[width]);
      QPainterPath p;
      p.addRegion(r[width]);
      QPen pen = painter.pen();
      pen.setColor(isPresent?presentBrush:awayBrush);
      painter.setBrush(Qt::NoBrush);
      painter.setRenderHint  (QPainter::Antialiasing, true      );
      for (int i=2;i<=9;i+=2) {
         pen.setWidth(i);
         painter.setPen(pen);
         painter.setOpacity(0.30f-(((i-2)*0.45f)/10.0f));
         painter.drawPath(p);
      }
   }
   return pxm;
}

const char* KDEPixmapManipulation::icnPath[2][2] = {
   {
   /* INCOMING  */ ICON_HISTORY_INCOMING  ,
   /* OUTGOING  */ ICON_HISTORY_OUTGOING  ,
   },
   {
   /* MISSED_IN */ ICON_HISTORY_MISSED    ,
   /* MISSED_OUT*/ ICON_HISTORY_MISSED_OUT,
   }
};
