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
#include "account.h"

//Ring
#include <account.h>
#include <accountmodel.h>

//Binder
#include "accountserializationadapter.h"

#include "basic.h"

Pages::Account::Account(QWidget *parent) : PageBase(parent)
{
//    setAccount(a);
   setupUi(this);

//    setAccount(a);
//    dlgSecurity

}

void Pages::Account::setAccount(::Account* a)
{
   PageBase::setAccount(a);

   dlgBasic       -> setAccount(a);
   dlgAdvanced    -> setAccount(a);
   dlgNetwork     -> setAccount(a);
   dlgCodec       -> setAccount(a);
   dlgCredentials -> setAccount(a);
   dlgRingtone    -> setAccount(a);

   new AccountSerializationAdapter(a, this);
}

void Pages::Account::setAccount(const QModelIndex& idx)
{
   if (::Account* a = AccountModel::instance()->getAccountByModelIndex(idx))
      setAccount(a);
}

void Pages::Account::updateWidgets()
{
   qDebug() << "Update widgets";
   account() << ::Account::EditAction::CANCEL;
}

void Pages::Account::updateSettings()
{
   qDebug() << "Update settings";
}

void Pages::Account::slotUpdateButtons()
{
   qDebug() << "Update buttons";
   emit updateButtons();
}

void Pages::Account::selectAlias()
{
   QLineEdit* le  = dlgBasic->lrcfg_alias;

   le->setSelection(0,le->text().size());
   le->setFocus(Qt::OtherFocusReason);
}