/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#include "pixmapinterface.h"

//Qt
#include <QtCore/QSize>
#include <QtCore/QBuffer>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QBitmap>
#include <QtWidgets/QApplication>
#include <QtGui/QImage>
#include <QtGui/QPalette>
#include <QtGui/QIcon>

//KDE
#include <KColorScheme>
#include <klocalizedstring.h>


//Ring
#include <person.h>
#include <contactmethod.h>
#include <presencestatusmodel.h>
#include <securityevaluationmodel.h>
#include <picocms/collectioninterface.h>
#include <useractionmodel.h>
#include <session.h>
#include <video/sourcemodel.h>
#include <QStandardPaths>
#include "cmiconengine.h"
#include "personiconengine.h"

#include "../jamivideoview/imageprovider.h" //FIXME

namespace RingIcons {
    constexpr static const char* INCOMING                 = ":/sharedassets/phone_dark/ringing.svg"         ;
    constexpr static const char* RINGING                  = ":/sharedassets/phone_dark/ringing.svg"         ;
    constexpr static const char* CURRENT                  = ":/sharedassets/phone_dark/current.svg"         ;
    constexpr static const char* DIALING                  = ":/sharedassets/phone_dark/dial.svg"            ;
    constexpr static const char* HOLD                     = ":/sharedassets/phone_dark/hold.svg"            ;
    constexpr static const char* FAILURE                  = ":/sharedassets/phone_dark/failed.svg"          ;
    constexpr static const char* BUSY                     = ":/sharedassets/phone_dark/busy.svg"            ;
    constexpr static const char* TRANSFER                 = ":/sharedassets/phone_dark/transfert.svg"       ;
    constexpr static const char* TRANSF_HOLD              = ":/sharedassets/phone_dark/transfert.svg"       ;
    constexpr static const char* CONFERENCE               = ":/images/icons/user-group-properties.svg"      ;
    constexpr static const char* CALL                     = ":/sharedassets/phone_dark/current.svg"         ;
    constexpr static const char* HANGUP                   = ":/sharedassets/phone_dark/hang_up.svg"         ;
    constexpr static const char* UNHOLD                   = ":/sharedassets/phone_dark/unhold.svg"          ;
    constexpr static const char* ACCEPT                   = ":/sharedassets/phone_dark/accept.svg"          ;
    constexpr static const char* REFUSE                   = ":/sharedassets/phone_dark/failed.svg"          ;
    constexpr static const char* INITIALIZATION           = ":/sharedassets/phone_dark/initialization.svg"  ;
    constexpr static const char* CONNECTED                = ":/sharedassets/phone_dark/connected.svg"       ;
    constexpr static const char* DISPLAY_VOLUME_CONSTROLS = ":/images/icons/icon_volume_off.svg"            ;
    constexpr static const char* DISPLAY_DIALPAD          = ":/images/icons/icon_dialpad.svg"               ;
    constexpr static const char* HISTORY_INCOMING         = ":/sharedassets/phone_dark/incoming.svg"        ;
    constexpr static const char* HISTORY_OUTGOING         = ":/sharedassets/phone_dark/outgoing.svg"        ;
    constexpr static const char* HISTORY_MISSED           = ":/sharedassets/phone_dark/missed_incoming.svg" ;
    constexpr static const char* HISTORY_MISSED_OUT       = ":/sharedassets/security/missed_outgoing.svg"            ;
}

static const char* callStateIcons[] = {
    RingIcons::DIALING       , /* NEW            */
    RingIcons::INCOMING      , /* INCOMING       */
    RingIcons::RINGING       , /* RINGING        */
    RingIcons::CURRENT       , /* CURRENT        */
    RingIcons::DIALING       , /* DIALING        */
    RingIcons::HOLD          , /* HOLD           */
    RingIcons::FAILURE       , /* FAILURE        */
    RingIcons::BUSY          , /* BUSY           */
    RingIcons::TRANSFER      , /* TRANSFERRED    */
    RingIcons::TRANSF_HOLD   , /* TRANSF_HOLD    */
    ""                       , /* OVER           */
    RingIcons::FAILURE       , /* ERROR          */
    RingIcons::CONFERENCE    , /* CONFERENCE     */
    RingIcons::HOLD          , /* CONFERENCE_HOLD*/
    RingIcons::INITIALIZATION, /* INITIALIZATION */
    RingIcons::FAILURE       , /* ABORTED        */
    RingIcons::CONNECTED     , /* CONNECTED      */
};

PixmapInterface::PixmapInterface() : Interfaces::PixmapManipulatorI()
{

}

