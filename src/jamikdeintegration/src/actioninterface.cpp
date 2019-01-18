/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#include "actioninterface.h"

// Qt
#include <QtCore/QMimeData>
#include <QtGui/QGuiApplication>
#include <QtGui/QClipboard>
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QQuickView>
#include <QQuickItem>

// KDE
#include <KMessageBox>
#include <KLocalizedString>

// LibRingQt
#include "contactmethod.h"
#include "individualdirectory.h"
#include "media/textrecording.h"
#include "person.h"
#include "individual.h"
#include "session.h"

// Ring-KDE
#include "plugin.h"

void ActionInterface::editPerson(Person* p)
{
   QPointer<QQuickView> d = new QQuickView( JamiKDEIntegration::engine(), nullptr );

   d->setSource(QUrl(QStringLiteral("qrc:/ContactDialog.qml")));
   d->setResizeMode(QQuickView::SizeRootObjectToView);
   auto item = d->rootObject();
   item->setProperty("currentPerson", QVariant::fromValue(p));
   item->setProperty("showStat"   , false     );
   item->setProperty("showImage"  , true      );
   item->setProperty("forcedState", "profile" );

   d->resize(800, 600);
   d->show();
}

void ActionInterface::viewChatHistory(ContactMethod* cm)
{
   if (!cm)
      return;

   // Get a real contact method when necessary
   if (cm->type() == ContactMethod::Type::TEMPORARY) {
      cm = Session::instance()->individualDirectory()->getExistingNumberIf(
         cm->uri(),
         [](const ContactMethod* cm) -> bool {
            return cm->textRecording() && !cm->textRecording()->isEmpty();
      });
   }

   if (!cm)
      return;

   //FIXME open the timeline
}

void ActionInterface::viewChatHistory(Person* p)
{
   if (!p)
      return;

   foreach(ContactMethod* cm, p->individual()->phoneNumbers())
      viewChatHistory(cm);
}

void ActionInterface::copyInformation(QMimeData* data)
{
   QClipboard* clipboard = QGuiApplication::clipboard();
   clipboard->setMimeData(data);
}

bool ActionInterface::warnDeletePerson(Person* p)
{
   const int ret = KMessageBox::questionYesNo(nullptr,
      i18n("Are you sure you want to permanently delete %1?",
      p->formattedName()), i18n("Delete contact")
   );

   return ret == KMessageBox::Yes;
}

bool ActionInterface::warnDeleteCall(Call* c)
{
   Q_UNUSED(c)
   const int ret = KMessageBox::questionYesNo(nullptr,
      i18n("Are you sure you wish to remove this call?"), i18n("Delete call")
   );

   return ret == KMessageBox::Yes;
}

Person* ActionInterface::selectPerson(FlagPack<SelectPersonHint> hints, const QVariant& hintVar) const
{
   Q_UNUSED(hints)
   Q_UNUSED(hintVar)
   return nullptr;
}

ContactMethod* ActionInterface::selectContactMethod(FlagPack<ActionExtenderI::SelectContactMethodHint> hints, const QVariant& hintVar) const
{
   Q_UNUSED(hints)
   Q_UNUSED(hintVar)

   return nullptr;
}
