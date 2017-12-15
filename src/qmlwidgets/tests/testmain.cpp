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

//Qt
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtGui/QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QMetaObject>
#include <QSharedPointer>

#include "modelviewtester.h"

#include <hierarchyview.h>
#include <quicklistview.h>
#include <quicktreeview.h>

#include <functional>

#define DO(slot) steps << QString(#slot) ;

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    auto m = new ModelViewTester(&app);
    auto mptr= QSharedPointer<QAbstractItemModel>(m);

    QQuickView view;
    view.engine()->rootContext()->setContextProperty("testmodel", QVariant::fromValue(mptr));

    qmlRegisterType<HierarchyView>("RingQmlWidgets", 1, 0, "HierarchyView");
    qmlRegisterType<QuickTreeView>("RingQmlWidgets", 1, 0, "QuickTreeView");
    qmlRegisterType<QuickListView>("RingQmlWidgets", 1, 0, "QuickListView");

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///modeltest.qml"));

    QStringList steps;

    // Append
    DO(appendSimpleRoot);
    DO(appendSimpleRoot);
    DO(appendSimpleRoot);
    DO(appendSimpleRoot);
    DO(appendSimpleRoot);

    DO(appendRootChildren);
    DO(appendRootChildren);
    DO(appendRootChildren);
    DO(appendRootChildren);

    // Prepend
    DO(prependSimpleRoot);

    // Move
    DO(moveRootToFront);
    DO(moveChildByOne);
    DO(moveChildByParent);
    DO(moveToGrandChildren);

    // Insert
    DO(insertRoot);
    DO(insertFirst);
    DO(insertChild);

    // Remove
    DO(removeRoot);
    DO(resetModel);

    int count = 0;

    QTimer r(&app);
    r.setInterval(100);

    QObject::connect(&r, &QTimer::timeout, &app, [&r, m, steps, &count]() {
        int methodIndex = m->metaObject()->indexOfMethod((steps[count]+"()").toLatin1());
        m->metaObject()->method(methodIndex).invoke(m, Qt::QueuedConnection);
        count++;
        if (count == steps.size())
            r.stop();
    });

    r.start();

    return app.exec();
}
