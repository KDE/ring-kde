/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#ifndef SFLPHONE_CONST_H
#define SFLPHONE_CONST_H

#include <QtCore/QString>

/* @file sflphone_const.h
 * @brief Contains the global variables for the client code
 */

#define APP_NAME                          "SFLphone KDE Client"

#define ACTION_LABEL_CALL                 i18n("New call")
#define ACTION_LABEL_HANG_UP              i18n("Hang up")
#define ACTION_LABEL_HOLD                 i18n("Hold on")
#define ACTION_LABEL_TRANSFER             i18n("Transfer")
#define ACTION_LABEL_RECORD               i18n("Record")
#define ACTION_LABEL_ACCEPT               i18n("Pick up")
#define ACTION_LABEL_REFUSE               i18n("Hang up")
#define ACTION_LABEL_UNHOLD               i18n("Hold off")
#define ACTION_LABEL_GIVE_UP_TRANSF       i18n("Give up transfer")
#define ACTION_LABEL_MAILBOX              i18n("Voicemail")

#define ICON_INCOMING                     ":/images/icons/ring.svg"
#define ICON_RINGING                      ":/images/icons/ring.svg"
#define ICON_CURRENT                      ":/images/icons/current.svg"
#define ICON_DIALING                      ":/images/icons/dial.svg"
#define ICON_HOLD                         ":/images/icons/hold.svg"
#define ICON_FAILURE                      ":/images/icons/fail.svg"
#define ICON_BUSY                         ":/images/icons/busy.svg"
#define ICON_TRANSFER                     ":/images/icons/transfert.svg"
#define ICON_TRANSF_HOLD                  ":/images/icons/transfert.svg"
#define ICON_CONFERENCE                   ":/images/icons/user-group-properties.svg"
#define ICON_CALL                         ":/images/icons/call.svg"
#define ICON_HANGUP                       ":/images/icons/hang_up.svg"
#define ICON_UNHOLD                       ":/images/icons/unhold.svg"
#define ICON_ACCEPT                       ":/images/icons/accept.svg"
#define ICON_REFUSE                       ":/images/icons/refuse.svg"
#define ICON_EXEC_TRANSF                  ":/images/icons/call.svg"
#define ICON_REC_DEL_OFF                  ":/images/icons/record_disabled.svg"
#define ICON_REC_DEL_ON                   ":/images/icons/record.svg"
#define ICON_MAILBOX                      ":/images/icons/mailbox.svg"
#define ICON_REC_VOL_0                    ":/images/icons/mic.svg"
#define ICON_REC_VOL_1                    ":/images/icons/mic_25.svg"
#define ICON_REC_VOL_2                    ":/images/icons/mic_50.svg"
#define ICON_REC_VOL_3                    ":/images/icons/mic_75.svg"
#define ICON_SND_VOL_0                    ":/images/icons/speaker.svg"
#define ICON_SND_VOL_1                    ":/images/icons/speaker_25.svg"
#define ICON_SND_VOL_2                    ":/images/icons/speaker_50.svg"
#define ICON_SND_VOL_3                    ":/images/icons/speaker_75.svg"
#define ICON_DISPLAY_VOLUME_CONSTROLS     ":/images/icons/icon_volume_off.svg"
#define ICON_DISPLAY_DIALPAD              ":/images/icons/icon_dialpad.svg"
#define ICON_HISTORY_INCOMING             ":/images/icons/incoming.svg"
#define ICON_HISTORY_OUTGOING             ":/images/icons/outgoing.svg"
#define ICON_HISTORY_MISSED               ":/images/icons/missed.svg"
#define ICON_SFLPHONE                     ":/images/icons/sflphone.svg"

#define RECORD_DEVICE                     "mic"
#define SOUND_DEVICE                      "speaker"


/** Account details */
#define ACCOUNT_SIP_STUN_SERVER            "STUN.server"
#define ACCOUNT_SIP_STUN_ENABLED           "STUN.enable"
#define ACCOUNT_KEY_EXCHANGE               "SRTP.keyExchange"
#define ACCOUNT_SRTP_ENABLED               "SRTP.enable"
#define ACCOUNT_SRTP_RTP_FALLBACK          "SRTP.rtpFallback"
#define ACCOUNT_ZRTP_DISPLAY_SAS           "ZRTP.displaySAS"
#define ACCOUNT_ZRTP_NOT_SUPP_WARNING      "ZRTP.notSuppWarning"
#define ACCOUNT_ZRTP_HELLO_HASH            "ZRTP.helloHashEnable"
#define ACCOUNT_DISPLAY_SAS_ONCE           "ZRTP.displaySasOnce"
#define KEY_EXCHANGE_NONE                  "none"
// #define ZRTP                               "zrtp"
// #define SDES                               "sdes"



