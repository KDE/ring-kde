/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#include "dlggeneral.h"

//Qt
#include <QtWidgets/QToolButton>
#include <QtWidgets/QAction>
#include <QtGui/QIcon>

//KDE
#include <kmessagebox.h>
#include <klocalizedstring.h>

//Ring
#include "klib/kcfg_settings.h"
#include "callhistorymodel.h"
#include "session.h"

///Constructor
DlgGeneral::DlgGeneral(KConfigDialog *parent)
 : QWidget(parent),m_HasChanged(false)
{
//    setupUi(this);
//    connect(toolButton_historyClear, &QAbstractButton::clicked, this, &DlgGeneral::slotClearCallHistoryAsked);
//    const bool isLimited = Session::instance()->historyModel()->isHistoryLimited();
//    m_pKeepHistory->setChecked(!isLimited);
//    m_pHistoryMax ->setEnabled(isLimited );
//
//    m_pHistoryMax->setValue(Session::instance()->historyModel()->historyLimit());
//    m_HasChanged = false;
}

///Destructor
DlgGeneral::~DlgGeneral()
{
}

///Have this dialog changed
bool DlgGeneral::hasChanged()
{
   return m_HasChanged;
}

///Tag this dialog as changed
void DlgGeneral::slotChanged()
{
   m_HasChanged = true;
   emit updateButtons();
}

///Update all widgets
void DlgGeneral::updateWidgets()
{
}

///Save current settings
void DlgGeneral::updateSettings()
{
//    Session::instance()->historyModel()->setHistoryLimited(!m_pKeepHistory->isChecked());
//
//    if (!m_pKeepHistory->isChecked())
//       Session::instance()->historyModel()->setHistoryLimit(m_pHistoryMax->value());
//
//    m_HasChanged = false;
}

void DlgGeneral::slotClearCallHistoryAsked()
{
//    Session::instance()->historyModel()->clearAllCollections();
}

// kate: space-indent on; indent-width 3; replace-tabs on;
