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

//Qt
#include <QtCore/QTimer>

//KDE
#include <KLocalizedString>

//Ring
#include <account.h>
#include <accountmodel.h>
#include <profilemodel.h>
#include <person.h>

//Binder
#include "conf/accountserializationadapter.h"

#include "basic.h"

#include "dlgprofiles.h"

Pages::Account::Account(QWidget *parent) : PageBase(parent)
{
   setupUi(this);

   //Remove profile
   m_pPages->removeTab(m_pPages->count()-1);

   //Remove Wizard
   m_pPages->removeTab(m_pPages->count()-1);

   // Don't ask
   QTimer::singleShot(0, [this]() {
      m_pPages->setCurrentIndex(0);
   });
}

void Pages::Account::setEngine(QQmlEngine* e)
{
   m_pEngine = e;
   auto l = new QHBoxLayout(dlgProfileHolder);
   m_pProfile = new DlgProfiles(dlgProfileHolder, e);
   l->addWidget(m_pProfile);
   connect(m_pProfile, &DlgProfiles::changed, this, [this]() {
      emit changed();
   });
}

void Pages::Account::setAccount(::Account* a)
{
   PageBase::setAccount(a);

   dlgBasic       -> setAccount(a);
   dlgAdvanced    -> setAccount(a);
   dlgNetwork     -> setAccount(a);
   dlgCodec       -> setAccount(a);
   dlgDevices     -> setAccount(a);
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

      m_pPages->insertTab(0, m_pProfile,i18n("Profile"));
   }
   else if (m_pPages->count() == 1) {
      m_pPages->removeTab(0);
      m_pPages->insertTab(0, dlgRingtone   ,i18n("Ringtone"   ));
      m_pPages->insertTab(0, dlgSecurity   ,i18n("Security"   ));
      m_pPages->insertTab(0, dlgCredentials,i18n("Credentials"));
      m_pPages->insertTab(0, dlgCodec      ,i18n("Codecs"     ));
      m_pPages->insertTab(0, dlgDevices    ,i18n("Devices"    ));
      m_pPages->insertTab(0, dlgNetwork    ,i18n("Network"    ));
      m_pPages->insertTab(0, dlgAdvanced   ,i18n("Advanced"   ));
      m_pPages->insertTab(0, dlgBasic      ,i18n("Basic"      ));
      m_pPages->setCurrentIndex(0);
   }
}

void Pages::Account::setProfile(Person* p)
{
   m_pProfile->loadPerson(p);
   m_lToSave << p;
}

void Pages::Account::setAccount(const QModelIndex& idx)
{
   auto prof = idx;

   while (qobject_cast<const QSortFilterProxyModel*>(prof.model()))
      prof = static_cast<const QSortFilterProxyModel*>(prof.model())->mapToSource(prof);

   if (auto a = ProfileModel::instance().getAccount(prof)) {
      displayProfile(false);
      setAccount(a);
   }
   else if (auto pr = ProfileModel::instance().getProfile(prof)) {
      setProfile(pr);
      displayProfile(true);
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
   m_pProfile->saveToPerson();

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

// kate: space-indent on; indent-width 3; replace-tabs on;
