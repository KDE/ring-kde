/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
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
#include "accountserializationadapter.h"

//Qt
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QAbstractItemView>

//KDE
#include <KUrlRequester>
#include <kcolorscheme.h>

#include <account.h>
#include <accountmodel.h>

QHash<int, QString> AccountSerializationAdapter::m_hProblems = {};

constexpr static const char LRC_CFG[]   = "lrcfg_";
constexpr static const int  LRC_CFG_LEN = 6       ;

struct ConnHolder final{
   ConnHolder(const QMetaObject::Connection& _c) : c(_c) {}
   QMetaObject::Connection c;
   ConnHolder(const ConnHolder&) = delete;
   ~ConnHolder();
};
Q_DECLARE_METATYPE(ConnHolder*);

ConnHolder::~ConnHolder()
{
   QObject::disconnect(c);
}

static void avoidDuplicate(QWidget* w, QList<ConnHolder*>& holders )
{
    Q_UNUSED(w)
    Q_UNUSED(holders)
    //FIXME this will need a better design
//    if (qvariant_cast<ConnHolder*>(w->property("lrcfgConn"))) {
//       auto c = qvariant_cast<ConnHolder*>(w->property("lrcfgConn"));
//       delete c;
//    }
}

QWidget* buddyWidget(QWidget* w)
{
   if (w->property("lrcfgBuddy").canConvert<QWidget*>())
      return qvariant_cast<QWidget*>(w->property("lrcfgBuddy"));

   QFormLayout* fm = qobject_cast<QFormLayout*>(w->parentWidget()->layout());

   //There is many of corner case here, this only handle the one that's
   //created by Qt Designer
   if (!fm) {
      QLayoutItem* il = w->parentWidget()->layout()->itemAt(0);
      if (il && il->layout())
         fm = qobject_cast<QFormLayout*>(il->layout());
   }

   if (fm)
      return fm->labelForField(w);

   return nullptr;
}

void AccountSerializationAdapter::updateProblemList(int role, Account::RoleStatus status, QWidget* buddy)
{
   Q_UNUSED(buddy)
   switch(status) {
      case Account::RoleStatus::OK            :
         if (m_hProblems.contains(role))
            m_hProblems.remove(role);
         break;
      case Account::RoleStatus::UNTESTED      :
         m_hProblems[role] = QStringLiteral("UNTESTED");
         break;
      case Account::RoleStatus::INVALID       :
         m_hProblems[role] = QStringLiteral("INVALID");
         break;
      case Account::RoleStatus::REQUIRED_EMPTY:
         m_hProblems[role] = QStringLiteral("REQUIRED_EMPTY");
         break;
      case Account::RoleStatus::OUT_OF_RANGE  :
         m_hProblems[role] = QStringLiteral("OUT_OF_RANGE");
         break;
   }
}

/**
 * This check for some supported widgets and bind the widgets and property
 */
