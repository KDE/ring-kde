/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
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

//KDE
#include <KLocale>

//SFLPhone
#include <lib/numbercompletionmodel.h>
#include <lib/call.h>
#include <lib/callmodel.h>

constexpr static const int TOOLBAR_HEIGHT = 72;
constexpr static const int MARGINS        = 15;

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
   connect(m_pModel,SIGNAL(enabled(bool)),this,SLOT(setVisible(bool)));
   if (parent) {
      connect(parent->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex)));
      parent->installEventFilter(this);
   }
   setMinimumSize(0,125);
}

AutoCompletion::~AutoCompletion()
{
   delete m_pView;
   delete m_pLabel;
}

void AutoCompletion::moveUp()
{
   
}

void AutoCompletion::moveDown()
{
   
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
   customPainter.setBrush(QBrush(brightOrDarkBase()?Qt::black:Qt::white));
   customPainter.drawRoundedRect(0,0,width(),height(),10,10);
   customPainter.setRenderHint(QPainter::Antialiasing, true);
   QWidget::paintEvent(event);
}

bool AutoCompletion::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize) {
      const QWidget* p = parentWidget();
      int vOffset(0),wOffset(0);

      if (p) {
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