QVariant PixmapInterface::contactPhoto(Person* c, const QSize& size, bool displayPresence)
{
    Q_UNUSED(displayPresence)
    Q_UNUSED(size)

    // Keep the engine cached
    if (c->property("icon").canConvert<QIcon>())
        return c->property("icon");

    auto icn = QIcon(new PersonIconEngine(c));
    c->setProperty("icon", icn);

    return icn;
}

QVariant PixmapInterface::callPhoto(const ContactMethod* n, const QSize& size, bool displayPresence)
{
    // Use the photo when available
    if (n->contact())
        return contactPhoto(n->contact(), size, displayPresence);

    // Keep the engine cached
    if (n->property("icon2").canConvert<QIcon>())
        return n->property("icon2");

    auto cm = const_cast<ContactMethod*>(n);

    QIcon icn(new CMIconEngine(cm));
    cm->setProperty("icon2", icn);

    return icn;
}

QVariant PixmapInterface::callPhoto(Call* c, const QSize& size, bool displayPresence)
{
    if (c->peerContactMethod()->contact()) {
        return contactPhoto(c->peerContactMethod()->contact(),size,displayPresence);
    }
    else
        return QIcon(callStateIcons[(int)c->state()]);
}

QVariant PixmapInterface::numberCategoryIcon(const QVariant& p, const QSize& size, bool displayPresence, bool isPresent)
{
    Q_UNUSED(size);
    if (displayPresence) {
        QPixmap pxm;
        if(p.isValid())
            pxm = qvariant_cast<QPixmap>(p);
        else
            pxm = QPixmap(QStringLiteral(":/mini/icons/miniicons/call.png"));
        QPainter painter(&pxm);
        painter.setOpacity(0.3);
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(pxm.rect(),isPresent?Qt::green:Qt::red);
        return pxm;
    }
    if (p.isValid())
        return qvariant_cast<QPixmap>(p);
    return QPixmap(QStringLiteral(":/mini/icons/miniicons/call.png"));
}

QVariant PixmapInterface::securityIssueIcon(const QModelIndex& index)
{
    SecurityEvaluationModel::Severity sev = qvariant_cast<SecurityEvaluationModel::Severity>(index.data((int)SecurityEvaluationModel::Role::Severity));
    switch(sev) {
        case SecurityEvaluationModel::Severity::INFORMATION:
            return QIcon::fromTheme(QStringLiteral("dialog-information"));
        case SecurityEvaluationModel::Severity::WARNING:
            return QIcon::fromTheme(QStringLiteral("dialog-warning"));
        case SecurityEvaluationModel::Severity::ISSUE:
        case SecurityEvaluationModel::Severity::FATAL_WARNING:
            return QIcon::fromTheme(QStringLiteral("view-barcode"));
        case SecurityEvaluationModel::Severity::ERROR:
            return QIcon::fromTheme(QStringLiteral("dialog-error"));
        case SecurityEvaluationModel::Severity::UNSUPPORTED:
        case SecurityEvaluationModel::Severity::COUNT__:
            break;
    }
    return QVariant();
}

QByteArray PixmapInterface::toByteArray(const QVariant& pxm, const QString& type)
{
    //Preparation of our QPixmap
    QByteArray bArray;
    QBuffer buffer(&bArray);
    buffer.open(QIODevice::WriteOnly);

    //PNG ?
    (qvariant_cast<QPixmap>(pxm)).save(&buffer, type.toLatin1());
    buffer.close();

    return bArray;
}

QVariant PixmapInterface::personPhoto(const QByteArray& data, const QString& type)
{
    if (data.isEmpty()) {
        return {};
    }

    QImage image;
    //For now, ENCODING is only base64 and image type PNG or JPG
    const bool ret = image.loadFromData(QByteArray::fromBase64(data),type.toLower().toLatin1());

    if (!ret) {
        qDebug() << "vCard image loading failed";
        return {};
    } //TODO check is width || height == 0

    return QPixmap::fromImage(image);
}

