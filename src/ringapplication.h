/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <elv1313@gmail.com>                   *
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
#ifndef RINGAPPLICATION_H
#define RINGAPPLICATION_H

#include <QtWidgets/QApplication>

//Qt
class QEvent;
class QQmlApplicationEngine;
class QQuickWindow;

//KF5

namespace KDeclarative {
   class KDeclarative;
}

//Ring
class RingQmlWidgets;
class PhotoSelectorPlugin;
class CanvasIndicator;
class DesktopView;
class DialView;
class TimelinePlugin;
class Call;
class PhoneWindowEvent;

///RingApplication: Main application
class RingApplication final : public QApplication
{
  Q_OBJECT

public:
   // Constructor
   RingApplication(int & argc, char ** argv);

   // Destructor
   virtual ~RingApplication();

   // Manage new instances
   Q_INVOKABLE virtual int newInstance();

   // Exit gracefully
   virtual bool notify (QObject* receiver, QEvent* e) override;

   //Getter
   bool startIconified() const;

   QQuickWindow* desktopWindow() const;

   static QQmlApplicationEngine* engine();

   static RingApplication* instance();
   void init();

   /**
    * An unreliable way to track the application focus
    *
    * It is better than nothing
    */
   bool mayHaveFocus();

   //Setter
   void setIconify(bool iconify);

   bool m_HasFocus       {false};
private:
   //Attributes
   bool m_StartIconified {false};
   bool m_StartPhone     {false};
   bool m_StartTimeLine  {false};

   static KDeclarative::KDeclarative* m_pDeclarative;
   static RingQmlWidgets* m_pQmlWidget;
   static PhotoSelectorPlugin* m_pPhotoSelector;
   static CanvasIndicator* m_pCanvasIndicator;
   static RingApplication* m_spInstance;
   static DesktopView* m_pDesktopView;
   static TimelinePlugin* m_pTimeline;
   PhoneWindowEvent* m_pEventFilter {nullptr};

private Q_SLOTS:
   void daemonTimeout();
};

#endif // RINGAPPLICATION_H

// kate: space-indent on; indent-width 3; replace-tabs on;
