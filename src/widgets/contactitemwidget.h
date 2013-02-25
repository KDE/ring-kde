/***************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                         *
 *   Author : Mathieu Leduc-Hamel mathieu.leduc-hamel@savoirfairelinux.com *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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

#ifndef CONTACTITEMWIDGET_H
#define CONTACTITEMWIDGET_H

#include <QtGui/QWidget>
#include "lib/contact.h"

//Qt
class QLabel;
class QTreeWidgetItem;
class QMenu;
class QMimeData;

//KDE
class KAction;

namespace KABC {
   class Addressee;
}

//SFLPhone
class Contact;
class TranslucentButtons;

///ContactItemWidget: Item for the contact tree
class ContactItemWidget : public QWidget
{
   Q_OBJECT
 public:
    //Constructor
    explicit ContactItemWidget(QWidget* parent = nullptr);
    ~ContactItemWidget();

    //Getters
    KABC::Addressee*  contact()         const;
    const QString&    getContactName()  const;
    PhoneNumbers      getCallNumbers()  const;
    const QString&    getOrganization() const;
    const QString&    getEmail()        const;
    QPixmap*          getPicture()      const;
    QTreeWidgetItem*  getItem()         const;
    Contact*          getContact()      const;
    virtual QSize     sizeHint ()       const;

    //Setters
    void setContact ( Contact*         contact );
    void setItem    ( QTreeWidgetItem* item    );

    //Const
    static const char * callStateIcons[12];
    
protected:
   virtual void dragEnterEvent        ( QDragEnterEvent *e );
   virtual void dragMoveEvent         ( QDragMoveEvent  *e );
   virtual void dragLeaveEvent        ( QDragLeaveEvent *e );
   virtual void dropEvent             ( QDropEvent      *e );
   virtual void mouseDoubleClickEvent ( QMouseEvent     *e );

 private:
    //Attributes
    Contact* m_pContactKA    ;
    QLabel*  m_pIconL        ;
    QLabel*  m_pContactNameL ;
    QLabel*  m_pCallNumberL  ;
    QLabel*  m_pOrganizationL;
    QLabel*  m_pEmailL       ;
    QTreeWidgetItem* m_pItem ;
    QSize    m_Size          ;

    //Actions
    KAction* m_pCallAgain   ;
    KAction* m_pEditContact ;
    KAction* m_pCopy        ;
    KAction* m_pEmail       ;
    KAction* m_pAddPhone    ;
    KAction* m_pBookmark    ;
    QMenu*   m_pMenu        ;
    
    TranslucentButtons* m_pBtnTrans;

    //Helper
    QString showNumberSelector(bool& ok);

public Q_SLOTS:
   void updated();

private Q_SLOTS:
   void showContext(const QPoint& pos);
   void sendEmail   ();
   void callAgain   ();
   void copy        ();
   void editContact ();
   void addPhone    ();
   void bookmark    ();
   void transferEvent( QMimeData* data   );
 };

#endif // CONTACTITEMWIDGET_H
