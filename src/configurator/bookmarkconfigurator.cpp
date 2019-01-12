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
#include "bookmarkconfigurator.h"

#include <KLocalizedString>

#include <categorizedbookmarkmodel.h>
#include <session.h>

#include "ui_bookmark.h"

BookmarkConfigurator::BookmarkConfigurator(QObject* parent) : CollectionConfigurationInterface(parent)
{
}

BookmarkConfigurator::~BookmarkConfigurator()
{
    delete m_pUi;
}

QByteArray BookmarkConfigurator::id() const
{
    return "peerProfileConfigurator";
}

QString BookmarkConfigurator::name() const
{
    return i18n("Peer profile configurator");
}

QVariant BookmarkConfigurator::icon() const
{
    return QVariant();
}

void BookmarkConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
    Q_UNUSED(col)

    if (m_Init)
        return;

    if (auto w = qobject_cast<QWidget*>(parent)) {
        m_pUi = new Ui_Bookmark();
        m_pUi->setupUi(w);
        connect(m_pUi->pushButton, &QPushButton::clicked, Session::instance()->bookmarkModel(), &CategorizedBookmarkModel::clear);
        connect(m_pUi->pushButton, &QPushButton::clicked, this, [this]() {
            m_HasChanged = true;
        });
        connect(m_pUi->kcfg_displayPopularAsBookmark, &QPushButton::toggled, this, [this]() {
            m_HasChanged = true;
        });
    }

    m_Init = true;
}

bool BookmarkConfigurator::hasChanged()
{
    return m_HasChanged;
}

void BookmarkConfigurator::save()
{
    if (!m_pUi)
        return;

    Session::instance()->bookmarkModel()->setDisplayPopular(
        m_pUi->kcfg_displayPopularAsBookmark->isChecked()
    );

    m_HasChanged = false;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
