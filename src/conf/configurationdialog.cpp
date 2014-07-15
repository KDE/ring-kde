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
#include "configurationdialog.h"

//KDE
#include <KDebug>
#include <KLocale>
#include <KStandardDirs>
#include <KConfigDialogManager>

#include "klib/kcfg_settings.h"

#include "dlggeneral.h"
#include "dlgdisplay.h"
#include "dlgaccounts.h"
#include "dlgaudio.h"
#include "dlgaddressbook.h"
#include "dlghooks.h"
#include "dlgaccessibility.h"
#include "dlgvideo.h"
#include "dlgpresence.h"

#include "lib/sflphone_const.h"
#include "lib/accountlistmodel.h"

typedef  QWidget* QWidgetPtr;

typedef QWidgetPtr(*PageConstructor)(ConfigurationDialog* dialog);
class PlaceHolderWidget : public QWidget
{
   Q_OBJECT
public:
   PlaceHolderWidget(int id,ConfigurationDialog* parent, PageConstructor callback);
private:
   PageConstructor m_fCallback;
   ConfigurationDialog* m_pParent;
   QWidget* m_pReal;
   int m_Id;

public Q_SLOTS:
   void display(KPageWidgetItem *current);
};

PlaceHolderWidget::PlaceHolderWidget(int id,ConfigurationDialog* parent, PageConstructor callback) : 
QWidget(parent),m_pParent(parent),m_fCallback(callback),m_pReal(nullptr),m_Id(id)
{}

void PlaceHolderWidget::display(KPageWidgetItem *current)
{
   //Create the widget if it doesn't exist, then disconnect itself
   if ((!m_pReal) && (current->property("id").toInt() == m_Id)) {
      m_pReal = m_fCallback(m_pParent);
      QHBoxLayout* l = new QHBoxLayout(this);
      l->setContentsMargins(0,0,0,0);
      l->addWidget(m_pReal);
      disconnect(m_pParent,SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),this,SLOT(display(KPageWidgetItem*)));
   }
}

#define GUARD(a,b) if (a) {a->b;}
#define GUARD_TRUE(a,b) (a?(a->b):true)
#define GUARD_FALSE(a,b) (a?(a->b):false)

/*
 * This file use macros because every pages can be null at all time,
 * so in the end it make the code less dense. Loading all pages
 * by default caused too many dbus call and was slow (> 1 second)
 * so it is better to lazy load them.
 */

