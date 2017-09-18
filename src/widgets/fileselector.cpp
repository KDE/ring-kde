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
#include "fileselector.h"

#include <QtWidgets/QFileDialog>

#include <klocalizedstring.h>

FileSelector::FileSelector(QWidget* parent) : QWidget(parent)
{
    setupUi(this);
}

void FileSelector::slotFileSelector()
{
}

void FileSelector::slotOpenFile()
{
    const auto fn = QFileDialog::getOpenFileName(this, i18n("Open File"), "", tr("All files"));
    m_pFilePath->setText(fn);
    emit urlSelected(fn);
}

void FileSelector::setText(const QString& text)
{
    m_pFilePath->setText(text);
}

QString FileSelector::text() const
{
    return m_pFilePath->text();
}

// kate: space-indent on; indent-width 4; replace-tabs on;
