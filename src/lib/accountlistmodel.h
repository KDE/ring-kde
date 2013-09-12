/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>          *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com> *
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

#ifndef ACCOUNT_LIST_H
#define ACCOUNT_LIST_H


#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QAbstractListModel>

#include "account.h"
#include "typedefs.h"
// #include "dbus/metatypes.h"

class AccountListColorVisitor;

///AccountList: List of all daemon accounts
class LIB_EXPORT AccountListModel : public QAbstractListModel {
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
   Q_OBJECT
   #pragma GCC diagnostic pop

public:
   friend class Account;
   //Static getter and destructor
   static AccountListModel* instance();
   static void destroy();

   //Getters
   const QVector<Account*>& getAccounts            (                         );
   QVector<Account*>        getAccountsByState     ( const QString& state    );
   Q_INVOKABLE Account*     getAccountById         ( const QString& id       ) const;
   Q_INVOKABLE Account*     getAccountAt           ( int i                   ) const;
   int                      size                   (                         ) const;
   Account*                 firstRegisteredAccount (                         ) const;
   Account*                 getDefaultAccount      (                         ) const;
   static Account*          currentAccount         (                         );
   Account*                 getAccountByModelIndex ( const QModelIndex& item ) const;
   static QString           getSimilarAliasIndex   ( const QString& alias    );
   AccountListColorVisitor* colorVisitor           (                         );

   //Abstract model accessors
   QVariant      data     ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
   int           rowCount ( const QModelIndex& parent = QModelIndex()            ) const;
   Qt::ItemFlags flags    ( const QModelIndex& index                             ) const;

   //Setters
   void setPriorAccount          ( const Account*                                           );
   virtual bool setData          ( const QModelIndex& index, const QVariant &value, int role);
   void         setColorVisitor  ( AccountListColorVisitor* visitor                         );
   void         setDefaultAccount(Account* a);

   //Mutators
   Q_INVOKABLE Account* addAccount          ( const QString & alias   )      ;
   Q_INVOKABLE void     removeAccount       ( Account* account        )      ;
   void                 removeAccount       ( QModelIndex index       )      ;
   QVector<Account*>    registeredAccounts  (                         ) const;
   void                 save                (                         )      ;
   Q_INVOKABLE bool     accountUp           ( int index               )      ;
   Q_INVOKABLE bool     accountDown         ( int index               )      ;
   Q_INVOKABLE void     cancel              (                         )      ;

   //Operators
   Account*       operator[] (int            i)      ;
   Account*       operator[] (const QString& i)      ;
   const Account* operator[] (int            i) const;

private:
   //Constructors & Destructors
   explicit AccountListModel();
   void init();
   ~AccountListModel();
   void setupRoleName();

   //Attributes
   QVector<Account*>        m_lAccounts       ;
   static AccountListModel* m_spAccountList   ;
   static Account*          m_spPriorAccount  ;
   Account*                 m_pDefaultAccount ;
   AccountListColorVisitor* m_pColorVisitor   ;
   QStringList              m_lDeletedAccounts;

public Q_SLOTS:
   void update        ();
   void updateAccounts();
   void registerAllAccounts();

private Q_SLOTS:
   void accountChanged(const QString& account,const QString& state, int code);
   void accountChanged(Account* a);

Q_SIGNALS:
   ///The account list changed
   void accountListUpdated();
   ///Emitted when an account state change
   void accountStateChanged  ( Account* account, QString state);
   ///Emitted when an account enable attribute change
   void accountEnabledChanged( Account* source                );
   ///Emitted when the default account change
   void defaultAccountChanged( Account* a                     );
   ///Emitted when the default account change
   void priorAccountChanged  ( Account* a                     );
};
Q_DECLARE_METATYPE(AccountListModel*)

class LIB_EXPORT AccountListNoCheckProxyModel : public QAbstractListModel
{
public:
   virtual QVariant data(const QModelIndex& index,int role = Qt::DisplayRole ) const;
   virtual bool setData( const QModelIndex& index, const QVariant &value, int role);
   virtual Qt::ItemFlags flags (const QModelIndex& index) const;
   virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;
};

#endif
