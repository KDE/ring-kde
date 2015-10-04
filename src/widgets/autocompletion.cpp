/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QScrollBar>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtCore/QEvent>
#include <QtGui/QResizeEvent>

//KDE
#include <klocalizedstring.h>

//Ring
#include <numbercompletionmodel.h>
#include <call.h>
#include <callmodel.h>
#include <delegates/autocompletiondelegate2.h>
#include "klib/kcfg_settings.h"

AutoCompletion::AutoCompletion(QTreeView* parent) : ResizableTip(parent),m_Height(125)
{
   m_Height = ConfigurationSkeleton::autoCompletionHeight();
   setVisible(true);

   m_pLabel = new QLabel(this);
   m_pLabel->setText(i18n("Use ⬆ up and ⬇ down arrows to select one of these numbers"));
   m_pLabel->setStyleSheet(QString("color:%1;font-weight:bold;").arg(QApplication::palette().base().color().name()));
   m_pLabel->setWordWrap(true);
   m_pView = new QListView(this);
   addWidget(m_pLabel);
   addWidget(m_pView);

   m_pModel = new NumberCompletionModel();
   m_pModel->setDisplayMostUsedNumbers(true);
   m_pView->setModel(m_pModel);
   m_pView->setSelectionModel(m_pModel->selectionModel());

   connect(m_pModel,SIGNAL(layoutChanged()),this, SLOT(slotLayoutChanged()));
   connect(m_pView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotDoubleClicked(QModelIndex)));

   if (parent) {
      connect(CallModel::instance()->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectionChanged(QModelIndex)));
      parent->installEventFilter(this);
      QResizeEvent r(size(),size());
      eventFilter(nullptr,&r);
   }

   setMinimumSize(0,m_Height);
   m_pDelegate = new AutoCompletionDelegate2();
   m_pView->setItemDelegate(m_pDelegate);
   selectionChanged(CallModel::instance()->selectionModel()->currentIndex());

   connect(this, &ResizableTip::heightChanged, [](int h) {
      ConfigurationSkeleton::setAutoCompletionHeight(h);
   });
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

void AutoCompletion::callSelectedNumber()
{
   m_pModel->callSelectedNumber();
}

void AutoCompletion::setUseUnregisteredAccounts(bool value)
{
   m_pModel->setUseUnregisteredAccounts(value);
}

void AutoCompletion::slotLayoutChanged()
{
   if (!m_pModel->rowCount())
      m_pView->selectionModel()->setCurrentIndex(QModelIndex(),QItemSelectionModel::Clear);
}

void AutoCompletion::slotCallStateChanged(Call::State s)
{
   Q_UNUSED(s)
   Call* call = m_pModel->call();

   setVisible(call && call->lifeCycleState() == Call::LifeCycleState::CREATION);
}

void AutoCompletion::selectionChanged(const QModelIndex& idx)
{
   if (!idx.isValid()) {
      setCall(nullptr);
      return;
   }

   Call* call = CallModel::instance()->getCall(idx);
   if (call && call->lifeCycleState() == Call::LifeCycleState::CREATION) {
      setCall(call);
   }
   else {
      setCall(nullptr);
   }
}

void AutoCompletion::setCall(Call* call)
{
   Call* old = m_pModel->call();

   if (call == old)
      return;

   if (old) {
      disconnect(old, &Call::stateChanged, this, &AutoCompletion::slotCallStateChanged);
   }


   m_pModel->setCall(call);

   if (call) {
      connect(call, &Call::stateChanged, this, &AutoCompletion::slotCallStateChanged);
      slotCallStateChanged(call->state());
   }
   else
      setVisible(false);
}

Call* AutoCompletion::call() const
{
   return m_pModel->call();
}

ContactMethod* AutoCompletion::selection() const
{
   if (isVisible()) {
      const QModelIndex idx = m_pView->selectionModel()->currentIndex();
      if (idx.isValid()) {
         return m_pModel->number(idx);
      }
   }
   return nullptr;
}

void AutoCompletion::slotDoubleClicked(const QModelIndex& idx)
{
   Q_UNUSED(idx)
   emit doubleClicked(selection());
}

void AutoCompletion::reset()
{
   m_pView->selectionModel()->clear();
   setCall(nullptr);
}

#include "autocompletion.moc"
