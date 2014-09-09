/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "implementation.h"

//Qt
#include <QtCore/QDebug>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtGui/QApplication>

//KDE
#include <KIcon>
#include <KColorScheme>
#include <KLocale>
#include <KStandardDirs>

//SFLPhone
#include <lib/contact.h>
#include <lib/phonenumber.h>
#include <lib/presencestatusmodel.h>
#include <lib/securityvalidationmodel.h>
#include "klib/kcfg_settings.h"
#include <lib/abstractitembackend.h>

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

QDebug operator<<(QDebug dbg, const Call::State& c)
{
   dbg.nospace() << QString(Call::toHumanStateName(c));
   return dbg.space();
}

QDebug operator<<(QDebug dbg, const Call::DaemonState& c)
{
   dbg.nospace() << static_cast<int>(c);
   return dbg.space();
}

QDebug operator<<(QDebug dbg, const Call::Action& c)
{
   dbg.nospace() << static_cast<int>(c);
   return dbg.space();
}

ColorVisitor::ColorVisitor(QPalette pal) : m_Pal(pal) {
   m_Green = QColor(m_Pal.color(QPalette::Base));
   if (m_Green.green()+20 >= 255) {
      m_Green.setRed ( ((int)m_Green.red()  -20));
      m_Green.setBlue( ((int)m_Green.blue() -20));
   }
   else
      m_Green.setGreen(((int)m_Green.green()+20));

   m_Red = QColor(m_Pal.color(QPalette::Base));
   if (m_Red.red()+20 >= 255) {
      m_Red.setGreen(  ((int)m_Red.green()  -20));
      m_Red.setBlue(   ((int)m_Red.blue()   -20));
   }
   else
      m_Red.setRed(    ((int)m_Red.red()     +20));

   m_Yellow = QColor(m_Pal.color(QPalette::Base));
   if (m_Yellow.red()+20 >= 255 || m_Green.green()+20 >= 255) {
      m_Yellow.setBlue(((int)m_Yellow.blue() -20));
   }
   else {
      m_Yellow.setGreen(((int)m_Yellow.green()+20));
      m_Yellow.setRed( ((int)m_Yellow.red()   +20));
   }
}

QVariant ColorVisitor::getColor(const Account* a) {
   if(a->registrationStatus() == Account::State::UNREGISTERED || !a->isEnabled())
      return m_Pal.color(QPalette::Base);
   if(a->registrationStatus() == Account::State::REGISTERED || a->registrationStatus() == Account::State::READY) {
      return m_Green;
   }
   if(a->registrationStatus() == Account::State::TRYING)
      return m_Yellow;
   return m_Red;
}

QVariant ColorVisitor::getIcon(const Account* a) {
   if (a->state() == Account::AccountEditState::MODIFIED)
      return KIcon("document-save");
   else if (a->state() == Account::AccountEditState::OUTDATED) {
      return KIcon("view-refresh");
   }
   return QVariant();
}

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
   const QPixmap* pxmPtr = c->photo();
   bool isTracked = displayPresence && c->isTracked();
   bool isPresent = displayPresence && c->isPresent();
   static QColor presentBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::PositiveText ).brush(QPalette::Normal).color();
   static QColor awayBrush    = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();

   QPixmap pxm;
   if (pxmPtr) {
      QPixmap contactPhoto(pxmPtr->scaledToWidth(size.height()-6));
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

QVariant KDEPixmapManipulation::callPhoto(Call* c, const QSize& size, bool displayPresence) {
   if (c->peerPhoneNumber()->contact()) {
      return contactPhoto(c->peerPhoneNumber()->contact(),size,displayPresence);
   }
   else
      return QPixmap(callStateIcons[c->state()]);
}

QVariant KDEPixmapManipulation::numberCategoryIcon(const QPixmap* p, const QSize& size, bool displayPresence, bool isPresent) {
   Q_UNUSED(size)
   if (displayPresence) {
      QPixmap pxm = p?(*p):QPixmap(KStandardDirs::locate("data" , "sflphone-client-kde/mini/call.png"));
      QPainter painter(&pxm);
      painter.setOpacity(0.3);
      painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
      painter.fillRect(pxm.rect(),isPresent?Qt::green:Qt::red);
      return pxm;
   }
   if (p)
      return *p;
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

void KDEPresenceSerializationVisitor::serialize() {
   PresenceStatusModel* m = PresenceStatusModel::instance();
   QStringList list;
   for (int i=0;i<m->rowCount();i++) {
      QString line = QString("%1///%2///%3///%4///%5")
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Name),Qt::DisplayRole).toString())
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Message),Qt::DisplayRole).toString())
         .arg(qvariant_cast<QColor>(m->data(m->index(i,(int)PresenceStatusModel::Columns::Message),Qt::BackgroundColorRole)).name())    //Color
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Status),Qt::CheckStateRole) == Qt::Checked)    //Status
         .arg(m->data(m->index(i,(int)PresenceStatusModel::Columns::Default),Qt::CheckStateRole) == Qt::Checked);   //Default
         list << line;
   }
   ConfigurationSkeleton::setPresenceStatusList(list);
}

