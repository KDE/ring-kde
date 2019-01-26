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
#include "mainpage.h"

// Qt
#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtCore/QMimeData>
#include <QtCore/QTimer>
#include <QtCore/QSortFilterProxyModel>
#include <QQmlContext>

// LibRingQt
#include "peerstimelinemodel.h"
#include "individualdirectory.h"
#include <contactmethod.h>
#include <libcard/eventaggregate.h>
#include <individual.h>
#include <person.h>
#include <call.h>
#include <callmodel.h>
#include <session.h>

// Remove inactive calls from the CallModel
class ActiveCallProxy2 : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ActiveCallProxy2(QObject* parent) : QSortFilterProxyModel(parent) {}

protected:
    virtual bool filterAcceptsRow(int row, const QModelIndex & srcParent ) const override;
};

class MainPagePrivate final : public QObject {
    Q_OBJECT
public:
    QSharedPointer<QAbstractItemModel> m_CallsModel;
    Individual* m_Invididual {nullptr};
    QSharedPointer<QAbstractItemModel> m_TimelineModel;
//     QQuickItem* m_pItem   {nullptr};
//     QQuickItem* m_pHeader {nullptr};
    bool m_IsLocked{false};

    QList< QTimer* > m_lTimers;

    MainPage* q_ptr;

public Q_SLOTS:
    void slotWindowChanged();
};

MainPage::MainPage(QQuickItem* parent) :
    QQuickItem(parent), d_ptr(new MainPagePrivate)
{
    d_ptr->q_ptr = this;
    auto cp = new ActiveCallProxy2(Session::instance()->callModel());
    cp->setSourceModel(Session::instance()->callModel());

    connect(this, &QQuickItem::windowChanged, d_ptr, &MainPagePrivate::slotWindowChanged);
    connect(Session::instance()->callModel(), &CallModel::callAttentionRequest, this, &MainPage::showVideo);

    installEventFilter(this);

    // Wait a bit, the timeline will change while it is loading, so before
    // picking something at random, give it a chance to load some things.
    QTimer::singleShot(500, [this]() {
        if (d_ptr->m_Invididual)
            return;

        auto i = Session::instance()->peersTimelineModel()->mostRecentIndividual();

        // Select whatever is first (if any)
        if ((!i) && Session::instance()->peersTimelineModel()->rowCount())
            if (auto rawI = qvariant_cast<Individual*>(
                Session::instance()->peersTimelineModel()->index(0,0).data((int)Ring::Role::Object)
            ))
                i = rawI;

        // There is nothing yet, wait
        if (!i) {
            QMetaObject::Connection c;

            c = connect(Session::instance()->peersTimelineModel(), &QAbstractItemModel::rowsInserted, this, [c, this]() {
                if (d_ptr->m_Invididual) {
                    disconnect(c);
                    return;
                }

                if (auto i = Session::instance()->peersTimelineModel()->mostRecentIndividual()) {
                    disconnect(c);
                    setIndividual(i);
                    const auto idx = Session::instance()->peersTimelineModel()->individualIndex(i);
                    emit suggestSelection(i, idx);
                }
            });

            return;
        }

        setIndividual(i);

        if (i) {
            const auto idx = Session::instance()->peersTimelineModel()->individualIndex(i);
            emit suggestSelection(i, idx);
        }
    });
}

MainPage::~MainPage()
{
    d_ptr->m_TimelineModel.clear();
    d_ptr->m_Invididual = nullptr;
    d_ptr->m_CallsModel.clear();

    // Release the shared pointer reference from the timer.
    for (auto t : qAsConst(d_ptr->m_lTimers)) {
        disconnect(t);
        delete t;
    }

    delete d_ptr;
}

bool MainPage::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    Q_UNUSED(event)
    //TODO the context menu
    return false;
}

