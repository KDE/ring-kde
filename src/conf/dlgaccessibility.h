/****************************************************************************
 *   Copyright (C)  2012-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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
#ifndef DLGACCESSIBILITY_H
#define DLGACCESSIBILITY_H

#include <QWidget>

#include "ui_dlgaccessibility.h"

class KConfigDialog;
class Macro;

class CategorizedDelegate;
class QStyledItemDelegate;

///DlgAccessibility: Display option for the visually impaired
class DlgAccessibility : public QWidget, public Ui_DlgAccessibility
{
Q_OBJECT
public:
   //Constructor
   explicit DlgAccessibility(KConfigDialog *parent = nullptr);

   //Destructor
   ~DlgAccessibility();

   //Getters
   bool hasChanged();

private:
   //Attributes
   bool m_Changed;
   CategorizedDelegate* m_pCategoryDelegate;
   QStyledItemDelegate* m_pItemDelegate    ;

public Q_SLOTS:
   void updateSettings();
   void updateWidgets();

private Q_SLOTS:
   void changed();
   void addMacro();
   void removeMacro();
   void selectMacro(Macro*);

   //On widget change
   void slotNameLE(const QString& newText);
   void slotCategoryCBB(const QString& newText);
   void slotDelaySB(int newValue);
   void slotSequenceLE(const QString& newText);
   void slotDescriptionLE(const QString& newText);
   void slotShortcut(const KShortcut& shortcut);

Q_SIGNALS:
   void updateButtons();
};

#endif
