/******************************************************************************
 *   Copyright (C) 2012-2013 by Savoir-Faire Linux                            *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>   *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Lesser General Public               *
 *   License as published by the Free Software Foundation; either             *
 *   version 2.1 of the License, or (at your option) any later version.       *
 *                                                                            *
 *   This library is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *   Lesser General Public License for more details.                          *
 *                                                                            *
 *   You should have received a copy of the Lesser GNU General Public License *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/
#ifndef VIDEO_MODEL_H
#define VIDEO_MODEL_H
//Base
#include "typedefs.h"
#include <QtCore/QThread>

//Qt
#include <QtCore/QHash>

//SFLPhone
#include "videodevice.h"
class VideoRenderer;
class Call;
struct SHMHeader;

///VideoModel: Video event dispatcher
class LIB_EXPORT VideoModel : public QThread {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   //Singleton
   static VideoModel* instance();

   //Getters
   bool       isPreviewing       ();
   VideoRenderer* getRenderer(Call* call);
   VideoRenderer* previewRenderer();

   //Setters
   void       setBufferSize(uint size);

protected:
   void run();

private:
   //Constructor
   VideoModel();

   //Static attributes
   static VideoModel* m_spInstance;

   //Attributes
   bool           m_PreviewState;
   uint           m_BufferSize  ;
   uint           m_ShmKey      ;
   uint           m_SemKey      ;
   QHash<QString,VideoRenderer*> m_lRenderers;

public Q_SLOTS:
   void stopPreview ();
   void startPreview();

private Q_SLOTS:
   void startedDecoding(QString id, QString shmPath, int width, int height);
   void stoppedDecoding(QString id, QString shmPath);
   void deviceEvent();

Q_SIGNALS:
   ///Emitted when a new frame is ready
   void frameUpdated();
   ///Emmitted when the video is stopped, before the framebuffer become invalid
   void videoStopped();
   ///Emmitted when a call make video available
   void videoCallInitiated(VideoRenderer*);
};

#endif
