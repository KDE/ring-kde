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
#include "viewcontactdock.h"

#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QQuickWidget>
#include <QQmlEngine>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QMimeData>
#include <QtCore/QTimer>

#include <../ringapplication.h>
#include "peerstimelinemodel.h"
#include <contactmethod.h>
#include <person.h>

#include "recentmodel.h"

class ViewContactDockPrivate {
public:
    QQuickWidget* m_pQuickWidget;
    QSharedPointer<QAbstractItemModel> m_CallsModel;
    QSharedPointer<QAbstractItemModel> m_PersomCMModel;
    QSharedPointer<QAbstractItemModel> m_TimelineModel;
    QSharedPointer<QAbstractItemModel> m_DeduplicatedTimelineModel;
};

ViewContactDock::ViewContactDock(QWidget* parent) :
    QWidget(parent), d_ptr(new ViewContactDockPrivate)
{
    d_ptr->m_pQuickWidget = new QQuickWidget(RingApplication::engine(), this);

    d_ptr->m_pQuickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    d_ptr->m_pQuickWidget->setSource(QUrl("qrc:/viewcontact.qml"));

    auto l = new QHBoxLayout(this);
    l->addWidget(d_ptr->m_pQuickWidget);
    setAcceptDrops(true);
    installEventFilter(this);

    QTimer::singleShot(0, [this]() {
        setContactMethod(qvariant_cast<ContactMethod*>(
            PeersTimelineModel::instance().index(0, 0).data((int)Ring::Role::Object)
        ));
    });

    d_ptr->m_DeduplicatedTimelineModel = PeersTimelineModel::instance().deduplicatedTimelineModel();
}

ViewContactDock::~ViewContactDock()
{
    delete d_ptr;
}

bool ViewContactDock::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    Q_UNUSED(event)
    //TODO the context menu
    return false;
}

void ViewContactDock::setContactMethod(ContactMethod* cm)
{
    if (!cm)
        return;

    // Keep a reference for 5 minutes to avoid double free from QML
    for (auto ptr : {d_ptr->m_PersomCMModel, d_ptr->m_CallsModel, d_ptr->m_TimelineModel})
        if (ptr)
            QTimer::singleShot(5 * 60 * 1000, [ptr]() {});

    // Keep a strong reference because QML wont
    d_ptr->m_PersomCMModel = cm->contact() ? cm->contact()->phoneNumbersModel()
        : QSharedPointer<QAbstractItemModel>();
    d_ptr->m_CallsModel = cm->callsModel();

    d_ptr->m_TimelineModel = cm->timelineModel();

    d_ptr->m_pQuickWidget->rootObject()->setProperty(
        "currentContactMethod", QVariant::fromValue(cm));

}

void ViewContactDock::setPerson(Person* p)
{
    d_ptr->m_pQuickWidget->rootObject()->setProperty(
        "currentPerson", QVariant::fromValue(p));
}

void ViewContactDock::setCurrentPage(ViewContactDock::Pages page)
{
    QString name;
    switch (page) {
        case ViewContactDock::Pages::INFORMATION:
            name = QStringLiteral("INFORMATION");
            break;
        case ViewContactDock::Pages::TIMELINE:
            name = QStringLiteral("TIMELINE");
            break;
        case ViewContactDock::Pages::CALL_HISTORY:
            name = QStringLiteral("CALL_HISTORY");
            break;
        case ViewContactDock::Pages::RECORDINGS:
            name = QStringLiteral("RECORDINGS");
            break;
        case ViewContactDock::Pages::SEARCH:
            name = QStringLiteral("SEARCH");
            break;
        case ViewContactDock::Pages::MEDIA:
            name = QStringLiteral("MEDIA");
            break;
    }

    d_ptr->m_pQuickWidget->rootObject()->setProperty(
        "currentPage", name);
}

#include <viewcontactdock.moc>
// kate: space-indent on; indent-width 4; replace-tabs on;
