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
#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QtWidgets/QWidget>

#include "ui_fileselect.h"

/**
 * This was provided by KF5::KIOWidgets, but as this app is being moved to mobile
 * and QML, that dependency was increasingly unmaintainable. This code is
 * temporary and will be removed once the port is complete.
 */
class FileSelector : public QWidget, public Ui_FileSelect
{
   Q_OBJECT
public:
    explicit FileSelector(QWidget* parent = nullptr);

    void setText(const QString& text);
    QString text() const;

private Q_SLOTS:
    void slotFileSelector();
    void slotOpenFile();

Q_SIGNALS:
    void urlSelected(const QUrl& url);
};

#endif

// kate: space-indent on; indent-width 4; replace-tabs on;
