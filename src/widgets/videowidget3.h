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
#ifndef VIDEOWIDGET3_H
#define VIDEOWIDGET3_H

#include <QtGui/QGraphicsView>

//Qt
class QGLWidget;

//SFLPhone
class VideoRenderer;
class VideoScene;

class VideoWidget3 : public QGraphicsView
{
   Q_OBJECT

public:
   VideoWidget3(QWidget *parent);
   ~VideoWidget3();
//    virtual int    heightForWidth( int w ) const;
//    virtual QSize  sizeHint      (       ) const;

protected:
   virtual void resizeEvent(QResizeEvent* event);

private:
   VideoScene*          m_pScene  ;
   QGLWidget*           m_pWdg    ;

public Q_SLOTS:
   void addRenderer(VideoRenderer* renderer);

Q_SIGNALS:
   void changed();
};

#endif
