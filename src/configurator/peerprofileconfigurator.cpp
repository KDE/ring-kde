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
#include "peerprofileconfigurator.h"

#include <KLocalizedString>

#include <peerprofilecollection2.h>

#include "ui_peerprofile.h"

#include "klib/kcfg_settings.h"

#define TO_OPTION(b) b? PeerProfileCollection2::MergeOption::MERGE : PeerProfileCollection2::MergeOption::IGNORE

PeerProfileConfigurator::PeerProfileConfigurator(QObject* parent) : CollectionConfigurationInterface(parent)
{
    m_pCol->setMergeOption( Person::Role::FormattedName  , TO_OPTION(ConfigurationSkeleton::formattedName()));
    m_pCol->setMergeOption( Person::Role::NickName       , TO_OPTION(ConfigurationSkeleton::nickname     ()));
    m_pCol->setMergeOption( Person::Role::PrimaryName    , TO_OPTION(ConfigurationSkeleton::primaryName  ()));
    m_pCol->setMergeOption( Person::Role::LastName       , TO_OPTION(ConfigurationSkeleton::lastName     ()));
    m_pCol->setMergeOption( Person::Role::PreferredEmail , TO_OPTION(ConfigurationSkeleton::email        ()));
    m_pCol->setMergeOption( Person::Role::Organization   , TO_OPTION(ConfigurationSkeleton::org          ()));
}

PeerProfileConfigurator::~PeerProfileConfigurator()
{
    delete m_pUi;
}

QByteArray PeerProfileConfigurator::id() const
{
    return "peerProfileConfigurator";
}

QString PeerProfileConfigurator::name() const
{
    return i18n("Peer profile configurator");
}

QVariant PeerProfileConfigurator::icon() const
{
    return QVariant();
}

void PeerProfileConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
    Q_UNUSED(col)

    if (m_Init)
        return;

    if (auto w = qobject_cast<QWidget*>(parent)) {
        m_pCol = static_cast<PeerProfileCollection2*>(col);
        m_pUi = new Ui_PeerProfile;
        m_pUi->setupUi(w);

        switch(m_pCol->defaultMode()) {
            case PeerProfileCollection2::DefaultMode::NEW_CONTACT:
                m_pUi->m_pNewContact->setChecked(true);
                break;
            case PeerProfileCollection2::DefaultMode::IGNORE_DUPLICATE:
                m_pUi->m_pIgnore->setChecked(true);
                break;
            case PeerProfileCollection2::DefaultMode::QUICK_MERGE:
                m_pUi->m_pAddMissing->setChecked(true);
                break;
            case PeerProfileCollection2::DefaultMode::ALWAYS_ASK:
                m_pUi->m_pAlwaysAsk->setChecked(true);
                break;
            case PeerProfileCollection2::DefaultMode::CUSTOM:
                m_pUi->m_pCustom->setChecked(true);
                break;
        }

        auto l = [this]() {
            ConfigurationSkeleton::setDefaultPeerProfileMode((int)m_pCol->defaultMode());
            emit this->changed();
        };

        connect(m_pUi->m_pNewContact , &QRadioButton::toggled, [this, l](bool c) { if (c) m_pCol->setDefaultMode(PeerProfileCollection2::DefaultMode::NEW_CONTACT      ); l(); });
        connect(m_pUi->m_pIgnore     , &QRadioButton::toggled, [this, l](bool c) { if (c) m_pCol->setDefaultMode(PeerProfileCollection2::DefaultMode::IGNORE_DUPLICATE ); l(); });
        connect(m_pUi->m_pAddMissing , &QRadioButton::toggled, [this, l](bool c) { if (c) m_pCol->setDefaultMode(PeerProfileCollection2::DefaultMode::QUICK_MERGE      ); l(); });
        connect(m_pUi->m_pAlwaysAsk  , &QRadioButton::toggled, [this, l](bool c) { if (c) m_pCol->setDefaultMode(PeerProfileCollection2::DefaultMode::ALWAYS_ASK       ); l(); });
        connect(m_pUi->m_pCustom     , &QRadioButton::toggled, [this, l](bool c) { if (c) m_pCol->setDefaultMode(PeerProfileCollection2::DefaultMode::CUSTOM           ); l(); });

        for (auto w : {m_pUi->kcfg_formattedName, m_pUi->kcfg_nickname, m_pUi->kcfg_primaryName, m_pUi->kcfg_lastName,
                       m_pUi->kcfg_email, m_pUi->kcfg_org, m_pUi->kcfg_addresses, m_pUi->kcfg_phoneNumbers}) {
            connect(w, &QCheckBox::toggled, this, &PeerProfileConfigurator::slotCheckboxChecked);
        }
    }

    m_Init = true;
}

void PeerProfileConfigurator::slotCheckboxChecked()
{
    emit this->changed();
    m_pCol->setMergeOption( Person::Role::FormattedName  , TO_OPTION(m_pUi->kcfg_formattedName->isChecked()));
    m_pCol->setMergeOption( Person::Role::NickName       , TO_OPTION(m_pUi->kcfg_nickname     ->isChecked()));
    m_pCol->setMergeOption( Person::Role::PrimaryName    , TO_OPTION(m_pUi->kcfg_primaryName  ->isChecked()));
    m_pCol->setMergeOption( Person::Role::LastName       , TO_OPTION(m_pUi->kcfg_lastName     ->isChecked()));
    m_pCol->setMergeOption( Person::Role::PreferredEmail , TO_OPTION(m_pUi->kcfg_email        ->isChecked()));
    m_pCol->setMergeOption( Person::Role::Organization   , TO_OPTION(m_pUi->kcfg_org          ->isChecked()));

//     m_pCol->setMergeOption( Person::Role:: , kcfg_addresses    ->isChecked());
//     m_pCol->setMergeOption( Person::Role:: , kcfg_phoneNumbers ->isChecked());
}

#undef TO_OPTION

// kate: space-indent on; indent-width 4; replace-tabs on;
