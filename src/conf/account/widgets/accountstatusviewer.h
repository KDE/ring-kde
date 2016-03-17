/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#ifndef ACCOUNTSTATUSVIEWER_H
#define ACCOUNTSTATUSVIEWER_H

#include <QtWidgets/QDialog>
#include "typedefs.h"

//Qt
class QTableView;
class QWidget;

//Ring
class Account;

class LIB_EXPORT AccountStatusViewer : public QDialog
{
   Q_OBJECT
public:
   explicit AccountStatusViewer(Account* a , QWidget* parent = nullptr);
   virtual ~AccountStatusViewer();

private:
   QTableView* m_pView;
};

#endif
