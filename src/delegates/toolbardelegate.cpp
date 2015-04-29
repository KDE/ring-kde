/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
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
#include "toolbardelegate.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtGui/QPainter>
#include <QtWidgets/QStylePainter>
#include <QtCore/QDebug>

ToolbarDelegate::ToolbarDelegate(QWidget* parent) : QStyledItemDelegate(parent),m_pParent(parent)
{

}

// static QStyleOptionToolButton fillOptions(const QStyleOptionViewItem & option,QStyleOptionViewItem opt)
// {
//    QStyleOptionToolButton opt2;
// 
//    opt2.direction  = option.direction  ;
//    opt2.fontMetrics= option.fontMetrics;
//    opt2.styleObject= option.styleObject;
//    opt2.version    = option.version    ;
// 
//    opt2.toolButtonStyle = Qt::ToolButtonTextUnderIcon;
// 
//    opt2.features = QStyleOptionToolButton::None;
// 
//    opt2.icon    = opt.icon;
//    opt2.iconSize= QSize(32,32);
//    opt2.text    = opt.text;
//    opt2.state   = option.state;
//    opt2.rect    = opt.rect;
//    opt2.palette = opt.palette;
// 
//    return opt2;
// }

void ToolbarDelegate::paint(QPainter* painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
   Q_ASSERT(index.isValid());
   Q_UNUSED(painter)

   //Init
   static const QColor hoverBg = QApplication::palette().highlight().color();
   static const QColor textCol = QColor("#F2F2F2"); //Same as the icons
   static       QPen   checked ( hoverBg );
   static bool init = false;
   if (!init) {
      checked.setWidth(3);
      init = true;
   }

   painter->setRenderHint(QPainter::Antialiasing,true);

   //Background
   if (option.state & QStyle::State_MouseOver) {
      painter->setPen(Qt::transparent);
      painter->setBrush(hoverBg);
      painter->drawRoundedRect(option.rect,7,7);
   }

   if (option.checkState == Qt::Checked) {
      painter->setBrush(Qt::transparent);
      painter->setPen(hoverBg);
      painter->drawRoundedRect(option.rect,7,7);
   }

   //Text
   const QString text = index.data(Qt::DisplayRole).toString();
   int x = option.rect.x()+option.rect.width()/2-option.fontMetrics.width(text)/2;
   int y = option.rect.y()+option.rect.height()-5;
   painter->setPen(textCol);
   painter->drawText(x,y,text);

   //Icon
   const QPixmap pxm = index.data(Qt::DecorationRole).value<QIcon>().pixmap(24,24);
   x = option.rect.x()+option.rect.width()/2- 24/2;
   y = option.rect.y()+5;
   painter->drawPixmap(QRect(x,y,24,24),pxm);
}

QSize ToolbarDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   Q_UNUSED(option)
   Q_UNUSED(index)
   /*QStyleOptionViewItem opt = option;

   initStyleOption(&opt, index);

   QStyleOptionToolButton opt2 = fillOptions(option,opt);

   QStyle *style = m_pParent ? m_pParent->style() : QApplication::style();

   QRect geom = style->subElementRect(QStyle::SE_PushButtonContents, &opt2, m_pParent);

   return style->sizeFromContents(QStyle::CT_ItemViewItem, &opt, QSize(), m_pParent);*/
   return QSize(100,100);
}

ToolbarDelegate::~ToolbarDelegate()
{
}

