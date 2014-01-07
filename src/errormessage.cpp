/***************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                         *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "errormessage.h"
#include <KLocale>

///Error to display when there is nothing else to say
const QString ErrorMessage::GENERIC_ERROR = i18n("An unknown error occurred. SFLPhone KDE will now exit. If the problem persist, please report a bug.\n\n"
      "It is known that this message can be caused by trying to open SFLPhone KDE while the SFLPhone daemon is exiting. If so, waiting 15 seconds and "
      "trying again will solve the issue.");

///When the daemon is not detected
const QString ErrorMessage::NO_DAEMON_ERROR = i18n("The SFLPhone daemon (sflphoned) is not available. Please be sure it is installed correctly or launch it manually. \n\n\
Check in your distribution repository if the sflphone daemon (sometime called \"sflphone-common\") is available.\n\
Help for building SFLPhone daemon from source are present at https://projects.savoirfairelinux.com/projects/sflphone/wiki/How_to_build");
