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
#include "simpleflickable.h"

// Qt
#include <QtCore/QAbstractItemModel>
#include <QtCore/QEasingCurve>
#include <QtCore/QTimer>
#include <QtCore/QDateTime>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQmlContext>

// LibStdC++
#include <cmath>

class SimpleFlickablePrivate final : public QObject
{
    Q_OBJECT
public:
    typedef bool(SimpleFlickablePrivate::*StateF)(QMouseEvent*);

    /// The current status of the inertial viewport state machine
    enum class DragState {
        IDLE    , /*!< Nothing is happening */
        PRESSED , /*!< A potential drag     */
        DRAGGED , /*!< An in progress grag  */
        INERTIA , /*!< A leftover drag      */
    };

    /// Events affecting the behavior of the inertial viewport state machine
    enum class DragEvent {
        TIMEOUT , /*!< When inertia is exhausted       */
        PRESS   , /*!< When a mouse button is pressed  */
        RELEASE , /*!< When a mouse button is released */
        MOVE    , /*!< When the mouse moves            */
        TIMER   , /*!< 30 times per seconds            */
    };

    // Actions
    bool nothing (QMouseEvent* e); /*!< No operations                */
    bool error   (QMouseEvent* e); /*!< Warn something went wrong    */
    bool start   (QMouseEvent* e); /*!< From idle to pre-drag        */
    bool stop    (QMouseEvent* e); /*!< Stop inertia                 */
    bool drag    (QMouseEvent* e); /*!< Move the viewport            */
    bool cancel  (QMouseEvent* e); /*!< Cancel potential drag        */
    bool inertia (QMouseEvent* e); /*!< Iterate on the inertia curve */
    bool release (QMouseEvent* e); /*!< Trigger the inertia          */

    // Attributes
    QQuickItem* m_pContainer {nullptr};
    QPointF     m_StartPoint {       };
    QPointF     m_DragPoint  {       };
    QTimer*     m_pTimer     {nullptr};
    qint64      m_StartTime  {   0   };
    int         m_LastDelta  {   0   };
    qreal       m_Velocity   {   0   };

    DragState m_State {DragState::IDLE};

    // Helpers
    void loadVisibleElements();
    bool applyEvent(DragEvent event, QMouseEvent* e);
    bool updateVelocity();

    // State machine
    static const StateF m_fStateMachine[4][5];
    static const DragState m_fStateMap [4][5];

    SimpleFlickable* q_ptr;

public Q_SLOTS:
    void tick();
};

#define A &SimpleFlickablePrivate::           // Actions
#define S SimpleFlickablePrivate::DragState:: // Next state
/**
 * This is a Mealy machine, states callbacks are allowed to throw more events
 */
const SimpleFlickablePrivate::DragState SimpleFlickablePrivate::m_fStateMap[4][5] {
/*             TIMEOUT      PRESS      RELEASE     MOVE       TIMER  */
/* IDLE    */ {S IDLE   , S PRESSED, S IDLE    , S IDLE   , S IDLE   },
/* PRESSED */ {S PRESSED, S PRESSED, S IDLE    , S DRAGGED, S PRESSED},
/* DRAGGED */ {S DRAGGED, S DRAGGED, S INERTIA , S DRAGGED, S DRAGGED},
/* INERTIA */ {S IDLE   , S IDLE   , S IDLE    , S DRAGGED, S INERTIA}};
const SimpleFlickablePrivate::StateF SimpleFlickablePrivate::m_fStateMachine[4][5] {
/*             TIMEOUT      PRESS      RELEASE     MOVE       TIMER  */
/* IDLE    */ {A error  , A start  , A nothing , A nothing, A error  },
/* PRESSED */ {A error  , A nothing, A cancel  , A drag   , A error  },
/* DRAGGED */ {A error  , A drag   , A release , A drag   , A error  },
/* INERTIA */ {A stop   , A stop   , A stop    , A error  , A inertia}};
#undef S
#undef A

