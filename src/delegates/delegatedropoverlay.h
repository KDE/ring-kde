/***************************************************************************
 *   Copyright (C) 2011-2014 by Savoir-Faire Linux                         *
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
#ifndef DELEGATEDROPOVERLAY_H
#define DELEGATEDROPOVERLAY_H
#include <QtGui/QPushButton>
#include <QtGui/QPen>
#include <QtGui/QStyleOptionViewItem>
#include <QtCore/QModelIndex>

#include <lib/call.h>

class QTimer;
class QImage;

///DelegateDropOverlay: Fancy buttons for the call widget
class DelegateDropOverlay : public QObject
{
   Q_OBJECT
public:
   struct OverlayButton {
      QImage* m_pImage;
      Call::DropAction role;
      ~OverlayButton() {
         delete m_pImage;
      }
      OverlayButton(QImage* i,Call::DropAction r) : m_pImage(i),role(r) {}
   };
   //Constructor
   explicit DelegateDropOverlay(QObject* parent = nullptr);
   ~DelegateDropOverlay();

   //Setters
   void setHoverState(bool hover);
   void setButtons(QMap<QString,OverlayButton*>* buttons) {
      m_lpButtons = buttons;
   }
   virtual void paintEvent(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index);

private:
   //Attributes
   QTimer*            m_pTimer        ;
   QPen               m_Pen           ;
   bool               m_Init          ;
   char               m_Reverse       ;
   QList<QModelIndex> m_lActiveIndexes;
   QMap<QString,OverlayButton*>* m_lpButtons;

private Q_SLOTS:
   void changeVisibility();
   void slotLayoutChanged();
};

#endif
