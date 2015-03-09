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

#include "dlgaddressbook.h"

#include <kmessagebox.h>
#include <QPointer>
#include "klib/kcfg_settings.h"
// #include "klib/akonadibackend.h"
#include "numbercategorymodel.h"
#include "delegates/autocompletiondelegate.h"
#include "delegates/categorizeddelegate.h"
// #include <akonadi/collectionmodel.h>
#include "personmodel.h"
#include "collectionmodel.h"
// #include "klib/akonadicontactcollectionmodel.h"
// #include <akonadi/collectionpropertiesdialog.h>
// #include <akonadi/agenttypedialog.h>
// #include <akonadi/agentfilterproxymodel.h>
// #include <akonadi/agentinstancecreatejob.h>
// #include <akonadi/standardactionmanager.h>

///Constructor
DlgAddressBook::DlgAddressBook(KConfigDialog* parent)
 : QWidget(parent),m_HasChanged(false)
{
   setupUi(this);

   m_pAddCollection->setIcon(QIcon::fromTheme("list-add"));
   m_pEditCollection->setIcon(QIcon::fromTheme("document-edit"));

   m_pDelegate = new AutoCompletionDelegate();

   m_pCategoryDelegate = new CategorizedDelegate(m_pItemBackendW);
   m_pCategoryDelegate->setChildDelegate(m_pDelegate);
   m_pItemBackendW->setItemDelegate(m_pCategoryDelegate);

   m_pItemBackendW->setModel(ConfigurationSkeleton::displayAllCollections()? CollectionModel::instance() : CollectionModel::instance()->manageableCollections());

   //Resize the columns
   if (m_pItemBackendW->header()) {
      m_pItemBackendW->header()->setSectionResizeMode(0,QHeaderView::Stretch);
      for (int i =1;i<CollectionModel::instance()->columnCount();i++)
         m_pItemBackendW->header()->setSectionResizeMode(i,QHeaderView::ResizeToContents);
   }

   connect(this            , SIGNAL(updateButtons())              , parent , SLOT(updateButtons()));
   connect(CollectionModel::instance(),SIGNAL(checkStateChanged()),this,SLOT(changed()));

   connect(CollectionModel::instance(),&CollectionModel::rowsInserted,[this](const QModelIndex&,int,int) {
      m_pItemBackendW->expandAll();
   });

   connect(kcfg_displayAllCollections,&QCheckBox::toggled,[this](bool checked) {
      m_pItemBackendW->setModel( checked ? CollectionModel::instance() : CollectionModel::instance()->manageableCollections());
      m_pItemBackendW->expandAll();
   });

   m_pItemBackendW->expandAll();
} //DlgAddressBook

///Destructor
DlgAddressBook::~DlgAddressBook()
{
   m_pItemBackendW->setItemDelegate(nullptr);
}

///Reload the widget
void DlgAddressBook::updateWidgets()
{
//    AkonadiPersonCollectionModel::instance()->reload();
//    PersonModel::instance()->backendModel()->load();
}

///Save the settings
void DlgAddressBook::updateSettings()
{
   NumberCategoryModel::instance()->save();
//    AkonadiPersonCollectionModel::instance()->save();
//    PersonModel::instance()->backendModel()->save();
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
   return m_HasChanged;
}

///Edit the selection collection
void DlgAddressBook::slotEditCollection()
{
//    const QModelIndex& idx = m_pCollectionW->selectionModel()->currentIndex();
//    CollectionInterface* backend = PersonModel::instance()->backendModel()->backendAt(idx);
//    AkonadiBackend* akoBackend = dynamic_cast<AkonadiBackend*>(backend);
//    if (akoBackend) {
//       Akonadi::CollectionPropertiesDialog dlg( akoBackend->collection(), this );
//       dlg.exec();
//    } //FIXME
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

