/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
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
#include "codecs.h"

// #include <QtCore/QSortFilterProxyModel>
// #include <QtCore/QMimeData>
//
//
// Pages::Codecs::Codecs(QWidget *parent) : PageBase(parent)
// {
//    setupUi(this);
//    connect(this,&PageBase::accountSet,[this]() {
//       disconnect(button_audiocodecUp  );
//       disconnect(button_audiocodecDown);
//       m_pCodecs->setModel(account()->codecModel());
//       m_pCodecs->setSelectionModel(account()->codecModel()->selectionModel());
//       connect(button_audiocodecUp  , &QToolButton::clicked,account()->codecModel(), &CodecModel::moveUp  );
//       connect(button_audiocodecDown, &QToolButton::clicked,account()->codecModel(), &CodecModel::moveDown);
//    });
//
//    connect(m_pCodecs, &QListView::clicked, this, [this](const QModelIndex& idx) {
//       m_pSampleRate->setText(idx.data(CodecModel::Role::SAMPLERATE).toString());
//    });
//
//    connect(m_pCodecs, &QListView::clicked, this, [this](const QModelIndex& idx) {
//       m_pBitrate->setText(idx.data(CodecModel::Role::BITRATE).toString());
//    });
// }


// Qt
#include <QQmlEngine>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QQmlError>
#include <QtWidgets/QHBoxLayout>

// Ring
#include <account.h>
#include <codecmodel.h>
#include <ringapplication.h>

Pages::Codecs::Codecs(QWidget* parent) : PageBase(parent),m_pAccount(nullptr)
{
    setupUi(this);

    connect(this,&PageBase::accountSet,[this]() {
//         disconnect(button_audiocodecUp  );
//         disconnect(button_audiocodecDown);
//         m_pCodecs->setModel(account()->codecModel());
//         m_pCodecs->setSelectionModel(account()->codecModel()->selectionModel());
//         connect(button_audiocodecUp  , &QToolButton::clicked,account()->codecModel(), &CodecModel::moveUp  );
//         connect(button_audiocodecDown, &QToolButton::clicked,account()->codecModel(), &CodecModel::moveDown);
    });

//     connect(m_pCodecs, &QListView::clicked, this, [this](const QModelIndex& idx) {
//         m_pSampleRate->setText(idx.data(CodecModel::Role::SAMPLERATE).toString());
//     });
//
//     connect(m_pCodecs, &QListView::clicked, this, [this](const QModelIndex& idx) {
//         m_pBitrate->setText(idx.data(CodecModel::Role::BITRATE).toString());
//     });
}

void Pages::Codecs::setAccount(Account* a)
{
    m_pAccount = a;
}

// kate: space-indent on; indent-width 4; replace-tabs on;