QVariant PixmapInterface::userActionIcon(const UserActionElement& state) const
{
    switch(state.action) {
        case UserActionModel::Action::ACCEPT          :
            return QIcon(":/sharedassets/phone_light/accept.svg"   );
        case UserActionModel::Action::HOLD            :
            return QIcon(":/sharedassets/phone_light/hold.svg"     );
        case UserActionModel::Action::MUTE_AUDIO      :
            return QIcon(":/sharedassets/phone_light/mic_off.svg"  );
        case UserActionModel::Action::MUTE_VIDEO      :
            return QIcon(":/sharedassets/phone_light/mic_off.svg"  );
        case UserActionModel::Action::SERVER_TRANSFER :
            return QIcon(":/sharedassets/phone_light/transfert.svg" );
        case UserActionModel::Action::RECORD          :
            return QIcon(":/sharedassets/phone_light/record_call.svg"   );
        case UserActionModel::Action::HANGUP          :
            return QIcon(":/sharedassets/phone_light/refuse.svg"   );
        case UserActionModel::Action::JOIN            :
            return QIcon();
        case UserActionModel::Action::ADD_NEW         :
            return QIcon(":/sharedassets/phone_light/accept.svg"   );
        case UserActionModel::Action::ADD_CONTACT:
        case UserActionModel::Action::ADD_TO_CONTACT:
        case UserActionModel::Action::ADD_CONTACT_METHOD:
            return QIcon::fromTheme(QStringLiteral("contact-new"));
        case UserActionModel::Action::DELETE_CONTACT:
            return QIcon::fromTheme(QStringLiteral("list-remove-user"), QIcon::fromTheme(QStringLiteral("edit-delete")));
        case UserActionModel::Action::EMAIL_CONTACT:
            return QIcon::fromTheme(QStringLiteral("mail-message-new"));
        case UserActionModel::Action::BOOKMARK:
            return QIcon::fromTheme(QStringLiteral("bookmarks"));
        case UserActionModel::Action::VIEW_CHAT_HISTORY:
            return QIcon::fromTheme(QStringLiteral("view-history"));
        case UserActionModel::Action::REMOVE_HISTORY :
            return QIcon::fromTheme(QStringLiteral("list-remove"));
        case UserActionModel::Action::CALL_CONTACT:
            return QIcon::fromTheme(QStringLiteral("call-start"));
        case UserActionModel::Action::EDIT_CONTACT:
            return QIcon::fromTheme(QStringLiteral("contact-new"));
        case UserActionModel::Action::COPY_CONTACT:
            return QIcon::fromTheme(QStringLiteral("edit-copy"));
        case UserActionModel::Action::TOGGLE_VIDEO:
        case UserActionModel::Action::MARK_AS_CONSUMED:
        case UserActionModel::Action::COUNT__:
            break;
    };

    return QVariant();
}

QVariant PixmapInterface::collectionIcon(const CollectionInterface* interface, Interfaces::PixmapManipulatorI::CollectionIconHint hint) const
{
    Q_UNUSED(interface)

    switch(hint) {
        case Interfaces::PixmapManipulatorI::CollectionIconHint::CONTACT:
            return QIcon::fromTheme(QStringLiteral("folder-publicshare"));
        case Interfaces::PixmapManipulatorI::CollectionIconHint::HISTORY:
            return QIcon::fromTheme(QStringLiteral("view-history"));
        case Interfaces::PixmapManipulatorI::CollectionIconHint::BOOKMARK:
            return QIcon::fromTheme(QStringLiteral("folder-bookmark"));
        case Interfaces::PixmapManipulatorI::CollectionIconHint::CERTIFICATE:
            return QIcon::fromTheme(QStringLiteral("certificate-server"));
        case Interfaces::PixmapManipulatorI::CollectionIconHint::RECORDING:
            return QIcon(":/sharedassets/phone_dark/mailbox.svg");
        case Interfaces::PixmapManipulatorI::CollectionIconHint::RINGTONE:
        case Interfaces::PixmapManipulatorI::CollectionIconHint::NONE:
        case Interfaces::PixmapManipulatorI::CollectionIconHint::PROFILE:
        case Interfaces::PixmapManipulatorI::CollectionIconHint::PHONE_NUMBER:
        case Interfaces::PixmapManipulatorI::CollectionIconHint::ACCOUNT:
        case Interfaces::PixmapManipulatorI::CollectionIconHint::MACRO:
            break;
    };

    return QVariant();
}

const char* PixmapInterface::icnPath[2][2] = {
    {
    /* INCOMING  */ RingIcons::HISTORY_INCOMING  ,
    /* OUTGOING  */ RingIcons::HISTORY_OUTGOING  ,
    },
    {
    /* MISSED_IN */ RingIcons::HISTORY_MISSED    ,
    /* MISSED_OUT*/ RingIcons::HISTORY_MISSED_OUT,
    }
};

QVariant PixmapInterface::securityLevelIcon(const SecurityEvaluationModel::SecurityLevel level) const
{
    switch (level) {
        case SecurityEvaluationModel::SecurityLevel::COUNT__:
        case SecurityEvaluationModel::SecurityLevel::NONE      :
            return QIcon(":/sharedassets/security/lock_off.svg");
        case SecurityEvaluationModel::SecurityLevel::WEAK      :
        case SecurityEvaluationModel::SecurityLevel::MEDIUM    :
            return QIcon(":/sharedassets/security/lock_unconfirmed.svg");
        case SecurityEvaluationModel::SecurityLevel::ACCEPTABLE:
        case SecurityEvaluationModel::SecurityLevel::STRONG    :
        case SecurityEvaluationModel::SecurityLevel::COMPLETE  :
            return QIcon(":/sharedassets/security/lock_confirmed.svg");
    }

    return QVariant();
}

