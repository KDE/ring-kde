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

#include "dlgaddressbook.h"

//Qt
#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QPointer>

//KDE
#include <kmessagebox.h>
#include <KConfigDialogManager>

//Ring-KDE
#include "klib/kcfg_settings.h"

//LibRingClient
#include "numbercategorymodel.h"
#include "delegates/autocompletiondelegate.h"
#include "delegates/categorizeddelegate.h"
#include "collectionextensionmodel.h"
#include <collectionconfigurationinterface.h>
#include "personmodel.h"
#include "collectionmodel.h"

///Constructor
DlgAddressBook::DlgAddressBook(KConfigDialog* parent)
 : QWidget(parent),m_HasChanged(false),m_pCurrent(nullptr)
{
   setupUi(this);

   m_pDelegate = new AutoCompletionDelegate();

   m_pCategoryDelegate = new CategorizedDelegate(m_pItemBackendW);
   m_pCategoryDelegate->setChildDelegate(m_pDelegate);
   m_pCategoryDelegate->setChildChildDelegate(m_pDelegate);
   m_pItemBackendW->setItemDelegate(m_pCategoryDelegate);

   CollectionModel::instance().load();

   m_pProxyModel = new QSortFilterProxyModel(this);
   m_pProxyModel->setSortRole(Qt::DisplayRole);
   m_pProxyModel->setSourceModel(ConfigurationSkeleton::displayAllCollections()? &CollectionModel::instance() : CollectionModel::instance().manageableCollections());
   m_pItemBackendW->sortByColumn(0,Qt::AscendingOrder);

   m_pItemBackendW->setModel(m_pProxyModel);

   m_pManager = new KConfigDialogManager(this, ConfigurationSkeleton::self());

   connect(m_pManager, SIGNAL(settingsChanged()), this, SLOT(updateButtons()));
   connect(m_pManager, &KConfigDialogManager::widgetModified, this, &DlgAddressBook::updateButtons);

   //Resize the columns
   if (m_pItemBackendW->header()) {
      m_pItemBackendW->header()->setSectionResizeMode(0,QHeaderView::Stretch);
      for (int i =1;i<CollectionModel::instance().columnCount();i++)
         m_pItemBackendW->header()->setSectionResizeMode(i,QHeaderView::ResizeToContents);
   }

   connect(this            , SIGNAL(updateButtons())              , parent , SLOT(updateButtons()));
   connect(&CollectionModel::instance(),&CollectionModel::checkStateChanged,this,&DlgAddressBook::changed);

   connect(&CollectionModel::instance(),&CollectionModel::rowsInserted, this, &DlgAddressBook::slotRowsInserted);

   connect(kcfg_displayAllCollections,&QCheckBox::toggled, this, &DlgAddressBook::slotDisplayAll);

   connect(m_pItemBackendW->selectionModel(), &QItemSelectionModel::currentChanged,this,&DlgAddressBook::slotEditCollection);

   m_pItemBackendW->expandAll();

   //Select the first useful element
   for (int i=0; i<CollectionModel::instance().rowCount(); i++) {
      const auto cat = CollectionModel::instance().index(i,0);
      for (int j=0;j<CollectionModel::instance().rowCount(cat);j++) {
         const auto orig = CollectionModel::instance().index(j,0,cat);
         CollectionInterface* col = CollectionModel::instance().collectionAt(orig);
         if (col) {
            CollectionConfigurationInterface* configurator = col->configurator();
            if (configurator) {
               if (m_pProxyModel->sourceModel() == &CollectionModel::instance())
                  m_pItemBackendW->selectionModel()->setCurrentIndex(m_pProxyModel->mapFromSource(orig), QItemSelectionModel::ClearAndSelect);
               else
                  m_pItemBackendW->selectionModel()->setCurrentIndex(m_pProxyModel->mapFromSource(((QAbstractProxyModel*)CollectionModel::instance().manageableCollections())->mapFromSource(orig)), QItemSelectionModel::ClearAndSelect);
            }
         }
      }
   }

} //DlgAddressBook

///Destructor
DlgAddressBook::~DlgAddressBook()
{
   m_pCategoryDelegate->setChildDelegate(nullptr);
   m_pCategoryDelegate->setChildChildDelegate(nullptr);
   delete m_pDelegate;
   delete m_pCategoryDelegate;
   m_pItemBackendW->setItemDelegate(nullptr);
   delete m_pManager;
   m_hWidgets.clear();
}

