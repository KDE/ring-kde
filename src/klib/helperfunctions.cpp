/****************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#include "helperfunctions.h"

//Qt
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtGui/QFontMetrics>

//KDE
#include <KLocale>
#include <KMessageBox>

//SFLPhone
#include "../lib/contact.h"
#include "../lib/phonenumber.h"
#include "../lib/numbercategory.h"

///Remove accent and upper caps, try to stay ascii as much as possible
QString HelperFunctions::normStrippped(QString str)
{
   QString normStripppedC;
   foreach(QChar char2,str.toLower().normalized(QString::NormalizationForm_KD) ) {
      if (!char2.combiningClass())
         normStripppedC += char2;
   }
   return normStripppedC;
}

///Escape lesser and greater
QString HelperFunctions::escapeHtmlEntities(QString str)
{
   while (str.indexOf('<') != -1) {
      str = str.replace('<',"&lt;");
   }
   while (str.indexOf('>') != -1) {
      str = str.replace('>',"&gt;");
   }
   return str;
}

///Display a message box
void HelperFunctions::displayNoAccountMessageBox(QWidget* parent)
{
   KMessageBox::error(parent,i18n("No registered accounts"));
}
