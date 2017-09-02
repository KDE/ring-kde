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
#include "recentdock.h"

#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlEngine>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QtCore/QMimeData>

#include <qrc_recentdock.cpp>
#include <../ringapplication.h>

#include "peerstimelinemodel.h"
#include <numbercompletionmodel.h>
#include <useractionmodel.h>
#include "contactmethod.h"
#include "itemdataroles.h"

#include "../widgets/menumodelview.h"

class RecentDockPrivate {
public:
    QQuickWidget* m_pQuickWidget;
    UserActionModel* m_pUserActionModel {nullptr};
    QItemSelectionModel* m_pMenuSelectionModel {nullptr};
    QSharedPointer<QAbstractItemModel> m_PeersModel;

    void initUAM();
};

RecentDock::RecentDock(QWidget* parent) :
    QDockWidget(parent), d_ptr(new RecentDockPrivate)
{

    auto completionModel = new NumberCompletionModel();

    RingApplication::engine()->rootContext()->setContextProperty(
        QStringLiteral("CompletionModel"), completionModel
    );

    d_ptr->m_pQuickWidget = new QQuickWidget(RingApplication::engine(), this);

    d_ptr->m_pQuickWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    d_ptr->m_pQuickWidget->setSource(QUrl(QStringLiteral("qrc:/peerstimeline.qml")));

    setWidget(d_ptr->m_pQuickWidget);
    setAcceptDrops(true);
    installEventFilter(this);

    QObject *item = d_ptr->m_pQuickWidget->rootObject();
    connect(item, SIGNAL(contactMethodSelected(QVariant)),
        this, SLOT(slotViewContactMethod(QVariant)));
    connect(item, SIGNAL(contextMenuRequested(QVariant, int)),
        this, SLOT(slotContextMenu(QVariant, int)));
}

RecentDock::~RecentDock()
{
    //FIXME https://bugreports.qt.io/browse/QTBUG-40745
    setWidget(nullptr);

    if (d_ptr->m_pUserActionModel)
        delete d_ptr->m_pUserActionModel;

    if (d_ptr->m_pMenuSelectionModel)
        delete d_ptr->m_pMenuSelectionModel;

    d_ptr->m_PeersModel.clear();

    d_ptr->m_pQuickWidget->setVisible(false);
    d_ptr->m_pQuickWidget->hide();
    d_ptr->m_pQuickWidget->setParent(nullptr);
    delete d_ptr;
}

bool RecentDock::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    Q_UNUSED(event)
    //TODO the context menu

    if (event->type() == QEvent::HoverLeave
        || event->type() == QEvent::Leave
        || event->type() == QEvent::TouchCancel) {
        d_ptr->m_pQuickWidget->rootObject()
            ->setProperty("overlayVisible", false);
    }

    return QDockWidget::eventFilter(obj, event);
}

void RecentDock::slotViewContactMethod(const QVariant& cm)
{
    if (!cm.canConvert<ContactMethod*>())
        return;

    auto cm_ = qvariant_cast<ContactMethod*>(cm);

    if (cm_->type() == ContactMethod::Type::TEMPORARY)
        cm_ = PhoneDirectoryModel::instance().getNumber(cm_->uri(), cm_->account());

    emit viewContactMethod(cm_);

    setContactMethod(cm_);
}

void RecentDock::slotContextMenu(const QVariant& cm, int index)
{
    if (!cm.canConvert<ContactMethod*>())
        return;

    const auto idx = PeersTimelineModel::instance()
        .deduplicatedTimelineModel()->index(index, 0);

    d_ptr->initUAM();

    auto m = new MenuModelView(
        d_ptr->m_pUserActionModel->activeActionModel(),
        new QItemSelectionModel(d_ptr->m_pUserActionModel->activeActionModel()),
        this
    );

    d_ptr->m_pMenuSelectionModel->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);

    connect(m, &MenuModelView::itemClicked, this, &RecentDock::slotContextMenuClicked);

    m->exec(QCursor::pos());
}

void RecentDock::setContactMethod(ContactMethod* cm)
{
    auto item = d_ptr->m_pQuickWidget->rootObject();

    if (auto selectionModel = item->findChild<PeersTimelineSelectionModel*>(QStringLiteral("selectionMapper"))) {
        selectionModel->setContactMethod(cm);
    }
}

void RecentDockPrivate::initUAM()
{
    if (!m_pUserActionModel) {

        if (!m_PeersModel)
            m_PeersModel = PeersTimelineModel::instance().deduplicatedTimelineModel();

        m_pMenuSelectionModel = new QItemSelectionModel(m_PeersModel.data());
        m_pMenuSelectionModel->setModel(m_PeersModel.data());

        m_pUserActionModel = new UserActionModel(
            m_PeersModel.data(),
            UserActionModel::Context::ALL
        );

        auto item = m_pQuickWidget->rootObject();

        m_pUserActionModel->setSelectionModel(m_pMenuSelectionModel);
    }
}

void RecentDock::slotContextMenuClicked(const QModelIndex& index)
{
   d_ptr->m_pUserActionModel->execute(index);
}

#include <recentdock.moc>
// kate: space-indent on; indent-width 4; replace-tabs on;
