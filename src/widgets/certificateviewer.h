/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/
#ifndef CERTIFICATE_VIEWER_H
#define CERTIFICATE_VIEWER_H

#include <QtWidgets/QDialog>
#include <QtCore/QAbstractItemModel>
#include "typedefs.h"

//Qt
class QTreeView;
class QWidget;

//Ring
class Certificate;

class LIB_EXPORT CertificateViewer : public QDialog
{
   Q_OBJECT
public:
   explicit CertificateViewer(const QModelIndex& idx, QWidget* parent = nullptr);
   explicit CertificateViewer(Certificate* cert     , QWidget* parent = nullptr);
   virtual ~CertificateViewer();

private Q_SLOTS:
   void selectCert(const QModelIndex& idx);

private:
   QTreeView* m_pChainOfTrust;
   QTreeView* m_pView;
};

#endif

// kate: space-indent on; indent-width 3; replace-tabs on;