SimpleFlickable::SimpleFlickable(QQuickItem* parent)
    : QQuickItem(parent), d_ptr(new SimpleFlickablePrivate)
{
    d_ptr->q_ptr = this;
    setClip(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildMouseEvents(true);

    d_ptr->m_pTimer = new QTimer(this);
    d_ptr->m_pTimer->setInterval(1000/30);
    connect(d_ptr->m_pTimer, &QTimer::timeout, d_ptr, &SimpleFlickablePrivate::tick);
}

SimpleFlickable::~SimpleFlickable()
{
    if (d_ptr->m_pContainer)
        delete d_ptr->m_pContainer;
    delete d_ptr;
}

QQuickItem* SimpleFlickable::contentItem()
{
    if (!d_ptr->m_pContainer) {
        QQmlEngine *engine = QQmlEngine::contextForObject(this)->engine();
        QQmlComponent rect1(engine, this);
        rect1.setData("import QtQuick 2.4; Item {}", {});
        d_ptr->m_pContainer = qobject_cast<QQuickItem *>(rect1.create());
        d_ptr->m_pContainer->setHeight(height());
        d_ptr->m_pContainer->setWidth(width ());
        engine->setObjectOwnership(d_ptr->m_pContainer, QQmlEngine::CppOwnership);
        d_ptr->m_pContainer->setParentItem(this);
    }

    return d_ptr->m_pContainer;
}

qreal SimpleFlickable::currentY() const
{
    if (!d_ptr->m_pContainer)
        return 0;

    return -d_ptr->m_pContainer->y();
}

void SimpleFlickable::setCurrentY(qreal y)
{
    if (!d_ptr->m_pContainer)
        return;

    // Do not allow out of bound scroll
    y = std::fmax(y, 0);
    y = std::fmin(y, d_ptr->m_pContainer->height() - height());

    d_ptr->m_pContainer->setY(-y);

    emit currentYChanged(y);
    emit percentageChanged(
        ((-d_ptr->m_pContainer->y()))/(d_ptr->m_pContainer->height()-height())
    );
}

qreal SimpleFlickable::contentHeight() const
{
    if (!d_ptr->m_pContainer)
        return 0;

    return  d_ptr->m_pContainer->height();
}

/// Timer events
void SimpleFlickablePrivate::tick()
{
    applyEvent(DragEvent::TIMER, nullptr);
}

/**
 * Use the linear velocity. This class currently mostly ignore horizontal
 * movements, but nevertheless the intention is to keep the inertia factor
 * from its vector.
 *
 * @return If there is inertia
 */
bool SimpleFlickablePrivate::updateVelocity()
{
    const qreal dy = m_DragPoint.y() - m_StartPoint.y();
    const qreal dt = (QDateTime::currentMSecsSinceEpoch() - m_StartTime)/(1000.0/30.0);

    // Points per frame
    m_Velocity = (dy/dt);

    // Do not start for low velocity mouse release
    if (std::fabs(m_Velocity) < 40)
        m_Velocity = 0;

    return m_Velocity;
}

/**
 * The tabs eat some mousePress events at random.
 *
 * Mitigate the issue by allowing the event series to begin later.
 */
bool SimpleFlickable::childMouseEventFilter(QQuickItem* item, QEvent* event)
{
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonPress) {
        SimpleFlickablePrivate::DragEvent e = SimpleFlickablePrivate::DragEvent::PRESS;
        switch (d_ptr->m_State) {
            case SimpleFlickablePrivate::DragState::IDLE:
            case SimpleFlickablePrivate::DragState::INERTIA:
                e = SimpleFlickablePrivate::DragEvent::PRESS;
                break;
            case SimpleFlickablePrivate::DragState::PRESSED:
            case SimpleFlickablePrivate::DragState::DRAGGED:
                e = SimpleFlickablePrivate::DragEvent::MOVE;
                break;
        }
        return d_ptr->applyEvent(e, static_cast<QMouseEvent*>(event) );
    }

    return QQuickItem::childMouseEventFilter(item, event);
}

