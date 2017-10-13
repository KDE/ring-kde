/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
 *   Author : Alexandre Lision <alexandre.lision@savoirfairelinux.com>      *
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
#ifndef DLGPROFILES_H
#define DLGPROFILES_H

#include <QQuickWidget>

#include <QtCore/QObject>
#include <QtCore/QObject>

class CollectionInterface;
class Person;
class QQmlEngine;

class DlgProfiles final : public QQuickWidget
{
Q_OBJECT
public:
   //Constructor
   explicit DlgProfiles(QWidget *parent = nullptr, QQmlEngine* e = nullptr, const QString& name = QString(), const QString& uri = QString());

   //Destructor
   virtual ~DlgProfiles();

   bool checkValues();

private:
   Person* m_pCurrentPerson;
   bool m_Lock {false};

public Q_SLOTS:
   void loadPerson(Person* p);
   bool saveToPerson(Person* p = nullptr);
   Person* create(CollectionInterface* col);
   void cancel();
   void accept();

private Q_SLOTS:
   void slotChanged();

Q_SIGNALS:
   void requestSave();
   void requestCancel();
   void changed();
};
#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
