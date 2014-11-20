/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         ***
 *   Author : Alexandre Lision <alexandre.lision@savoirfairelinux.com> *
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

#include <QWidget>
#include <QObject>
#include <QtCore/QObject>
#include "ui_dlgprofil.h"

class DlgProfiles : public QWidget, public Ui_DlgProfiles
{
Q_OBJECT
public:
   //Constructor
   explicit DlgProfiles(QWidget *parent = nullptr);

   //Destructor
   virtual ~DlgProfiles();

   bool checkValues();

public Q_SLOTS:
   void accept();
   void cancel();
   void choosePhoto();
   void addCustomField();
};
#endif
