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
#include "filterlineedit.h"


FilterLineEdit::FilterLineEdit(QWidget *parent) : KLineEdit(parent)
{
   connect(this,SIGNAL(textChanged(QString)),this,SLOT(slotTextChanged(QString)));
}

FilterLineEdit::~FilterLineEdit() {}

void FilterLineEdit::slotTextChanged(const QString& text)
{
   QString normStripppedC;
   foreach(QChar char2,text.toLower().normalized(QString::NormalizationForm_KD) ) {
      if (!char2.combiningClass())
         normStripppedC += char2;
   }
   emit filterStringChanged(normStripppedC);
}
