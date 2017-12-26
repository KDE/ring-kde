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
#include "photoselector.h"

// Qt
#include <QtCore/QDebug>
#include <QtGui/QPixmap>
#include <QtWidgets/QFileDialog>

//KDE
#include <klocalizedstring.h>

// Ring
#include <person.h>
#include <contactmethod.h>
#include <../qmlwidgets/contactbuilder.h>

class PhotoSelectorPrivate final
{
public:
    QRect m_SourceRect;
    QRect m_SelectedRect;
    QString m_Path;
    QImage m_Image;
};

PhotoSelector::PhotoSelector(QObject* parent) : QObject(parent), d_ptr(new PhotoSelectorPrivate)
{

}

PhotoSelector::~PhotoSelector()
{
    delete d_ptr;
}

QRect PhotoSelector::sourceRect() const
{
    return d_ptr->m_SourceRect;
}

QRect PhotoSelector::selectedSquare() const
{
    return d_ptr->m_SelectedRect;
}

void PhotoSelector::setSelectedSquare(QRect r)
{
    d_ptr->m_SelectedRect = r;
}

QString PhotoSelector::selectFile(const QString& defaultPath)
{
    Q_UNUSED(defaultPath)

    const auto fileName = QFileDialog::getOpenFileName(
        nullptr,
        i18n("Open Image"),
        QDir::currentPath(),
        i18n("Image Files (*.png *.jpg *.gif)")
    );

    d_ptr->m_Path = fileName;

    emit pathChanged(fileName);

    return QStringLiteral("file://")+path();
}

void PhotoSelector::setToPerson(Person* p)
{
    if (p) {
        p->setPhoto(QPixmap::fromImage(d_ptr->m_Image));
        qDebug() << "Set the photo on" << p << d_ptr->m_Image.size() << p->collection();
    }
    else {
        qWarning() << "Failed to set the photo: the contact is null";
    }
}

void PhotoSelector::setToContactMethod(ContactMethod* cm)
{
    if (cm->contact()) {
        setToPerson(cm->contact());
    }

    ContactBuilder b(this);

    b.from(cm);

    if (cm->contact()) {
        setToPerson(cm->contact());
        cm->contact()->save();
    }
    else
        qWarning() << "Failed to set the photo: failed to create a contact";
}

QString PhotoSelector::path() const
{
    return d_ptr->m_Path;
}


QImage PhotoSelector::image() const
{
    return d_ptr->m_Image;
}

void PhotoSelector::setImage(const QImage& image)
{
    d_ptr->m_Image = image;
}