void KDEPresenceSerializationVisitor::load() {
   QStringList list = ConfigurationSkeleton::presenceStatusList();
   PresenceStatusModel* m = PresenceStatusModel::instance();

   //Load the default one
   if (!list.size()) {
      PresenceStatusModel::StatusData* data = new PresenceStatusModel::StatusData();
      data->name       = i18n("Online")    ;
      data->message    = i18n("I am available");
      data->status     = true      ;
      data->defaultStatus = true;
      m->addStatus(data);
      data = new PresenceStatusModel::StatusData();
      data->name       = i18n("Away")    ;
      data->message    = i18n("I am away");
      data->status     = false      ;
      m->addStatus(data);
      data = new PresenceStatusModel::StatusData();
      data->name       = i18n("Busy")    ;
      data->message    = i18n("I am busy");
      data->status     = false      ;
      m->addStatus(data);
      data = new PresenceStatusModel::StatusData();
      data->name       = i18n("DND")    ;
      data->message    = i18n("Please do not disturb me");
      data->status     = false      ;
      m->addStatus(data);
   }
   else {
      foreach(const QString& line, list) {
         QStringList fields = line.split("///");
         PresenceStatusModel::StatusData* data = new PresenceStatusModel::StatusData();
         data->name          = fields[(int)PresenceStatusModel::Columns::Name   ];
         data->message       = fields[(int)PresenceStatusModel::Columns::Message];
         data->color         = QColor(fields[(int)PresenceStatusModel::Columns::Color]);
         data->status        = fields[(int)PresenceStatusModel::Columns::Status] == "1";
         data->defaultStatus = fields[(int)PresenceStatusModel::Columns::Default] == "1";
         m->addStatus(data);
      }
   }
}

KDEPresenceSerializationVisitor::~KDEPresenceSerializationVisitor()
{
   
}

bool KDEPresenceSerializationVisitor::isTracked(AbstractItemBackendBase* backend)
{
   Q_UNUSED(backend)
   if (!m_isLoaded) {
      foreach(const QString& str,ConfigurationSkeleton::presenceAutoTrackedCollections()) {
         m_hTracked[str] = true;
      }
      m_isLoaded = true;
   }
   return m_hTracked[backend->name()];
}

void KDEPresenceSerializationVisitor::setTracked(AbstractItemBackendBase* backend, bool tracked)
{
   if (!m_isLoaded) {
      foreach(const QString& str,ConfigurationSkeleton::presenceAutoTrackedCollections()) {
         m_hTracked[str] = true;
      }
      m_isLoaded = true;
   }
   m_hTracked[backend->name()] = tracked;
   QStringList ret;
   for (QHash<QString,bool>::iterator i = m_hTracked.begin(); i != m_hTracked.end(); ++i) {
      if (i.value())
         ret << i.key();
   }
   ConfigurationSkeleton::setPresenceAutoTrackedCollections(ret);
}
