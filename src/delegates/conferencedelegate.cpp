/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
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
#include "conferencedelegate.h"

//Qt
#include <QtCore/QSize>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QTreeWidget>
#include <QtGui/QGraphicsEffect>
#include <QtGui/QGraphicsOpacityEffect>
#include <QtGui/QApplication>

//KDE
#include <KStandardDirs>
#include <KLineEdit>

//SFLPhone
#include "../widgets/categorizedtreeview.h"
#include "sflphone.h"
#include "../lib/call.h"

///Constructor
ConferenceDelegate::ConferenceDelegate(CategorizedTreeView* widget,QPalette pal)
      : QStyledItemDelegate(widget) , m_tree(widget) , m_Pal(pal),
      m_LeftMargin(7),m_RightMargin(7),m_pCallDelegate(nullptr)
{
}

///Guess the size of the item
QSize ConferenceDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
   QSize sh = QStyledItemDelegate::sizeHint(option, index);

   //HACK make absolutely sure the editor (if any) is still required
   if (m_tree->indexWidget(index)) {
      Call* call = qvariant_cast<Call*>(index.data(Call::Role::Object));
      if (call && call->state() != Call::State::DIALING) {
         m_tree->closeEditor(m_tree->indexWidget(index),QAbstractItemDelegate::NoHint);
      }
   }

   if (index.parent().isValid()) {
      sh = m_pCallDelegate->sizeHint(option,index);
   }
   else if (!index.child(0,0).isValid()) {
      sh = m_pCallDelegate->sizeHint(option,index);
   }
   else {
      sh.setHeight(categoryHeight(index,option,&m_Pal));
   }

   //Bottom margin
   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid())
      sh.rheight() += 15;
   return sh;
}

///Generate the rectangle
QRect ConferenceDelegate::fullCategoryRect(const QStyleOptionViewItem& option, const QModelIndex& index) const 
{
   QModelIndex i(index),old(index);
   //BEGIN real sizeHint()
   //Otherwise it would be called too often (thanks to valgrind)
   const_cast<ConferenceDelegate*>(this)->m_SH = QStyledItemDelegate::sizeHint(option, index);
   if (!index.parent().isValid() && index.child(0,0).isValid()) {
      ((QSize)m_SH).rheight() += 2 * m_LeftMargin;
   } else {
      ((QSize)m_SH).rheight() += m_LeftMargin;
   }
   ((QSize)m_SH).rwidth() += m_LeftMargin;
   //END real sizeHint()

   if (i.parent().isValid()) {
      i = i.parent();
   }

   //Avoid repainting the category over and over (optimization)
   //note: 0,0,0,0 is actually wrong, but it wont change anything for this use case
//    if (i != old && old.row()>2)
//       return QRect(0,0,0,0);

   QRect r;
   if (option.state & QStyle::State_Small) //Hack to know when this is a drag popup
      r = option.rect;
   else
      r = m_tree->visualRect(i);
   // adapt width
   r.setLeft(m_LeftMargin);
   r.setWidth(m_tree->viewport()->width() - m_LeftMargin - m_RightMargin);

   // adapt height
   const int childCount = m_tree->model()->rowCount(i);
   if (m_tree->isExpanded(i) && childCount > 0) {
      //There is a massive implact on CPU usage to have massive rect
      r.setHeight(r.height() + sizeHint(option,i.child(0,0)).height()*childCount+2*15); //2*15 = margins
   }
   r.setTop(r.top() + m_LeftMargin);

   return r;
} //fullCategoryRect

