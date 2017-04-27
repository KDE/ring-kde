/***************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
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
#ifndef CONTACTMETHODSELECTOR_H
#define CONTACTMETHODSELECTOR_H

#include "ui_contactmethodselector.h"

#include <QtWidgets/QDialog>

class QSortFilterProxyModel;

class ContactMethod;

class ContactMethodSelector : public QDialog, public Ui_ContactMethodSelector
{
   Q_OBJECT
public:
   explicit ContactMethodSelector(QWidget* parent = nullptr);
   virtual ~ContactMethodSelector() = default;

private:
   QSortFilterProxyModel* m_pSortedContacts {nullptr};
   QSortFilterProxyModel* m_pNearMatchContact {nullptr};

};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