bool SimpleFlickable::event(QEvent *event)
{
    #pragma GCC diagnostic ignored "-Wswitch-enum"
    switch(event->type()) {
        case QEvent::MouseButtonPress:
            return d_ptr->applyEvent(SimpleFlickablePrivate::DragEvent::PRESS,
                                     static_cast<QMouseEvent*>(event));
        case QEvent::MouseButtonRelease:
            return d_ptr->applyEvent(SimpleFlickablePrivate::DragEvent::RELEASE,
                                     static_cast<QMouseEvent*>(event));
        case QEvent::MouseMove:
            return d_ptr->applyEvent(SimpleFlickablePrivate::DragEvent::MOVE,
                                     static_cast<QMouseEvent*>(event));
        case QEvent::Wheel:
            setCurrentY(currentY() - static_cast<QWheelEvent*>(event)->angleDelta().y());
            event->accept();
            return true;
        default:
            break;
    }
    #pragma GCC diagnostic pop
    return QQuickItem::event(event);
}

void SimpleFlickable::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    //TODO prevent out of scope
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}

/// State functions ///

/**
 * Make the Mealy machine move between the states
 */
bool SimpleFlickablePrivate::applyEvent(DragEvent event, QMouseEvent* e)
{
    if (!m_pContainer)
        return false;

    // Set the state before the callback so recursive events work
    const int s = (int)m_State;
    m_State     = m_fStateMap            [s][(int)event];
    bool ret    = (this->*m_fStateMachine[s][(int)event])(e);

    if (ret && e)
        e->accept();

    return ret && e;
}

bool SimpleFlickablePrivate::nothing(QMouseEvent*)
{
    return false;
}

bool SimpleFlickablePrivate::error(QMouseEvent*)
{
    qWarning() << "simpleFlickable: Invalid state change";
    return false;
}

bool SimpleFlickablePrivate::stop(QMouseEvent* event)
{
    m_pTimer->stop();
    m_Velocity = 0;

    // Resend for further processing
    if (event)
        applyEvent(SimpleFlickablePrivate::DragEvent::PRESS, event);

    return false;
}

bool SimpleFlickablePrivate::drag(QMouseEvent* e)
{
    if (!m_pContainer)
        return false;

    q_ptr->setKeepMouseGrab(true);
    q_ptr->grabMouse();

    const int dy(e->pos().y() - m_DragPoint.y());
    m_DragPoint = e->pos();
    q_ptr->setCurrentY(q_ptr->currentY() - dy);

    // Reset the inertia on the differential inflexion points
    if ((m_LastDelta >= 0) ^ (dy >= 0)) {
        m_StartPoint = e->pos();
        m_StartTime  = QDateTime::currentMSecsSinceEpoch();
    }

    m_LastDelta = dy;

    return true;
}

bool SimpleFlickablePrivate::start(QMouseEvent* e)
{
    m_StartPoint = m_DragPoint = e->pos();
    m_StartTime  = QDateTime::currentMSecsSinceEpoch();

    q_ptr->setFocus(true, Qt::MouseFocusReason);

    // The event itself may be a normal click, let the children handle it too
    return false;
}

bool SimpleFlickablePrivate::cancel(QMouseEvent*)
{
    m_StartPoint = m_DragPoint  = {};
    q_ptr->setKeepMouseGrab(false);
    return true;
}

bool SimpleFlickablePrivate::release(QMouseEvent*)
{
    q_ptr->setKeepMouseGrab(false);
    q_ptr->ungrabMouse();

    if (updateVelocity())
        m_pTimer->start();
    else
        applyEvent(DragEvent::TIMEOUT, nullptr);

    m_DragPoint = {};

    return true;
}

bool SimpleFlickablePrivate::inertia(QMouseEvent*)
{
    m_Velocity *= 0.90; //TODO a configurable easing curve

    q_ptr->setCurrentY(q_ptr->currentY() - m_Velocity);

    if (std::fabs(m_Velocity) < 0.05)
        applyEvent(DragEvent::TIMEOUT, nullptr);

    return true;
}

#include <simpleflickable.moc>
