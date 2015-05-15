/***************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/
#ifndef IM_MANAGER_H
#define IM_MANAGER_H

//Qt
#include <QHash>

//KDE
#include <QTabWidget>

//Ring
class IMTab;
class InstantMessagingModel;
class ContactMethod;

class IMManager : public QTabWidget
{
   Q_OBJECT
public:
   //Constructor
   explicit IMManager(QWidget* parent = nullptr);

private:
   //Attrubutes
   QHash<ContactMethod*,IMTab*> m_lTabs;

private Q_SLOTS:
   void newConversation(ContactMethod* cm, InstantMessagingModel* model);
   void closeRequest(int index);
};

#endif // IM_MANAGER
