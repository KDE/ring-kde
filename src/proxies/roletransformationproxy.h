/****************************************************************************
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                           *
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
#ifndef ROLETRANSFORMATIONPROXY_H
#define ROLETRANSFORMATIONPROXY_H

#include <functional>

#include <QtCore/QIdentityProxyModel>

class RoleTransformationProxyPrivate;

/**
 * Proxy that replace, manipulate, modify, remove or add roles
 *
 * This proxy can be useful to slightly change a model behavior without
 * have to sub-class it.
 *
 * It can be used at runtime to add dynamic filters. For example, one could
 * use this class to set the Qt::BackgroundRole based on runtime search
 * and criterias.
 *
 * <code>
 * auto highlight = new RoleTransformationProxy(dedup);
 *
 * highlight->setRole(Qt::BackgroundRole, [](const QModelIndex& idx) {
 *    return idx.data().toString().indexOf('-') != -1 ?
 *       QColor(Qt::red) : QColor();
 * });
 *
 * highlight->setSourceModel(source);
 * </code>
 *
 * Another use case is to replace Qt::DisplayRole with another role or add
 * new formated roles based on existing ones (like converting a timestamp
 * string to a formated date or convert string number of proper numbers
 * before passing the model to KDChart)
 *
 * <code>
 * auto highlight = new RoleTransformationProxy(dedup);
 *
 * highlight->setRole(Qt::DisplayRole, MyModel::Role::Email);
 *
 * highlight->setSourceModel(source);
 * </code>
 *
 * @note The current implementation is not as generic as it could be, this\
 * could change if there is interest in upstreaming the concept
 */
class RoleTransformationProxy : public QIdentityProxyModel
{
   Q_OBJECT

public:
   explicit RoleTransformationProxy(QObject* parent);
   virtual ~RoleTransformationProxy();

   void setRole(int destination, int sourceRole);
   void setRole(int destination, const QVariant& var);
   void setRole(int destination, const std::function<QVariant(const QModelIndex&)>& f);

   virtual QVariant data( const QModelIndex& index, int role ) const override;

private:
   RoleTransformationProxyPrivate* d_ptr;
   Q_DECLARE_PRIVATE(RoleTransformationProxy)
};

#endif
