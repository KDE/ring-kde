/****************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

//Base
#include <kconfigdialog.h>

#include "klib/kcfg_settings.h"
#include "view.h"

//KDE
class KConfigDialogManager;

//Ring
class DlgDisplay;
class DlgAccount;
class DlgAudio;
class DlgAddressBook;
class DlgAccessibility;
class DlgVideo;
class View;
class DlgPresence;

//Private
class PlaceHolderWidget;

/**
   @author Jérémy Quentin <jeremy.quentin@gmail.com>
   This class represents the config dialog for Ring.
   It uses the ConfigurationSkeleton class to handle most of the settings.
   It inherits KConfigDialog with the pages defined in dlg... files.
   A few complicated settings are handled directly by its pages.
   Some custom behaviors have been added to handle specific cases,
   as this config dialog is not the usual kind.
   A few things might be done a cleaner way by passing the handling
   to the skeleton like it has been done with codecs.
*/
class ConfigurationDialog : public KConfigDialog
{
Q_OBJECT
friend class PlaceHolderWidget;
private:
   enum Page {
      Display,
      Accounts,
      Audio,
      AddressBook,
      Accessibility,
      Video,
      Presence,
   };
   PlaceHolderWidget* dlgHolder[Page::Presence+1];

   DlgDisplay*       dlgDisplay      ;
   DlgAccount*       dlgAccount      ;
   DlgAudio*         dlgAudio        ;
   DlgAddressBook*   dlgAddressBook  ;
   DlgAccessibility* dlgAccessibility;
   DlgVideo*         dlgVideo        ;
   DlgPresence*      dlgPresence     ;

   KConfigDialogManager* m_pManager;

public:
   explicit ConfigurationDialog(View *parent = nullptr);
   virtual ~ConfigurationDialog();


public Q_SLOTS:
   /**
    *   Reimplements KConfigDialog
    */
   void updateWidgets() override;
   /**
    *   Reimplements KConfigDialog
    */
   void updateSettings() override;
   /**
    *   @author Jérémy Quentin <jeremy.quentin@gmail.com>
    *   Is implemented in KConfigDialog only from KDE4.3
    *   It it implemented here for KDE4.2 users.
    *   I didn't test with KDE4.3 so I leave it even for 4.3 users.
    *   Causes problems for a few cases (item managed by kconfig switched, item not managed
    *   switched and then switched back, apply becomes disabled).
    *   Can't be resolved without a method to know if items managed by kconfig have changed.
    *   Disable/Enable Apply Button according to hasChanged() result
    */
   void updateButtons();

   /**
    *   Cancel modifications (apply only to model changes)
    */
   void cancelSettings();

   /**
    * @return whether any custom widget has changed in the dialog.
    */
   bool hasChanged() override;

   ///Are there required fields left to complete
   bool hasIncompleteRequiredFields();

   /**
    * reloads the information before showing it.
    */
   void reload();

private Q_SLOTS:
   /**
    *   Apply settings not managed by kconfig (accounts)
    *   Should be removed when accounts are managed by kconfig.
    */
   void applyCustomSettings();

   void slotPresenceEnabled(bool);


Q_SIGNALS:
   ///Emitted when the history need to be cleaned
   void clearCallHistoryAsked();
   ///Emitted when changes are applied
   void changesApplied();

};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
