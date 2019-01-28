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
#include "qmlaction.h"

#include <QtCore/QDebug>

#include "actioncollection.h"

#ifdef HAS_QTWIDGET_SUPPORT
 #include <QtWidgets/QAction>
#endif

class QmlActionPrivate {
public:
    QString         m_Name       ;
    QString         m_Description;
    bool            m_Checkable  {false};
    bool            m_Checked    {false};
    bool            m_Enabled    {true};
    ActionIconGroup m_Icon       ;
    QIcon           m_QIcon      ;
    QString         m_IconName   ;
    QString         m_IconSource ;
    QObject*        m_Shortcut   {nullptr};
    QString         m_Text       ;
    QString         m_Tooltip    ;
    bool            m_Visible    {true};

#ifdef HAS_QTWIDGET_SUPPORT
    QAction*        m_pAction    {nullptr};
#endif

    QList<QmlAction*> m_lChildren;
};


QmlAction::QmlAction(const QIcon& icon, const QString& text, ActionCollection* c) :
    QObject(c), d_ptr(new QmlActionPrivate)
{
    d_ptr->m_Name = text,
    d_ptr->m_QIcon = icon;
}

QmlAction::QmlAction(QObject* parent) : QObject(parent), d_ptr(new QmlActionPrivate)
{

}

#ifdef HAS_QTWIDGET_SUPPORT
QmlAction::QmlAction(QAction* action) : QObject(action), d_ptr(new QmlActionPrivate)
{
    d_ptr->m_pAction = action;
}

QAction* QmlAction::action() const
{
    if ((!d_ptr->m_pAction) && !d_ptr->m_Name.isEmpty())
        d_ptr->m_pAction = new QAction(d_ptr->m_QIcon, d_ptr->m_Name, const_cast<QmlAction*>(this));
    return d_ptr->m_pAction;
}

void QmlAction::setAction(QAction* a)
{
    d_ptr->m_pAction = a;
}
#endif

QmlAction::~QmlAction()
{
    delete d_ptr;
}

QString QmlAction::name() const
{
    return d_ptr->m_Name;
}

QString QmlAction::description() const
{
    return d_ptr->m_Description;
}

bool QmlAction::isCheckable() const
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action())
        return d_ptr->m_pAction->isCheckable();
#endif
    return d_ptr->m_Checkable;
}

bool QmlAction::isChecked() const
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action())
        return d_ptr->m_pAction->isChecked();
#endif
    return d_ptr->m_Checked;
}

QQmlListProperty<QmlAction> QmlAction::children()
{
    return QQmlListProperty<QmlAction>(this, d_ptr->m_lChildren);
}

bool QmlAction::isEnabled() const
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action())
        return d_ptr->m_pAction->isEnabled();
#endif
    return d_ptr->m_Enabled;
}

ActionIconGroup QmlAction::icon() const
{
// #ifdef HAS_QTWIDGET_SUPPORT
//     if (d_ptr->m_pAction)
//         return d_ptr->m_pAction->icon().name();
// #endif
    return d_ptr->m_Icon;
}

QString QmlAction::iconName() const
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action())
        return d_ptr->m_pAction->icon().name();
#endif
    return d_ptr->m_IconName;
}

QString QmlAction::iconSource() const
{
    return d_ptr->m_IconSource;
}

QObject* QmlAction::shortcut() const
{
    return d_ptr->m_Shortcut;
}

QString QmlAction::text() const
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action())
        return d_ptr->m_pAction->text();
#endif
    return d_ptr->m_Text;
}

QString QmlAction::tooltip() const
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action())
        return d_ptr->m_pAction->toolTip();
#endif
    return d_ptr->m_Tooltip;
}

bool QmlAction::isVisible() const
{
    return d_ptr->m_Visible;
}

void QmlAction::setName(const QString& name)
{
    d_ptr->m_Name = name;
    Q_EMIT nameChanged();
}

void QmlAction::setDecription(const QString& description)
{
    d_ptr->m_Description = description;
    Q_EMIT descriptionChanged();
}

void QmlAction::setCheckable(bool checkable)
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action()) {
        d_ptr->m_pAction->setCheckable(checkable);
        return;
    }
#endif
    d_ptr->m_Checkable = checkable;
    Q_EMIT checkableChanged();
}

void QmlAction::setChecked(bool checked)
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action()) {
        d_ptr->m_pAction->setChecked(checked);
        return;
    }
#endif
    d_ptr->m_Checked = checked;
    Q_EMIT checkedChanged();
}

void QmlAction::setEnabled(bool enabled)
{
#ifdef HAS_QTWIDGET_SUPPORT
    if (action()) {
        d_ptr->m_pAction->setEnabled(enabled);
        return;
    }
#endif
    d_ptr->m_Enabled = enabled;
    Q_EMIT enabledChanged();
}

void QmlAction::setIcon(ActionIconGroup icon)
{
    d_ptr->m_Icon = icon;
    Q_EMIT iconChanged();
}

void QmlAction::setIconName(const QString& iconName)
{
    d_ptr->m_IconName = iconName;
    Q_EMIT iconNameChanged();
}

void QmlAction::setIconSource(const QString& iconSource)
{
    d_ptr->m_IconSource = iconSource;
    Q_EMIT iconSourceChanged();
}

void QmlAction::setShortcut(QObject* shortcut)
{
    d_ptr->m_Shortcut = shortcut;
    Q_EMIT shortcutChanged();
}

void QmlAction::setText(const QString& text)
{
    d_ptr->m_Text = text;
    Q_EMIT textChanged();
}

void QmlAction::setTooltip(const QString& tooltip)
{
    d_ptr->m_Tooltip = tooltip;
    Q_EMIT tooltipChanged();
}

void QmlAction::setVisible(bool visible)
{
    d_ptr->m_Visible = visible;
    Q_EMIT visibleChanged();
}

void QmlAction::trigger()
{
    if (!isEnabled()) {
        return;
    }

#ifdef HAS_QTWIDGET_SUPPORT
    if (action()) {
        d_ptr->m_pAction->trigger();
        return;
    }
#endif
    //root.triggered(source);
    //if (root.checkable) {
    //    root.checked = !root.checked;
    //    root.toggled(root.checked);
    //}
}
