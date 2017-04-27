/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#ifndef RESIZABLE_TIP_H
#define RESIZABLE_TIP_H

#include <QtWidgets/QWidget>

class QVBoxLayout;

class ResizableTip : public QWidget
{
   Q_OBJECT

   friend class Handle;

public:
   explicit ResizableTip(QWidget* parent = nullptr);
   virtual ~ResizableTip() {}

   //Mutator
   void addWidget(QWidget* w);

protected:
   //Virtual events
   void paintEvent ( QPaintEvent*  event) override;
   bool eventFilter( QObject *obj, QEvent *event) override;

private:
   QVBoxLayout* m_pLayout;

   //Helpers
   bool brightOrDarkBase();

Q_SIGNALS:
   void heightChanged(int h);
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
