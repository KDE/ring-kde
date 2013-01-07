/***************************************************************************
 *   Copyright (C) 2009-2012 by Savoir-Faire Linux                         *
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
// #ifndef EXTENDED_ACTION_H
// #define EXTENDED_ACTION_H
#include <KAction>

//KDE
class KIcon;

class ExtendedAction : public KAction
{
   Q_OBJECT
public:
   ExtendedAction(QObject* parent = nullptr);
   virtual ~ExtendedAction();

   const KIcon& altIcon();
   void setAltIcon(QString path);

private:
   KIcon* m_pIcon;

public slots:
   void setText(const QString&);
   void hasChanged();

signals:
   void textChanged(QString text);
};

// #endif //EXTENDED_ACTION_H