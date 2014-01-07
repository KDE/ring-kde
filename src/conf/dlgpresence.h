/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         ***
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

#ifndef DLGPRESENCE_H
#define DLGPRESENCE_H

#include <QWidget>

#include "ui_dlgpresence.h"

class TipManager;
class Tip;

class DlgPresence : public QWidget, public Ui_DlgPresence
{
Q_OBJECT
public:
   //Constructor
   explicit DlgPresence(QWidget *parent = nullptr);

   //Destructor
   virtual ~DlgPresence();

   //Getter
   bool hasChanged();

private:

   TipManager* m_pTipManager         ;
   Tip*        m_pTip                ;
   bool        m_Changed             ;

public Q_SLOTS:
   //Mutator
   void updateWidgets();
   void updateSettings();

private Q_SLOTS:
   void slotChanged();

private Q_SLOTS:
   void slotRemoveRow();
   void slotMoveUp   ();
   void slotMoveDown ();

Q_SIGNALS:
   ///Emitted when the buttons need to be updated
   void updateButtons();
};

#endif
