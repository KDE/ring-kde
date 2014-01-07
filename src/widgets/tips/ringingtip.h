/****************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                          *
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
#ifndef RINGINGTIP_H
#define RINGINGTIP_H

//Base
#include "../../klib/tip.h"

//Qt
#include <QtGui/QWidget>
#include <QtCore/QTimer>

class QSvgRenderer;

//Qt
class QPainter;

///A tip to be passed to the TipLoader
class RingingTip : public Tip
{
   Q_OBJECT
public:
   explicit RingingTip(QWidget* parent = nullptr);
   virtual ~RingingTip();

   //It is not a notification, it doesn't need to be as visible
   virtual float opacity() { return 0.5f; }

protected:
   virtual QRect getDecorationRect();
   virtual void paintDecorations(QPainter& p, const QRect& textRect);

private:
   QTimer*       m_pTimer           ;
   uchar         m_Counter          ;
   QByteArray    m_phoneOriginalSVG ;
   QByteArray    m_ring1OriginalSVG ;
   QByteArray    m_ring2OriginalSVG ;
   QByteArray    m_ring3OriginalSVG ;
   QSvgRenderer* m_pPhoneR          ;
   QSvgRenderer* m_pRing1R          ;
   QSvgRenderer* m_pRing2R          ;
   QSvgRenderer* m_pRing3R          ;
   QPixmap*      m_pPhonePix        ;
   QPixmap*      m_pRing1Pix        ;
   QPixmap*      m_pRing2Pix        ;
   QPixmap*      m_pRing3Pix        ;

private Q_SLOTS:
   void startAnimation(bool visibility);
   void timeout();
};

#endif
