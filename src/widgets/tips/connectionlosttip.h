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
#ifndef CONNECTIONLOSTTIP_H
#define CONNECTIONLOSTTIP_H

#include <klib/tip.h>

//Qt
#include <QtGui/QWidget>
#include <QtCore/QTimer>


//Qt
class QPainter;

///A tip to be passed to the TipLoader
class ConnectionLostTip : public Tip
{
   Q_OBJECT
public:
   explicit ConnectionLostTip(QWidget* parent = nullptr);
   virtual ~ConnectionLostTip();

protected:
   virtual QRect getDecorationRect();
   virtual void paintDecorations(QPainter& p, const QRect& textRect);
   virtual QRect getTextRect(const QString& text);

private:
   QTimer*   m_pTimer     ;
   uchar     m_Counter    ;
   QImage    m_RenderCache;

private Q_SLOTS:
   void startAnimation(bool visibility);
   void timeout();
};

#endif