void AccountSerializationAdapter::setupWidget(QWidget* w, Account* a, const QHash<QByteArray, int>& roles)
{
   if (w->objectName().left(LRC_CFG_LEN) == QLatin1String(LRC_CFG)) {

      static KStatefulBrush errorBrush( KColorScheme::View, KColorScheme::NegativeText );

      QByteArray prop = w->objectName().mid(LRC_CFG_LEN, 999).toLatin1();

      if (roles.contains(prop)) {
         const int role = roles[prop];
         const Account::RoleState rs = a->roleState((Account::Role)role);

         if (qobject_cast<QLineEdit*>(w)) {
            QLineEdit* le = qobject_cast<QLineEdit*>(w);
            avoidDuplicate(le, m_lConns);
            le->setText(a->roleData(role).toString());
            le->setReadOnly(rs == Account::RoleState::READ_ONLY);
            ConnHolder* c = new ConnHolder(
               QObject::connect(le, &QLineEdit::textChanged, [a,role,le](const QString& text) {
                  if (a->roleData(role) != text)
                     a->setRoleData(role, text);

                  const Account::RoleStatus rstatus = a->roleStatus((Account::Role)role);
                  QPalette pal = QApplication::palette();
                  static QPalette palOrig = QApplication::palette();
                  pal.setBrush(QPalette::Base,rstatus != Account::RoleStatus::OK ?
                     errorBrush.brush(QPalette::Normal) : palOrig.base()
                  );

                  le->setPalette(pal);
               })
            );
            le->setProperty("lrcfgConn",QVariant::fromValue(c));
            m_lConns << c;
         }
         else if (qobject_cast<QSpinBox*>(w)) {
            QSpinBox* sb = qobject_cast<QSpinBox*>(w);
            avoidDuplicate(sb, m_lConns);
            sb->setValue(a->roleData(role).toInt());
            ConnHolder* c = new ConnHolder {
               QObject::connect(sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [a,role](int value) {
                  if (a->roleData(role).toInt() != value)
                     a->setRoleData(role, value);
               })
            };
            sb->setProperty("lrcfgConn",QVariant::fromValue(c));
            m_lConns << c;
         }
         else if  (qobject_cast<QAbstractButton*>(w)) {
            //QCheckBox, QRadioButton, QToolButton, QPushButton
            QAbstractButton* b = qobject_cast<QAbstractButton*>(w);
            avoidDuplicate(b, m_lConns);
            b->setChecked(a->roleData(role).toBool());
            ConnHolder* c = new ConnHolder {
               QObject::connect(b, &QAbstractButton::toggled,[a,role](bool ck) {
                  if (a->roleData(role).toBool() != ck)
                     a->setRoleData(role, ck);
               })
            };
            b->setProperty("lrcfgConn",QVariant::fromValue(c));
            m_lConns << c;
         }
         else if  (qobject_cast<QGroupBox*>(w)) {
            QGroupBox* b = qobject_cast<QGroupBox*>(w);
            avoidDuplicate(b, m_lConns);
            b->setCheckable(rs == Account::RoleState::READ_WRITE);
            b->setChecked(a->roleData(role).toBool());
            ConnHolder* c = new ConnHolder {
               QObject::connect(b, &QGroupBox::toggled,[a,role](bool ck) {
                  if (a->roleData(role).toBool() != ck)
                     a->setRoleData(role, ck);
               })
            };
            b->setProperty("lrcfgConn",QVariant::fromValue(c));
            m_lConns << c;
         }
         else if  (qobject_cast<QAbstractItemView*>(w)) {
            QAbstractItemView* v = qobject_cast<QAbstractItemView*>(w);
            avoidDuplicate(v, m_lConns);
            if (a->roleData(role).canConvert<QAbstractItemModel*>())
               v->setModel(qvariant_cast<QAbstractItemModel*>(a->roleData(role)));
         }
         else if  (qobject_cast<KUrlRequester*>(w)) { //KDE only
            KUrlRequester* b = qobject_cast<KUrlRequester*>(w);
            avoidDuplicate(b, m_lConns);
            b->setText(a->roleData(role).toString());
            ConnHolder* c = new ConnHolder {
               QObject::connect(b, &KUrlRequester::urlSelected,[a,role](const QUrl& s) {
                  if (a->roleData(role).toString() != s.path())
                     a->setRoleData(role, s.path());
               })
            };
            b->setProperty("lrcfgConn",QVariant::fromValue(c));
            m_lConns << c;
         }
         else {
            qDebug() << "Unsupported widget type" << w;
         }

         //Check if the field is required for this account type
         if (rs == Account::RoleState::UNAVAILABLE) {

            w->setVisible(false);

            QFormLayout* fm = qobject_cast<QFormLayout*>(w->parentWidget()->layout());

            //There is many of corner case here, this only handle the one that's
            //created by Qt Designer
            if (!fm) {
               QLayoutItem* il = w->parentWidget()->layout()->itemAt(0);
               if (il && il->layout())
                  fm = qobject_cast<QFormLayout*>(il->layout());
            }

            QWidget* buddy = buddyWidget(w);

            if (buddy) {
               buddy->setVisible(false);
               w->setProperty("buddy", QVariant::fromValue(buddy));
            }

         }
         else {
            w->setVisible(true);
            if (w->property("buddy").canConvert<QWidget*>())
               qvariant_cast<QWidget*>(w->property("buddy"))->setVisible(true);
         }
      }
      else if (prop.right(5) == "Buddy") {
         prop = prop.left(prop.size()-5);
         const int role = roles[prop];
         const Account::RoleState rs = a->roleState((Account::Role)role);
         w->setVisible(rs != Account::RoleState::UNAVAILABLE);
      }
      else if (prop.right(6) == "Buddy2") { //HACK
         prop = prop.left(prop.size()-6);
         const int role = roles[prop];
         const Account::RoleState rs = a->roleState((Account::Role)role);
         w->setVisible(rs != Account::RoleState::UNAVAILABLE);
      }
      else {
         qWarning() << "Unknown config properties" << w->objectName();
      }
   }
}

static void clearConnections(QWidget* w, QList<ConnHolder*>& m_lConns)
{
   if (w->objectName().left(LRC_CFG_LEN) == LRC_CFG) {
      avoidDuplicate(w, m_lConns);
   }
}

void AccountSerializationAdapter::drill(QWidget* w, Account* a, const QHash<QByteArray, int>& roles, bool clear)
{
   for (QObject *object : w->children()) {
      if (!object->isWidgetType())
         continue;

      QWidget* w2 = static_cast<QWidget*>(object);
      if (clear)
         clearConnections(w2, m_lConns);
      else
         setupWidget(w2, a, roles);

      drill(w2, a, roles);
   }
}

AccountSerializationAdapter::AccountSerializationAdapter(Account* a, QWidget* w) : QObject(w)
{
   static QHash<QByteArray, int> reverse;
   if (reverse.isEmpty()) {
      const QHash<int, QByteArray> roles = AccountModel::instance().roleNames();
      for (QHash<int, QByteArray>::const_iterator i = roles.begin(); i != roles.end(); ++i) {
         reverse[i.value()] = i.key();
      }
   }

   drill(w, a, reverse);
}

AccountSerializationAdapter::~AccountSerializationAdapter()
{
    foreach(ConnHolder* c, m_lConns)
        delete c;
}

// kate: space-indent on; indent-width 3; replace-tabs on;
