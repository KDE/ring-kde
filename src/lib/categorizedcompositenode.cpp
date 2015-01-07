/****************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                          *
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
#include "categorizedcompositenode.h"

class CategorizedCompositeNodePrivate
{
public:
   CategorizedCompositeNodePrivate();
   CategorizedCompositeNode::Type m_type;
   char m_DropState;
   int  m_HoverState;
   CategorizedCompositeNode* m_pParent;
};

CategorizedCompositeNodePrivate::CategorizedCompositeNodePrivate():
m_DropState(0),m_pParent(nullptr),m_HoverState(0)
{
   
}

CategorizedCompositeNode::CategorizedCompositeNode(CategorizedCompositeNode::Type _type) : d_ptr(new CategorizedCompositeNodePrivate())
{
   d_ptr->m_type =_type;
}

CategorizedCompositeNode::~CategorizedCompositeNode()
{
}

char CategorizedCompositeNode::dropState()
{
   return d_ptr->m_DropState;
}

void CategorizedCompositeNode::setDropState(const char state)
{
   d_ptr->m_DropState = state;
}

CategorizedCompositeNode::Type CategorizedCompositeNode::type() const
{
   return d_ptr->m_type;
}

int CategorizedCompositeNode::hoverState()
{
   return d_ptr->m_HoverState;
}

void CategorizedCompositeNode::setHoverState(const int state)
{
   d_ptr->m_HoverState = state;
}

CategorizedCompositeNode* CategorizedCompositeNode::parentNode() const
{
   return d_ptr->m_pParent;
}

void CategorizedCompositeNode::setParentNode(CategorizedCompositeNode* node)
{
   d_ptr->m_pParent = node;
}