/**Security */
#define TLS_LISTENER_PORT                  "TLS.listenerPort"
#define TLS_ENABLE                         "TLS.enable"
#define TLS_PORT                           "TLS.port"
#define TLS_CA_LIST_FILE                   "TLS.certificateListFile"
#define TLS_CERTIFICATE_FILE               "TLS.certificateFile"
#define TLS_PRIVATE_KEY_FILE               "TLS.privateKeyFile"
#define TLS_PASSWORD                       "TLS.password"
#define TLS_METHOD                         "TLS.method"
#define TLS_CIPHERS                        "TLS.ciphers"
#define TLS_SERVER_NAME                    "TLS.serverName"
#define TLS_VERIFY_SERVER                  "TLS.verifyServer"
#define TLS_VERIFY_CLIENT                  "TLS.verifyClient"
#define TLS_REQUIRE_CLIENT_CERTIFICATE     "TLS.requireClientCertificate"
#define TLS_NEGOTIATION_TIMEOUT_SEC        "TLS.negotiationTimeoutSec"
#define TLS_NEGOTIATION_TIMEOUT_MSEC       "TLS.negotiationTimemoutMsec"




/** Maybe to remove **/
static const QString REGISTRATION_ENABLED_TRUE("true");
static const QString REGISTRATION_ENABLED_FALSE("false");
// #define ACCOUNT_TYPES_TAB                 {QString(Account::ProtocolName::SIP), QString(Account::ProtocolName::IAX)}
/*********************/

/** Constant variables */

/** Calls details */
#define CALL_PEER_NAME                    "DISPLAY_NAME"
//#define CALL_PEER_NAME                    "PEER_NAME"
#define CALL_PEER_NUMBER                  "PEER_NUMBER"
#define CALL_ACCOUNTID                    "ACCOUNTID"
#define CALL_STATE                        "CALL_STATE"
#define CALL_TYPE                         "CALL_TYPE"
#define CALL_TIMESTAMP_START              "TIMESTAMP_START"

#define CONF_STATE_CHANGE_HOLD            "HOLD"
#define CONF_STATE_CHANGE_ACTIVE          "ACTIVE_ATTACHED"

#define DAEMON_CALL_TYPE_INCOMING         "0"
#define DAEMON_CALL_TYPE_OUTGOING         "1"

/** Hooks settings */
#define HOOKS_ADD_PREFIX                  "PHONE_NUMBER_HOOK_ADD_PREFIX"
#define HOOKS_ENABLED                     "PHONE_NUMBER_HOOK_ENABLED"
#define HOOKS_COMMAND                     "URLHOOK_COMMAND"
#define HOOKS_IAX2_ENABLED                "URLHOOK_IAX2_ENABLED"
#define HOOKS_SIP_ENABLED                 "URLHOOK_SIP_ENABLED"
#define HOOKS_SIP_FIELD                   "URLHOOK_SIP_FIELD"

/** MIME API */
#define MIME_CALLID           "text/sflphone.call.id"
#define MIME_CONTACT          "text/sflphone.contact"
#define MIME_HISTORYID        "text/sflphone.history.id"
#define MIME_PHONENUMBER      "text/sflphone.phone.number"
#define MIME_PLAIN_TEXT       "text/plain"
#endif

/** HISTORY SERIALIZATION */
#define ACCOUNT_ID_KEY        "accountid"
#define CALLID_KEY            "callid"
#define DISPLAY_NAME_KEY      "display_name"
#define PEER_NUMBER_KEY       "peer_number"
#define RECORDING_PATH_KEY    "recordfile"
#define STATE_KEY             "state"
#define TIMESTAMP_START_KEY   "timestamp_start"
#define TIMESTAMP_STOP_KEY    "timestamp_stop"
#define MISSED_STRING         "missed"
#define INCOMING_STRING       "incoming"
#define OUTGOING_STRING       "outgoing"
