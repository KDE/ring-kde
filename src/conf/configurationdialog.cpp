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
#include "configurationdialog.h"

//KDE
#include <QtCore/QDebug>
#include <klocalizedstring.h>

#include <KConfigDialogManager>
#include <QStandardPaths>

#include "klib/kcfg_settings.h"

#include "dlgdisplay.h"
#include "account/dlgaccount.h"
#include "dlgaudio.h"
#include "dlgaddressbook.h"
#include "dlgaccessibility.h"
#include "dlgvideo.h"
#include "dlgpresence.h"
#include "icons/icons.h"

#include "accountmodel.h"

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
      disconnect(m_pParent,&KPageDialog::currentPageChanged,this,&PlaceHolderWidget::display);
   }
}

#define GUARD(a,b) if (a) {a->b;}
#define GUARD_FALSE(a,b) (a?(a->b):false)

/*
 * This file use macros because every pages can be null at all time,
 * so in the end it make the code less dense. Loading all pages
 * by default caused too many dbus call and was slow (> 1 second)
 * so it is better to lazy load them.
 */

///Constructor
ConfigurationDialog::ConfigurationDialog(QWidget *parent)
 :KConfigDialog(parent, QStringLiteral("settings"), ConfigurationSkeleton::self()),dlgVideo(nullptr),dlgDisplay(nullptr)
 ,dlgAudio(nullptr),dlgAddressBook(nullptr),dlgAccessibility(nullptr),dlgAccount(nullptr),
 dlgPresence(nullptr)
{
   setWindowIcon( QIcon(":/appicon/icons/sc-apps-ring-kde.svgz") );

   for(int i=0;i<=ConfigurationDialog::Page::Presence;i++)
      dlgHolder[i] = nullptr;

   //Usually, this is done automatically by KConfig, but for performance
   //there is too many widgets and too many dbus calls to do it all at once
   m_pManager = new KConfigDialogManager(this, ConfigurationSkeleton::self());
   connect(buttonBox()->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, m_pManager, &KConfigDialogManager::updateSettings);
   connect(buttonBox()->button(QDialogButtonBox::Apply), &QAbstractButton::clicked, m_pManager, &KConfigDialogManager::updateSettings);
   connect(buttonBox()->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, m_pManager, &KConfigDialogManager::updateWidgets);
   connect(buttonBox()->button(QDialogButtonBox::RestoreDefaults), &QAbstractButton::clicked, m_pManager, &KConfigDialogManager::updateWidgetsDefault);

   connect(m_pManager, SIGNAL(settingsChanged()), this, SLOT(updateButtons()));
   connect(m_pManager, SIGNAL(widgetModified()), this, SLOT(updateButtons()));


   //Account
   dlgHolder[ConfigurationDialog::Page::Accounts]   = new PlaceHolderWidget(Page::Accounts,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAccount = new DlgAccount(dialog);
      dialog->m_pManager->addWidget(dialog->dlgAccount);
      return dialog->dlgAccount;
   });
   auto accDlg = addPage( dlgHolder[ConfigurationDialog::Page::Accounts]      , i18n("Accounts")                     , QStringLiteral("user-identity")                     );
   accDlg->setProperty("id",ConfigurationDialog::Page::Accounts);
   dlgHolder[ConfigurationDialog::Page::Accounts]->display(accDlg);

   //Display
   dlgHolder[ConfigurationDialog::Page::Display]    = new PlaceHolderWidget(Page::Display,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgDisplay = new DlgDisplay(dialog);
      dialog->m_pManager->addWidget(dialog->dlgDisplay);
      return dialog->dlgDisplay;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Display]       , i18nc("User interterface settings"   ,"Display"), QStringLiteral("applications-graphics")  )
      ->setProperty("id",ConfigurationDialog::Page::Display);

   //Audio
   dlgHolder[ConfigurationDialog::Page::Audio]      = new PlaceHolderWidget(Page::Audio,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAudio = new DlgAudio(dialog);
      dialog->m_pManager->addWidget(dialog->dlgAudio);
      return dialog->dlgAudio;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Audio]         , i18n("Audio")                        , QStringLiteral("audio-volume-high")                     )
      ->setProperty("id",ConfigurationDialog::Page::Audio);

   //AddressBook
   dlgHolder[ConfigurationDialog::Page::AddressBook]= new PlaceHolderWidget(Page::AddressBook,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAddressBook = new DlgAddressBook(dialog);
      dialog->m_pManager->addWidget(dialog->dlgAddressBook);
      return dialog->dlgAddressBook;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::AddressBook]   , i18n("Personal data")                 , QStringLiteral("x-office-address-book")             )
      ->setProperty("id",ConfigurationDialog::Page::AddressBook);

   //Accessibility
   dlgHolder[ConfigurationDialog::Page::Accessibility]= new PlaceHolderWidget(Page::Accessibility,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgAccessibility = new DlgAccessibility (dialog);
      dialog->m_pManager->addWidget(dialog->dlgAccessibility);
      return dialog->dlgAccessibility;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Accessibility] , i18n("Accessibility")                , QStringLiteral("preferences-desktop-accessibility") )
      ->setProperty("id",ConfigurationDialog::Page::Accessibility);

   //Video
#ifdef ENABLE_VIDEO
   dlgHolder[ConfigurationDialog::Page::Video]      = new PlaceHolderWidget(Page::Video,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgVideo = new DlgVideo(dialog);
      dialog->m_pManager->addWidget(dialog->dlgVideo);
      return dialog->dlgVideo;
   });
   addPage( dlgHolder[ConfigurationDialog::Page::Video]         , i18nc("Video conversation","Video")  , QStringLiteral("camera-web")                        )
      ->setProperty("id",ConfigurationDialog::Page::Video);
