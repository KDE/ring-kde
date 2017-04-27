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

#include <QtWidgets/QWidget>
#include <QtCore/QObject>
#include <QtCore/QObject>
#include "ui_dlgprofiles.h"

class CollectionInterface;
class Person;

class DlgProfiles : public QWidget, public Ui_DlgProfiles
{
Q_OBJECT
public:
   //Constructor
   explicit DlgProfiles(QWidget *parent = nullptr, const QString& name = QString(), const QString& uri = QString());

   //Destructor
   virtual ~DlgProfiles();

   bool checkValues();

private:
   QHash<QString, QLineEdit*> m_hCustomFields;
   Person* m_pCurrentPerson;

public Q_SLOTS:
   void loadPerson(Person* p);
   bool saveToPerson(Person* p = nullptr);
   Person* create(CollectionInterface* col);
   void cancel();
   void accept();
   void choosePhoto();
   void addCustomField();

private Q_SLOTS:
   void slotChanged();

Q_SIGNALS:
   void requestSave();
   void requestCancel();
   void changed();
};
#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
