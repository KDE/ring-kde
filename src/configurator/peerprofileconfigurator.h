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

class PeerProfileCollection2;
class Ui_PeerProfile;

class PeerProfileConfigurator : public CollectionConfigurationInterface
{
    Q_OBJECT
public:
    explicit PeerProfileConfigurator(QObject* parent = nullptr);
    virtual ~PeerProfileConfigurator();

    //Getter
    virtual QByteArray id  () const override;
    virtual QString    name() const override;
    virtual QVariant   icon() const override;

    virtual void loadCollection(CollectionInterface* col, QObject* parent = nullptr) override;

private:
    bool m_Init {false};
    PeerProfileCollection2* m_pCol {nullptr};
    Ui_PeerProfile* m_pUi {nullptr};

private Q_SLOTS:
    void slotCheckboxChecked();

};

// kate: space-indent on; indent-width 4; replace-tabs on;