void MainPage::setIndividual(Individual* ind)
{
    if (!ind)
        return;


    d_ptr->m_IsLocked = true;
    emit changed();

    if (d_ptr->m_TimelineModel) {
        disconnect(d_ptr->m_TimelineModel.data(), &QAbstractItemModel::rowsInserted,
            this, &MainPage::contextInserted);
    }

    d_ptr->m_Invididual = ind;
    d_ptr->m_TimelineModel = ind->timelineModel();
    d_ptr->m_CallsModel = ind->eventAggregate()->unsortedListView();
    Q_ASSERT(d_ptr->m_CallsModel);


//     d_ptr->m_pItem->setProperty( "currentContactMethod", QVariant::fromValue(
//         static_cast<ContactMethod*>(nullptr)
//     ));

//     d_ptr->m_pItem->setProperty( "currentIndividual", QVariant::fromValue(
//         ind
//     ));

//     d_ptr->m_pItem->setProperty( "timelineModel", QVariant::fromValue(d_ptr->m_TimelineModel));
//     d_ptr->m_pItem->setProperty( "unsortedListView", QVariant::fromValue(d_ptr->m_CallsModel));

    if (d_ptr->m_TimelineModel) {
        connect(d_ptr->m_TimelineModel.data(), &QAbstractItemModel::rowsInserted,
            this, &MainPage::contextInserted);
    }
    d_ptr->m_IsLocked = false;
    emit changed();
}

void MainPage::setContactMethod(ContactMethod* cm)
{
    if ((!cm))
        return;


    cm = Session::instance()->individualDirectory()->fromTemporary(cm);

    // Keep a reference for 5 minutes to avoid double free from QML
    for (auto ptr : {d_ptr->m_CallsModel, d_ptr->m_TimelineModel})
        if (ptr) {
            auto t = new QTimer(this);
            t->setInterval(5 * 60 * 1000);
            t->setSingleShot(true);
            connect(t, &QTimer::timeout, this, [t, this, ptr]() {
                this->d_ptr->m_lTimers.removeAll(t);
            });
            t->start();
            d_ptr->m_lTimers << t;
        }

    if (d_ptr->m_TimelineModel) {
        disconnect(d_ptr->m_TimelineModel.data(), &QAbstractItemModel::rowsInserted,
            this, &MainPage::contextInserted);
    }

    // Keep a strong reference because QML won't
    d_ptr->m_Invididual = cm->individual();
    d_ptr->m_CallsModel = cm->individual()->eventAggregate()->unsortedListView();
    Q_ASSERT(d_ptr->m_CallsModel);

    d_ptr->m_TimelineModel = cm->individual()->timelineModel();
    emit changed();

//     d_ptr->m_pItem->setProperty( "currentContactMethod", QVariant::fromValue(cm));
//     d_ptr->m_pItem->setProperty( "currentIndividual", QVariant::fromValue(
//         cm->individual()
//     ));
//     d_ptr->m_pItem->setProperty( "timelineModel", QVariant::fromValue(d_ptr->m_TimelineModel));
//     d_ptr->m_pItem->setProperty( "unsortedListView", QVariant::fromValue(d_ptr->m_CallsModel));
/*
    emit changed();*/

    if (d_ptr->m_TimelineModel) {
        connect(d_ptr->m_TimelineModel.data(), &QAbstractItemModel::rowsInserted,
            this, &MainPage::contextInserted);
    }
}

void MainPage::setPerson(Person* p)
{
    if (d_ptr->m_TimelineModel) {
        disconnect(d_ptr->m_TimelineModel.data(), &QAbstractItemModel::rowsInserted,
            this, &MainPage::contextInserted);
    }

    auto ind = p->individual();
    d_ptr->m_Invididual = ind;

// //     d_ptr->m_pItem->setProperty( "currentPerson", QVariant::fromValue(p));
//     d_ptr->m_pItem->setProperty( "currentIndividual", QVariant::fromValue(
//         ind
//     ));

    d_ptr->m_CallsModel = ind->eventAggregate()->unsortedListView();
    Q_ASSERT(d_ptr->m_CallsModel);
/**/
//     d_ptr->m_pItem->setProperty( "unsortedListView", QVariant::fromValue(d_ptr->m_CallsModel));

    if (d_ptr->m_TimelineModel) {
        connect(d_ptr->m_TimelineModel.data(), &QAbstractItemModel::rowsInserted,
            this, &MainPage::contextInserted);
    }
    emit changed();
}