#endif

   //Presence
   /*dlgHolder[ConfigurationDialog::Page::Presence]   = new PlaceHolderWidget(Page::Presence,this,[](ConfigurationDialog* dialog)->QWidget*{
      dialog->dlgPresence = new DlgPresence(dialog);
      dialog->m_pManager->addWidget(dialog->dlgPresence);
      return dialog->dlgPresence;
   });
   m_pPresPage = addPage( dlgHolder[ConfigurationDialog::Page::Presence]      , i18nc("SIP Presence","Presence")     , QStandardPaths::locate(QStandardPaths::GenericDataLocation, "ring-kde/presence-icon.svg"));
   m_pPresPage->setProperty("id",ConfigurationDialog::Page::Presence);
   m_pPresPage->setEnabled(AccountModel::instance().isPresencePublishSupported() && AccountModel::instance().isPresenceEnabled());
   connect(AccountModel::instance(),SIGNAL(presenceEnabledChanged(bool)),this,SLOT(slotPresenceEnabled(bool)));*/

   //Connect everything
   for(int i=0;i<=ConfigurationDialog::Page::Presence;i++)
      if (dlgHolder[i])
         connect(this,&KPageDialog::currentPageChanged,dlgHolder[i],&PlaceHolderWidget::display);

   connect(buttonBox()->button(QDialogButtonBox::Apply), &QAbstractButton::clicked , this, &ConfigurationDialog::applyCustomSettings);
   connect(buttonBox()->button(QDialogButtonBox::Ok), &QAbstractButton::clicked    , this, &ConfigurationDialog::applyCustomSettings);
   connect(buttonBox()->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &ConfigurationDialog::cancelSettings     );

   connect(dlgAccount, &DlgAccount::updateButtons,this,&ConfigurationDialog::updateButtons);

   if (dlgPresence)
      connect(dlgPresence, &DlgPresence::updateButtons  ,this,&ConfigurationDialog::updateButtons);
   setMinimumSize(1024,700);

} //ConfigurationDialog

///Destructor
ConfigurationDialog::~ConfigurationDialog()
{
   if (dlgDisplay      ) delete dlgDisplay      ;
   if (dlgAccount      ) delete dlgAccount      ;
   if (dlgAudio        ) delete dlgAudio        ;
   if (dlgAddressBook  ) delete dlgAddressBook  ;
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
   GUARD(dlgAccount,updateWidgets     ());
   GUARD(dlgAddressBook,updateWidgets  ());
   GUARD(dlgAccessibility,updateWidgets());
   GUARD(dlgPresence,updateWidgets     ());
   #ifdef ENABLE_VIDEO
   GUARD(dlgVideo,updateWidgets        ());
   #endif
}

///Save all settings when apply is clicked
void ConfigurationDialog::updateSettings()
{
   GUARD(dlgAudio,updateSettings        ());
   GUARD(dlgAccount,updateSettings     ());
   GUARD(dlgAddressBook,updateSettings  ());
   GUARD(dlgAccessibility,updateSettings());
   GUARD(dlgDisplay,updateSettings      ());
   GUARD(dlgPresence,updateSettings     ());
   #ifdef ENABLE_VIDEO
   GUARD(dlgVideo,updateSettings        ());
   #endif
}

///Cancel current modification
void ConfigurationDialog::cancelSettings()
{
   GUARD(dlgAccount,cancel());
}

///If the account changed
bool ConfigurationDialog::hasChanged()
{
   bool res =  ((GUARD_FALSE(dlgAudio,hasChanged()        ))
            || (GUARD_FALSE(dlgAccount,hasChanged()      ))
            || (GUARD_FALSE(dlgDisplay,hasChanged()       ))
            || (GUARD_FALSE(dlgAddressBook,hasChanged()   ))
            || (GUARD_FALSE(dlgAccessibility,hasChanged() ))
#ifdef ENABLE_VIDEO
            || (GUARD_FALSE(dlgVideo,hasChanged()         ))
#endif
            || (GUARD_FALSE(dlgPresence,hasChanged()      )));

   return res;
}

bool ConfigurationDialog::hasIncompleteRequiredFields()
{
   return AccountModel::instance().editState() == AccountModel::EditState::INVALID;
}

///Update the buttons
void ConfigurationDialog::updateButtons()
{
   bool changed      = hasChanged() || m_pManager->hasChanged();
   bool preventApply = hasIncompleteRequiredFields();
   buttonBox()->button(QDialogButtonBox::Apply)->setEnabled( changed && (!preventApply) );
   buttonBox()->button(QDialogButtonBox::Ok)->setEnabled   ( !preventApply              );
}

///Apply settings
void ConfigurationDialog::applyCustomSettings()
{
   if(hasChanged()) {
//           ConfigurationSkeleton::self()->writeConfig();
   }
   updateSettings();
   updateWidgets ();
   updateButtons ();
   emit changesApplied();
}

///Reload the pages
void ConfigurationDialog::reload()
{
   qDebug() << "Reloading config";
//    ConfigurationSkeleton::self()->readConfig();
   updateWidgets();
   updateButtons();
}


void ConfigurationDialog::slotPresenceEnabled(bool state)
{
   Q_UNUSED(state)
//    m_pPresPage->setEnabled(state && AccountModel::instance().isPresencePublishSupported());
}

#undef GUARD
#undef GUARD_FALSE
#include "configurationdialog.moc"
#include "moc_configurationdialog.cpp"

// kate: space-indent on; indent-width 3; replace-tabs on;
