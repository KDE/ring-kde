/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#include "serializationinterface.h"

#include <persondirectory.h>

#ifdef ENABLE_AKONADI
 #include "akonadibackend.h"
#endif

#include <picocms/collectioninterface.h>
#include "kcfg_settings.h"

SerializationInterface::~SerializationInterface()
{

}

bool SerializationInterface::save()
{
    //FIXME this is too tied to the contact backend, it need to support roles for
    //other collection types (aka: bookmarks)
    QStringList ret;
    for (QHash<QByteArray,bool>::iterator i = m_hChecked.begin(); i != m_hChecked.end(); ++i) {
        if (i.value())
            ret << i.key();
    }
    //    digg(QModelIndex());
    ConfigurationSkeleton::setDisabledCollectionList(ret);
    return true;
}

bool SerializationInterface::load()
{
    m_hChecked.clear();
    const QStringList disabled = ConfigurationSkeleton::disabledCollectionList();
    foreach(const QString& str, disabled) {
        m_hChecked[str.toLatin1()] = true; //Disabled == true, enabled == false
    }
    return true;
}

bool SerializationInterface::isChecked(const CollectionInterface* backend) const
{
    return backend ? (!(m_hChecked[backend->id()])) : false;
}

bool SerializationInterface::setChecked(const CollectionInterface* backend, bool enabled)
{
    m_hChecked[backend->id()] = ! enabled;
    return true;
}

CollectionInterface* SerializationInterface::preferredCollection(CollectionManagerInterfaceBase* manager, FlagPack<CollectionInterface::SupportedFeatures> features, FlagPack<Interfaces::ItemModelStateSerializerI::Hints> hints )
{
    Q_UNUSED(hints)

#ifdef ENABLE_AKONADI
    if (manager == Session::instance()->personDirectory()) {
        foreach(CollectionInterface* i, Session::instance()->personDirectory()->collections(features)) {
            if (dynamic_cast<AkonadiBackend*>(i)) //TODO use something better
                return i;
        }
    }
#else
    Q_UNUSED(manager )
    Q_UNUSED(features)
#endif

    return nullptr;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
