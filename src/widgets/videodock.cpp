/***************************************************************************
 *   Copyright (C) 2011-2015 by Savoir-Faire Linux                         *
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
#include "videodock.h"

#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidgetItem>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QDesktopWidget>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QLabel>

#include <klocalizedstring.h>
#include <QComboBox>
#include <QtWidgets/QToolButton>

#include "callview/videowidget.h"
#include "actioncollection.h"
#include "video/devicemodel.h"
#include "video/sourcemodel.h"
#include "media/media.h"
#include "call.h"
#include "media/video.h"
#include "ui_mediafilepicker.h"
#include "ui_screensharingwidget.h"
#include "videosettings.h"

class VideoWidgetItem : public QWidgetItem {
public:
   VideoWidgetItem(VideoWidget3* m_pMainWidget) : QWidgetItem(m_pMainWidget),m_pWdg(m_pMainWidget){}
   virtual ~VideoWidgetItem(){}
   virtual bool hasHeightForWidth () const override {
      return true;
   }
   virtual int heightForWidth ( int w ) const override {
      return m_pWdg->heightForWidth(w);
   }
private:
   VideoWidget3* m_pWdg;
};

class MediaPicker : public QWidget, public Ui_MediaPicker
{
   Q_OBJECT
public:
   MediaPicker(QWidget* parent) : QWidget(parent)
   {
      setupUi(this);
   }
};

class ScreenSharingWidget : public QWidget, public Ui_ScreenSharing
{
   Q_OBJECT
public:
   ScreenSharingWidget(QWidget* parent) : QWidget(parent)
   {
      setupUi(this);
      setupScreenCombo();
      m_pFrame->setVisible(false);
      slotScreenIndexChanged(0);
   }
private:
   //Helper
   void setupScreenCombo();
private Q_SLOTS:
   void slotScreenIndexChanged(int idx);
};

class FullscreenEventFilter : public QObject {
   Q_OBJECT
public:
   explicit FullscreenEventFilter(VideoDock* parent) : QObject(parent),m_pParent(parent){}
protected:
   virtual bool eventFilter(QObject *obj, QEvent *event) override {
      Q_UNUSED(obj);
      if (event->type() == QEvent::KeyPress) {
         QKeyEvent* e = static_cast<QKeyEvent*>(event);
         if (e->key() == Qt::Key_Escape) {
            ActionCollection::instance()->videoFullscreenAction()->setChecked(false);
            m_pParent->slotFullscreen(false);
            return true;
         }
      }
      return false;
   }
   VideoDock* m_pParent;
};

void ScreenSharingWidget::setupScreenCombo()
{
   for (int i =0; i < QApplication::desktop()->screenCount();i++) {
      m_pScreens->addItem(QString::number(i));
   }
   m_pScreens->addItem(i18n("Custom"));
}


void ScreenSharingWidget::slotScreenIndexChanged(int idx)
{
   if (idx == m_pScreens->count()-1) {
      m_pFrame->setVisible(true);
   }
//FIXME
//    else if (m_pSourceModel) {
//       m_pSourceModel->setDisplay(0,QApplication::desktop()->screenGeometry(idx));
//       m_pFrame->setVisible(false);
//    }
}


///Constructor
VideoDock::VideoDock(QWidget* parent) : QDockWidget(parent),m_pVideoSettings(nullptr),
   m_pScreenSharing(nullptr), m_pMediaPicker(nullptr), m_pWindow(nullptr)
{
   setWindowTitle(i18nc("Video conversation","Video"));
   setObjectName(QStringLiteral("Video Dock"));
   m_pMainWidget = new QWidget(this);

   m_pVideoWidet = new VideoWidget3(m_pMainWidget);
   m_pVideoWidet->setMode(VideoWidget3::Mode::CONVERSATION);
   m_pVideoWidet->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
   auto l = new QGridLayout(m_pMainWidget);
   l->setContentsMargins(0,0,0,0);
//    l->addItem(new VideoWidgetItem(m_pVideoWidet),1,0);
   l->addWidget(m_pVideoWidet,1,0,1,3);
//    l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),0,0);
//    l->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding),2,0);
   setWidget(m_pMainWidget);
   setMinimumSize(320,240);

   QToolButton* btn = new QToolButton(this);
   btn->setIcon(QIcon::fromTheme(QStringLiteral("arrow-down-double")));
   btn->setCheckable(true);
   l->addWidget(btn,2,2);

   QWidget* moreOptions = new QWidget(this);
   moreOptions->setVisible(false);
   moreOptions->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
   l->addWidget(moreOptions,3,0,1,3);

   m_pMoreOpts = new QGridLayout(moreOptions);
   QLabel* devL = new QLabel(i18n("Device:"));
   m_pMoreOpts->addWidget(devL,1,0,1,1);

   m_pDevice = new QComboBox(this);
   m_pMoreOpts->addWidget(m_pDevice,1,1,2,1);
   connect(btn,&QAbstractButton::toggled,moreOptions,&QWidget::setVisible);
   connect(m_pDevice,SIGNAL(currentIndexChanged(int)),this,SLOT(slotDeviceChanged(int)));

   connect(ActionCollection::instance()->videoRotateLeftAction() ,&QAction::triggered,m_pVideoWidet,&VideoWidget3::slotRotateLeft);
   connect(ActionCollection::instance()->videoRotateRightAction(),&QAction::triggered,m_pVideoWidet,&VideoWidget3::slotRotateRight);
   connect(ActionCollection::instance()->videoPreviewAction()    ,&QAction::triggered,m_pVideoWidet,&VideoWidget3::slotShowPreview);
   connect(ActionCollection::instance()->videoMuteAction()       ,&QAction::triggered,m_pVideoWidet,&VideoWidget3::slotMuteOutgoindVideo);
   connect(ActionCollection::instance()->videoScaleAction()      ,&QAction::triggered,m_pVideoWidet,&VideoWidget3::slotKeepAspectRatio);
   connect(ActionCollection::instance()->videoFullscreenAction() ,&QAction::triggered,this         ,&VideoDock::slotFullscreen);
}


VideoDock::~VideoDock()
{
   if (m_pWindow)
      delete m_pWindow;
}

void VideoDock::setCall(Call* c)
{
   m_pVideoWidet->setCall(c);

   if (!c)
      return;

   if (auto videoOut = c->firstMedia<Media::Video>(Media::Media::Direction::OUT)) {
      setSourceModel(videoOut->sourceModel());
   }
}

///Set current renderer
void VideoDock::addRenderer(Video::Renderer* r)
{
   Q_UNUSED(r)
   m_pVideoWidet->addRenderer(r);
}


void VideoDock::setSourceModel(Video::SourceModel* model)
{
   if (model) {
      m_pVideoWidet->setSourceModel(model);

      m_pDevice->blockSignals(true);
      m_pDevice->setModel(model);
      m_pDevice->setCurrentIndex(model->activeIndex());
      m_pDevice->blockSignals(false);

      if (!m_pSourceModel)
         connect(m_pDevice,SIGNAL(currentIndexChanged(int)),m_pSourceModel,SLOT(switchTo(int)));
   }
   else if (m_pSourceModel) {
      disconnect(m_pDevice,SIGNAL(currentIndexChanged(int)),m_pSourceModel,SLOT(switchTo(int)));
   }
   m_pSourceModel = model;
}

void VideoDock::slotDeviceChanged(int index)
{
   switch (index) {
      case Video::SourceModel::ExtendedDeviceList::NONE:
         if ( m_pVideoSettings )
            m_pVideoSettings->setVisible(false);
         if ( m_pScreenSharing         )
            m_pScreenSharing->setVisible(false);
         if ( m_pMediaPicker   )
            m_pMediaPicker->setVisible(false);
         break;
      case Video::SourceModel::ExtendedDeviceList::SCREEN:
         if ( m_pVideoSettings  )
            m_pVideoSettings->setVisible(false);
         if ( !m_pScreenSharing ) {
            m_pScreenSharing = new ScreenSharingWidget(this);
            m_pMoreOpts->addWidget(m_pScreenSharing,10,0,1,4);
         }
         if ( m_pMediaPicker    )
            m_pMediaPicker->setVisible(false);
         m_pScreenSharing->setVisible(true);
         break;
      case Video::SourceModel::ExtendedDeviceList::FILE:
         if ( m_pVideoSettings )
            m_pVideoSettings->setVisible(false);
         if ( m_pScreenSharing )
            m_pScreenSharing->setVisible(false);
         if ( !m_pMediaPicker  ) {
            m_pMediaPicker = new MediaPicker(this);
            m_pMoreOpts->addWidget(m_pMediaPicker,11,0,1,4);
            connect(m_pMediaPicker->m_pPicker,&KUrlRequester::urlSelected,
               this,&VideoDock::slotFileSelected);
         }
         m_pMediaPicker->setVisible(true);
         break;
      default:
         if ( !m_pVideoSettings ) {
            m_pVideoSettings = new VideoSettings(this);
            m_pVideoSettings->hideDevices();
            m_pMoreOpts->addWidget(m_pVideoSettings,12,0,1,4);
         }
         if ( m_pScreenSharing  )
            m_pScreenSharing->setVisible(false);
         if ( m_pMediaPicker    )
            m_pMediaPicker->setVisible(false);

         if (m_pSourceModel)
            m_pVideoSettings->setDevice(m_pSourceModel->deviceAt(
               m_pSourceModel->index(index,0))
            );

         m_pVideoSettings->setVisible(true);
   };
}

void VideoDock::slotFileSelected(const QUrl& url)
{
   if (!m_pSourceModel)
       return;

   m_pSourceModel->setFile(url);
}

///Make the video widget temporarely fullscreen
void VideoDock::slotFullscreen(bool isFullScreen)
{
   if (isFullScreen) {
      if (!m_pWindow) {
         m_pWindow = new QWidget(nullptr);
         m_pWindow->setWindowTitle(i18n("Full screen video"));
         m_pWindow->installEventFilter(new FullscreenEventFilter(this));
      }
      m_pMainWidget->setParent(m_pWindow);
      m_pMainWidget->resize(m_pWindow->width()-20,m_pWindow->height()-20);
      QHBoxLayout* l = new QHBoxLayout(m_pWindow);
      m_pWindow->setLayout(l);
      l->setContentsMargins(0,0,0,0);
      l->setSpacing(0);
      l->addWidget(m_pMainWidget);
      l->invalidate();
      //It has to be done twice, X11 issue
      m_pWindow->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
      m_pWindow->showFullScreen ();
      m_pMainWidget->layout()->invalidate();
      l->invalidate();
   }
   else if(m_pWindow) {
      m_pMainWidget->setParent(this);
      m_pMainWidget->resize(100,100);
      m_pMainWidget->adjustSize ();
      setWidget(m_pMainWidget);
      m_pMainWidget->layout()->invalidate();
      m_pWindow->hide();
   }
}

#include "moc_videodock.cpp"
#include "videodock.moc"
// kate: space-indent on; indent-width 3; replace-tabs on;
