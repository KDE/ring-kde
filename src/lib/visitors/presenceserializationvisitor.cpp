/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                           *
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
#include "presenceserializationvisitor.h"

class LIB_EXPORT DummyPresenceSerializationVisitor : public PresenceSerializationVisitor {
public:
   virtual void serialize() override;
   virtual void load     () override;
   virtual bool isTracked(AbstractItemBackendBase* backend) override;
   virtual void setTracked(AbstractItemBackendBase* backend, bool tracked) override;
   virtual ~DummyPresenceSerializationVisitor();

};

PresenceSerializationVisitor* PresenceSerializationVisitor::m_spInstance = new DummyPresenceSerializationVisitor();

void DummyPresenceSerializationVisitor::serialize()
{
   
}
void DummyPresenceSerializationVisitor::load()
{
   
}

bool DummyPresenceSerializationVisitor::isTracked(AbstractItemBackendBase* backend)
{
   Q_UNUSED(backend)
   return false;
}

void DummyPresenceSerializationVisitor::setTracked(AbstractItemBackendBase* backend, bool tracked)
{
   Q_UNUSED(backend)
   Q_UNUSED(tracked)
}

DummyPresenceSerializationVisitor::~DummyPresenceSerializationVisitor()
{}

PresenceSerializationVisitor* PresenceSerializationVisitor::instance()
{
   return m_spInstance;
}

void PresenceSerializationVisitor::setInstance(PresenceSerializationVisitor* ins)
{
   m_spInstance = ins;
   ins->load();
}