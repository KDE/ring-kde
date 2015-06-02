/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
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

#include "dlgaccount.h"

#include "accountpages/account.h"

#include <QtGui/QPainter>

DlgAccount::DlgAccount(QWidget* parent) : QWidget(parent)
{
   setupUi(this);

   Pages::Account* acc = new Pages::Account("ring/account789.ini",this);
   QHBoxLayout* l = new QHBoxLayout(m_pPanel);
   l->addWidget(acc);
   m_lPages["account456.ini"] = acc;

}

DlgAccount::~DlgAccount()
{
}

void DlgAccount::cancel()
{

}

bool DlgAccount::hasChanged()
{
   return false;
}

void DlgAccount::updateSettings()
{

}

void DlgAccount::updateWidgets()
{

}

#include "dlgaccount.moc"
