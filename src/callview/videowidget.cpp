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
#include <QtCore/QMimeData>

#include <qrc_callview.cpp>
#include <../ringapplication.h>

#include <video/previewmanager.h>
#include <video/sourcemodel.h>
#include <call.h>
#include <useractionmodel.h>

#include "imageprovider.h"

class VideoWidget3Private {
public:
    VideoWidget3::Mode m_Mode {VideoWidget3::Mode::CONVERSATION};
    static ImageProvider* m_spProvider;
    Video::SourceModel* m_pSourceModel {nullptr};
};

// Only one must exist per context, better not try to regenerate it
ImageProvider* VideoWidget3Private::m_spProvider = nullptr;

VideoWidget3::VideoWidget3(QWidget* parent) :
    VideoWidget3(VideoWidget3::Mode::CONVERSATION, parent)
{}

void VideoWidget3::initProvider()
{
    if (!VideoWidget3Private::m_spProvider) {
        VideoWidget3Private::m_spProvider = new ImageProvider();
        RingApplication::engine()->addImageProvider(
            "VideoFrame", VideoWidget3Private::m_spProvider
        );
    }
}

VideoWidget3::VideoWidget3(VideoWidget3::Mode mode, QWidget* parent) :
    QQuickWidget(RingApplication::engine(), parent), d_ptr(new VideoWidget3Private)
{
    d_ptr->m_Mode = mode;

    initProvider();

    setResizeMode(QQuickWidget::SizeRootObjectToView);
    setSource(QUrl("qrc:/CallView.qml"));

    setMode(mode);

    setAcceptDrops(true);
    installEventFilter(this);
}

VideoWidget3::~VideoWidget3()
{
    delete d_ptr;
}

bool VideoWidget3::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if (event->type() == QEvent::DragEnter) {
        auto e = static_cast<QDragEnterEvent*>(event);
        e->acceptProposedAction();
        event->accept();
        return true;
    }
    else if (event->type() == QEvent::DragMove) {
        event->accept();
        return true;
    }
    else if (event->type() == QEvent::KeyRelease) {
        auto e = static_cast<QKeyEvent*>(event);
        if (e->key() == Qt::Key_Escape) {
            if (auto c = qvariant_cast<Call*>(rootObject()->property("call"))) {
                c->userActionModel() << UserActionModel::Action::HANGUP;
                e->accept();
                return true;
            }
        }
    }
    else if (event->type() == QEvent::Drop) {
        auto e = static_cast<QDropEvent*>(event);
        if (d_ptr->m_pSourceModel && e->mimeData()->hasFormat(QStringLiteral("text/uri-list"))) {
            const auto url =  QUrl(e->mimeData()->data(QStringLiteral("text/uri-list")).trimmed());
            d_ptr->m_pSourceModel->setFile(url);
            e->accept();
            return true;
        }
    }

    return false;
}

void VideoWidget3::setCall(Call* c)
{
   rootObject()->setProperty("call", QVariant::fromValue(c));
}

void VideoWidget3::addRenderer(Video::Renderer* renderer)
{
    Q_UNUSED(renderer)
    rootObject()->setProperty("peerRunning", true);
}

void VideoWidget3::removeRenderer(Video::Renderer* renderer)
{
    Q_UNUSED(renderer)
    rootObject()->setProperty("peerRunning", false);
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
    rootObject()->setProperty("displayPreview", show);
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
    d_ptr->m_pSourceModel = model;
}

void VideoWidget3::setMode(VideoWidget3::Mode mode)
{
    d_ptr->m_Mode = mode;

    Q_ASSERT(rootObject());

    switch(mode) {
        case Mode::CONVERSATION:
            rootObject()->setProperty("mode", "CONVERSATION");
            rootObject()->setProperty("rendererName", "peer");
            break;
        case Mode::PREVIEW:
            rootObject()->setProperty("mode", "PREVIEW");
            rootObject()->setProperty("rendererName", "preview");
            break;
        case Mode::SELFIE:
            rootObject()->setProperty("mode", "SELFIE");
            rootObject()->setProperty("rendererName", "preview");
            break;
    }
}

#include <videowidget.moc>