/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                           *
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
#include "ringtonedelegate.h"

#include <QtGui/QTableView>
#include <QtGui/QPushButton>
#include <QtCore/QDebug>

#include <KIcon>

#include <lib/ringtonemodel.h>

///RingToneListItem: Ringtone list widget
class RingToneListItem : public QPushButton
{
   Q_OBJECT
   friend class DlgAccounts;
public:
   ///Constructor
   RingToneListItem(const QModelIndex& idx) : QPushButton(nullptr),m_Idx(idx) {
      setIcon(KIcon("media-playback-start"));
      setCheckable(true);
      connect(this,SIGNAL(clicked()),this,SLOT(playRingtone()));
   }
private Q_SLOTS:
   ///Play the ringtone file when the button is clicked
   void playRingtone() {
      RingToneModel* m =  const_cast<RingToneModel*>(qobject_cast<const RingToneModel*>(m_Idx.model()));
      if (m) {
         m->play(m_Idx);
      }
   }
private:
   QModelIndex m_Idx;
};

RingToneDelegate::RingToneDelegate(QTableView* view) : QStyledItemDelegate(view),m_pParent(view){}

void RingToneDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   QStyledItemDelegate::paint(painter,option,index);
   if (index.column() == 1) {
      if (!m_pParent->indexWidget(index))
         m_pParent->setIndexWidget(index,new RingToneListItem(index));
      qobject_cast<QPushButton*>(m_pParent->indexWidget(index))->setChecked(index.data(RingToneModel::Role::IsPlaying).toBool());
   }
}

RingToneDelegate::~RingToneDelegate()
{
}

#include "ringtonedelegate.moc"
#include "moc_ringtonedelegate.cpp"
