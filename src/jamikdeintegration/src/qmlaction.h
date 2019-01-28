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

// Qt
#include <QtCore/QObject>
#include <QQmlListProperty>
class QIcon;

class QmlActionPrivate;

typedef int ActionIconGroup ; //FIXME

class ActionCollection;

#define HAS_QTWIDGET_SUPPORT 1 //FIXME remove

#ifdef HAS_QTWIDGET_SUPPORT
    class QAction;
#endif

/**
 * Why does this C++ copy of Kirigami.Action exists:
 *
 *  * The menu on macOS is still a thing and it's mandatory.
 *  * It makes easier to port older QAction based code to Kirigami
 *  * Allows to have action collections and editors similar (or even compatible)
 *    with KXMLGui.
 *  * Having QML glue code to connect actions to their C++ SLOTs, check state
 *    and enables/disabled status is fragile.
 *  * It's required to expose the standard DBus actions interface and Automator
 *    action interface on macOS. That's useful for features such as Unity HUD
 *    that allows to search actions by names/description (accessibility).
 *  * It's possible to bridge KXMLGui and Kirigami to soften the blow of removing
 *    QtWidgets in smaller steps.
 *  * Optional way to bind QAction and QmlAction to ease porting
 */
class QmlAction : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("DefaultProperty", "children")
    Q_PROPERTY(QQmlListProperty<QmlAction> children READ children NOTIFY childrenChanged)
public:
    Q_PROPERTY(QString           name        READ name        WRITE setName       NOTIFY nameChanged        )
    Q_PROPERTY(QString           description READ description WRITE setDecription NOTIFY descriptionChanged )
    Q_PROPERTY(bool              checkable   READ isCheckable WRITE setCheckable  NOTIFY checkableChanged   )
    Q_PROPERTY(bool              checked     READ isChecked   WRITE setChecked    NOTIFY checkedChanged     )
    Q_PROPERTY(bool              enabled     READ isEnabled   WRITE setEnabled    NOTIFY enabledChanged     )
    Q_PROPERTY(ActionIconGroup   icon        READ icon        WRITE setIcon       NOTIFY iconChanged        )
    Q_PROPERTY(QString           iconName    READ iconName    WRITE setIconName   NOTIFY iconNameChanged    )
    Q_PROPERTY(QString           iconSource  READ iconSource  WRITE setIconSource NOTIFY iconSourceChanged  )
    Q_PROPERTY(QObject*          shortcut    READ shortcut    WRITE setShortcut   NOTIFY shortcutChanged    )
    Q_PROPERTY(QString           text        READ text        WRITE setText       NOTIFY textChanged        )
    Q_PROPERTY(QString           tooltip     READ tooltip     WRITE setTooltip    NOTIFY tooltipChanged     )
    Q_PROPERTY(bool              visible     READ isVisible   WRITE setVisible    NOTIFY visibleChanged     )

    Q_INVOKABLE explicit QmlAction(QObject* parent = nullptr);
    QmlAction(const QIcon& icon, const QString& text, ActionCollection* c);

#ifdef HAS_QTWIDGET_SUPPORT
    Q_PROPERTY(QAction* action READ action WRITE setAction)
    QmlAction(QAction* action);
    void setAction(QAction* a);
    QAction* action() const;
#endif

    virtual ~QmlAction();

    QString           name       () const;
    QString           description() const;
    bool              isCheckable() const;
    bool              isChecked  () const;
    bool              isEnabled  () const;
    ActionIconGroup   icon       () const;
    QString           iconName   () const;
    QString           iconSource () const;
    QObject*          shortcut   () const;
    QString           text       () const;
    QString           tooltip    () const;
    bool              isVisible  () const;

    QQmlListProperty<QmlAction> children();

    void setName      (const QString&  name        );
    void setDecription(const QString&  description );
    void setCheckable (bool            checkable   );
    void setChecked   (bool            checked     );
    void setEnabled   (bool            enabled     );
    void setIcon      (ActionIconGroup icon        );
    void setIconName  (const QString&  iconName    );
    void setIconSource(const QString&  iconSource  );
    void setShortcut  (QObject*        shortcut    );
    void setText      (const QString&  text        );
    void setTooltip   (const QString&  tooltip     );
    void setVisible   (bool            visible     );

public Q_SLOTS:
    void trigger();

Q_SIGNALS:
    void nameChanged       ();
    void descriptionChanged();
    void checkableChanged  ();
    void checkedChanged    ();
    void childrenChanged   ();
    void enabledChanged    ();
    void iconChanged       ();
    void iconNameChanged   ();
    void iconSourceChanged ();
    void shortcutChanged   ();
    void textChanged       ();
    void tooltipChanged    ();
    void visibleChanged    ();

    void toggled (bool checked);
    void triggered (QObject* source);

private:
    QmlActionPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QmlAction)
};

Q_DECLARE_METATYPE(QmlAction*)
