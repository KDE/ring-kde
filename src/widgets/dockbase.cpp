/***************************************************************************
 *   Copyright (C) 2009-2015 by Savoir-Faire Linux                         *
 *   Copyright (C) 2015 by Emmanuel Lepage Vallee                          *
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
#include "dockbase.h"

//Qt
#include <QtCore/QMimeData>
#include <QtCore/QSortFilterProxyModel>
#include <QtGui/QKeyEvent>
#include <QtWidgets/QMenu>

//Ring
#include "mime.h"
#include "menumodelview.h"
#include "useractionmodel.h"
#include "proxies/filtertoplevelproxy.h"
#include "klib/kcfg_settings.h"

class ArrowGrabber : public QObject
{
   Q_OBJECT
public:
   explicit ArrowGrabber(DockBase* parent) : QObject(parent), m_pDock(parent)
   {}

protected:
   bool eventFilter(QObject *obj, QEvent *event) override {
      Q_UNUSED(obj)
      if (event->type() == QEvent::KeyPress) {
         QKeyEvent* e = (QKeyEvent*)event;

         switch(e->key()) {
            // Those need to go into the dock
            case Qt::Key_Up:
            case Qt::Key_Down:
               m_pDock->m_pView->forwardInput(e);
               return true;
            case Qt::Key_Return:
            case Qt::Key_Enter:
               m_pDock->slotDoubleClick(m_pDock->m_pView->selectionModel()->currentIndex());
               m_pDock->m_pView->selectionModel()->clear();
               m_pDock->m_pFilterLE->clear();
               return true;
            default:
               break;
         }
      }

      return false;
   }

private:
   DockBase* m_pDock;
};

///KeyPressEaterC: keygrabber
class KeyPressEaterC : public QObject
{
   Q_OBJECT
public:
   explicit KeyPressEaterC(DockBase* parent) : QObject(parent), m_pDock(parent)
   {}

protected:
   bool eventFilter(QObject *obj, QEvent *event) override {
      Q_UNUSED(obj)

      if (event->type() == QEvent::KeyPress) {
         QKeyEvent* e = (QKeyEvent*)event;

         switch(e->key()) {
            // Those need to go into the dock
            case Qt::Key_Up:
            case Qt::Key_Down:
               break;
            case Qt::Key_Return:
            case Qt::Key_Enter:
               m_pDock->slotDoubleClick(m_pDock->m_pView->selectionModel()->currentIndex());
               m_pDock->m_pView->selectionModel()->clear();
               m_pDock->m_pFilterLE->clear();
               return true;
            default:
               m_pDock->m_pFilterLE->setFocus(Qt::OtherFocusReason);
               m_pDock->m_pFilterLE->forwardInput(e);
               return true;
         }

      }

      return false;
   }

private:
   DockBase* m_pDock;
};

///Constructor
DockBase::DockBase(QWidget* parent) : QDockWidget(parent)
{
   QWidget* mainWidget = new QWidget(this);
   setupUi(mainWidget);

   setWidget(mainWidget);
   m_pMenuBtn  ->setHidden(true);
   m_pKeyPressEater = new KeyPressEaterC( this );

   m_pView->installEventFilter(m_pKeyPressEater    );
   m_pView->setSortingEnabled (true                );
   m_pView->sortByColumn      (0,Qt::AscendingOrder);

   m_pFilterLE->installEventFilter(new ArrowGrabber(this));

   connect(m_pView     ,SIGNAL(contextMenuRequest(QModelIndex)), this , SLOT(slotContextMenu(QModelIndex)));
   connect(m_pFilterLE ,SIGNAL(textChanged(QString))           , this , SLOT(expandTree())                );
   connect(m_pView     ,SIGNAL(doubleClicked(QModelIndex))     , this , SLOT(slotDoubleClick(QModelIndex)));

   expandTree();
} //DockBase

///Destructor
DockBase::~DockBase()
{
   delete m_pKeyPressEater;
}

void DockBase::setProxyModel(QAbstractItemModel* model, QSortFilterProxyModel* filterProxy)
{
   if (!model)
      return;

   auto removeEmpty = new FilterTopLevelProxy(model);

   m_pView->setModel(removeEmpty);
   if (model) {
      if (filterProxy)
         connect(m_pFilterLE ,SIGNAL(filterStringChanged(QString))     , filterProxy, SLOT(setFilterRegExp(QString))   );

      connect   (removeEmpty ,SIGNAL(layoutChanged())                  , this , SLOT(expandTree())               );
      connect   (removeEmpty ,SIGNAL(rowsInserted(QModelIndex,int,int)), this , SLOT(expandTreeRows(QModelIndex)));
   }

   expandTreeRows({});
}

void DockBase::setDelegate(QStyledItemDelegate* delegate)
{
   m_pView->setDelegate(delegate);
}

CategorizedTreeView* DockBase::view() const
{
   return m_pView;
}

void DockBase::setSortingModel(QAbstractItemModel* m, QItemSelectionModel* s)
{
   m_pMenuBtn->setHidden(false);
   m_pMenuBtn->setModel(m, s);
}

void DockBase::initUAM()
{
   if (!m_pUserActionModel) {

      m_pUserActionModel = new UserActionModel(m_pView->model(), UserActionModel::Context::ALL );
      m_pUserActionModel->setSelectionModel(m_pView->selectionModel());
   }
}

///Called when someone right click on the 'index'
void DockBase::slotContextMenu(const QModelIndex& index)
{
   if (!index.parent().isValid())
      return;

   initUAM();

   auto m = new MenuModelView(m_pUserActionModel->activeActionModel(), new QItemSelectionModel(m_pUserActionModel->activeActionModel()), this);
   connect(m, &MenuModelView::itemClicked, this, &DockBase::slotContextMenuClicked);

   m->exec(QCursor::pos());
}

void DockBase::slotContextMenuClicked(const QModelIndex& index )
{
   m_pUserActionModel->execute(index);
}

void DockBase::slotDoubleClick(const QModelIndex& index)
{
   Q_UNUSED(index)
   initUAM();

   m_pUserActionModel << UserActionModel::Action::CALL_CONTACT;
}

///Called when a call is dropped on transfer
void DockBase::transferEvent(QMimeData* data)
{
   Q_UNUSED(data)
   //TODO add an execute(QModelIndex) variant to the UAM, use it for transfer
//    if (data->hasFormat( RingMimes::CALLID)) {
//       bool ok = false;
// 
//       if (!m_pMenu)
//          m_pMenu = new Menu::Person(this);
// 
//       const ::ContactMethod* result = m_pMenu->showNumberSelector(ok);
// 
//       if (ok && result) {
//          Call* call = CallModel::instance().fromMime(data->data(RingMimes::CALLID));
//          if (dynamic_cast<Call*>(call)) {
//             CallModel::instance().transfer(call, result);
//          }
//       }
//    }
//    else
//       qDebug() << "Invalid mime data";
}

///Expand the tree according to the user preferences
void DockBase::expandTree()
{
   m_pView->expandToDepth( 2 );
}

void DockBase::expandTreeRows(const QModelIndex& idx)
{
   if (!idx.isValid()) //Only top level
      m_pView->expandToDepth( 2 );
}

#include <dockbase.moc>
