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
#include <QtWidgets/QAbstractButton>

#include <account.h>
#include <accountmodel.h>

constexpr static const char LRC_CFG[]   = "lrcfg_";
constexpr static const int  LRC_CFG_LEN = 6       ;

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
            le->setText(a->roleData(role).toString());
            QObject::connect(le, &QLineEdit::textChanged, [a,role](const QString& text) {
               if (a->roleData(role) != text)
                  a->setRoleData(role, text);
            });
         }
         else if (qobject_cast<QSpinBox*>(w)) {
            QSpinBox* sb = qobject_cast<QSpinBox*>(w);
            sb->setValue(a->roleData(role).toInt());
            QObject::connect(sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [a,role](int value) {
               if (a->roleData(role).toInt() != value)
                  a->setRoleData(role, value);
            });
         }
         else if  (qobject_cast<QAbstractButton*>(w)) {
            //QCheckBox, QRadioButton, QToolButton, QPushButton
            QAbstractButton* b = qobject_cast<QAbstractButton*>(w);
            b->setChecked(a->roleData(role).toBool());
            QObject::connect(b, &QAbstractButton::toggled,[a,role](bool c) {
               if (a->roleData(role).toBool() != c)
                  a->setRoleData(role, c);
            });
         }
      }
      else {
         qWarning() << "Unknown config properties" << w->objectName();
      }
   }
}

static void drill(QWidget* w, Account* a, const QHash<QByteArray, int>& roles)
{
   for (QObject *object : w->children()) {
      if (!object->isWidgetType())
         continue;

      QWidget* w2 = static_cast<QWidget*>(object);
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