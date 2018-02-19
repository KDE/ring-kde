/***************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                         *
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
#include "basic.h"

#include <QtCore/QPointer>

#include <account.h>
#include <protocolmodel.h>
#include <profilemodel.h>
#include <bootstrapmodel.h>
#include <credentialmodel.h>

#include <widgets/accountstatusviewer.h>

Pages::Basic::Basic(QWidget *parent) : PageBase(parent)
{
   setupUi(this);
   connect(this,&PageBase::accountSet,[this]() {
      disconnect(m_CredConn );

      m_pProtocol->bindToModel(account()->protocolModel(),account()->protocolModel()->selectionModel());

      m_pProfile->bindToModel(
         &ProfileModel::instance(),
         ProfileModel::instance().getAccountSelectionModel(account())
      );

      m_pBootstrapModel->setModel((account()->protocol() == Account::Protocol::RING)?
         account()->bootstrapModel() : nullptr
      );

      if (m_pBootstrapModel->horizontalHeader() && m_pBootstrapModel->model())
         m_pBootstrapModel->horizontalHeader()->setSectionResizeMode (0,QHeaderView::Stretch);

      m_pBootstrapModel->setVisible(account()->roleData((int)Account::Role::HasCustomBootstrap).toBool());

//       disconnect(this, &Pages::Basic::updateStatus); //TODO track previous account
      connect(account(), &Account::stateChanged, this, &Pages::Basic::updateStatus);

/*      m_CredConn = connect(account()->credentialModel(), &CredentialModel::primaryCredentialChanged,[this](Credential::Type t, Credential* c) {
         if (t == Credential::Type::SIP) {
            lrcfg_password->setText(c->password());
         }
      });*/

      updateStatus();
   });

   connect(m_pDisplayLog, &QToolButton::clicked, this, [this]() {
      if (account()) {
         QPointer<AccountStatusViewer> d = new AccountStatusViewer(account(),this);
         d->show();
      }
   });
}

void Pages::Basic::updateStatus()
{
   const int errorCode = account()->lastErrorCode();
   QString message = account()->toHumanStateName();

   if (account()->registrationState() == Account::RegistrationState::ERROR && !account()->lastErrorMessage().isEmpty())
      message = account()->lastErrorMessage();

   edit7_state->setText(QStringLiteral("<span style='color:%3'> %1 </span>(%2)")
      .arg(message                          )
      .arg( errorCode==-1 ? 200 : errorCode )
      .arg(account()->stateColorName()      )
   );
}

// kate: space-indent on; indent-width 3; replace-tabs on;
