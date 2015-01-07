/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
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

//Parent
#include "contactmodel.h"

//SFLPhone library
#include "contact.h"
#include "call.h"
#include "uri.h"
#include "phonenumber.h"
#include "abstractitembackend.h"
#include "itembackendmodel.h"
#include "visitors/itemmodelstateserializationvisitor.h"

//Qt
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

ContactModel* ContactModel::m_spInstance = nullptr;


class ContactModelPrivate : public QObject
{
   Q_OBJECT
public:
   ContactModelPrivate(ContactModel* parent);

   //Attributes
   QVector<AbstractContactBackend*> m_lBackends;
   CommonItemBackendModel* m_pBackendModel;
   QHash<QByteArray,ContactPlaceHolder*> m_hPlaceholders;

   //Indexes
   QHash<QByteArray,Contact*> m_hContactsByUid;
   QVector<Contact*> m_lContacts;

private:
   ContactModel* q_ptr;

private Q_SLOTS:
   void slotReloaded();
   void slotContactAdded(Contact* c);
};

ContactModelPrivate::ContactModelPrivate(ContactModel* parent) : QObject(parent), q_ptr(parent),
m_pBackendModel(nullptr)
{
   
}

///Constructor
ContactModel::ContactModel(QObject* par) : QAbstractItemModel(par?par:QCoreApplication::instance()), d_ptr(new ContactModelPrivate(this))
{
}

///Destructor
ContactModel::~ContactModel()
{
   d_ptr->m_hContactsByUid.clear();
   while (d_ptr->m_lContacts.size()) {
      Contact* c = d_ptr->m_lContacts[0];
      d_ptr->m_lContacts.remove(0);
      delete c;
   }
}

ContactModel* ContactModel::instance() {
   if (!m_spInstance)
      m_spInstance = new ContactModel(QCoreApplication::instance());
   return m_spInstance;
}

/*****************************************************************************
 *                                                                           *
 *                                   Model                                   *
 *                                                                           *
 ****************************************************************************/


bool ContactModel::setData( const QModelIndex& idx, const QVariant &value, int role)
{
   Q_UNUSED(idx)
   Q_UNUSED(value)
   Q_UNUSED(role)
   return false;
}

QVariant ContactModel::data( const QModelIndex& idx, int role) const
{
   if (!idx.isValid())
      return QVariant();
   if (!idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      const Contact* c = d_ptr->m_lContacts[idx.row()];
      if (c)
         return QVariant(c->formattedName());
   }
   else if (idx.parent().isValid() && (role == Qt::DisplayRole || role == Qt::EditRole)) {
      const Contact* c = d_ptr->m_lContacts[idx.parent().row()];
      if (c)
         return QVariant(c->phoneNumbers()[idx.row()]->uri());
   }
   return QVariant();
}

QVariant ContactModel::headerData(int section, Qt::Orientation orientation, int role) const
{
   Q_UNUSED(section)
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      return QVariant(tr("Contacts"));
   return QVariant();
}

int ContactModel::rowCount( const QModelIndex& par ) const
{
   if (!par.isValid()) {
      return d_ptr->m_lContacts.size();
   }
   else if (!par.parent().isValid() && par.row() < d_ptr->m_lContacts.size()) {
      const Contact* c = d_ptr->m_lContacts[par.row()];
      if (c) {
         const int size = c->phoneNumbers().size();
         return size==1?0:size;
      }
   }
   return 0;
}

Qt::ItemFlags ContactModel::flags( const QModelIndex& idx ) const
{
   if (!idx.isValid())
      return Qt::NoItemFlags;
   return Qt::ItemIsEnabled | ((idx.parent().isValid())?Qt::ItemIsSelectable:Qt::ItemIsEnabled);
}

int ContactModel::columnCount ( const QModelIndex& par) const
{
   Q_UNUSED(par)
   return 1;
}

QModelIndex ContactModel::parent( const QModelIndex& idx) const
{
   if (!idx.isValid())
      return QModelIndex();
   CategorizedCompositeNode* modelItem = (CategorizedCompositeNode*)idx.internalPointer();
   if (modelItem && modelItem->type() == CategorizedCompositeNode::Type::NUMBER) {
      int idx2 = d_ptr->m_lContacts.indexOf(((Contact::PhoneNumbers*)modelItem)->contact());
      if (idx2 != -1) {
         return ContactModel::index(idx2,0,QModelIndex());
      }
   }
   return QModelIndex();
}

