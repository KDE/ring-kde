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
#include "autocompletion.h"

//Qt
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollBar>
#include <QtGui/QPainter>
#include <QtGui/QApplication>
#include <QtCore/QEvent>
#include <QtGui/QResizeEvent>

//KDE
#include <KLocale>

//SFLPhone
#include <lib/numbercompletionmodel.h>
#include <lib/call.h>
#include <lib/callmodel.h>
#include <delegates/autocompletiondelegate.h>

static const int TOOLBAR_HEIGHT = 72;
static const int MARGINS        = 15;

AutoCompletion::AutoCompletion(QTreeView* parent) : QWidget(parent)
{
   setVisible(false);
   QVBoxLayout* l = new QVBoxLayout(this);
   m_pLabel = new QLabel(this);
   m_pLabel->setText(i18n("Use ⬆ up and ⬇ down arrows to select one of these numbers"));
   m_pLabel->setStyleSheet(QString("color:%1;font-weight:bold;").arg(QApplication::palette().base().color().name()));
   m_pLabel->setWordWrap(true);
   m_pView = new QListView(this);
   l->addWidget(m_pLabel);
   l->addWidget(m_pView);

   m_pModel = new NumberCompletionModel();
   m_pView->setModel(m_pModel);

   connect(m_pModel,SIGNAL(enabled(bool))  ,this, SLOT(slotVisibilityChange(bool))   );
   connect(m_pModel,SIGNAL(layoutChanged()),this, SLOT(slotLayoutChanged()));
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotDoubleClicked(QModelIndex)));

   if (parent) {
      connect(parent->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex)));
      parent->installEventFilter(this);
      QResizeEvent r(size(),size());
      eventFilter(nullptr,&r);
   }
   setMinimumSize(0,125);
   m_pDelegate = new AutoCompletionDelegate();
   m_pView->setItemDelegate(m_pDelegate);
}

AutoCompletion::~AutoCompletion()
{
   m_pView->setItemDelegate(nullptr);
   delete m_pDelegate;
   delete m_pView;
   delete m_pLabel;
}

void AutoCompletion::moveUp()
{
   const QModelIndex idx = m_pView->selectionModel()->currentIndex();
   if (idx.isValid() ) {
      if (idx.row() != 0)
         m_pView->selectionModel()->setCurrentIndex(m_pModel->index(idx.row()-1,0),QItemSelectionModel::ClearAndSelect);
      else
         m_pView->selectionModel()->setCurrentIndex(QModelIndex(),QItemSelectionModel::Clear);
   }
   else
      m_pView->selectionModel()->setCurrentIndex(m_pModel->index(0,0),QItemSelectionModel::ClearAndSelect);
}

void AutoCompletion::moveDown()
{
   const QModelIndex idx = m_pView->selectionModel()->currentIndex();
   if (idx.isValid() ) {
      if (idx.row() != m_pModel->rowCount()-1)
         m_pView->selectionModel()->setCurrentIndex(m_pModel->index(idx.row()+1,0),QItemSelectionModel::ClearAndSelect);
   }
   else
      m_pView->selectionModel()->setCurrentIndex(m_pModel->index(0,0),QItemSelectionModel::ClearAndSelect);
}

void AutoCompletion::setUseUnregisteredAccounts(bool value) {
   m_pModel->setUseUnregisteredAccounts(value);
}

void AutoCompletion::slotLayoutChanged()
{
   if (!m_pModel->rowCount())
      m_pView->selectionModel()->setCurrentIndex(QModelIndex(),QItemSelectionModel::Clear);
}

void AutoCompletion::selectionChanged(const QModelIndex& idx)
{
   if (!idx.isValid()) {
      setCall(nullptr);
      return;
   }

   Call* call = CallModel::instance()->getCall(idx);
   if (call && call->state() == Call::State::DIALING)
      setCall(call);
   else
      setCall(nullptr);
}

void AutoCompletion::setCall(Call* call)
{
   m_pModel->setCall(call);
}

Call* AutoCompletion::call() const
{
   return m_pModel->call();
}

PhoneNumber* AutoCompletion::selection() const
{
   if (isVisible()) {
      const QModelIndex idx = m_pView->selectionModel()->currentIndex();
      if (idx.isValid()) {
         return m_pModel->number(idx);
      }
   }
   return nullptr;
}

bool AutoCompletion::brightOrDarkBase()
{
   const QColor color = palette().base().color();
   return (color.red() > 128 && color.green() > 128 && color.blue() > 128);
}

void AutoCompletion::paintEvent(QPaintEvent* event)
{
   QPainter customPainter(this);
   customPainter.setOpacity(0.1);
   customPainter.setPen(Qt::NoPen);
   customPainter.setRenderHint(QPainter::Antialiasing, true);
   customPainter.setBrush(QBrush(brightOrDarkBase()?Qt::black:Qt::white));
   customPainter.drawRoundedRect(0,0,width(),height(),10,10);
   QWidget::paintEvent(event);
}

bool AutoCompletion::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize) {
      const QWidget* p = parentWidget();

      if (p) {
         int vOffset(0),wOffset(0);
         //If the parent has scrollbar, take this into account
         if (p->inherits("QAbstractScrollArea")) {
            const QAbstractScrollArea* scrl = static_cast< const QAbstractScrollArea*>(p);
            if (scrl && scrl->horizontalScrollBar()->isVisible())
               vOffset += scrl->horizontalScrollBar()->height();
            if (scrl && scrl->verticalScrollBar()->isVisible())
               wOffset += scrl->verticalScrollBar()->width();
         }
         resize(p->width()-wOffset-2*MARGINS,height());
         move(MARGINS,p->height()-TOOLBAR_HEIGHT-vOffset - MARGINS - height());
      }
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}

void AutoCompletion::slotVisibilityChange(bool visible)
{
   if (!visible && ((!m_pModel->call()) || (m_pModel->call()->state() != Call::State::DIALING)))
      m_pModel->setCall(nullptr);
   emit requestVisibility(visible,m_pModel->call()!=nullptr);
}

void AutoCompletion::slotDoubleClicked(const QModelIndex& idx)
{
   qDebug() << "double clicked" << idx;
   emit doubleClicked(selection());
}

void AutoCompletion::reset()
{
   m_pView->selectionModel()->clear();
   setCall(nullptr);
}
