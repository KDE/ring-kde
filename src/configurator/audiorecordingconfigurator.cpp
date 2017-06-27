/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
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
#include "audiorecordingconfigurator.h"

#include <media/recordingmodel.h>

#include <KLocalizedString>

#include "ui_recording.h"

AudioRecordingConfigurator::AudioRecordingConfigurator(QObject* parent) : CollectionConfigurationInterface(parent)
{
}

QByteArray AudioRecordingConfigurator::id() const
{
    return "peerProfileConfigurator";
}

QString AudioRecordingConfigurator::name() const
{
    return i18n("Peer profile configurator");
}

QVariant AudioRecordingConfigurator::icon() const
{
    return QVariant();
}

void AudioRecordingConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
    Q_UNUSED(col)

    if (m_Init)
        return;

    if (auto w = qobject_cast<QWidget*>(parent)) {
        Ui_Recording ui;
        ui.setupUi(w);
        ui.tableView->setModel(&Media::RecordingModel::instance());
    }

    m_Init = true;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