///Paint the delegate
void ConferenceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   Q_ASSERT(index.isValid());

   QStyleOptionViewItem opt(option);
   //BEGIN: draw toplevel items
   if (!index.parent().isValid() && index.child(0,0).isValid()) {
      const QRegion cl = painter->clipRegion();
      painter->setClipRect(opt.rect);
      opt.rect = fullCategoryRect(option, index);
      drawCategory(index, 0, opt, painter,&m_Pal);

      //Draw the conference icon and info
      static const QPixmap* pxm = nullptr;
      if (!pxm) //Static
         pxm = new QPixmap(KStandardDirs::locate("data","sflphone-client-kde/conf-small.png"));
      painter->drawPixmap ( opt.rect.x()+5, opt.rect.y()+2, 24, 24, *pxm);
      QFont font = painter->font();
      font.setBold(true);

      static QColor baseColor = Qt::red;
      /*static block*/if (baseColor == Qt::red) {
         QColor textColor = QApplication::palette().text().color();
         baseColor = QApplication::palette().base().color().name();
         baseColor.setBlue (baseColor.blue() + (textColor.blue() -baseColor.blue()) *0.6);
         baseColor.setRed  (baseColor.red()  + (textColor.red()  -baseColor.red())  *0.6);
         baseColor.setGreen(baseColor.green()+ (textColor.green()-baseColor.green())*0.6);
      }
      painter->setPen(baseColor);
      painter->setFont(font);
      painter->drawText(opt.rect.x()+33,opt.rect.y()+font.pointSize()+8,"Conference");
      font.setBold(false);
      painter->setFont(font);
      baseColor.setAlpha(150);
      painter->setPen(baseColor);
      baseColor.setAlpha(255);
      static QFontMetrics* fm = nullptr;
      if (!fm) {
         fm = new QFontMetrics(painter->font());
      }
      const QString len = index.data(Call::Role::Length).toString();
      painter->drawText(opt.rect.x()+opt.rect.width()-fm->width(len),opt.rect.y()+font.pointSize()+8,len);
      painter->setClipRegion(cl);
      return;
   }

   if (!index.parent().parent().isValid()) {
      opt.rect = fullCategoryRect(option, index);
      const QRegion cl = painter->clipRegion();
      QRect cr = option.rect;
      if (index.column() == 0) {
         if (m_tree->layoutDirection() == Qt::LeftToRight) {
            cr.setLeft(5);
         } else {
            cr.setRight(opt.rect.right());
         }
      }
      painter->setClipRect(cr);

      //Box background
      if (index.parent().isValid())
         drawCategory(index, 0, opt, painter,&m_Pal);

      painter->setClipRegion(cl);
      painter->setRenderHint(QPainter::Antialiasing, false);
   }

   //END: draw background of category for all other items

   QStyleOptionViewItem opt2(option);
   if (index.parent().isValid())
      opt2.rect.setWidth(opt2.rect.width()-15);
   painter->setClipRect(option.rect);

   //Check if it is the last item
   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid()) {
      opt2.rect.setHeight(opt2.rect.height()-15);
      m_pCallDelegate->paint(painter,opt2,index);
   }
   else if (index.parent().isValid() || (!index.parent().isValid() && !index.child(0,0).isValid())) {
      m_pCallDelegate->paint(painter,opt2,index);
   }

   if (index.parent().isValid() && !index.parent().child(index.row()+1,0).isValid()) {
//       QStyleOptionViewItem opt5(option);
//       opt5.rect.setLeft(leftMargin());
//       opt5.rect.setWidth(m_tree->viewport()->width() - leftMargin() - rightMargin());
      painter->setClipRect(opt.rect);
      drawBoxBottom(index, 0, option, painter);
   }
} //paint

void ConferenceDelegate::drawCategory(const QModelIndex& index, int sortRole, const QStyleOption& option, QPainter* painter, const QPalette* pal) const
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
void ConferenceDelegate::drawBoxBottom(const QModelIndex &index, int sortRole, const QStyleOption &option, QPainter *painter,const QPalette* pal) const {
   Q_UNUSED(index)
   Q_UNUSED(sortRole)
   painter->setClipping(false);
   const QPalette* palette = (pal)?pal:&option.palette  ;
   painter->setRenderHint(QPainter::Antialiasing);
   QColor outlineColor = palette->text().color();
   outlineColor.setAlphaF(0.35);
   painter->setPen(outlineColor);

   //BEGIN: bottom horizontal line
   {
   QPoint bl = option.rect.bottomLeft();
   bl.setY(bl.y());
   bl.setX(m_LeftMargin+3);

   painter->fillRect(QRect(bl, QSize(option.rect.width()+4,1)), outlineColor);
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

   //BEGIN: bottom left corner
   {
      QRectF arc;
      QPointF br(option.rect.bottomRight());
      br.setY(br.y()-4);
      br.setX(m_LeftMargin);
      arc = QRectF(br, QSizeF(4, 4));
      arc.translate(0.5, 0.5);
      painter->drawArc(arc, 1440*2, 1440);
   }
   //END: bottom left corner
   painter->setClipping(true);
} //drawBoxBottom

///Return the height of the conference box
int ConferenceDelegate::categoryHeight(const QModelIndex &index, const QStyleOption &option,const QPalette* pal) const
{
   Q_UNUSED( index  );
   Q_UNUSED( option );
   Q_UNUSED( pal    );
   QFont font(QApplication::font());
   font.setBold(true);
   const QFontMetrics fontMetrics = QFontMetrics(font);

   return fontMetrics.height() + 2 + 16 /* vertical spacing */;
}

///Draw the delegate when it is being dragged
QPixmap ConferenceDelegate::getDragPixmap(CategorizedTreeView* parent, const QModelIndex& index)
{
   QStyleOptionViewItemV4 option;
   option.locale = parent->locale();
   option.widget = parent;
   option.state = QStyle::State_Selected | QStyle::State_Enabled | QStyle::State_Active | QStyle::State_Small;
   option.rect = QRect(0,0,parent->width()-(index.parent().isValid()?15:0),parent->height());
   QSize size = parent->itemDelegate()->sizeHint(option,index);
   QSize itemSize = size;
   const int rowCount = parent->model()->rowCount(index);
   for (int i=0;i<rowCount;i++) {
      size.setHeight(size.height()+parent->itemDelegate()->sizeHint(option,index.child(i,0)).height());
   }
   if (index.parent().isValid() && index.model()->rowCount(index.parent()) -1 == index.row()) {
      size = parent->itemDelegate()->sizeHint(option,index.parent().child(0,0));
   }

   //Setup the painter
   QPixmap pixmap(parent->width()-(index.parent().isValid()?15:0),size.height());
   QPainter customPainter(&pixmap);
   customPainter.eraseRect(option.rect);
   customPainter.setCompositionMode(QPainter::CompositionMode_Clear);
   customPainter.fillRect(option.rect,QBrush(Qt::white));
   customPainter.setCompositionMode(QPainter::CompositionMode_SourceOver);

   //Draw the parent
   option.rect = QRect(0,0,parent->width(),itemSize.height());
   if (rowCount)
      option.rect.setHeight(pixmap.height()-3);
   parent->itemDelegate()->paint(&customPainter, option, index);
   option.rect = QRect(0,0,parent->width(),itemSize.height());

   //Draw the children
   for (int i=0;i<rowCount;i++) {
      itemSize.setHeight(parent->itemDelegate()->sizeHint(option,index.child(i,0)).height());
      option.rect = QRect(10,option.rect.y()+option.rect.height(),parent->width()-20,itemSize.height());
      option.state = QStyle::State_Enabled | QStyle::State_Active | QStyle::State_Small;
      parent->itemDelegate()->paint(&customPainter, option, index.child(i,0));
   }
   return pixmap;
}

///Create an editor widget
QWidget* ConferenceDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   Q_UNUSED(option)
   KLineEdit* ed = new KLineEdit(parent);
   ed->setStyleSheet(QString("QLineEdit { background-color:transparent;border:0px;color:white;font-weight:bold;padding-left:%1 }").arg(option.rect.height()));
   ed->setAutoFillBackground(false);
   ed->setProperty("call",index.data(Call::Role::Object));
   connect(ed,SIGNAL(textChanged(QString)),this,SLOT(slotTextChanged(QString)));
   connect(ed,SIGNAL(returnPressed()),this,SLOT(slotReturnPressed()));
   ed->deselect();

   return ed;
}

