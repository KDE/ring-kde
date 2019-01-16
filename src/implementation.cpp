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
#include "implementation.h"

//Qt
#include <QtGui/QColor>
#include <QtGui/QGuiApplication>
#include <QtCore/QMimeData>
#include <QtCore/QPointer>
#include <QtCore/QUrl>
#include <QtGui/QClipboard>
#include <QtGui/QIcon>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlApplicationEngine>
#include <QtWidgets/QFileDialog>

//KDE
#include <klocalizedstring.h>
#include <KMessageBox>

//Ring
#include <person.h>
#include <session.h>
#include <contactmethod.h>
#include <presencestatusmodel.h>
#include <individualdirectory.h>
#include <individual.h>
#include <media/textrecording.h>
#include <securityevaluationmodel.h>
#include "klib/kcfg_settings.h"
#include <collectioninterface.h>
#include "icons/icons.h"
#include <ringapplication.h>

ColorDelegate::ColorDelegate() : m_Pal(QGuiApplication::palette()) {
   m_Green = QColor(m_Pal.color(QPalette::Base));
   if (m_Green.green()+20 >= 255) {
      m_Green.setRed ( ((int)m_Green.red()  -20));
      m_Green.setBlue( ((int)m_Green.blue() -20));
   }
   else
      m_Green.setGreen(((int)m_Green.green()+20));

   m_Red = QColor(m_Pal.color(QPalette::Base));
   if (m_Red.red()+20 >= 255) {
      m_Red.setGreen(  ((int)m_Red.green()  -20));
      m_Red.setBlue(   ((int)m_Red.blue()   -20));
   }
   else
      m_Red.setRed(    ((int)m_Red.red()     +20));

   m_Yellow = QColor(m_Pal.color(QPalette::Base));
   if (m_Yellow.red()+20 >= 255 || m_Green.green()+20 >= 255) {
      m_Yellow.setBlue(((int)m_Yellow.blue() -20));
   }
   else {
      m_Yellow.setGreen(((int)m_Yellow.green()+20));
      m_Yellow.setRed( ((int)m_Yellow.red()   +20));
   }
}

QVariant ColorDelegate::color(const Account* a)
{
   switch(a->registrationState()) {
      case Account::RegistrationState::READY:
         return m_Green;
      case Account::RegistrationState::UNREGISTERED:
         return m_Pal.color(QPalette::Base);
      case Account::RegistrationState::TRYING:
      case Account::RegistrationState::INITIALIZING:
         return m_Yellow;
      case Account::RegistrationState::ERROR:
         return m_Red;
      case Account::RegistrationState::COUNT__:
         break;
   };
   return QVariant();
}

QVariant ColorDelegate::icon(const Account* a) {
   if (a->editState() == Account::EditState::MODIFIED_COMPLETE)
      return QIcon::fromTheme(QStringLiteral("document-save"));
   if (a->editState() == Account::EditState::MODIFIED_INCOMPLETE)
      return QIcon::fromTheme(QStringLiteral("dialog-warning"));
   else if (a->editState() == Account::EditState::OUTDATED) {
      return QIcon::fromTheme(QStringLiteral("view-refresh"));
   }
   return QVariant();
}

QVariant KDEShortcutDelegate::createAction(Macro* macro)
{
   Q_UNUSED(macro)
   return {};
}

void KDEActionExtender::editPerson(Person* p)
{
   QPointer<QQuickView> d = new QQuickView( RingApplication::engine(), nullptr );

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

void KDEActionExtender::viewChatHistory(ContactMethod* cm)
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

void KDEActionExtender::viewChatHistory(Person* p)
{
   if (!p)
      return;

   foreach(ContactMethod* cm, p->individual()->phoneNumbers())
      viewChatHistory(cm);
}

void KDEActionExtender::copyInformation(QMimeData* data)
{
   QClipboard* clipboard = QGuiApplication::clipboard();
   clipboard->setMimeData(data);
}

bool KDEActionExtender::warnDeletePerson(Person* p)
{
   const int ret = KMessageBox::questionYesNo(nullptr,
      i18n("Are you sure you want to permanently delete %1?",
      p->formattedName()), i18n("Delete contact")
   );

   return ret == KMessageBox::Yes;
}

bool KDEActionExtender::warnDeleteCall(Call* c)
{
   Q_UNUSED(c)
   const int ret = KMessageBox::questionYesNo(nullptr,
      i18n("Are you sure you wish to remove this call?"), i18n("Delete call")
   );

   return ret == KMessageBox::Yes;
}

Person* KDEActionExtender::selectPerson(FlagPack<SelectPersonHint> hints, const QVariant& hintVar) const
{
   Q_UNUSED(hints)
   Q_UNUSED(hintVar)
   return nullptr;
}

ContactMethod* KDEActionExtender::selectContactMethod(FlagPack<ActionExtenderI::SelectContactMethodHint> hints, const QVariant& hintVar) const
{
   Q_UNUSED(hints)
   Q_UNUSED(hintVar)

   return nullptr;
}

QUrl KDEDesktopFileProvider::getAnyFile(const QStringList& extensions) const
{
    Q_UNUSED(extensions)
    return QFileDialog::getOpenFileName(
        nullptr,
        QStringLiteral("Open File"),
        QDir::currentPath(),
        QStringLiteral("Media Files (*.png *.jpg *.gif *.mp4 *.mkv *.webm *.txt *.avi *.mpg)")
    );
}

QList<QUrl> KDEDesktopFileProvider::recentFiles() const
{
    const QStringList files = ConfigurationSkeleton::recentStreamedFiles();
    QList<QUrl> ret;

    for (const auto& f : qAsConst(files))
        ret << f;

    return ret;
}

void KDEDesktopFileProvider::addRecentFile(const QUrl& path) const
{
    QStringList files = ConfigurationSkeleton::recentStreamedFiles();
    files << path.path();

    ConfigurationSkeleton::setRecentStreamedFiles(files);

}

// kate: space-indent on; indent-width 4; replace-tabs on;
