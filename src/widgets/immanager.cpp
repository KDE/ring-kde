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
#include "immanager.h"
#include "call.h"
#include "callmodel.h"
#include "contactmethod.h"
#include "person.h"
#include <media/text.h>
#include <media/textrecording.h>
#include <media/recordingmodel.h>
#include "../delegates/imdelegate.h"
#include <eventmanager.h>

//KDE
#include <KColorScheme>
#include <klocalizedstring.h>

// Qt
#include <QtWidgets/QScrollBar>
#include <QtGui/QPixmap>
#include <QtCore/QTimer>

///Constructor
IMManager::IMManager(QWidget* parent) : QTabWidget(parent)
{
   setVisible(false);
   setTabsClosable(true);

   connect(&CallModel::instance(), &CallModel::mediaAdded, this, &IMManager::addMedia);

   connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeRequest(int)));

   connect(&Media::RecordingModel::instance(), &Media::RecordingModel::newTextMessage, this, &IMManager::newMessageInserted);

   foreach(Call* c, CallModel::instance().getActiveCalls()) {
      if (c->hasMedia(Media::Media::Type::TEXT, Media::Media::Direction::IN))
         addMedia(c, c->firstMedia<Media::Text>(Media::Media::Direction::IN));
      else if (c->hasMedia(Media::Media::Type::TEXT, Media::Media::Direction::OUT))
         addMedia(c, c->firstMedia<Media::Text>(Media::Media::Direction::OUT));
   }

   connect(this, &IMManager::currentChanged, this, &IMManager::clearColor);
}

void IMManager::newMessageInserted(Media::TextRecording* r, ContactMethod* cm)
{
   if (!r->instantMessagingModel()->index(
         r->instantMessagingModel()->rowCount()-1, 0
      ).data((int)Media::TextRecording::Role::HasText).toBool()
   )
      return;

   IMTab* tab = nullptr;

   if (!(tab  = m_lTabs[cm]))
      tab = newConversation(cm, r->instantTextMessagingModel());

   if (currentWidget() != tab
     || (!EventManager::mayHaveFocus())
     || tab->verticalScrollBar()->value() != tab->verticalScrollBar()->maximum()) {
      static QColor awayBrush = KStatefulBrush( KColorScheme::Window, KColorScheme::NegativeText ).brush(QPalette::Normal).color();

      tabBar()->setTabTextColor(indexOf(tab), awayBrush);
   }

}

void IMManager::addMedia(Call* c, Media::Media* m)
{
   if (m->type() == Media::Media::Type::TEXT) {

      Media::Text* media = static_cast<Media::Text*>(m);

      bool isRelevant = media->hasMimeType(QStringLiteral("text/plain"))
         || media->hasMimeType(QStringLiteral("text/html"));

      auto cm = c->peerContactMethod();

      if (!isRelevant) {
         connect(media, &Media::Text::mimeTypesChanged, [this, media, cm]() {
            bool isRelevant = media->hasMimeType(QStringLiteral("text/plain"))
               || media->hasMimeType(QStringLiteral("text/html"));

            if (isRelevant)
               newConversation(cm, media->recording()->instantTextMessagingModel());

         });
      }
   }
}

///Destructor
IMTab* IMManager::newConversation(ContactMethod* cm, QAbstractItemModel* model)
{
   if (auto tab = m_lTabs[cm]) {
      setCurrentWidget(m_lTabs[cm]);
      setVisible(true);
      return tab;
   }

   IMTab* newTab = new IMTab(model,this);
   m_lTabs[cm] = newTab;
   setVisible(true);
   const QString name = cm->primaryName();

   if (cm->contact())
      setCurrentIndex(addTab(newTab,qvariant_cast<QPixmap>(cm->contact()->photo()), name));
   else
      setCurrentIndex(addTab(newTab,name));


   // If the IMManager widget is not in the view yet, the scrollbar will be wrong
   QTimer::singleShot(0,[this, model, newTab] {
      newTab->scrollTo(model->index(model->rowCount()-1,0));

      if (newTab->verticalScrollBar())
         newTab->verticalScrollBar()->setValue(newTab->verticalScrollBar()->maximum());
   });

   return newTab;
}

///Display a conversation from history
bool IMManager::showConversation(ContactMethod* cm)
{
   if (!cm)
      return false;

   auto textRec = cm->textRecording();

   if (!textRec)
      return false;

   QAbstractItemModel* model = textRec->instantTextMessagingModel();

   if (model->rowCount())
      newConversation(cm, model);

   return true;
}

///Close a tab
void IMManager::closeRequest(int index)
{
   QWidget* wdg = widget(index);
   ContactMethod* cm = m_lTabs.key(qobject_cast<IMTab*>(wdg));
   if (cm) {
      m_lTabs[cm] = nullptr;
      delete wdg;
      if (!count())
         setVisible(false);
   }
}

void IMManager::clearColor(int idx)
{
   if (idx == -1)
      for(int i =0; i < count(); i++)
         clearColor(i);
   else
      tabBar()->setTabTextColor(idx, QColor());
}