QVariant PixmapInterface::historySortingCategoryIcon(const CallHistoryModel::SortedProxy::Categories cat) const
{
    switch(cat) {
        case CallHistoryModel::SortedProxy::Categories::DATE      :
            break;
        case CallHistoryModel::SortedProxy::Categories::NAME      :
            return QIcon::fromTheme(QStringLiteral("folder-publicshare"));
        case CallHistoryModel::SortedProxy::Categories::POPULARITY:
            return QIcon::fromTheme(QStringLiteral("folder-bookmark"));
        case CallHistoryModel::SortedProxy::Categories::LENGTH    :
        case CallHistoryModel::SortedProxy::Categories::SPENT_TIME:
        case CallHistoryModel::SortedProxy::Categories::COUNT__   :
            break;
    }
    return QVariant();
}

QVariant PixmapInterface::contactSortingCategoryIcon(const ContactModel::SortedProxy::Categories cat) const
{
    switch(cat) {
        case ContactModel::SortedProxy::Categories::NAME        :
            return QIcon::fromTheme(QStringLiteral("folder-publicshare"));
        case ContactModel::SortedProxy::Categories::RECENTLYUSED:
            return QIcon::fromTheme(QStringLiteral("view-history"));
        case ContactModel::SortedProxy::Categories::ORGANIZATION:
        case ContactModel::SortedProxy::Categories::GROUP       :
        case ContactModel::SortedProxy::Categories::DEPARTMENT  :
        case ContactModel::SortedProxy::Categories::COUNT__     :
            break;
    }
    return QVariant();
}

QVariant PixmapInterface::videoDeviceIcon(const QModelIndex& idx) const
{
    switch (idx.row()) {
        case Video::SourceModel::ExtendedDeviceList::NONE:
            return QIcon::fromTheme(QStringLiteral("camera-off"));
        case Video::SourceModel::ExtendedDeviceList::SCREEN:
            return QIcon::fromTheme(QStringLiteral("video-display"));
        case Video::SourceModel::ExtendedDeviceList::FILE:
            return QIcon::fromTheme(QStringLiteral("video-mp4"));
        default:
            return QIcon::fromTheme(QStringLiteral("camera-on"));
    }

    return {};
}

QString PixmapInterface::takeSnapshot(Call* call)
{
    return ImageProvider::takeSnapshot(call);
}

QVariant PixmapInterface::decorationRole(const QModelIndex& index)
{
    if (!index.isValid())
        return QVariant();

    const QVariant tv = index.data(static_cast<int>(Ring::Role::ObjectType));

    if (!tv.canConvert<Ring::ObjectType>())
        return QVariant();

    const Ring::ObjectType type = qvariant_cast<Ring::ObjectType>(tv);

    switch (type) {
        case Ring::ObjectType::Person         : {

            Person* p = qvariant_cast<Person*>(
                index.data(static_cast<int>(Ring::Role::Object))
            );

            return contactPhoto(p, QSize(22,22), true);
            }
        case Ring::ObjectType::ContactMethod  : {

            const ContactMethod* cm = qvariant_cast<ContactMethod*>(
                index.data(static_cast<int>(Ring::Role::Object))
            );

            return callPhoto(cm, QSize(22,22), true);
            }
        case Ring::ObjectType::Call           : {

            const Call* c = qvariant_cast<Call*>(
                index.data(static_cast<int>(Ring::Role::Object))
            );

            return callPhoto(c->peerContactMethod(), QSize(22,22), true);
            }
        case Ring::ObjectType::Media          : //TODO
        case Ring::ObjectType::Certificate    :
        case Ring::ObjectType::ContactRequest :
        case Ring::ObjectType::Event :
        case Ring::ObjectType::Individual :
        case Ring::ObjectType::COUNT__        :
            break;
    }

    return QVariant();
}

QVariant PixmapInterface::decorationRole(const Call* c)
{
    return callPhoto(c->peerContactMethod(), QSize(22,22), true);
}

QVariant PixmapInterface::decorationRole(const ContactMethod* cm)
{
    if (!cm)
        return {};

    return callPhoto((ContactMethod*)cm, QSize(22,22), true);
}

QVariant PixmapInterface::decorationRole(const Person* p)
{
    if (!p)
        return {};

    return contactPhoto((Person*)p, QSize(22,22), true);
}

QVariant PixmapInterface::decorationRole(const Account* a)
{
    Q_UNUSED(a)
    return {};
}

// kate: space-indent on; indent-width 4; replace-tabs on;
