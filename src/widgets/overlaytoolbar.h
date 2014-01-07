/***************************************************************************
 *   Copyright (C) 2012-2014 by Savoir-Faire Linux                         *
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
#ifndef OVERLAYTOOLBAR_H
#define OVERLAYTOOLBAR_H

//Qt
#include <QtGui/QToolButton>
#include <QtGui/QItemSelectionModel>

//Base
#include <QtGui/QWidget>
#include <QtCore/QHash>
#include "lib/sflphone_const.h"

//Qt
class QSvgRenderer;
class QToolButton;
class QHBoxLayout;

//KDE

//SFLPhone
class ExtendedAction;
class ObserverToolButton;

class ObserverToolButton : public QToolButton
{
   Q_OBJECT
public:
   explicit ObserverToolButton(QWidget* parent = nullptr);
public Q_SLOTS:
   void setNewText(const QString& text) {
      setText(text);
   }
};

class OverlayToolbar : public QWidget
{
   Q_OBJECT
public:
   explicit OverlayToolbar(QWidget* parent = nullptr);
   virtual ~OverlayToolbar();

   //Setters
   void setForcedParent(QWidget* parent);
   void setIconSize(int size);

   //Getter
   ObserverToolButton* actionButton(int key);

   //Mutator
   void resizeToolbar();
   void addAction(ExtendedAction* action, int key = -1);

private:
   //Attributes
   QSvgRenderer* m_pRightRender ;
   QSvgRenderer* m_pLeftRender  ;
   QWidget*      m_pForcedParent;
   int           m_IconSize     ;
   QHBoxLayout*  m_pLayout      ;
   QHash<int,ObserverToolButton*> m_hButtons;


public Q_SLOTS:
   virtual void updateState();

protected:
   //Virtual events
   void paintEvent ( QPaintEvent*  event);
   void hideEvent  ( QHideEvent*   event);
   void showEvent  ( QShowEvent*   event);
   bool eventFilter( QObject *obj, QEvent *event);

   //Helpers
   ObserverToolButton* createButton(ExtendedAction* action);

Q_SIGNALS:
   void visibilityChanged(bool);

};

#endif
