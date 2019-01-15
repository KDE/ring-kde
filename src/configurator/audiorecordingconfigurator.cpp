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
#include <call.h>
#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QWidget>

#include <KLocalizedString>

// #include "ui_recording.h"

AudioRecordingConfigurator::AudioRecordingConfigurator(QObject* parent, AudioRecordingConfigurator::Mode mode)
    : CollectionConfigurationInterface(parent), m_Mode(mode)
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

void AudioRecordingConfigurator::slotSelectionChanged(const QModelIndex& idx)
{
    auto date = idx.data((int)Ring::Role::FormattedLastUsed);

    if (date.isNull())
        date = idx.data((int)Call::Role::FormattedDate);

//     m_pLastUpdated->setText(date.toString());
//     m_pLength->setText(idx.data((int)Ring::Role::Length).toString());
}

void AudioRecordingConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
    Q_UNUSED(col)

    if (m_Init)
        return;

    if (auto w = qobject_cast<QWidget*>(parent)) {
//         Ui_Recording ui;
//         ui.setupUi(w);
//         m_pLastUpdated = ui.lastUpdated;
//         m_pLength      = ui.length;
//
//         QAbstractItemModel* m = nullptr;
//
//         if (m_Mode == Mode::AUDIO)
//             m = Media::Session::instance()->recordingModel()->audioRecordingModel();
//         else
//             m = Media::Session::instance()->recordingModel()->textRecordingModel();
//
//         auto p = new QSortFilterProxyModel(this);
//         connect(ui.lineEdit ,SIGNAL(filterStringChanged(QString)), p, SLOT(setFilterRegExp(QString)));
//         p->setSourceModel(m);
//         p->setFilterRole(Qt::DisplayRole);
//         p->setFilterCaseSensitivity( Qt::CaseInsensitive );
//         p->setSortCaseSensitivity  ( Qt::CaseInsensitive );
//
//         ui.tableView->setModel(p);
//         connect(ui.tableView->selectionModel(), &QItemSelectionModel::currentChanged,
//             this, &AudioRecordingConfigurator::slotSelectionChanged);
//
//         if (p->rowCount())
//             ui.tableView->selectionModel()->setCurrentIndex(
//                 p->index(0,0),
//                 QItemSelectionModel::ClearAndSelect
//             );
    }

    m_Init = true;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
