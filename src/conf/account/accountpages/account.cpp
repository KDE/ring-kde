/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include <profilemodel.h>
#include <person.h>
#include <profile.h>

//Binder
#include "accountserializationadapter.h"

#include "basic.h"

Pages::Account::Account(QWidget *parent) : PageBase(parent)
{
   setupUi(this);

   connect(dlgProfile, &DlgProfiles::changed,[this]() {
      emit changed();
   });

   //Remove profile
   m_pPages->removeTab(m_pPages->count()-1);

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
   dlgSecurity    -> setAccount(a);

   if (m_pAdapter)
       delete m_pAdapter;

   m_pAdapter = new AccountSerializationAdapter(a, this);

   switch (a->protocol()) {
      case ::Account::Protocol::RING:
      case ::Account::Protocol::SIP:
         m_pPages->setTabEnabled(4, a->isNew() || a->id() != "IP2IP");
         break;
      case ::Account::Protocol::COUNT__:
         break;
   }
}

void Pages::Account::displayProfile(bool display)
{
   if (display) {
      while (m_pPages->count())
         m_pPages->removeTab(0);

      m_pPages->insertTab(0,dlgProfile,tr("Profile"));
   }
   else if (m_pPages->count() == 1) {
      m_pPages->removeTab(0);
      m_pPages->insertTab(0, dlgRingtone   ,tr("Ringtone"   ));
      m_pPages->insertTab(0, dlgSecurity   ,tr("Security"   ));
      m_pPages->insertTab(0, dlgCredentials,tr("Credentials"));
      m_pPages->insertTab(0, dlgCodec      ,tr("Codecs"     ));
      m_pPages->insertTab(0, dlgNetwork    ,tr("Network"    ));
      m_pPages->insertTab(0, dlgAdvanced   ,tr("Advanced"   ));
      m_pPages->insertTab(0, dlgBasic      ,tr("Basic"      ));
      m_pPages->setCurrentIndex(0);
   }
}

void Pages::Account::setProfile(Person* p)
{
   dlgProfile->loadPerson(p);
   m_lToSave << p;
}

void Pages::Account::setAccount(const QModelIndex& idx)
{

   QModelIndex prof = idx;
   QModelIndex i = idx;

   while (qobject_cast<const QSortFilterProxyModel*>(i.model()))
      i = prof = static_cast<const QSortFilterProxyModel*>(i.model())->mapToSource(i);

   if (i.model() == &ProfileModel::instance())
      i = ProfileModel::instance().mapToSource(i);


   if (!i.isValid()) {
      Profile* pr = ProfileModel::instance().getProfile(prof);

      if (pr) {
         Person* p = pr->person();

         if (p) {
            setProfile(p);
            displayProfile(true);
         }
      }
   }
   else if (::Account* a = AccountModel::instance().getAccountByModelIndex(i)) {

      displayProfile(false);
      setAccount(a);
   }
}

void Pages::Account::updateWidgets()
{
   qDebug() << "Update widgets";
   account() << ::Account::EditAction::CANCEL;
}

void Pages::Account::updateSettings()
{
   qDebug() << "Update settings";
   dlgProfile->saveToPerson();

   foreach(Person* p, m_lToSave) {
      if (p->isActive())
         p->save();
   }
}

void Pages::Account::slotUpdateButtons()
{
   qDebug() << "Update buttons";
   emit updateButtons();
}

void Pages::Account::selectAlias()
{
   QLineEdit* le = dlgBasic->lrcfg_alias;

   le->setSelection(0,le->text().size());
   le->setFocus(Qt::OtherFocusReason);
}
