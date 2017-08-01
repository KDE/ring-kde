/***************************************************************************
 *   Copyright (C) 2012-2015 by Savoir-Faire Linux                         *
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
#include <QtWidgets/QToolButton>
#include <QtCore/QItemSelectionModel>

//Base
#include <QtWidgets/QWidget>
#include <QHash>

//Qt
class QSvgRenderer;
class QToolButton;
class QHBoxLayout;

//KDE

class OverlayToolbar : public QWidget
{
   Q_OBJECT
public:
   explicit OverlayToolbar(QWidget* parent = nullptr);
   virtual ~OverlayToolbar();

   //Setters
   void setForcedParent(QWidget* parent);

   //Mutator
   void resizeToolbar();
   void addWidget(QWidget* w);

private:
   //Attributes
   QSvgRenderer* m_pRightRender ;
   QSvgRenderer* m_pLeftRender  ;
   QWidget*      m_pForcedParent;
   int           m_IconSize     ;
   QHBoxLayout*  m_pLayout      ;

public Q_SLOTS:
   virtual void updateState();

protected:
   //Virtual events
   void paintEvent ( QPaintEvent*  event) override;
   void hideEvent  ( QHideEvent*   event) override;
   void showEvent  ( QShowEvent*   event) override;
   bool eventFilter( QObject *obj, QEvent *event) override;

Q_SIGNALS:
   void visibilityChanged(bool);
   void resized(int width);

};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
