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
#pragma once

#include <collectionconfigurationinterface.h>

class QLabel;

class AudioRecordingConfigurator : public CollectionConfigurationInterface
{
    Q_OBJECT
public:
    enum class Mode {
        AUDIO,
        TEXT,
    };

    explicit AudioRecordingConfigurator(QObject* parent, Mode mode);

    //Getter
    virtual QByteArray id  () const override;
    virtual QString    name() const override;
    virtual QVariant   icon() const override;

    //Mutator

    virtual void loadCollection(CollectionInterface* col, QObject* parent = nullptr) override;

private Q_SLOTS:
    void slotSelectionChanged(const QModelIndex& idx);

private:
    bool m_Init {false};
    Mode m_Mode;
    QLabel* m_pLastUpdated;
    QLabel* m_pLength;
};

// kate: space-indent on; indent-width 4; replace-tabs on;
