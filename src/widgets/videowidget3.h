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
#ifndef VIDEOWIDGET3_H
#define VIDEOWIDGET3_H

#include <QtGui/QGraphicsView>
#include "videoglframe.h"
#include <lib/video/videorenderer.h>

//Qt
class QGLWidget;
class QDragLeaveEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

//SFLPhone
class VideoRenderer;
class VideoScene;
class VideoGLFrame;

class VideoWidget3 : public QGraphicsView
{
   Q_OBJECT

public:
   explicit VideoWidget3(QWidget *parent);
   ~VideoWidget3();
//    virtual int    heightForWidth( int w ) const;
//    virtual QSize  sizeHint      (       ) const;

protected:
   virtual void resizeEvent(QResizeEvent* event);
   virtual void dragLeaveEvent   ( QDragLeaveEvent   * e );
   virtual void dragEnterEvent   ( QDragEnterEvent   * e );
   virtual void dragMoveEvent    ( QDragMoveEvent    * e );
   virtual void dropEvent        ( QDropEvent        * e );

private:
   VideoScene*          m_pScene  ;
   QGLWidget*           m_pWdg    ;
   QHash<VideoRenderer*,VideoGLFrame*> m_hFrames;
   VideoDevice*         m_pBackDevice;

public Q_SLOTS:
   void addRenderer(VideoRenderer* renderer);
   void removeRenderer(VideoRenderer* renderer);
   void slotRotateLeft();
   void slotRotateRight();
   void slotShowPreview(bool show);
   void slotMuteOutgoindVideo(bool mute);
   void slotKeepAspectRatio(bool mute);
   void slotPreviewEnabled(bool show);

Q_SIGNALS:
   void changed();
};

#endif
