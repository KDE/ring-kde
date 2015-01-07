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
#include "numbercategoryvisitor.h"
#include "../numbercategorymodel.h"

class DummyNumberCategoryVisitor : public NumberCategoryVisitor{
public:
   virtual void serialize(NumberCategoryModel* model) override;
   virtual void load     (NumberCategoryModel* model) override;
   virtual ~DummyNumberCategoryVisitor();
};

NumberCategoryVisitor* NumberCategoryVisitor::m_spInstance = new DummyNumberCategoryVisitor();


void DummyNumberCategoryVisitor::serialize(NumberCategoryModel* model)
{
   Q_UNUSED(model)
}

void DummyNumberCategoryVisitor::load(NumberCategoryModel* model)
{
   Q_UNUSED(model)
}

DummyNumberCategoryVisitor::~DummyNumberCategoryVisitor()
{}

NumberCategoryVisitor* NumberCategoryVisitor::instance()
{
   return m_spInstance;
}

void NumberCategoryVisitor::setInstance(NumberCategoryVisitor* ins)
{
   m_spInstance = ins;
   ins->load(NumberCategoryModel::instance());
}