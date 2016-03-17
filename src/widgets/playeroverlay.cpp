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
#include "playeroverlay.h"

// Qt
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtGui/QIcon>
#include <QtCore/QUrl>

// KDE
#include <kmessagebox.h>
#include <klocalizedstring.h>

// Ring
#include "media/recording.h"
#include "media/avrecording.h"

PlayerOverlay::PlayerOverlay(Media::AVRecording* rec, QWidget* parent) : QWidget(parent),m_pRecording(rec)
{
   setupUi(this);
   m_pPlayer->setVisible(false);
   m_pDelete->setIcon( QIcon::fromTheme(QStringLiteral("edit-delete")) );
   m_pPlay->setIcon( QIcon::fromTheme( QStringLiteral("media-playback-start") ));
   QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(m_pDelete);
   effect->setOpacity(0.3);
   m_pDelete->setGraphicsEffect(effect);
   effect = new QGraphicsOpacityEffect(m_pPlay);
   effect->setOpacity(0.3);
   m_pPlay->setGraphicsEffect(effect);
}

void PlayerOverlay::slotDeleteRecording()
{
   const int ret = KMessageBox::questionYesNo(this, i18n("Are you sure you want to delete this recording?"), i18n("Delete recording"));
   if (ret == KMessageBox::Yes) {
      qDebug() << "Deleting file";
      QFile::remove(m_pRecording->path().path());
      setVisible(false);
   }
}

void PlayerOverlay::setRecording(Media::AVRecording* rec)
{
   m_pRecording = rec;
}
