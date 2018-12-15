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

#include <QQuickItem>
#include <QtCore/QAbstractItemModel>

class MainPagePrivate;
class ContactMethod;
class Person;
class Call;
class Individual;

class MainPage : public QQuickItem
{
    Q_OBJECT

public:
    enum class Pages {
        INFORMATION,
        TIMELINE,
        CALL_HISTORY,
        RECORDINGS,
        SEARCH,
        MEDIA,
    };
    Q_ENUM(Pages);

    Q_PROPERTY(QQuickItem* page READ page CONSTANT)
    Q_PROPERTY(QQuickItem* header READ header WRITE setHeader CONSTANT)
    Q_PROPERTY(bool mobile READ isMobile WRITE setMobile CONSTANT)
    Q_PROPERTY(Individual* individual READ individual)
    Q_PROPERTY(QModelIndex suggestedTimelineIndex READ suggestedTimelineIndex)

    Q_INVOKABLE explicit MainPage(QQuickItem* parent = nullptr);
    virtual ~MainPage();

    Q_INVOKABLE void setCurrentPage(Pages page);

    QQuickItem* page() const;

    QQuickItem* header() const;
    void setHeader(QQuickItem* item);

    Individual* individual() const;
    QModelIndex suggestedTimelineIndex() const;

    bool isMobile() const;
    void setMobile(bool v);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

public Q_SLOTS:
    void setContactMethod(ContactMethod* cm);
    void setIndividual(Individual* ind);
    void setPerson(Person* p);
    void showVideo(Call* c);
    void contextInserted();

Q_SIGNALS:
    void suggestSelection(Individual* individual, const QModelIndex& modelIndex);

private:
    MainPagePrivate* d_ptr;
    Q_DECLARE_PRIVATE(MainPage)
};

Q_DECLARE_METATYPE(MainPage*)
