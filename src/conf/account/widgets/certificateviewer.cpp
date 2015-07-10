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
#include "certificateviewer.h"

//Qt
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHeaderView>

//Ring
#include <certificate.h>
#include <certificatemodel.h>
#include <chainoftrustmodel.h>

CertificateViewer::CertificateViewer(const QModelIndex& idx, QWidget* parent) : QDialog(parent),
m_pChainOfTrust(new QTreeView(this)),m_pView(new QTreeView(this))
{
   setModal(true);

   QVBoxLayout* l = new QVBoxLayout(this);
   m_pChainOfTrust->setMaximumSize(999999,150);
   l->addWidget(m_pChainOfTrust);
   l->addWidget(m_pView);
   m_pView->setWordWrap(true);
   m_pView->setUniformRowHeights(false);
   m_pView->setModel(CertificateModel::instance()->singleCertificateModel(idx));

   if (m_pView->header()) {
      m_pView->header()->setSectionResizeMode (0,QHeaderView::ResizeToContents);
      m_pView->header()->setSectionResizeMode (1,QHeaderView::Stretch         );
   }
   m_pView->expandAll();
   resize(600,800);

   connect(m_pChainOfTrust, &QTreeView::clicked, this, &CertificateViewer::selectCert );
}

CertificateViewer::CertificateViewer(Certificate* cert, QWidget* parent) : QDialog(parent),m_pChainOfTrust(new QTreeView(this)), m_pView(new QTreeView(this))
{
   if (!cert)
      return;
   QVBoxLayout* l = new QVBoxLayout(this);
   m_pChainOfTrust->setMaximumSize(999999,150);
   m_pView->setWordWrap(true);
   m_pView->setUniformRowHeights(false);
   l->addWidget(m_pChainOfTrust);
   l->addWidget(m_pView);
//    l->setStretchFactor(m_pChainOfTrust,0.3);
   m_pView->setModel(cert->model());

   m_pChainOfTrust->setModel(cert->chainOfTrustModel());
   m_pChainOfTrust->expandAll();

   if (m_pView->header()) {
      m_pView->header()->setSectionResizeMode (0,QHeaderView::ResizeToContents);
      m_pView->header()->setSectionResizeMode (1,QHeaderView::Stretch         );
   }

   m_pView->expandAll();
   resize(600,800);

   connect(m_pChainOfTrust, &QTreeView::clicked, this, &CertificateViewer::selectCert );
}

CertificateViewer::~CertificateViewer()
{

}

void CertificateViewer::selectCert(const QModelIndex& idx)
{
   Certificate* c = qvariant_cast<Certificate*>(idx.data((int)ChainOfTrustModel::Role::OBJECT));

   if (!c)
      return;

   m_pView->setModel(c->model());
   m_pView->expandAll();
}
