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
        EVAL    , /*!< Drag without lock    */
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
        OTHER   , /*!< Doesn't affect the state        */
        ACCEPT  , /*!< Accept the drag ownership       */
        REJECT  , /*!< Reject the drag ownership       */
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
    bool eval    (QMouseEvent* e); /*!< Check for potential drag ops */
    bool lock    (QMouseEvent* e); /*!< Lock the input grabber       */

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
    DragEvent eventMapper(QEvent* e) const;

    // State machine
    static const StateF m_fStateMachine[5][8];
    static const DragState m_fStateMap [5][8];

    SimpleFlickable* q_ptr;

public Q_SLOTS:
    void tick();
};

#define A &SimpleFlickablePrivate::           // Actions
#define S SimpleFlickablePrivate::DragState:: // Next state
/**
 * This is a Mealy machine, states callbacks are allowed to throw more events
 */
const SimpleFlickablePrivate::DragState SimpleFlickablePrivate::m_fStateMap[5][8] {
/*             TIMEOUT      PRESS      RELEASE     MOVE       TIMER      OTHER      ACCEPT    REJECT  */
/* IDLE    */ {S IDLE   , S PRESSED, S IDLE    , S IDLE   , S IDLE   , S IDLE   , S IDLE   , S IDLE   },
/* PRESSED */ {S PRESSED, S PRESSED, S IDLE    , S EVAL   , S PRESSED, S PRESSED, S PRESSED, S PRESSED},
/* EVAL    */ {S IDLE   , S EVAL   , S IDLE    , S EVAL   , S EVAL   , S EVAL   , S DRAGGED, S IDLE   },
/* DRAGGED */ {S DRAGGED, S DRAGGED, S INERTIA , S DRAGGED, S DRAGGED, S DRAGGED, S DRAGGED, S DRAGGED},
/* INERTIA */ {S IDLE   , S IDLE   , S IDLE    , S DRAGGED, S INERTIA, S INERTIA, S INERTIA, S INERTIA}};
const SimpleFlickablePrivate::StateF SimpleFlickablePrivate::m_fStateMachine[5][8] {
/*             TIMEOUT      PRESS      RELEASE     MOVE       TIMER      OTHER     ACCEPT   REJECT  */
/* IDLE    */ {A error  , A start  , A nothing , A nothing, A error  , A nothing, A error, A error  },
/* PRESSED */ {A error  , A nothing, A cancel  , A eval   , A error  , A nothing, A error, A error  },
/* EVAL    */ {A error  , A nothing, A cancel  , A eval   , A error  , A nothing, A lock , A cancel },
/* DRAGGED */ {A error  , A drag   , A release , A drag   , A error  , A nothing, A error, A error  },
/* INERTIA */ {A stop   , A stop   , A stop    , A error  , A inertia, A nothing, A error, A error  }};
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
    if (std::fabs(m_Velocity) < 40) //TODO C++17 use std::clamp
        m_Velocity = 0;

    return m_Velocity;
}

/**
 * Map qevent to DragEvent
 */
SimpleFlickablePrivate::DragEvent SimpleFlickablePrivate::eventMapper(QEvent* event) const
{
    auto e = SimpleFlickablePrivate::DragEvent::OTHER;

    #pragma GCC diagnostic ignored "-Wswitch-enum"
    switch(event->type()) {
        case QEvent::MouseMove:
            e = SimpleFlickablePrivate::DragEvent::MOVE;
            break;
        case QEvent::MouseButtonPress:
            e = SimpleFlickablePrivate::DragEvent::PRESS;
            break;
        case QEvent::MouseButtonRelease:
            e = SimpleFlickablePrivate::DragEvent::RELEASE;
            break;
        default:
            break;
    }
    #pragma GCC diagnostic pop

    return e;
}

/**
 * The tabs eat some mousePress events at random.
 *
 * Mitigate the issue by allowing the event series to begin later.
 */
bool SimpleFlickable::childMouseEventFilter(QQuickItem* item, QEvent* event)
{
    const auto e = d_ptr->eventMapper(event);

    return e == SimpleFlickablePrivate::DragEvent::OTHER ?
        QQuickItem::childMouseEventFilter(item, event) :
        d_ptr->applyEvent(e, static_cast<QMouseEvent*>(event) );
}

bool SimpleFlickable::event(QEvent *event)
{
    const auto e = d_ptr->eventMapper(event);

    if (event->type() == QEvent::Wheel) {
        setCurrentY(currentY() - static_cast<QWheelEvent*>(event)->angleDelta().y());
        event->accept();
        return true;
    }

    return e == SimpleFlickablePrivate::DragEvent::OTHER ?
        QQuickItem::event(event) :
        d_ptr->applyEvent(e, static_cast<QMouseEvent*>(event) );
}

void SimpleFlickable::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d_ptr->m_pContainer) {
        d_ptr->m_pContainer->setWidth(std::max(newGeometry.width(), d_ptr->m_pContainer->width()));
        d_ptr->m_pContainer->setHeight(std::max(newGeometry.height(), d_ptr->m_pContainer->height()));
    }

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
    Q_ASSERT(false);
    return false;
}

bool SimpleFlickablePrivate::stop(QMouseEvent* event)
{
    m_pTimer->stop();
    m_Velocity = 0;
    m_StartPoint = m_DragPoint  = {};

    // Resend for further processing
    if (event)
        applyEvent(SimpleFlickablePrivate::DragEvent::PRESS, event);

    return false;
}

bool SimpleFlickablePrivate::drag(QMouseEvent* e)
{
    if (!m_pContainer)
        return false;

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
    m_StartPoint = m_DragPoint = {};
    q_ptr->setKeepMouseGrab(false);

    // Reject the event, let the click pass though
    return false;
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

    return false;
}

bool SimpleFlickablePrivate::lock(QMouseEvent*)
{
    q_ptr->setKeepMouseGrab(true);
    q_ptr->grabMouse();

    return true;
}

bool SimpleFlickablePrivate::eval(QMouseEvent* e)
{
    // Reject large horizontal swipe and allow large vertical ones
    if (std::fabs(m_StartPoint.x() - e->pos().x()) > 10) {
        applyEvent(DragEvent::REJECT, e);
        return false;
    }
    else if (std::fabs(m_StartPoint.y() - e->pos().y()) > 10)
        applyEvent(DragEvent::ACCEPT, e);

    return drag(e);
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
