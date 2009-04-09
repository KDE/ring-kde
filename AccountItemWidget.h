/***************************************************************************
 *   Copyright (C) 2009 by Savoir-Faire Linux                              *
 *   Author : Jérémy Quentin                                               *
 *   jeremy.quentin@savoirfairelinux.com                                   *
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
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ACCOUNTITEMWIDGET_H
#define ACCOUNTITEMWIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <kled.h>

/**
	@author Jérémy Quentin <jeremy.quentin@gmail.com>
*/
class AccountItemWidget : public QWidget
{
Q_OBJECT

private:

	int state;
	bool enabled;
	KLed * led;
	QCheckBox * checkBox;

public:

	enum State {Registered, Unregistered, NotWorking};

	AccountItemWidget(QWidget *parent = 0);

	~AccountItemWidget();
    
	void setState(int state);
    
	void setEnabled(bool enabled);
	
	void updateStateDisplay();
	
	void updateEnabledDisplay();
	
	void updateDisplay();
	
	int getState();
	
	bool getEnabled();
	

};

#endif
