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

#include "dlgaddressbook.h"

#include <KMessageBox>
#include <QtCore/QPointer>
#include "klib/kcfg_settings.h"
#include "klib/akonadibackend.h"
#include "lib/numbercategorymodel.h"
#include "delegates/autocompletiondelegate.h"
#include <akonadi/collectionmodel.h>
#include "lib/contactmodel.h"
#include "lib/itembackendmodel.h"
#include "klib/akonadicontactcollectionmodel.h"
#include <akonadi/collectionpropertiesdialog.h>
#include <akonadi/agenttypedialog.h>
#include <akonadi/agentfilterproxymodel.h>
#include <akonadi/agentinstancecreatejob.h>
#include <akonadi/standardactionmanager.h>

///Constructor
DlgAddressBook::DlgAddressBook(KConfigDialog* parent)
 : QWidget(parent),m_HasChanged(false)
{
   setupUi(this);

   m_pAddCollection->setIcon(KIcon("list-add"));
   m_pEditCollection->setIcon(KIcon("document-edit"));

   m_pPhoneTypeList->setModel(NumberCategoryModel::instance());
   m_pDelegate = new AutoCompletionDelegate();
   m_pPhoneTypeList->setItemDelegate(m_pDelegate);

   m_pItemBackendW->setModel(ContactModel::instance()->backendModel());

   //Resize the columns
   m_pItemBackendW->header()->setResizeMode(0,QHeaderView::Stretch);
   for (int i =1;i<ContactModel::instance()->backendModel()->columnCount();i++)
      m_pItemBackendW->header()->setResizeMode(i,QHeaderView::ResizeToContents);

   connect(m_pPhoneTypeList->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this   , SLOT(changed())      );
   connect(this            , SIGNAL(updateButtons())              , parent , SLOT(updateButtons()));
   connect(AkonadiContactCollectionModel::instance()  , SIGNAL(changed())                    , this   , SLOT(changed()));
   connect(ContactModel::instance()->backendModel(),SIGNAL(checkStateChanged()),this,SLOT(changed()));
} //DlgAddressBook

///Destructor
DlgAddressBook::~DlgAddressBook()
{
   m_pPhoneTypeList->setItemDelegate(nullptr);
   delete m_pDelegate;
}

///Reload the widget
void DlgAddressBook::updateWidgets()
{
   AkonadiContactCollectionModel::instance()->reload();
   ContactModel::instance()->backendModel()->load();
}

///Save the settings
void DlgAddressBook::updateSettings()
{
   NumberCategoryModel::instance()->save();
   AkonadiContactCollectionModel::instance()->save();
   ContactModel::instance()->backendModel()->save();
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
   const QModelIndex& idx = m_pItemBackendW->selectionModel()->currentIndex();
   AbstractContactBackend* backend = ContactModel::instance()->backendModel()->backendAt(idx);
   AkonadiBackend* akoBackend = qobject_cast<AkonadiBackend*>(backend);
   if (akoBackend) {
      Akonadi::CollectionPropertiesDialog dlg( akoBackend->collection(), this );
      dlg.exec();
   }
}

///Add a new Akonadi collection
void DlgAddressBook::slotAddCollection()
{
   QPointer<Akonadi::AgentTypeDialog> dlg = new Akonadi::AgentTypeDialog( this );
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
   delete dlg;
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

