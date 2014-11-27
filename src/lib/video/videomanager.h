/******************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                            *
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
#include "../typedefs.h"
#include <QtCore/QThread>

//Qt
#include <QtCore/QHash>

//SFLPhone
#include "videodevice.h"
class VideoRenderer;
class Call;
class QMutex;
struct SHMHeader;

class VideoManagerPrivate;

///VideoModel: Video event dispatcher
class LIB_EXPORT VideoManager : public QThread {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop
public:
   //Singleton
   static VideoManager* instance();

   //Getters
   bool       isPreviewing       ();
   VideoRenderer* getRenderer(const Call* call) const;
   VideoRenderer* previewRenderer();
   QMutex* startStopMutex() const;

   //Setters
   void setBufferSize(uint size);
   void switchDevice(const VideoDevice* device) const;

protected:
//    void run();

private:
   //Constructor
   explicit VideoManager();
   virtual ~VideoManager();

   VideoManagerPrivate* d_ptr;
   Q_DECLARE_PRIVATE(VideoManager)

   //Static attributes
   static VideoManager* m_spInstance;

public Q_SLOTS:
   void stopPreview ();
   void startPreview();

Q_SIGNALS:
   ///Emitted when a new frame is ready
//    void frameUpdated();
   ///Emmitted when the video is stopped, before the framebuffer become invalid
//    void videoStopped();
   ///Emmitted when a call make video available
   void videoCallInitiated(VideoRenderer*);
   ///The preview started/stopped
   void previewStateChanged(bool startStop);
   void previewStarted(VideoRenderer* renderer);
   void previewStopped(VideoRenderer* renderer);
   
};

#endif
