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
#include "Contact.h"

Contact::Contact(Addressee addressee, QString number)
{
	this->firstName = addressee.name();
	this->secondName = addressee.familyName();
	this->nickName = addressee.nickName();
	this->phoneNumber = number;
	this->photo = addressee.photo().url();
	
	initItem();
}


Contact::~Contact()
{
}

void Contact::initItem()
{
	this->item = new QListWidgetItem(phoneNumber);
	this->itemWidget = new QWidget();
}


//TODO
QListWidgetItem * Contact::getItem()
{
	return item;
}
    
QWidget * Contact::getItemWidget()
{
	return itemWidget;
}

