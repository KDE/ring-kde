/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
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
#ifndef FILTERLINEEDIT_H
#define FILTERLINEEDIT_H

#include <KLineEdit>

///FilterLineEdit: An internalional filter string
class FilterLineEdit : public KLineEdit
{
Q_OBJECT

public:
    explicit FilterLineEdit(QWidget *parent = nullptr);
    ~FilterLineEdit();

private Q_SLOTS:
   void slotTextChanged(const QString& text);

Q_SIGNALS:
   ///When the text change
   void filterStringChanged(const QString& string);
};

#endif