void DlgAddressBook::slotRowsInserted(const QModelIndex&, int, int)
{
   m_pItemBackendW->expandAll();
}

void DlgAddressBook::slotDisplayAll(bool checked)
{
   m_pProxyModel->setSourceModel( checked ? &CollectionModel::instance() : CollectionModel::instance().manageableCollections());
   m_pItemBackendW->expandAll();
}

///Reload the widget
void DlgAddressBook::updateWidgets()
{
//    AkonadiPersonCollectionModel::instance().reload();
   m_pManager->updateWidgets();
   CollectionModel::instance().load();
}

///Save the settings
void DlgAddressBook::updateSettings()
{
//    AkonadiPersonCollectionModel::instance().save();
   CollectionModel::instance().save();
   m_pManager->updateSettings();

   foreach (CollectionConfigurationInterface* c, m_lConfigurators) {
      if (c->hasChanged())
         c->save();
   }

   m_HasChanged = false;
}

///Tag this dialog as changed
void DlgAddressBook::changed()
{
   m_HasChanged = true;
   emit updateButtons();
}

///If the "Apply" button need to be enabled
bool DlgAddressBook::hasChanged()
{
   bool configuratorChanged = false;
   foreach (CollectionConfigurationInterface* c, m_lConfigurators) {
      configuratorChanged |= c->hasChanged();
   }

   return m_HasChanged || m_pManager->hasChanged();
}

///Edit the selection collection
void DlgAddressBook::slotEditCollection()
{
   QModelIndex dest = m_pItemBackendW->selectionModel()->currentIndex();
   if (m_pProxyModel->sourceModel() != &CollectionModel::instance()) {
      dest = m_pProxyModel->mapToSource(dest);
      dest = ((QAbstractProxyModel*)CollectionModel::instance().manageableCollections())->mapToSource(dest);
   }
   else
      dest = m_pProxyModel->mapToSource(dest);

   CollectionInterface* col = CollectionModel::instance().collectionAt(dest);
   if (col) {

      if (m_hWidgets[col]) {
         if (m_pCurrent)
            m_pCurrent->setVisible(false);
         m_pCurrent = m_hWidgets[col];
         m_pCurrent->setVisible(true);
         return;
      }

      CollectionConfigurationInterface* configurator = col->configurator();

      if (m_pCurrent) {
         m_pCurrent->setVisible(false);
         m_pCurrent = nullptr;
      }

      if (configurator) {
         QWidget* w = new QWidget(frame_2);
         m_hWidgets[col] = w;
         configurator->loadCollection(col,w);
         verticalLayout->addWidget(w);
         m_pManager->addWidget(w);
         m_pCurrent = w;
         connect(configurator,&CollectionConfigurationInterface::changed,this,&DlgAddressBook::changed);
         m_lConfigurators << configurator;
      }
   }
}

///Add a new Akonadi collection
void DlgAddressBook::slotAddCollection()
{
   /*QPointer<Akonadi::AgentTypeDialog> dlg = new Akonadi::AgentTypeDialog( this );
   dlg->agentFilterProxyModel()->addMimeTypeFilter( "text/x-vcard" );
   dlg->agentFilterProxyModel()->addMimeTypeFilter( "X-IMAddress" );
   if ( dlg->exec() ) {
      const Akonadi::AgentType agentType = dlg->agentType();

      if ( agentType.isValid() ) {
         Akonadi::AgentInstanceCreateJob *job = new Akonadi::AgentInstanceCreateJob( agentType, this );
         connect( job, SIGNAL(result(KJob*)), SLOT(slotResourceCreationResult(KJob*)) );
         job->configure( this );
         job->start();
      }

   }
   delete dlg;*/
}

void DlgAddressBook::slotResourceCreationResult(KJob* job)
{
   Q_UNUSED(job)
//    if ( job->error() ) {
//       KMessageBox::error( parentWidget,
//          contextText( Akonadi::StandardActionManager::CreateResource, Akonadi::StandardActionManager::ErrorMessageText, job->errorString() ),
//          contextText( Akonadi::StandardActionManager::CreateResource, Akonadi::StandardActionManager::ErrorMessageTitle ) );
//    }
}

// kate: space-indent on; indent-width 3; replace-tabs on;