///Update line edit text when in dialing mode
void ConferenceDelegate::setEditorData(QWidget * editor, const QModelIndex & index ) const
{
   KLineEdit* ed = qobject_cast<KLineEdit*>(editor);
   if (ed) {
      const QString text = index.data(Qt::EditRole).toString();
      //If the text was typed while the editor has focus, ed->text() == text
      //this check is require to preserve the cursor and avoid "blinking"
      if (ed->text() != text) {
         ed->setText(text);
         ed->deselect();
      }
   }
}

///Update call PhoneNumber when leaving edit mode
void ConferenceDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index ) const
{
   KLineEdit* ed = qobject_cast<KLineEdit*>(editor);
   if (index.data(Call::Role::CallState) != static_cast<int>(Call::State::DIALING)) {
      ed->setVisible(false);
      emit const_cast<ConferenceDelegate*>(this)->closeEditor(editor,NoHint);
   }

   if (ed)
      model->setData(index,ed->text(),Qt::EditRole);
}

///Intercept KLineEdit show() call, to deselect the text (as it is selected automagically)
bool ConferenceDelegate::eventFilter(QObject *obj, QEvent *event)
{
   Q_UNUSED(obj)
   if (event->type() == QEvent::Show) {
      KLineEdit* ed = qobject_cast<KLineEdit*>(obj);
      if (ed)
         ed->deselect();
   }
   else if (event->type() == QEvent::MouseButtonDblClick || event->type() == QEvent::MouseButtonPress) {
      KLineEdit* ed = qobject_cast<KLineEdit*>(obj);
      if (ed) {
         QObject* obj= qvariant_cast<Call*>(ed->property("call"));
         Call* call  = nullptr;
         if (obj)
            call = qobject_cast<Call*>(obj);
         if (call) {
            if (event->type() == QEvent::MouseButtonDblClick)
               call->performAction(Call::Action::ACCEPT);
            else if (event->type() == QEvent::MouseButtonPress)
               m_tree->selectionModel()->setCurrentIndex(CallModel::instance()->getIndex(call),QItemSelectionModel::SelectCurrent);
         }
      }
   }
   return false;
}

///Update the model text as soon as
void ConferenceDelegate::slotTextChanged(const QString& text)
{
   KLineEdit* ed = qobject_cast<KLineEdit*>(QObject::sender());
   if (ed) {
      ed->deselect();
      QObject* obj= qvariant_cast<Call*>(ed->property("call"));
      Call* call  = nullptr;
      if (obj)
         call = qobject_cast<Call*>(obj);
      if (call && call->state() != Call::State::DIALING) {
         emit closeEditor(ed);
      }
      if (call && call->dialNumber() != text) {
         if (text.left(text.size()-1) == call->dialNumber()) {
            call->playDTMF(text.right(1));
         }
         call->setDialNumber(text);
      }
      else if (!call) {
         emit closeEditor(ed);
      }
   }
   else {
      emit closeEditor(ed);
   }
} //slotTextChanged

void ConferenceDelegate::slotReturnPressed()
{
   KLineEdit* ed = qobject_cast<KLineEdit*>(QObject::sender());
   if (ed) {
      emit closeEditor(ed);
   }
}
