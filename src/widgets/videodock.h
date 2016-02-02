/***************************************************************************
 *   Copyright (C) 2011-2015 by Savoir-Faire Linux                         *
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
#ifndef VIDEO_DOCK_H
#define VIDEO_DOCK_H

#include <QtWidgets/QDockWidget>

//Qt
class QGridLayout;
class QComboBox;

//KDE
class QUrl;

//Ring
class VideoWidget;
class VideoSettings;
class MediaPicker;
class ScreenSharingWidget;
namespace Video {
   class Renderer;
   class SourceModel;
}

///VideoDock: A dock hosting a VideoWidget or AcceleratedVideoWidget
class VideoDock : public QDockWidget {
   Q_OBJECT
public:
   explicit VideoDock(QWidget* parent = nullptr );
   ~VideoDock();
   void addRenderer(Video::Renderer* r);

private:
   QGridLayout*         m_pMoreOpts     ;
   VideoWidget*         m_pVideoWidet   ;
   VideoSettings*       m_pVideoSettings;
   ScreenSharingWidget* m_pScreenSharing;
   MediaPicker*         m_pMediaPicker  ;
   QWidget*             m_pMainWidget   ;
   QWidget*             m_pWindow       ;
   QComboBox*           m_pDevice       ;
   Video::SourceModel*  m_pSourceModel {nullptr};

private Q_SLOTS:
   void slotDeviceChanged(int index);
   void slotFileSelected(const QUrl& url);
public Q_SLOTS:
   void slotFullscreen  (bool);
   void setSourceModel  (Video::SourceModel* model);
};

#endif
