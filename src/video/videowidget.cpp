/***************************************************************************
 *   Copyright (C) 2017 by Bluesystems                                     *
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
#include "videowidget.h"

#include <QQmlApplicationEngine>
#include <QQuickPaintedItem>
#include <QQmlEngine>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>

#include <qrc_video.cpp>
#include <../ringapplication.h>

#include <video/previewmanager.h>

#include "imageprovider.h"

class ModelIconBinder : public QQuickPaintedItem
{
   Q_OBJECT
   Q_PROPERTY(QVariant pixmap READ pixmap WRITE setPixmap)

public:
    explicit ModelIconBinder(QQuickItem* parent = nullptr) : QQuickPaintedItem(parent) {}

    QPixmap pixmap() const {
       return m_pixmap;
    }
    void setPixmap(const QVariant& var) {
      m_pixmap = qvariant_cast<QPixmap>(var);
      m_icon   = qvariant_cast<QIcon  >(var);
    }

    virtual void paint(QPainter *painter) override {
      if (!m_pixmap.isNull()) {
         painter->drawPixmap(
            boundingRect(),
            m_pixmap,
            QRect( 0,0,m_pixmap.width(),m_pixmap.height())
         );
      }
      else if (!m_icon.isNull()) {
         const QPixmap pxm = m_icon.pixmap(boundingRect().size().toSize());

         painter->drawPixmap(boundingRect().toRect(),pxm);
      }
    }

private:
    QPixmap m_pixmap;
    QIcon   m_icon  ;
};

class VideoWidget3Private {
public:
    VideoWidget3::Mode m_Mode {VideoWidget3::Mode::CONVERSATION};
    static ImageProvider* m_spProvider;
};

// Only one must exist per context, better not try to regenerate it
ImageProvider* VideoWidget3Private::m_spProvider = nullptr;

VideoWidget3::VideoWidget3(QWidget* parent) :
    VideoWidget3(VideoWidget3::Mode::CONVERSATION, parent)
{}

VideoWidget3::VideoWidget3(VideoWidget3::Mode mode, QWidget* parent) :
    QQuickWidget(RingApplication::engine(), parent), d_ptr(new VideoWidget3Private)
{
    d_ptr->m_Mode = mode;

    if (!VideoWidget3Private::m_spProvider) {
        VideoWidget3Private::m_spProvider = new ImageProvider();
        RingApplication::engine()->addImageProvider("VideoFrame", d_ptr->m_spProvider);
        qmlRegisterType<ModelIconBinder>("Ring", 1,0, "ModelIconBinder");
    }

    if (Video::PreviewManager::instance().isPreviewing()) {
        Video::PreviewManager::instance().stopPreview();
        Video::PreviewManager::instance().startPreview();
    }
    else {
        Video::PreviewManager::instance().startPreview();
    }

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSource(QUrl("qrc:/mainvid.qml"));
}

VideoWidget3::~VideoWidget3()
{
    delete d_ptr;
}

void VideoWidget3::addRenderer(Video::Renderer* renderer)
{
    //TODO
}

void VideoWidget3::removeRenderer(Video::Renderer* renderer)
{
    //TODO
}

void VideoWidget3::slotRotateLeft()
{
    //TODO
}

void VideoWidget3::slotRotateRight()
{
    //TODO
}

void VideoWidget3::slotShowPreview(bool show)
{
    Q_UNUSED(show)
    //TODO
}

void VideoWidget3::slotMuteOutgoindVideo(bool mute)
{
    Q_UNUSED(mute)
    //TODO
}

void VideoWidget3::slotKeepAspectRatio(bool mute)
{
    Q_UNUSED(mute)
    //TODO
}

void VideoWidget3::slotPreviewEnabled(bool show)
{
    Q_UNUSED(show)
    //TODO
}

void VideoWidget3::setSourceModel(Video::SourceModel* model)
{
    Q_UNUSED(model)
    //TODO
}

void VideoWidget3::setMode(VideoWidget3::Mode mode)
{
    d_ptr->m_Mode = mode;
}

#include <videowidget.moc>