///Constructor
ConfigurationDialog::ConfigurationDialog(SFLPhoneView *parent)
 :KConfigDialog(parent, SETTINGS_NAME, ConfigurationSkeleton::self()),dlgVideo(nullptr),dlgGeneral(nullptr),dlgDisplay(nullptr)
 ,dlgAudio(nullptr),dlgAddressBook(nullptr),dlgHooks(nullptr),dlgAccessibility(nullptr),dlgAccounts(nullptr),
 dlgPresence(nullptr)
{
   this->setWindowIcon(QIcon(ICON_SFLPHONE));
   for(int i=0;i<=ConfigurationDialog::Page::Presence;i++)
      dlgHolder[i] = nullptr;

   //Usually, this is done automatically by KConfig, but for performance
   //there is too many widgets and too many dbus calls to do it all at once
   m_pManager = new KConfigDialogManager(this, ConfigurationSkeleton::self());
   connect(this, SIGNAL(okClicked()), m_pManager, SLOT(updateSettings()));
   connect(this, SIGNAL(applyClicked()), m_pManager, SLOT(updateSettings()));
   connect(this, SIGNAL(cancelClicked()), m_pManager, SLOT(updateWidgets()));
   connect(this, SIGNAL(defaultClicked()), m_pManager, SLOT(updateWidgetsDefault()));

   connect(m_pManager, SIGNAL(settingsChanged()), this, SLOT(updateButtons()));
   connect(m_pManager, SIGNAL(widgetModified()), this, SLOT(updateButtons()));


   //Account
   dlgHolder[ConfigurationDialog::Page::Accounts]   = new PlaceHolderWidget(Page::Accounts,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAccounts = new DlgAccounts(dialog);
      dialog->m_pManager->addWidget(dialog->dlgAccounts);
      return dialog->dlgAccounts;
   });
   auto accDlg = addPage( dlgHolder[ConfigurationDialog::Page::Accounts]      , i18n("Accounts")                     , "user-identity"                     );
   accDlg->setProperty("id",ConfigurationDialog::Page::Accounts);
   dlgHolder[ConfigurationDialog::Page::Accounts]->display(accDlg);

   //General
   dlgHolder[ConfigurationDialog::Page::General]   = new PlaceHolderWidget(Page::General,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgGeneral = new DlgGeneral(dialog);
      dialog->m_pManager->addWidget(dialog->dlgGeneral);
      connect(dialog->dlgGeneral, SIGNAL(clearCallHistoryAsked()), dialog, SIGNAL(clearCallHistoryAsked()));
      return dialog->dlgGeneral;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::General]       , i18nc("History settings","History")  , "view-history"               )
      ->setProperty("id",ConfigurationDialog::Page::General);

   //Display
   dlgHolder[ConfigurationDialog::Page::Display]    = new PlaceHolderWidget(Page::Display,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgDisplay = new DlgDisplay(dialog);
      dialog->m_pManager->addWidget(dialog->dlgDisplay);
      return dialog->dlgDisplay;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Display]       , i18nc("User interterface settings"   ,"Display"), "applications-graphics"  )
      ->setProperty("id",ConfigurationDialog::Page::Display);

   //Audio
   dlgHolder[ConfigurationDialog::Page::Audio]      = new PlaceHolderWidget(Page::Audio,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAudio = new DlgAudio(dialog);
      dialog->m_pManager->addWidget(dialog->dlgAudio);
      return dialog->dlgAudio;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Audio]         , i18n("Audio")                        , "audio-headset"                     )
      ->setProperty("id",ConfigurationDialog::Page::Audio);

   //AddressBook
   dlgHolder[ConfigurationDialog::Page::AddressBook]= new PlaceHolderWidget(Page::AddressBook,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAddressBook = new DlgAddressBook(dialog);
      dialog->m_pManager->addWidget(dialog->dlgAddressBook);
      return dialog->dlgAddressBook;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::AddressBook]   , i18n("Address Book")                 , "x-office-address-book"             )
      ->setProperty("id",ConfigurationDialog::Page::AddressBook);

   //Hooks
   dlgHolder[ConfigurationDialog::Page::Hooks]      = new PlaceHolderWidget(Page::Hooks,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgHooks = new DlgHooks(dialog);
      dialog->m_pManager->addWidget(dialog->dlgHooks);
      return dialog->dlgHooks;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Hooks]         , i18n("Hooks")                        , "insert-link"                       )
      ->setProperty("id",ConfigurationDialog::Page::Hooks);

   //Accessibility
   dlgHolder[ConfigurationDialog::Page::Accessibility]= new PlaceHolderWidget(Page::Accessibility,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAccessibility = new DlgAccessibility (dialog);
      dialog->m_pManager->addWidget(dialog->dlgAccessibility);
      return dialog->dlgAccessibility;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Accessibility] , i18n("Accessibility")                , "preferences-desktop-accessibility" )
      ->setProperty("id",ConfigurationDialog::Page::Accessibility);

   //Video
#ifdef ENABLE_VIDEO
   dlgHolder[ConfigurationDialog::Page::Video]      = new PlaceHolderWidget(Page::Video,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgVideo = new DlgVideo(dialog);
      dialog->m_pManager->addWidget(dialog->dlgVideo);
      return dialog->dlgVideo;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Video]         , i18nc("Video conversation","Video")  , "camera-web"                        )
      ->setProperty("id",ConfigurationDialog::Page::Video);
#endif

   //Presence
   dlgHolder[ConfigurationDialog::Page::Presence]   = new PlaceHolderWidget(Page::Presence,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgPresence = new DlgPresence(dialog);
      dialog->m_pManager->addWidget(dialog->dlgPresence);
      return dialog->dlgPresence;
   });
   m_pPresPage = addPage( dlgHolder[ConfigurationDialog::Page::Presence]      , i18nc("SIP Presence","Presence")     , KStandardDirs::locate("data" , "sflphone-client-kde/presence-icon.svg"));
   m_pPresPage->setProperty("id",ConfigurationDialog::Page::Presence);
   m_pPresPage->setEnabled(AccountListModel::instance()->isPresencePublishSupported() && AccountListModel::instance()->isPresenceEnabled());
   connect(AccountListModel::instance(),SIGNAL(presenceEnabledChanged(bool)),this,SLOT(slotPresenceEnabled(bool)));

   //Connect everything
   for(int i=0;i<=ConfigurationDialog::Page::Presence;i++)
      if (dlgHolder[i])
         connect(this,SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),dlgHolder[i],SLOT(display(KPageWidgetItem*)));

   connect(this, SIGNAL(applyClicked()) , this, SLOT(applyCustomSettings()));
   connect(this, SIGNAL(okClicked())    , this, SLOT(applyCustomSettings()));
   connect(this, SIGNAL(cancelClicked()), this, SLOT(cancelSettings())     );

   setMinimumSize(1024,600);

} //ConfigurationDialog

