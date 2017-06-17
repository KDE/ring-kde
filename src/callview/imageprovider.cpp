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
#include "imageprovider.h"

// Qt
#include <QtCore/QMutexLocker>

#include <call.h>

#include <callmodel.h>
#include <video/previewmanager.h>

struct FrameBuffer {
    Video::Renderer* m_pRenderer {nullptr};
    int              m_Skip {3};
    char*            m_pFrameCopy {nullptr};
    uint             m_FrameSize {0};
    Video::Frame     m_Frame;
};

class ImageProviderPrivate : public QObject
{
    Q_OBJECT
public:
    QHash<QByteArray, FrameBuffer*> m_hActiveRenderers;
    QMutex m_FrameReader {};

public Q_SLOTS:
    void addRenderer(Call* c, Video::Renderer* renderer);
    void removeRenderer(Call* c, Video::Renderer* renderer);

    void previewStarted(Video::Renderer* r);
    void previewStopped(Video::Renderer* r);
};

ImageProvider::ImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image), d_ptr(new ImageProviderPrivate)
{
    QObject::connect(&CallModel::instance(), &CallModel::rendererAdded,
        d_ptr, &ImageProviderPrivate::addRenderer);

    QObject::connect(&CallModel::instance(), &CallModel::rendererRemoved,
        d_ptr, &ImageProviderPrivate::removeRenderer);

    QObject::connect(&Video::PreviewManager::instance(), &Video::PreviewManager::previewStarted,
        d_ptr, &ImageProviderPrivate::previewStarted);

    QObject::connect(&Video::PreviewManager::instance(), &Video::PreviewManager::previewStopped,
        d_ptr, &ImageProviderPrivate::previewStopped);

    // Check if there is active renderers

    foreach(Call* c, CallModel::instance().getActiveCalls()) {
        if (c->videoRenderer())
            d_ptr->addRenderer(c, c->videoRenderer());
    }

    if (Video::PreviewManager::instance().isPreviewing())
        d_ptr->previewStarted(Video::PreviewManager::instance().previewRenderer());
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize)

    const QStringList path =  id.split('/');

    if (Q_UNLIKELY(path.size() < 2)) {
        qWarning() << "Invalid video renderer id" << id;
        if (size)
            *size = QSize(0, 0);
        return {};
    }

    QMutexLocker locker(&d_ptr->m_FrameReader);

    //FIXME eventually find a better id, but currently, there is always only
    // zero to 2 video renderers.
    FrameBuffer* fb = path[0] == QLatin1String("preview") ?
        d_ptr->m_hActiveRenderers["preview"] :
        d_ptr->m_hActiveRenderers["peer"];

    if (fb) {

        //To give a bit of history about why this code is weird, it used to have another copy buffer
        //but this was removed and replaced with a flip buffer in libQt, there is some leftover
        //code that need to be cleaned
        Video::Frame frm = fb->m_pRenderer->currentFrame();

        QMutexLocker l2(fb->m_pRenderer->mutex());
        // Repaint the old frame if there is no new ones
        if (frm.size) {
            fb->m_Frame = std::move(frm);

            // I give up, every month, the daemon manage to break their SHM
            // memory model. From now on, ring-kde copy the frame. It's slow,
            // but wont cause surprise SIGSEGV (2015)
            if (!fb->m_pFrameCopy) {
                fb->m_pFrameCopy = new char[fb->m_Frame.size];
                fb->m_FrameSize  = fb->m_Frame.size;
            }
            else if (fb->m_Frame.size != fb->m_FrameSize) {
                delete[] fb->m_pFrameCopy;
                fb->m_pFrameCopy = new char[fb->m_Frame.size];
                fb->m_FrameSize  = fb->m_Frame.size;
            }

            memcpy(fb->m_pFrameCopy, fb->m_Frame.ptr, fb->m_FrameSize);
        }

        QSize res = fb->m_pRenderer->size();

        if (size)
            *size = res;

        return QImage((uchar*)fb->m_pFrameCopy, res.width(), res.height(), QImage::Format_ARGB32 );
    }

    if (size)
        *size = QSize(0, 0);

    return {};
}

void ImageProviderPrivate::addRenderer(Call* c, Video::Renderer* renderer)
{
    Q_UNUSED(c)

    QMutexLocker locker(&m_FrameReader);

    if (m_hActiveRenderers.contains("peer")) {
        qWarning() << "The peer video renderer already exists";
        delete m_hActiveRenderers["peer"];
    }

    auto fb = new FrameBuffer;
    fb->m_pRenderer = renderer;
    m_hActiveRenderers["peer"] = fb;
}

void ImageProviderPrivate::removeRenderer(Call* c, Video::Renderer* renderer)
{
    Q_UNUSED(c)
    Q_UNUSED(renderer)

    if (m_hActiveRenderers.contains("peer")) {
        QMutexLocker locker(&m_FrameReader);
        delete m_hActiveRenderers["peer"];
        m_hActiveRenderers.remove("peer");
    }
}

void ImageProviderPrivate::previewStarted(Video::Renderer* r)
{
    QMutexLocker locker(&m_FrameReader);
    if (m_hActiveRenderers.contains("preview") && m_hActiveRenderers["preview"]) {
        if (m_hActiveRenderers["preview"]->m_pRenderer == r)
            return;

        qWarning() << "The preview renderer already exists";
        delete m_hActiveRenderers["preview"];
        m_hActiveRenderers.remove("preview");
    }

    auto fb = new FrameBuffer;
    fb->m_pRenderer = r;
    m_hActiveRenderers["preview"] = fb;
}

void ImageProviderPrivate::previewStopped(Video::Renderer* r)
{
    Q_UNUSED(r)

    if (m_hActiveRenderers.contains("preview")) {
        QMutexLocker locker(&m_FrameReader);
        delete m_hActiveRenderers["preview"];
        m_hActiveRenderers.remove("preview");
    }

}

#include <imageprovider.moc>
