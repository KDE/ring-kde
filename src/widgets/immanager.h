/***************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#include <QtCore/QHash>
class QAbstractItemModel;

//KDE
#include <QtWidgets/QTabWidget>

//Ring
class IMTab;
class ContactMethod;
class Call;
namespace Media {
   class Media;
   class TextRecording;
}

class IMManager : public QTabWidget
{
   Q_OBJECT
public:
   //Constructor
   explicit IMManager(QWidget* parent = nullptr);

   //Mutator
   bool showConversation(ContactMethod* cm);

   Media::TextRecording* currentConversation() const;

private:
   //Attrubutes
   QHash<ContactMethod*,IMTab*> m_lTabs;
   QHash<Media::TextRecording*,IMTab*> m_lTabsByTR;

public Q_SLOTS:
   void clearColor(int idx = -1);

private Q_SLOTS:
   IMTab* newConversation(ContactMethod* cm, Media::TextRecording* rec);
   void closeRequest(int index);
   void addMedia(Call* c, Media::Media* m);
   void newMessageInserted(Media::TextRecording* r, ContactMethod* cm);
};

#endif // IM_MANAGER