QModelIndex ContactModel::index( int row, int column, const QModelIndex& par) const
{
   if (!par.isValid() && d_ptr->m_lContacts.size() > row) {
      return createIndex(row,column,d_ptr->m_lContacts[row]);
   }
   else if (par.isValid() && d_ptr->m_lContacts[par.row()]->phoneNumbers().size() > row) {
      return createIndex(row,column,(CategorizedCompositeNode*)(&(d_ptr->m_lContacts[par.row()]->phoneNumbers())));
   }
   return QModelIndex();
}

/*****************************************************************************
 *                                                                           *
 *                                  Mutator                                  *
 *                                                                           *
 ****************************************************************************/


///Find contact by UID
Contact* ContactModel::getContactByUid(const QByteArray& uid)
{
   return d_ptr->m_hContactsByUid[uid];
}

/**
 * Create a temporary contact or return the existing one for an UID
 * This temporary contact should eventually be merged into the real one
 */
Contact* ContactModel::getPlaceHolder(const QByteArray& uid )
{
   Contact* ct = d_ptr->m_hContactsByUid[uid];

   //Do not create a placeholder if the real deal exist
   if (ct) {
      return ct;
   }

   //Do not re-create if it already exist
   ct = d_ptr->m_hPlaceholders[uid];
   if (ct)
      return ct;

   ContactPlaceHolder* ct2 = new ContactPlaceHolder(uid);

   d_ptr->m_hPlaceholders[ct2->uid()] = ct2;
   return ct2;
}

///Return if there is backends
bool ContactModel::hasBackends() const
{
   return d_ptr->m_lBackends.size();
}


const QVector<AbstractContactBackend*> ContactModel::enabledBackends() const
{
   return d_ptr->m_lBackends;
}

bool ContactModel::hasEnabledBackends() const
{
   return d_ptr->m_lBackends.size()>0;
}

CommonItemBackendModel* ContactModel::backendModel() const
{
   if (!d_ptr->m_pBackendModel) {
      d_ptr->m_pBackendModel = new CommonItemBackendModel(const_cast<ContactModel*>(this));
   }
   return d_ptr->m_pBackendModel; //TODO
}

const QVector<AbstractContactBackend*> ContactModel::backends() const
{
   return d_ptr->m_lBackends;
}

bool ContactModel::enableBackend(AbstractContactBackend* backend, bool enabled)
{
   Q_UNUSED(backend)
   Q_UNUSED(enabled)
   //TODO;
   return false;
}

bool ContactModel::addContact(Contact* c)
{
   if (!c)
      return false;
   beginInsertRows(QModelIndex(),d_ptr->m_lContacts.size()-1,d_ptr->m_lContacts.size());
   d_ptr->m_lContacts << c;
   d_ptr->m_hContactsByUid[c->uid()] = c;

   //Deprecate the placeholder
   if (d_ptr->m_hPlaceholders.contains(c->uid())) {
      ContactPlaceHolder* c2 = d_ptr->m_hPlaceholders[c->uid()];
      if (c2) {
         c2->merge(c);
         d_ptr->m_hPlaceholders[c->uid()] = nullptr;
      }
   }
   endInsertRows();
   emit layoutChanged();
   emit newContactAdded(c);
   return true;
}


void ContactModel::disableContact(Contact* c)
{
   if (c)
      c->setActive(false);
}

const ContactList ContactModel::contacts() const
{
   return d_ptr->m_lContacts;
}

void ContactModel::addBackend(AbstractContactBackend* backend, LoadOptions options)
{
   d_ptr->m_lBackends << backend;
   connect(backend,SIGNAL(reloaded()),d_ptr.data(),SLOT(slotReloaded()));
   connect(backend,SIGNAL(newContactAdded(Contact*)),d_ptr.data(),SLOT(slotContactAdded(Contact*)));
   if (options & LoadOptions::FORCE_ENABLED || ItemModelStateSerializationVisitor::instance()->isChecked(backend))
      backend->load();
   emit newBackendAdded(backend);
}

bool ContactModel::addNewContact(Contact* c, AbstractContactBackend* backend)
{
   Q_UNUSED(backend);
   return d_ptr->m_lBackends[0]->addNew(c);
}


/*****************************************************************************
 *                                                                           *
 *                                    Slot                                   *
 *                                                                           *
 ****************************************************************************/

void ContactModelPrivate::slotReloaded()
{
   emit q_ptr->reloaded();
}

void ContactModelPrivate::slotContactAdded(Contact* c)
{
   q_ptr->addContact(c);
}

#include <contactmodel.moc>
