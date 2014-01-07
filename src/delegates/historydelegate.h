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
#ifndef HISTORYDELEGATE_H
#define HISTORYDELEGATE_H

#include <QtGui/QStyledItemDelegate>
#include "delegatedropoverlay.h"

class QTreeView;
class DelegateDropOverlay;
class TipAnimationWrapper;
class Tip;

class HistoryDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
   explicit HistoryDelegate(QTreeView* parent = nullptr);
   virtual ~HistoryDelegate();

   QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

   virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
private:
   QTreeView* m_pParent;
   DelegateDropOverlay* m_pDelegatedropoverlay;
   TipAnimationWrapper* m_AnimationWrapper;
   Tip* m_pRingingTip;
   QMap<QString,DelegateDropOverlay::OverlayButton*> historyMap,callMap;
private slots:
   void slotStopRingingAnimation();
};

#endif
