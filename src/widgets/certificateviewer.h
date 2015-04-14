/****************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                               *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

//Qt
class QTreeView;
class QWidget;

//Ring
class Certificate;

class CertificateViewer : public QDialog
{
   Q_OBJECT
public:
   CertificateViewer(const QModelIndex& idx, QWidget* parent = nullptr);
   CertificateViewer(Certificate* cert     , QWidget* parent = nullptr);
   ~CertificateViewer();

private:
   QTreeView* m_pView;
};

#endif