///Destructor
ConfigurationDialog::~ConfigurationDialog()
{
   if (dlgGeneral      ) delete dlgGeneral      ;
   if (dlgDisplay      ) delete dlgDisplay      ;
   if (dlgAccounts     ) delete dlgAccounts     ;
   if (dlgAudio        ) delete dlgAudio        ;
   if (dlgAddressBook  ) delete dlgAddressBook  ;
   if (dlgHooks        ) delete dlgHooks        ;
   if (dlgAccessibility) delete dlgAccessibility;
   if (dlgPresence     ) delete dlgPresence     ;
   #ifdef ENABLE_VIDEO
   if (dlgVideo        ) delete dlgVideo        ;
   #endif
   delete m_pManager;
}

///Update all widgets when something is reloaded
void ConfigurationDialog::updateWidgets()
{
   GUARD(dlgAudio,updateWidgets        ());
   GUARD(dlgAccounts,updateWidgets     ());
   GUARD(dlgGeneral,updateWidgets      ());
   GUARD(dlgAddressBook,updateWidgets  ());
   GUARD(dlgAccessibility,updateWidgets());
   GUARD(dlgPresence,updateWidgets     ());
   GUARD(dlgHooks,updateWidgets        ());
   #ifdef ENABLE_VIDEO
   GUARD(dlgVideo,updateWidgets        ());
   #endif
}

///Save all settings when apply is clicked
void ConfigurationDialog::updateSettings()
{
   GUARD(dlgAudio,updateSettings        ());
   GUARD(dlgAccounts,updateSettings     ());
   GUARD(dlgGeneral,updateSettings      ());
   GUARD(dlgAddressBook,updateSettings  ());
   GUARD(dlgAccessibility,updateSettings());
   GUARD(dlgDisplay,updateSettings      ());
   GUARD(dlgPresence,updateSettings     ());
   GUARD(dlgHooks,updateSettings        ());
   #ifdef ENABLE_VIDEO
   GUARD(dlgVideo,updateSettings        ());
   #endif
}

///Cancel current modification
void ConfigurationDialog::cancelSettings()
{
   GUARD(dlgAccounts,cancel());
}

///If the account changed
bool ConfigurationDialog::hasChanged()
{
   bool res =  ((GUARD_FALSE(dlgAudio,hasChanged()        ))
            || (GUARD_FALSE(dlgAccounts,hasChanged()      ))
            || (GUARD_FALSE(dlgGeneral,hasChanged()       ))
            || (GUARD_FALSE(dlgDisplay,hasChanged()       ))
            || (GUARD_FALSE(dlgAddressBook,hasChanged()   ))
            || (GUARD_FALSE(dlgAccessibility,hasChanged() ))
            || (GUARD_FALSE(dlgHooks,hasChanged()         ))
#ifdef ENABLE_VIDEO
            || (GUARD_FALSE(dlgVideo,hasChanged()         ))
#endif
            || (GUARD_FALSE(dlgPresence,hasChanged()      )));

   return res;
}

bool ConfigurationDialog::hasIncompleteRequiredFields()
{
   return GUARD_FALSE(dlgAccounts,hasIncompleteRequiredFields());
}

///Update the buttons
void ConfigurationDialog::updateButtons()
{
   bool changed      = hasChanged() || m_pManager->hasChanged();
   bool preventApply = hasIncompleteRequiredFields();
   enableButtonApply( changed && (!preventApply) );
   enableButtonOk   ( !preventApply              );
}

///Apply settings
void ConfigurationDialog::applyCustomSettings()
{
   if(hasChanged()) {
          ConfigurationSkeleton::self()->writeConfig();
   }
   updateSettings();
   updateWidgets ();
   updateButtons ();
   emit changesApplied();
}

///Reload the pages
void ConfigurationDialog::reload()
{
   kDebug() << "Reloading config";
   ConfigurationSkeleton::self()->readConfig();
   updateWidgets();
   updateButtons();
}


void ConfigurationDialog::slotPresenceEnabled(bool state)
{
   m_pPresPage->setEnabled(state && AccountListModel::instance()->isPresencePublishSupported());
}

#undef GUARD
#undef GUARD_TRUE
#undef GUARD_FALSE
#include "configurationdialog.moc"
#include "moc_configurationdialog.cpp"
