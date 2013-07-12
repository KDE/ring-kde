/***************************************************************************
 *   Copyright (C) 2013 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Valle <emmanuel.lepage@savoirfairelinux.com >*
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
#include "callviewoverlay.h"

//Qt
#include <QtCore/QTimer>
#include <QtCore/QEvent>
#include <QtGui/QPainter>
#include <QtGui/QGraphicsBlurEffect>
#include <QtGui/QGraphicsOpacityEffect>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>

//KDE
#include <KPushButton>
#include <KLineEdit>
#include <KLocale>

//SFLPhone 
#include "../sflphoneaccessibility.h"
#include "klib/kcfg_settings.h"

/*****************************************************************************
 *                                                                           *
 *                                 Overlay                                   *
 *                                                                           *
 ****************************************************************************/

///Constructor
CallViewOverlay::CallViewOverlay(QWidget* parent) : QWidget(parent),m_pIcon(0),m_pTimer(0),m_enabled(true),m_black("black"),
m_step(0)
{
   m_black.setAlpha(200);
   if (parent)
      parent->installEventFilter(this);
   if (parent) {
      resize(parent->width(),parent->height());
      move(0,0);
   }

//    m_pBackground = new QWidget(this);
//    m_pBackground->resize(parent->width(),parent->height());
//    QGraphicsOpacityEffect* eff2 = new QGraphicsOpacityEffect(m_pBackground);
//    eff2->setOpacity(0.7);
//    m_pBackground->setGraphicsEffect(eff2);
//    QPalette pal = m_pBackground->palette();
//    pal.setColor(QPalette::Window,Qt::black);
//    m_pBackground->setPalette(pal);
//    m_pBackground->setAutoFillBackground(true);
   
   m_pMainWidget = new QWidget(this);
   m_pMainWidget->resize(parent->width(),parent->height());
   setupUi(m_pMainWidget);
//    QVBoxLayout* mainL = new QVBoxLayout(m_pMainWidget);

   
   static const QString btnStyle = "QPushButton {\
      border-style:solid;\
      color:#dddddd;\
      border-width:1px;\
      border-radius:5;\
      border-color:#dddddd;\
      padding-top:3px;\
      padding-bottom:3px;\
      padding-left:10px;\
      padding-right:10px;\
      margin-left:10px;\
      margin-right:10px;\
   }\
   \
   QPushButton:hover {\
      background-color:white;\
      color:#333333;\
   }";
   
   /*static const QString grbStyle = "QGroupBox::title { \
      font-weight:900;\
      padding-left:5px;\
      padding-top:3px;\
      padding-bottom:7px;\
      color:white;\
      text-align:left;\
      text-decoration:underline;\
   }\
   \
   QGroupBox { \
      margin-bottom:40px;\
      padding-top;75px;\
      min-height:90px;\
   }";
   
   QLabel* lbl = new QLabel("<center><h1>Transfer options</h1></center>");
   lbl->setStyleSheet("color:#dddddd;");
   mainL->addWidget(lbl);
   mainL->addItem(new QSpacerItem(0,25,QSizePolicy::Fixed,QSizePolicy::Fixed));
   
   QGroupBox* grb2 = new QGroupBox(this);
   QVBoxLayout* l2 = new QVBoxLayout(grb2);
   grb2->setStyleSheet(grbStyle);
   grb2->setTitle(i18n("To:"));
   grb2->setMinimumSize(0,90);
   mainL->addWidget(grb2);
   mainL->addItem(new QSpacerItem(0,40,QSizePolicy::Fixed,QSizePolicy::Fixed));
   
   KLineEdit* le = new KLineEdit(grb2);
   le->setPlaceholderText("dsfsdfsdf");
   l2->addWidget(le);
   l2->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding));
   
   KPushButton* btn1 = new KPushButton(m_pMainWidget);
   btn1->setText("1-418-123-4567");
   btn1->setStyleSheet(btnStyle);
   
   KPushButton* btn2 = new KPushButton(m_pMainWidget);
   btn2->setText("Emmanuel Lepage");
   btn2->setStyleSheet(btnStyle);
   btn2->move(20,300);
   
   QGroupBox* grb = new QGroupBox(m_pMainWidget);
   grb->setTitle(i18n("Existing calls:"));
   grb->setStyleSheet(grbStyle);
   mainL->addWidget(grb);
   QVBoxLayout* l = new QVBoxLayout(grb);
   l->addWidget(btn2);
   l->addWidget(btn1);
   l->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding));
   
   mainL->addItem(new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Expanding));*/
}

///Destructor
CallViewOverlay::~CallViewOverlay()
{

}

///Add a widget (usually an icon) in the corner
void CallViewOverlay::setCornerWidget(QWidget* wdg) {
   wdg->setParent      ( this                        );
   wdg->setMinimumSize ( 100         , 100           );
   wdg->resize         ( 100         , 100           );
   wdg->move           ( width()/2-50 , height()-175 );
   m_pIcon = wdg;
}

///Overload the setVisible method to trigger the timer
void CallViewOverlay::setVisible(bool enabled) {
   if (m_enabled != enabled) {
      if (m_pTimer) {
         m_pTimer->stop();
         disconnect(m_pTimer);
         delete m_pTimer;
      }
      m_pTimer = new QTimer(this);
      connect(m_pTimer, SIGNAL(timeout()), this, SLOT(changeVisibility()));
      m_step = 0;
      m_black.setAlpha(0);
      repaint();
      m_pTimer->start(10);
   }
   m_enabled = enabled;
   QWidget::setVisible(enabled);
   if (!m_accessMessage.isEmpty() && enabled == true && ConfigurationSkeleton::enableReadLabel()) {
      SFLPhoneAccessibility::instance()->say(m_accessMessage);
   }
} //setVisible

///How to paint the overlay
void CallViewOverlay::paintEvent(QPaintEvent* event) {
   Q_UNUSED(event)
   QPainter customPainter(this);
   customPainter.fillRect(rect(),m_black);
   customPainter.drawPixmap(rect().width()-125-10,rect().height()-50-10,QPixmap("/home/etudiant/dev/sflphone-kde/src/icons/transferarraw.png"));
}

///Be sure the event is always the right size
void CallViewOverlay::resizeEvent(QResizeEvent *e) {
   Q_UNUSED(e)
   if (m_pIcon) {
      m_pIcon->setMinimumSize(100,100);
      m_pIcon->move(width()/2-50,height()-175);
   }
}

///Step by step animation to fade in/out
void CallViewOverlay::changeVisibility() {
   m_step++;
   m_black.setAlpha(0.1*m_step*m_step);
   repaint();
   if (m_step >= 35)
      m_pTimer->stop();
}

///Set accessibility message
void CallViewOverlay::setAccessMessage(QString message)
{
   m_accessMessage = message;
}

bool CallViewOverlay::eventFilter(QObject *obj, QEvent *event)
{
   if (event->type() == QEvent::Resize && parentWidget()) {
      resize(parentWidget()->width(),parentWidget()->height());
      m_pMainWidget->resize(parentWidget()->width(),parentWidget()->height());
      move(0,0);
   }
   // standard event processing
   return QObject::eventFilter(obj, event);
}
