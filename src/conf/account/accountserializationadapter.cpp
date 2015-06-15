/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
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
#include "accountserializationadapter.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QAbstractButton>

#include <account.h>
#include <accountmodel.h>

constexpr static const char LRC_CFG[]   = "lrcfg_";
constexpr static const int  LRC_CFG_LEN = 6       ;

struct ConnHolder {
   QMetaObject::Connection c;
   ~ConnHolder();
};
Q_DECLARE_METATYPE(ConnHolder*);

ConnHolder::~ConnHolder()
{
   QObject::disconnect(c);
}

static void avoidDuplicate(QWidget* w)
{
   if (qvariant_cast<ConnHolder*>(w->property("lrcfgConn")))
      delete qvariant_cast<ConnHolder*>(w->property("lrcfgConn"));
}

/**
 * This check for some supported widgets and bind the widgets and property
 */
static void setupWidget(QWidget* w, Account* a, const QHash<QByteArray, int>& roles)
{
   if (w->objectName().left(LRC_CFG_LEN) == LRC_CFG) {
      const QByteArray prop = w->objectName().mid(LRC_CFG_LEN, 999).toLatin1();
      if (roles.contains(prop)) {
         const int role = roles[prop];
         if (qobject_cast<QLineEdit*>(w)) {
            QLineEdit* le = qobject_cast<QLineEdit*>(w);
            avoidDuplicate(le);
            le->setText(a->roleData(role).toString());
            ConnHolder* c = new ConnHolder {
               QObject::connect(le, &QLineEdit::textChanged, [a,role](const QString& text) {
                  if (a->roleData(role) != text)
                     a->setRoleData(role, text);
               })
            };
            le->setProperty("lrcfgConn",QVariant::fromValue(c));
         }
         else if (qobject_cast<QSpinBox*>(w)) {
            QSpinBox* sb = qobject_cast<QSpinBox*>(w);
            avoidDuplicate(sb);
            sb->setValue(a->roleData(role).toInt());
            ConnHolder* c = new ConnHolder {
               QObject::connect(sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [a,role](int value) {
                  if (a->roleData(role).toInt() != value)
                     a->setRoleData(role, value);
               })
            };
            sb->setProperty("lrcfgConn",QVariant::fromValue(c));
         }
         else if  (qobject_cast<QAbstractButton*>(w)) {
            //QCheckBox, QRadioButton, QToolButton, QPushButton
            QAbstractButton* b = qobject_cast<QAbstractButton*>(w);
            avoidDuplicate(b);
            b->setChecked(a->roleData(role).toBool());
            ConnHolder* c = new ConnHolder {
               QObject::connect(b, &QAbstractButton::toggled,[a,role](bool c) {
                  if (a->roleData(role).toBool() != c)
                     a->setRoleData(role, c);
               })
            };
            b->setProperty("lrcfgConn",QVariant::fromValue(c));
         }

         //Check if the field is required for this account type
         if (a->roleState((Account::Role)role) == Account::RoleState::UNAVAILABLE) {

            w->setProperty("lrcfgVisible", w->isVisible() ? 2 : 1);
            w->setVisible(false);

            QFormLayout* fm = qobject_cast<QFormLayout*>(w->parentWidget()->layout());

            //There is many of corner case here, this only handle the one that's
            //created by Qt Designer
            if (!fm) {
               QLayoutItem* il = w->parentWidget()->layout()->itemAt(0);
               if (il && il->layout())
                  fm = qobject_cast<QFormLayout*>(il->layout());
            }

            if (fm) {
               QWidget* buddy = fm->labelForField(w);

               if (buddy)
                  buddy->setVisible(false);

            }
         }
         else {
            //0 = unset, 1=invisible, 2=visible
            const int oldVisibleState = w->property("lrcfgVisible").toInt();
            if (oldVisibleState)
               w->setVisible(oldVisibleState-1);
         }
      }
      else {
         qWarning() << "Unknown config properties" << w->objectName();
      }
   }
}

static void clearConnections(QWidget* w)
{
   if (w->objectName().left(LRC_CFG_LEN) == LRC_CFG) {
      avoidDuplicate(w);
   }
}

static void drill(QWidget* w, Account* a, const QHash<QByteArray, int>& roles, bool clear = false)
{
   for (QObject *object : w->children()) {
      if (!object->isWidgetType())
         continue;

      QWidget* w2 = static_cast<QWidget*>(object);
      if (clear)
         clearConnections(w2);
      else
         setupWidget(w2, a, roles);

      drill(w2, a, roles);
   }
}

AccountSerializationAdapter::AccountSerializationAdapter(Account* a, QWidget* w) : QObject(w)
{
   static QHash<QByteArray, int> reverse;
   if (reverse.isEmpty()) {
      const QHash<int, QByteArray> a = AccountModel::instance()->roleNames();
      for (QHash<int, QByteArray>::const_iterator i = a.begin(); i != a.end(); ++i) {
         reverse[i.value()] = i.key();
      }
   }

   drill(w, a, reverse);
}

AccountSerializationAdapter::~AccountSerializationAdapter()
{

}