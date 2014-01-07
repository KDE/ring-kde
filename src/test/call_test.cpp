/******************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                            *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
#include <QtCore/QString>
#include <QtTest/QtTest>

#include "../src/lib/dbus/configurationmanager.h"
#include "../src/lib/dbus/callmanager.h"
#include "../src/lib/dbus/instancemanager.h"
#include "../src/lib/accountlistmodel.h"
#include "../src/lib/callmodel.h"

callModel<>* m_pModel = new CallModel<>();

class CallTests: public QObject
{
   Q_OBJECT
private slots:
   void testCallWithoutAccounts();
private:

};

///When there is no accounts, no call should be created
void CallTests::testCallWithoutAccounts()
{
   QMap<Account*,bool> saveState;
   //Disable all accounts
   for (int i=0;i<AccountListModel::instance()->size();i++) {
      saveState[(*AccountListModel::instance())[i]] = (*AccountListModel::instance())[i]->isAccountEnabled();
      qDebug() << "Disabling" << (*AccountListModel::instance())[i]->accountId();
      (*AccountListModel::instance())[i]->setAccountEnabled(false);
      (*AccountListModel::instance())[i]->save();
   }

    Call* call = m_pModel->addDialingCall("test call", AccountListModel::currentAccount());
    QCOMPARE( call, (Call*)nullptr );

   //Restore state
   for (int i=0;i<AccountListModel::instance()->size();i++) {
      (*AccountListModel::instance())[i]->setAccountEnabled(saveState[(*AccountListModel::instance())[i]]);
      (*AccountListModel::instance())[i]->save();
   }
}

QTEST_MAIN(CallTests)
#include "call_test.moc"
