/***************************************************************************
 *   Copyright (C) 2009 by Rafael Fernández López <ereslibre@kde.org>      *
 *                                                                         *
 * This library is free software; you can redistribute it and/or           *
 * modify it under the terms of the GNU Library General Public             *
 * License version 2 as published by the Free Software Foundation.         *
 *                                                                         *
 * This library is distributed in the hope that it will be useful,         *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * Library General Public License for more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

// this code is taken from SystemSettings/icons/CategoryDrawer.{h,cpp}
// Rafael agreet to relicense it under LGPLv2 or LGPLv3, just as we need it,
// see: http://lists.kde.org/?l=kwrite-devel&m=133061943317199&w=2

#include "conferencebox.h"

#include <QPainter>
#include <QApplication>
#include <QStyleOption>
#include <QModelIndex>
#include <QDebug>

///Constructor
ConferenceBox::ConferenceBox()
{
   setLeftMargin ( 7 );
   setRightMargin( 7 );
}

///Draw a conference box
void ConferenceBox::drawCategory(const QModelIndex&  index   ,
                                 int                 sortRole,
                                 const QStyleOption& option  ,
                                 QPainter*           painter ,
                                 const QPalette* pal) const
{
   Q_UNUSED( sortRole )
   Q_UNUSED( index    )
   const QPalette* palette   = (pal)?pal:&option.palette  ;
   painter->setRenderHint(QPainter::Antialiasing);

   const QRect optRect = option.rect;

   //BEGIN: decoration gradient
   {
      QPainterPath path(optRect.bottomLeft());

      path.lineTo(QPoint(optRect.topLeft().x(), optRect.topLeft().y() - 3));
      const QPointF topLeft(optRect.topLeft());
      QRectF arc(topLeft, QSizeF(4, 4));
      path.arcTo(arc, 180, -90);
      path.lineTo(optRect.topRight());
      path.lineTo(optRect.bottomRight());
      path.lineTo(optRect.bottomLeft());

      QColor window(palette->window().color());
      const QColor base(palette->base().color());

      window.setAlphaF(option.state & QStyle::State_Selected?0.9:0.9);

      QColor window2(window);
      window2.setAlphaF(option.state & QStyle::State_Selected?0.4:0.4);

      QLinearGradient decoGradient1;
      decoGradient1.setStart(optRect.topLeft());
      decoGradient1.setFinalStop(optRect.bottomLeft());
      decoGradient1.setColorAt(0, window);
      decoGradient1.setColorAt(1, Qt::transparent);

      QLinearGradient decoGradient2;
      decoGradient2.setStart(optRect.topLeft());
      decoGradient2.setFinalStop(optRect.topRight());
      decoGradient2.setColorAt(0, window2);
      decoGradient2.setColorAt(1, Qt::transparent);

      painter->fillPath(path, decoGradient1);
      painter->fillRect(optRect, decoGradient2);
   }
   //END: decoration gradient

   {
      QRect newOptRect(optRect);

      newOptRect.translate(1, 1);

      //BEGIN: inner top left corner
      {
         painter->save();
         painter->setPen(palette->base().color());
         const QPointF topLeft(newOptRect.topLeft());
         QRectF arc = QRectF(topLeft, QSizeF(4, 4));
         arc.translate(0.5, 0.5);
         painter->drawArc(arc, 1440, 1440);
         painter->restore();
      }
      //END: inner top left corner

      //BEGIN: inner left vertical line
      {
         QPoint start = newOptRect.topLeft();
         QPoint verticalGradBottom = newOptRect.topLeft();
         start.ry() += 3;
         verticalGradBottom.ry() += newOptRect.height() - 3;
         QLinearGradient gradient(start, verticalGradBottom);
         gradient.setColorAt(0, palette->base().color());
         gradient.setColorAt(1, Qt::transparent);
         painter->fillRect(QRect(start, QSize(1, newOptRect.height() - 3)), gradient);
      }
      //END: inner left vertical line

      //BEGIN: top inner horizontal line
      {
         QPoint start = newOptRect.topLeft();
         QPoint horizontalGradTop = newOptRect.topLeft();
         start.rx() += 3;
         horizontalGradTop.rx() += newOptRect.width() - 3;
         QLinearGradient gradient(start, horizontalGradTop);
         gradient.setColorAt(0, palette->base().color());
         gradient.setColorAt(1, Qt::transparent);
         QSize rectSize = QSize(newOptRect.width() - 30, 1);
         painter->fillRect(QRect(start, rectSize), gradient);
      }
      //END: top inner horizontal line
   }

   QColor outlineColor = palette->text().color();
   outlineColor.setAlphaF(0.35);

   //BEGIN: top left corner
   {
      painter->save();
      painter->setPen(outlineColor);
      QRectF arc;
      const QPointF topLeft(optRect.topLeft());
      arc = QRectF(topLeft, QSizeF(4, 4));
      arc.translate(0.5, 0.5);
      painter->drawArc(arc, 1440, 1440);
      painter->restore();
   }
   //END: top left corner

   //BEGIN: top right corner
   {
      painter->save();
      painter->setPen(outlineColor);
      QPointF topRight(optRect.topRight());
      topRight.rx() -= 3;
      QRectF arc = QRectF(topRight, QSizeF(4, 4));
      arc.translate(-0.5, 0.5);
      painter->drawArc(arc, 0, 1440);
      painter->restore();
   }
   //END: top right corner

   //BEGIN: left vertical line
   {
      QPoint start = optRect.topLeft();
      QPoint verticalGradBottom = optRect.topLeft();
      start.ry() += 3;
      verticalGradBottom.ry() += optRect.height() - 3 + 200;
      painter->fillRect(QRect(start, QSize(1, optRect.height() - 21)), outlineColor);
   }
   //END: left vertical line

   //BEGIN: right vertical line
   {
      QPoint start = optRect.topRight();
      QPoint verticalGradBottom = optRect.topRight();
      start.ry() += 3;
      verticalGradBottom.ry() += optRect.height() - 3 + 200;
      painter->fillRect(QRect(start, QSize(1, optRect.height() - 21)), outlineColor);
   }
   //END: right vertical line

   //BEGIN: horizontal line
   {
      QPoint start = optRect.topLeft();
      QPoint horizontalGradTop = optRect.topLeft();
      start.rx() += 3;
      horizontalGradTop.rx() += optRect.width() - 3;
      QLinearGradient gradient(start, horizontalGradTop);
      gradient.setColorAt(0, outlineColor);
      gradient.setColorAt(1, outlineColor);
      QSize rectSize = QSize(optRect.width() - 6, 1);
      painter->fillRect(QRect(start, rectSize), gradient);
   }
   //END: horizontal line
} //drawCategory

///Draw the bottom border of the box
void ConferenceBox::drawBoxBottom(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter,const QPalette* pal) const {
   Q_UNUSED(index)
   Q_UNUSED(sortRole)
   const QPalette* palette = (pal)?pal:&option.palette  ;
   painter->setRenderHint(QPainter::Antialiasing);
   QColor outlineColor = palette->text().color();
   outlineColor.setAlphaF(0.35);
   painter->setPen(outlineColor);

   //BEGIN: bottom horizontal line
   {
   QPoint bl = option.rect.bottomLeft();
   bl.setY(bl.y());
   bl.setX(0);

   painter->fillRect(QRect(bl, QSize(option.rect.width()+4+10,1)), outlineColor);
   }
   //END: bottom horizontal line

   //BEGIN: bottom right corner
   {
      QRectF arc;
      QPointF br(option.rect.bottomRight());
      br.setY(br.y()-4);
      br.setX(br.x()-12);
      arc = QRectF(br, QSizeF(4, 4));
      arc.translate(0.5, 0.5);
      painter->drawArc(arc, 4320, 1440);
   }
   //END: bottom right corner
} //drawBoxBottom

///Return the height of the conference box
int ConferenceBox::categoryHeight(const QModelIndex &index, const QStyleOption &option,const QPalette* pal) const
{
   Q_UNUSED( index  );
   Q_UNUSED( option );
   Q_UNUSED( pal    );
   QFont font(QApplication::font());
   font.setBold(true);
   const QFontMetrics fontMetrics = QFontMetrics(font);

   return fontMetrics.height() + 2 + 16 /* vertical spacing */;
}
