/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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

#include <QtWidgets/QGraphicsView>
#include "videoglframe.h"
#include <video/renderer.h>

//Qt
class QGLWidget;
class QDragLeaveEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

//Ring
namespace Video {
   class Renderer;
   class SourceModel;
}
class VideoScene;
class VideoGLFrame;

class VideoWidget : public QGraphicsView
{
   Q_OBJECT

public:
   explicit VideoWidget(QWidget *parent, const bool previewOnly = false);
   virtual ~VideoWidget();
   void setPreviewOnly(bool prev);

protected:
   virtual void resizeEvent(QResizeEvent* event) override;
   virtual void dragLeaveEvent   ( QDragLeaveEvent   * e ) override;
   virtual void dragEnterEvent   ( QDragEnterEvent   * e ) override;
   virtual void dragMoveEvent    ( QDragMoveEvent    * e ) override;
   virtual void dropEvent        ( QDropEvent        * e ) override;

private:
   VideoScene*          m_pScene  ;
   QGLWidget*           m_pWdg    ;
   Video::Device*       m_pBackDevice;
   QHash<Video::Renderer*,VideoGLFrame*> m_hFrames;
   bool m_IsPreviewOnly;
   Video::SourceModel* m_pSourceModel {nullptr};

public Q_SLOTS:
   void addRenderer(Video::Renderer* renderer);
   void removeRenderer(Video::Renderer* renderer);
   void slotRotateLeft();
   void slotRotateRight();
   void slotShowPreview(bool show);
   void slotMuteOutgoindVideo(bool mute);
   void slotKeepAspectRatio(bool mute);
   void slotPreviewEnabled(bool show);
   void setSourceModel(Video::SourceModel* model);

Q_SIGNALS:
   void changed();
};

#endif
