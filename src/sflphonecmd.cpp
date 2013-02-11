/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
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
#include "sflphonecmd.h"
#include <KCmdLineArgs>
#include <KUniqueApplication>

///Setup command line options before passing them to the KUniqueApplication
void SFLPhoneCmd::parseCmd(int argc, char **argv, KAboutData& about)
{
      KCmdLineArgs::init(argc, argv, &about);
      KCmdLineOptions options;
      options.add("place-call <number>", ki18n("Place a call to a given number"),"");
      KCmdLineArgs::addCmdLineOptions(options);

      KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

      KUniqueApplication::addCmdLineOptions();
}