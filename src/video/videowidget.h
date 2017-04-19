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
#pragma once

#include <QQuickWidget>

namespace Video {
    class Renderer;
    class SourceModel;
}

class Call;

class VideoWidget3Private;

class VideoWidget3 : public QQuickWidget
{
    Q_OBJECT
public:
    enum class Mode {
        CONVERSATION,
        PREVIEW,
        SELFIE,
    };
    Q_ENUMS(Mode)

    explicit VideoWidget3(QWidget* parent = nullptr);
    VideoWidget3(Mode mode, QWidget* parent = nullptr);
    virtual ~VideoWidget3();

    void setMode(Mode mode);
    void setCall(Call* c);

    static void initProvider();
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

private:
    VideoWidget3Private* d_ptr;
    Q_DECLARE_PRIVATE(VideoWidget3)
};

Q_DECLARE_METATYPE(VideoWidget3*)

