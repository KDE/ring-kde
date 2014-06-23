/******************************************************************************
 *   Copyright (C) 2014 by Savoir-Faire Linux                                 *
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

#include "../lib/phonedirectorymodel.h"
#include "../lib/phonenumber.h"
#include "../lib/numbercompletionmodel.h"


// #include "../src/lib/dbus/configurationmanager.h"
// #include "../src/lib/dbus/callmanager.h"
// #include "../src/lib/dbus/instancemanager.h"
// #include "../src/lib/accountlistmodel.h"
// #include "../src/lib/callmodel.h"
/*
callModel<>* m_pModel = new CallModel<>();*/

QDebug operator<<(QDebug dbg, const PhoneNumber* c)
{
   dbg.nospace() << QString(c->uri());
   return dbg.space();
}



class AutoCompletionTest: public NumberCompletionModel
{
   Q_OBJECT
public:
   typedef QMap<QString,PhoneDirectoryModel::NumberWrapper*> NbWrap;
private slots:
   void testGetRange();
   void testGetRange_data();
private:
   QString getMapContent(AutoCompletionTest::NbWrap* map)
   {
      QString ret ='\'';
      foreach(PhoneDirectoryModel::NumberWrapper* wrap,*map)
         ret += ' ' + wrap->numbers[0]->uri();
      return ret+'\'';
   }
   QString getSetContent(QSet<PhoneNumber*> set)
   {
      QString ret ='\'';
      foreach(PhoneNumber* n,set)
         ret += ' ' + n->uri();
      return ret+'\'';
   }
};
Q_DECLARE_METATYPE(AutoCompletionTest::NbWrap*)
// Q_DECLARE_METATYPE(NbWrap*)

//We mainly want to make sure it doesn't crash or go into an infinite loop
void AutoCompletionTest::testGetRange()
{
   QFETCH( QVariant , data  );
   QFETCH(QString   , prefix);
   AutoCompletionTest::NbWrap* realData = data.value<AutoCompletionTest::NbWrap*>();
   //qDebug() << "Values are" << prefix << *realData;

   //Check if there is a result
   QSet<PhoneNumber*> set;
   getRange(PhoneDirectoryModel::instance()->m_hSortedNumbers,prefix,set);
   if (realData->size()) {
      QVERIFY2(set.size() == realData->size(), QString("Prefix: %1 Result: %2 %3 Data: %4 Result: ").arg(prefix).arg(set.size()).arg(realData->size()).arg(getMapContent(realData)).arg(getSetContent(set)).toAscii());
   }
   else {
      QCOMPARE(set.size() == 0, true);
   }
//    QCOMPARE( QString("123") == prefix, true );
}

void AutoCompletionTest::testGetRange_data()
{
   QTest::addColumn< QVariant >( "data"   );
   QTest::addColumn< QString  >( "prefix" );

   PhoneDirectoryModel* ins = PhoneDirectoryModel::instance();

   QVariant val = QVariant::fromValue(&ins->m_lSortedNames );
   QTest::newRow(QString("SortedName set"+QString::number(1)).toAscii()) << val << "123";

   //1) Test if it worj with only 1 value
   //2) See if it still work with multiple values
   QString prefix = "";
   for (int i =1;i<10;i++) {
      prefix += QString::number(i);
      QVariant val2 = QVariant::fromValue(&ins->m_hSortedNumbers );
//       QTest::newRow(QString("SortedNumber set (b)"+QString::number(1)).toAscii()) << val2 << prefix;
      ins->getNumber(prefix);
      QTest::newRow(QString("SortedNumber set (a)"+QString::number(1)).toAscii()) << val2 << prefix;
   }
}

QTEST_MAIN(AutoCompletionTest)
#include "autocompletion_test.moc"
