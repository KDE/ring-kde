/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
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
#ifndef DLGADDRESSBOOK_H
#define DLGADDRESSBOOK_H

//Qt
#include <QHash>
#include <QString>
#include <QtWidgets/QWidget>
#include <QtWidgets/QListWidgetItem>
class QSortFilterProxyModel;

#include "ui_dlgaddressbookbase.h"
#include <kconfigdialog.h>


//KDE
class KJob;
class KConfigDialogManager;

//Ring
class AutoCompletionDelegate;
class CategorizedDelegate;
class CollectionInterface;

class DlgAddressBook : public QWidget, public Ui_DlgAddressBookBase
{
Q_OBJECT
public:
   explicit DlgAddressBook(KConfigDialog* parent = nullptr);

   virtual ~DlgAddressBook();

   bool hasChanged();

private:
   //Attributes
   QHash<QString,QListWidgetItem*> m_mNumbertype;
   bool m_HasChanged;
   AutoCompletionDelegate* m_pDelegate;
   CategorizedDelegate* m_pCategoryDelegate;
   QSortFilterProxyModel* m_pProxyModel;
   QHash<CollectionInterface*,QWidget*> m_hWidgets;
   QWidget* m_pCurrent;
   KConfigDialogManager* m_pManager;
   QList<CollectionConfigurationInterface*> m_lConfigurators;

public Q_SLOTS:
   void updateWidgets ();
   void updateSettings();

private Q_SLOTS:
   void changed();
   void slotEditCollection();
   void slotAddCollection();
   void slotResourceCreationResult(KJob*);
   void slotRowsInserted(const QModelIndex&, int, int);
   void slotDisplayAll(bool);

Q_SIGNALS:
   ///Emitted when the buttons need to be updated
   void updateButtons();
};

#endif
