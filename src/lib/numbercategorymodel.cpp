/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
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
#include "numbercategorymodel.h"
#include "visitors/numbercategoryvisitor.h"
#include "phonenumber.h"
#include "numbercategory.h"

NumberCategoryModel* NumberCategoryModel::m_spInstance = nullptr;

class NumberCategoryModelPrivate
{
public:
   struct InternalTypeRepresentation {
      NumberCategory* category;
      int             index   ;
      bool            enabled ;
      int             counter ;
   };
   QVector<InternalTypeRepresentation*>   m_lCategories;
   QHash<int,InternalTypeRepresentation*> m_hByIdx;
   QHash<QString,InternalTypeRepresentation*> m_hByName;
   static NumberCategory*                 m_spOther    ;
};

NumberCategory*      NumberCategoryModelPrivate::m_spOther    = nullptr;

NumberCategoryModel::NumberCategoryModel(QObject* parent) : QAbstractListModel(parent),d_ptr(new NumberCategoryModelPrivate())
{
}

NumberCategoryModel::~NumberCategoryModel()
{
   delete d_ptr;
}

//Abstract model member
QVariant NumberCategoryModel::data(const QModelIndex& index, int role) const
{
   if (!index.isValid()) return QVariant();
   switch (role) {
      case Qt::DisplayRole: {
         const QString name = d_ptr->m_lCategories[index.row()]->category->name();
         return name.isEmpty()?tr("Uncategorized"):name;
      }
      case Qt::DecorationRole:
         return d_ptr->m_lCategories[index.row()]->category->icon();//m_pVisitor->icon(m_lCategories[index.row()]->icon);
      case Qt::CheckStateRole:
         return d_ptr->m_lCategories[index.row()]->enabled?Qt::Checked:Qt::Unchecked;
      case Role::INDEX:
         return d_ptr->m_lCategories[index.row()]->index;
      case Qt::UserRole:
         return 'x'+QString::number(d_ptr->m_lCategories[index.row()]->counter);
   }
   return QVariant();
}

int NumberCategoryModel::rowCount(const QModelIndex& parent) const
{
   if (parent.isValid()) return 0;
   return d_ptr->m_lCategories.size();
}

Qt::ItemFlags NumberCategoryModel::flags(const QModelIndex& index) const
{
   Q_UNUSED(index)
   return (d_ptr->m_lCategories[index.row()]->category->name().isEmpty()?Qt::NoItemFlags :Qt::ItemIsEnabled) | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
}

bool NumberCategoryModel::setData(const QModelIndex& idx, const QVariant &value, int role)
{
   if (idx.isValid() && role == Qt::CheckStateRole) {
      d_ptr->m_lCategories[idx.row()]->enabled = value.toBool();
      emit dataChanged(idx,idx);
      return true;
   }
   return false;
}

NumberCategory* NumberCategoryModel::addCategory(const QString& name, QPixmap* icon, int index, bool enabled)
{
   NumberCategoryModelPrivate::InternalTypeRepresentation* rep = d_ptr->m_hByName[name];
   if (!rep) {
      rep = new NumberCategoryModelPrivate::InternalTypeRepresentation();
      rep->counter = 0      ;
   }
   NumberCategory* cat = new NumberCategory(this,name);
   cat->setIcon(icon);
   rep->category   = cat    ;
   rep->index      = index  ;
   rep->enabled    = enabled;
   d_ptr->m_hByIdx[index] = rep    ;
   d_ptr->m_hByName[name] = rep    ;
   d_ptr->m_lCategories  << rep    ;
   emit layoutChanged()     ;
   return cat;
}

NumberCategoryModel* NumberCategoryModel::instance()
{
   if (!m_spInstance)
      m_spInstance = new NumberCategoryModel();
   return m_spInstance;
}

void NumberCategoryModel::setIcon(int idx, QPixmap* icon)
{
   NumberCategoryModelPrivate::InternalTypeRepresentation* rep = d_ptr->m_hByIdx[idx];
   if (rep) {
      rep->category->setIcon(icon);
      emit dataChanged(index(d_ptr->m_lCategories.indexOf(rep),0),index(d_ptr->m_lCategories.indexOf(rep),0));
   }
}

void NumberCategoryModel::save()
{
   NumberCategoryVisitor::instance()->serialize(this);
}

QModelIndex NumberCategoryModel::nameToIndex(const QString& name) const
{
   if (!d_ptr->m_hByName[name])
      return QModelIndex();
   else {
      return index(d_ptr->m_hByName[name]->index,0);
   }
}

///Be sure the category exist, increment the counter
void NumberCategoryModel::registerNumber(PhoneNumber* number)
{
   NumberCategoryModelPrivate::InternalTypeRepresentation* rep = d_ptr->m_hByName[number->category()->name()];
   if (!rep) {
      addCategory(number->category()->name(),nullptr,-1,true);
      rep = d_ptr->m_hByName[number->category()->name()];
   }
   rep->counter++;
}

void NumberCategoryModel::unregisterNumber(PhoneNumber* number)
{
   NumberCategoryModelPrivate::InternalTypeRepresentation* rep = d_ptr->m_hByName[number->category()->name()];
   if (rep)
      rep->counter--;
}

NumberCategory* NumberCategoryModel::getCategory(const QString& type)
{
   NumberCategoryModelPrivate::InternalTypeRepresentation* internal = d_ptr->m_hByName[type];
   if (internal)
      return internal->category;
   return addCategory(type,nullptr);
}


NumberCategory* NumberCategoryModel::other()
{
   if (instance()->d_ptr->m_hByName["Other"])
      return instance()->d_ptr->m_hByName["Other"]->category;
   if (NumberCategoryModelPrivate::m_spOther)
      NumberCategoryModelPrivate::m_spOther = new NumberCategory(instance(),"Other");
   return NumberCategoryModelPrivate::m_spOther;
}

#include <numbercategorymodel.moc>