void MainPage::setCurrentPage(MainPage::Pages page)
{
    QString name;
    switch (page) {
        case MainPage::Pages::INFORMATION:
            name = QStringLiteral("INFORMATION");
            break;
        case MainPage::Pages::TIMELINE:
            name = QStringLiteral("TIMELINE");
            break;
        case MainPage::Pages::CALL_HISTORY:
            name = QStringLiteral("CALL_HISTORY");
            break;
        case MainPage::Pages::RECORDINGS:
            name = QStringLiteral("RECORDINGS");
            break;
        case MainPage::Pages::SEARCH:
            name = QStringLiteral("SEARCH");
            break;
        case MainPage::Pages::MEDIA:
            name = QStringLiteral("MEDIA");
            break;
    }

//     d_ptr->m_pItem->setProperty( "currentPage", name);
}

bool ActiveCallProxy2::filterAcceptsRow(int row, const QModelIndex& srcParent ) const
{
    // Conferences are always active
    if (srcParent.isValid())
        return true;

    return sourceModel()->index(row, 0)
        .data((int)Call::Role::LifeCycleState) == QVariant::fromValue(Call::LifeCycleState::PROGRESS);
}

void MainPagePrivate::slotWindowChanged()
{
    auto ctx = QQmlEngine::contextForObject(q_ptr);
    Q_ASSERT(ctx);

    QQmlComponent comp(ctx->engine(), QStringLiteral("qrc:/desktopview/qml/viewcontact.qml"), q_ptr);

//     m_pItem = qobject_cast<QQuickItem*>(comp.create(ctx));

//     if (!m_pItem) {
//         qDebug() << "Previous error" << comp.errorString();
//     }

//     Q_ASSERT(m_pItem);
//     m_pItem->setParentItem(q_ptr);

//     auto anchors = qvariant_cast<QObject*>(m_pItem->property("anchors"));
//     anchors->setProperty("fill", QVariant::fromValue(q_ptr));
//     m_pItem->setProperty( "unsortedListView", QVariant::fromValue(nullptr));
}

void MainPage::showVideo(Call* c)
{
    // Ignore clicks on dialing calls
    if ((!c) || c->state() == Call::State::NEW || c->state() == Call::State::DIALING)
        return;

    setContactMethod(c->peerContactMethod());
//     QMetaObject::invokeMethod(d_ptr->m_pItem, "showVideo");
}

// QQuickItem* MainPage::page() const
// {
//     return d_ptr->m_pItem;
// }


// QQuickItem* MainPage::header() const
// {
//     return d_ptr->m_pHeader;
// }

// void MainPage::setHeader(QQuickItem* item)
// {
//     d_ptr->m_pItem->setProperty("contactHeader", QVariant::fromValue(item));
//     d_ptr->m_pHeader = item;
// }

bool MainPage::isMobile() const
{
    return false;
//     return d_ptr->m_pItem->property("mobile").toBool();
}

void MainPage::setMobile(bool v)
{
//     d_ptr->m_pItem->setProperty("mobile", v);
}

Individual* MainPage::individual() const
{
    if (d_ptr->m_Invididual)
        return qobject_cast<Individual*>(d_ptr->m_Invididual);

    return nullptr;
}

QModelIndex MainPage::suggestedTimelineIndex() const
{
    if (d_ptr->m_Invididual)
        return Session::instance()->peersTimelineModel()->individualIndex(
            qobject_cast<Individual*>(d_ptr->m_Invididual)
        );

    return {};
}

void MainPage::contextInserted()
{
//     QMetaObject::invokeMethod(d_ptr->m_pItem, "showNewContent");
}

// ContactMethod* MainPage::currentContactMethod() const
// {
//     return nullptr;
// }

Individual* MainPage::currentIndividual() const
{
    return d_ptr->m_Invididual;
}

QAbstractItemModel* MainPage::timelineModel() const
{
    return d_ptr->m_IsLocked ? nullptr : d_ptr->m_TimelineModel.data();
}

QAbstractItemModel* MainPage::unsortedListView() const
{
    return d_ptr->m_IsLocked ? nullptr : d_ptr->m_CallsModel.data();
}

Person* MainPage::currentPerson() const
{
    return nullptr;
}


#include <mainpage.moc>
// kate: space-indent on; indent-width 4; replace-tabs on;
