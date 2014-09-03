/***************************************************************************
 *   Copyright (C) 2009-2014 by Savoir-Faire Linux                         *
 *   Author : Jérémy Quentin <jeremy.quentin@savoirfairelinux.com>         *
 *            Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
 *              ^^ Refuse any liability for this code                      *
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
#include "accountwizard.h"

//Unix
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//Qt
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>

//KDE
#include <KLocale>
#include <KDebug>
#include <KLineEdit>

//SFLphone
#include "lib/sflphone_const.h"
#include "lib/account.h"
#include "klib/kcfg_settings.h"
#include "lib/dbus/configurationmanager.h"

//Define
#define FIELD_SFL_ACCOUNT        "SFL"
#define FIELD_OTHER_ACCOUNT      "OTHER"
#define FIELD_SIP_ACCOUNT        "SIP"
#define FIELD_IAX_ACCOUNT        "IAX"
#define FIELD_EMAIL_ADDRESS      "EMAIL_ADDRESS"

#define FIELD_SIP_ALIAS          "SIP_ALIAS"
#define FIELD_SIP_SERVER         "SIP_SERVER"
#define FIELD_SIP_USER           "SIP_USER"
#define FIELD_SIP_PASSWORD       "SIP_PASSWORD"
#define FIELD_SIP_VOICEMAIL      "SIP_VOICEMAIL"
#define FIELD_SIP_ENABLE_STUN    "SIP_ENABLE_STUN"
#define FIELD_SIP_STUN_SERVER    "SIP_STUN_SERVER"

#define FIELD_ZRTP_ENABLED       "ZRTP_ENABLED"

#define FIELD_IAX_ALIAS          "IAX_ALIAS"
#define FIELD_IAX_SERVER         "IAX_SERVER"
#define FIELD_IAX_USER           "IAX_USER"
#define FIELD_IAX_PASSWORD       "IAX_PASSWORD"
#define FIELD_IAX_VOICEMAIL      "IAX_VOICEMAIL"


#define SFL_ACCOUNT_HOST         "sip.sflphone.org"

static const QString REGISTRATION_ENABLED_TRUE("true");
static const QString REGISTRATION_ENABLED_FALSE("false");

/***************************************************************************
 *                                                                         *
 *   Global functions for creating an account on sflphone.org              *
 *                                                                         *
 **************************************************************************/



///Validate if the connection can be done with the PBX
int AccountWizard::sendRequest(const QString& host, int port, const QString& req, QString& ret)
{
   int s;
   struct sockaddr_in servSockAddr;
   struct hostent *servHostEnt;
   long int length = 0;
   long int status = 0;
   int i           = 0;
   FILE *f;
   char buf[1024];

   bzero(&servSockAddr, sizeof(servSockAddr));
   servHostEnt = gethostbyname(host.toLatin1());
   if (servHostEnt == nullptr) {
      ret = "gethostbyname";
      return -1;
   }
   bcopy((char *)servHostEnt->h_addr, (char *)&servSockAddr.sin_addr, servHostEnt->h_length);
   servSockAddr.sin_port = htons(port);
   servSockAddr.sin_family = AF_INET;

   if ((s = socket(AF_INET,SOCK_STREAM,0)) < 0) {
      ret = "socket";
      return -1;
   }

   if(::connect(s, (const struct sockaddr *) &servSockAddr, (socklen_t) sizeof(servSockAddr)) < 0 ) {
      perror(nullptr);
      ret = "connect";
      ::close(s);
      return -1;
   }

   f = fdopen(s, "r+");

   const char * req2 = req.toLatin1();
   const char * host2 = host.toLatin1();
   fprintf(f, "%s HTTP/1.1\r\n", req2);
   fprintf(f, "Host: %s\r\n", host2);
   fputs("User-Agent: SFLphone\r\n", f);
   fputs("\r\n", f);

   while (strncmp(fgets(buf, sizeof(buf), f), "\r\n", 2)) {
      const char *len_h = "content-length";
      const char *status_h = "HTTP/1.1";
      if (strncasecmp(buf, len_h, strlen(len_h)) == 0)
         length = atoi(buf + strlen(len_h) + 1);
      if (strncasecmp(buf, status_h, strlen(status_h)) == 0)
         status = atoi(buf + strlen(status_h) + 1);
   }
   for (i = -1; i < length; ++i)
      ret[i] = fgetc(f);

   if (status != 200) {
      ret = "http error:" + QString::number(status);
      fclose(f);
//       sprintf(ret, "http error: %ld", status);
      return -1;
   }

   fclose(f);
   shutdown(s, 2);
   ::close(s);
   return 0;
}

///
rest_account AccountWizard::get_rest_account(const QString& host, const QString& email)
{
   const QString req = "GET /rest/accountcreator?email=" + email;
   QString ret;
   rest_account ra;
   kDebug() << "HOST: " << host;
   int res = sendRequest(host, 80, req, ret);
   if (res != -1) {
      QStringList list = ret.split('\n');
      ra.user = list[0];
      ra.passwd = list[1];\
      ra.success = true;
   } else {
      ra.success = false;
      ra.reason = ret;
   }
   kDebug() << ret;
   return ra;
}

/***************************************************************************
 *   Class AccountWizard                                                   *
 *   Widget of the wizard for creating an account.                         *
 **************************************************************************/

///Constructor
AccountWizard::AccountWizard(QWidget * parent)
 : QWizard(parent)
{
   setModal(true);
   setPage(Page_Intro      , new WizardIntroPage               );
   setPage(Page_Type       , new WizardAccountTypePage         );
   setPage(Page_SIPForm    , new WizardAccountSIPFormPage      );
   setPage(Page_IAXForm    , new WizardAccountIAXFormPage      );
   setPage(Page_Stun       , new WizardAccountStunPage         );
   setPage(Page_Conclusion , new WizardAccountConclusionPage   );

   setStartId(Page_Intro);
   setWindowTitle(i18n("Account creation wizard"));
   setWindowIcon(QIcon(ICON_SFLPHONE));
   setMinimumSize(500,350);
   setMaximumSize(50000,45000);
   resize(500,350);
   setPixmap(QWizard::WatermarkPixmap, QPixmap(ICON_SFLPHONE));
}

///Destructor
AccountWizard::~AccountWizard()
{
}

///The accept button have been pressed
void AccountWizard::accept()
{
   ConfigurationManagerInterface & configurationManager = DBus::ConfigurationManager::instance();

   QString ret;
   MapStringString accountDetails = configurationManager.getAccountTemplate();

   QString& alias    = accountDetails[ QString( Account::MapField::ALIAS    ) ];
   QString& enabled  = accountDetails[ QString( Account::MapField::ENABLED  ) ];
   QString& mailbox  = accountDetails[ QString( Account::MapField::MAILBOX  ) ];
   QString& protocol = accountDetails[ QString( Account::MapField::TYPE     ) ];
   QString& server   = accountDetails[ QString( Account::MapField::HOSTNAME ) ];
   QString& user     = accountDetails[ QString( Account::MapField::USERNAME ) ];
   QString& password = accountDetails[ QString( Account::MapField::PASSWORD ) ];

   // sip only parameters
   QString& stun_enabled = accountDetails[ QString(Account::MapField::STUN::ENABLED) ];
   QString& stun_server  = accountDetails[ QString(Account::MapField::STUN::SERVER)  ];

   // zrtp only parameters
   QString& srtp_enabled          = accountDetails[ QString( Account::MapField::SRTP::ENABLED          ) ];
   QString& key_exchange          = accountDetails[ QString( Account::MapField::SRTP::KEY_EXCHANGE          ) ];
   QString& zrtp_display_sas      = accountDetails[ QString( Account::MapField::ZRTP::DISPLAY_SAS      ) ];
   QString& zrtp_not_supp_warning = accountDetails[ QString( Account::MapField::ZRTP::NOT_SUPP_WARNING ) ];
   QString& zrtp_hello_hash       = accountDetails[ QString( Account::MapField::ZRTP::HELLO_HASH       ) ];
   QString& display_sas_once      = accountDetails[ QString( Account::MapField::ZRTP::DISPLAY_SAS_ONCE      ) ];

   //  interface parameters
   QString& locale_interface  = accountDetails[ QString(Account::MapField::LOCAL_INTERFACE)   ];
   QString& published_address = accountDetails[ QString(Account::MapField::PUBLISHED_ADDRESS) ];

   bool is_using_sflphone_org = field(FIELD_SFL_ACCOUNT).toBool();
   bool is_using_sip          = false;
   bool is_create_account     = false;

   // sflphone.org
   if(is_using_sflphone_org) {
      QString emailAddress = field(FIELD_EMAIL_ADDRESS).toString();
      char charEmailAddress[1024];
      strncpy(charEmailAddress, emailAddress.toLatin1(), sizeof(charEmailAddress) - 1);
      rest_account acc = get_rest_account(SFL_ACCOUNT_HOST, charEmailAddress);

      if(acc.success) {
         ret += i18n("This assistant is now finished.") + '\n';
         field( FIELD_SIP_ALIAS     ) = QString( acc.user) + '@' + SFL_ACCOUNT_HOST;
         field( FIELD_SIP_SERVER    ) = QString( SFL_ACCOUNT_HOST                 );
         field( FIELD_SIP_PASSWORD  ) = QString( acc.passwd                       );
         field( FIELD_SIP_USER      ) = QString( acc.user                         );
         field( FIELD_SIP_VOICEMAIL ).clear();

         protocol = QString( Account::ProtocolName::SIP    );
         server   = QString( SFL_ACCOUNT_HOST          );
         password = QString( acc.passwd                );
         user     = QString( acc.user                  );
         enabled  = QString( REGISTRATION_ENABLED_TRUE );

         is_create_account = true;
         is_using_sip      = true;
      }
      else {
         ret += i18n("Creation of account has failed for the reason:\n%1", acc.reason);
      }
   }
   else if(field(FIELD_SIP_ACCOUNT).toBool()) { //sip
      ret += i18n("This assistant is now finished.") + '\n';

      alias    = field   ( FIELD_SIP_ALIAS           ).toString();
      enabled  = QString ( REGISTRATION_ENABLED_TRUE )           ;
      mailbox  = field   ( FIELD_SIP_VOICEMAIL       ).toString();
      protocol = QString ( Account::ProtocolName::SIP    )           ;
      server   = field   ( FIELD_SIP_SERVER          ).toString();
      password = field   ( FIELD_SIP_PASSWORD        ).toString();
      user     = field   ( FIELD_SIP_USER            ).toString();

      is_create_account = true;
      is_using_sip = true;

   }
   else { // iax
      ret += i18n("This assistant is now finished.") + '\n';

      alias    = field   ( FIELD_IAX_ALIAS           ).toString();
      enabled  = QString ( REGISTRATION_ENABLED_TRUE )           ;
      mailbox  = field   ( FIELD_IAX_VOICEMAIL       ).toString();
      protocol = QString ( Account::ProtocolName::IAX    )           ;
      server   = field   ( FIELD_IAX_SERVER          ).toString();
      password = field   ( FIELD_IAX_PASSWORD        ).toString();
      user     = field   ( FIELD_IAX_USER            ).toString();

      is_create_account = true;
      is_using_sip = false;
   }


   // common sip parameters
   if(is_using_sip) {
      if(field(FIELD_SIP_ENABLE_STUN).toBool()) {
         stun_enabled = QString(REGISTRATION_ENABLED_TRUE);
         stun_server  = field(FIELD_SIP_STUN_SERVER).toString();
      }
      else {
         stun_enabled = QString(REGISTRATION_ENABLED_FALSE);
         stun_server.clear();
      }

      if(field(FIELD_ZRTP_ENABLED).toBool()) {
         srtp_enabled          = QString( REGISTRATION_ENABLED_TRUE       );
         key_exchange          = QString( KeyExchangeModel::Name::ZRTP );
         zrtp_display_sas      = QString( REGISTRATION_ENABLED_TRUE       );
         zrtp_not_supp_warning = QString( REGISTRATION_ENABLED_TRUE       );
         zrtp_hello_hash       = QString( REGISTRATION_ENABLED_TRUE       );
         display_sas_once      = QString( REGISTRATION_ENABLED_FALSE      );
      }

      QStringList ifaceList = configurationManager.getAllIpInterface();

//       locale_interface  = ifaceList.at(0);
      published_address = ifaceList.at(0);
      locale_interface = "default";

      ret += i18n( "Alias: %1",                            alias    ) + '\n';
      ret += i18n( "Server: %1",                           server   ) + '\n';
      ret += i18n( "Password: %1",                         password ) + '\n';
      ret += i18n( "Protocol: %1",                         protocol ) + '\n';
      ret += i18n( "Voicemail number: %1",                 mailbox  ) + '\n';
      ret += i18nc( "SIP Account username","Username: %1", user     ) + '\n';
   }

   if(is_create_account) {
      QString accountId = configurationManager.addAccount(accountDetails);
   }
   kDebug() << ret;
   QDialog::accept();
   restart();
}




/***************************************************************************
 *   Class WizardIntroPage                                                 *
 *   Widget of the introduction page of the wizard                         *
 **************************************************************************/

///The first page
WizardIntroPage::WizardIntroPage(QWidget *parent)
     : QWizardPage(parent)
{
   setTitle   (i18n( "Account creation wizard"                             ) );
   setSubTitle(i18n( "Welcome to the Account creation wizard of SFLphone"  ) );

   introLabel = new QLabel(i18n("This installation wizard will help you configure an account."));
   introLabel->setWordWrap(true);

   doNotshowAgain = new QCheckBox(i18n("Always display this dialog when there is no accounts"),this);
   doNotshowAgain->setChecked(ConfigurationSkeleton::displayAccountWizard());
   connect(doNotshowAgain,SIGNAL(checked(bool)),ConfigurationSkeleton::self(),SLOT(setDisplayAccountWizard(bool)));

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(introLabel);
   layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
   layout->addWidget(doNotshowAgain);

   setLayout(layout);
}


WizardIntroPage::~WizardIntroPage()
{
   delete introLabel;
}

int WizardIntroPage::nextId() const
{
   return AccountWizard::Page_Type;
}

/***************************************************************************
 *   Class WizardAccountTypePage                                           *
 *   Page in which user choses between SIP and IAX account.                *
 **************************************************************************/

///The third page
WizardAccountTypePage::WizardAccountTypePage(QWidget *parent)
     : QWizardPage(parent)
{
   setTitle    ( i18n("VoIP Protocols"         ));
   setSubTitle ( i18n("Select an account type" ));

   radioButton_SIP = new QRadioButton(i18n("SIP (Session Initiation Protocol)" ));
   radioButton_IAX = new QRadioButton(i18n("IAX2 (InterAsterix Exchange)"      ));
   radioButton_SIP->setChecked(true);

   registerField( FIELD_SIP_ACCOUNT, radioButton_SIP );
   registerField( FIELD_IAX_ACCOUNT, radioButton_IAX );

   QVBoxLayout *layout = new QVBoxLayout;
   layout->addWidget(radioButton_SIP);
   layout->addWidget(radioButton_IAX);
   setLayout(layout);
}

///Third page destructor
WizardAccountTypePage::~WizardAccountTypePage()
{
   delete radioButton_SIP;
   delete radioButton_IAX;
}

///
int WizardAccountTypePage::nextId() const
{
   if(radioButton_SIP->isChecked())
      return AccountWizard::Page_SIPForm;
   else
      return AccountWizard::Page_IAXForm;
}

/***************************************************************************
 *   Class WizardAccountFormPage                                           *
 *   Page of account settings.                                             *
 **************************************************************************/

WizardAccountSIPFormPage::WizardAccountSIPFormPage(QWidget *parent)
     : QWizardPage(parent)
{
   setTitle(i18n("SIP account settings"));
   setSubTitle(i18n("Please fill the following information"));

   label_alias         = new QLabel( i18n( "Alias"            ) + " *" );
   label_server        = new QLabel( i18n( "Server"           ) + " *" );
   label_password      = new QLabel( i18n( "Password"         ) + " *" );
   label_voicemail     = new QLabel( i18n( "Voicemail number" )        );
   label_enableZrtp    = new QLabel( i18n( "Secure with ZRTP" )        );
   label_user          = new QLabel( i18nc( "SIP Account Username","Username") + " *" );

   lineEdit_alias      = new KLineEdit;
   lineEdit_server     = new KLineEdit;
   lineEdit_user       = new KLineEdit;
   lineEdit_password   = new KLineEdit;
   lineEdit_voicemail  = new KLineEdit;
   checkBox_enableZrtp = new QCheckBox;

   lineEdit_alias->setPlaceholderText    ( i18n("Anything, used only locally")                 );
   lineEdit_server->setPlaceholderText   ( i18n("Example: sip.sflphone.org or 192.168.0.2")    );
   lineEdit_user->setPlaceholderText     ( i18n("Usually your extension number")               );
   lineEdit_password->setPlaceholderText ( i18n("SIP password, ask your network administrator"));
   lineEdit_voicemail->setPlaceholderText( i18n("Usually the same as the username")            );

   lineEdit_password->setEchoMode(KLineEdit::Password);

   registerField(QString( FIELD_SIP_ALIAS)    + '*' , lineEdit_alias      );
   registerField(QString( FIELD_SIP_SERVER)   + '*' , lineEdit_server     );
   registerField(QString( FIELD_SIP_USER)     + '*' , lineEdit_user       );
   registerField(QString( FIELD_SIP_PASSWORD) + '*' , lineEdit_password   );
   registerField(QString( FIELD_SIP_VOICEMAIL)      , lineEdit_voicemail  );
   registerField(QString( FIELD_ZRTP_ENABLED)       , checkBox_enableZrtp );

   QFormLayout *layout = new QFormLayout;


   layout->setWidget( 0, QFormLayout::LabelRole , label_alias         );
   layout->setWidget( 0, QFormLayout::FieldRole , lineEdit_alias      );
   layout->setWidget( 1, QFormLayout::LabelRole , label_server        );
   layout->setWidget( 1, QFormLayout::FieldRole , lineEdit_server     );
   layout->setWidget( 2, QFormLayout::LabelRole , label_user          );
   layout->setWidget( 2, QFormLayout::FieldRole , lineEdit_user       );
   layout->setWidget( 3, QFormLayout::LabelRole , label_password      );
   layout->setWidget( 3, QFormLayout::FieldRole , lineEdit_password   );
   layout->setWidget( 4, QFormLayout::LabelRole , label_voicemail     );
   layout->setWidget( 4, QFormLayout::FieldRole , lineEdit_voicemail  );
   layout->setWidget( 5, QFormLayout::LabelRole , label_enableZrtp    );
   layout->setWidget( 5, QFormLayout::FieldRole , checkBox_enableZrtp );

   setLayout(layout);
}


WizardAccountSIPFormPage::~WizardAccountSIPFormPage()
{
   delete label_alias        ;
   delete label_server       ;
   delete label_user         ;
   delete label_password     ;
   delete label_voicemail    ;
   delete label_enableZrtp   ;
   delete lineEdit_alias     ;
   delete lineEdit_server    ;
   delete lineEdit_user      ;
   delete lineEdit_password  ;
   delete lineEdit_voicemail ;
   delete checkBox_enableZrtp;
}

int WizardAccountSIPFormPage::nextId() const
{
   return AccountWizard::Page_Stun;
}

/***************************************************************************
 *   Class WizardAccountFormPage                                           *
 *   Page of account settings.                                             *
 ***************************************************************************/

WizardAccountIAXFormPage::WizardAccountIAXFormPage(QWidget *parent)
     : QWizardPage(parent)
{
   setTitle(i18n("IAX2 account settings"));
   setSubTitle(i18n("Please fill the following information"));

   label_alias        = new QLabel(i18n("Alias") + " *"     );
   label_server       = new QLabel(i18n("Server") + " *"    );
   label_password     = new QLabel(i18n("Password") + " *"  );
   label_voicemail    = new QLabel(i18n("Voicemail number" ));
   label_user         = new QLabel(i18nc("SIP Account Username","Username") + " *"  );

   lineEdit_alias     = new KLineEdit;
   lineEdit_server    = new KLineEdit;
   lineEdit_user      = new KLineEdit;
   lineEdit_password  = new KLineEdit;
   lineEdit_voicemail = new KLineEdit;

   lineEdit_password->setEchoMode(KLineEdit::Password);

   registerField( QString( FIELD_IAX_ALIAS     ) + '*', lineEdit_alias     );
   registerField( QString( FIELD_IAX_SERVER    ) + '*', lineEdit_server    );
   registerField( QString( FIELD_IAX_USER      ) + '*', lineEdit_user      );
   registerField( QString( FIELD_IAX_PASSWORD  ) + '*', lineEdit_password  );
   registerField( QString( FIELD_IAX_VOICEMAIL )      , lineEdit_voicemail );

   QFormLayout *layout = new QFormLayout;

   layout->setWidget( 0 , QFormLayout::LabelRole , label_alias        );
   layout->setWidget( 0 , QFormLayout::FieldRole , lineEdit_alias     );
   layout->setWidget( 1 , QFormLayout::LabelRole , label_server       );
   layout->setWidget( 1 , QFormLayout::FieldRole , lineEdit_server    );
   layout->setWidget( 2 , QFormLayout::LabelRole , label_user         );
   layout->setWidget( 2 , QFormLayout::FieldRole , lineEdit_user      );
   layout->setWidget( 3 , QFormLayout::LabelRole , label_password     );
   layout->setWidget( 3 , QFormLayout::FieldRole , lineEdit_password  );
   layout->setWidget( 4 , QFormLayout::LabelRole , label_voicemail    );
   layout->setWidget( 4 , QFormLayout::FieldRole , lineEdit_voicemail );

   setLayout(layout);
}


WizardAccountIAXFormPage::~WizardAccountIAXFormPage()
{
   delete label_alias       ;
   delete label_server      ;
   delete label_user        ;
   delete label_password    ;
   delete label_voicemail   ;
   delete lineEdit_alias    ;
   delete lineEdit_server   ;
   delete lineEdit_user     ;
   delete lineEdit_password ;
   delete lineEdit_voicemail;
}

int WizardAccountIAXFormPage::nextId() const
{
   return AccountWizard::Page_Conclusion;
}

/***************************************************************************
 *   Class WizardAccountStunPage                                           *
 *   Page of Stun settings.                                                *
 ***************************************************************************/

WizardAccountStunPage::WizardAccountStunPage(QWidget *parent)
     : QWizardPage(parent)
{
   setTitle(   i18n( "Network Address Translation (NAT)"                            ) );
   setSubTitle(i18n( "You should probably enable this if you are behind a firewall.") );

   checkBox_enableStun = new QCheckBox( i18n("Enable STUN") );
   label_StunServer    = new QLabel   ( i18n("STUN Server") );
   lineEdit_StunServer = new KLineEdit(                     );
   lineEdit_StunServer->setDisabled(true);

   registerField(FIELD_SIP_ENABLE_STUN, checkBox_enableStun);
   registerField(FIELD_SIP_STUN_SERVER, lineEdit_StunServer);

   QFormLayout *layout = new QFormLayout;
   layout->addWidget( checkBox_enableStun );
   layout->addWidget( label_StunServer    );
   layout->addWidget( lineEdit_StunServer );
   setLayout(layout);

   connect(checkBox_enableStun, SIGNAL(toggled(bool)), lineEdit_StunServer, SLOT(setEnabled(bool)));
}


WizardAccountStunPage::~WizardAccountStunPage()
{
   delete checkBox_enableStun;
   delete label_StunServer   ;
   delete lineEdit_StunServer;
}

int WizardAccountStunPage::nextId() const
{
   return AccountWizard::Page_Conclusion;
}

/***************************************************************************
 *   Class WizardAccountConclusionPage                                     *
 *   Conclusion page.                                                      *
 ***************************************************************************/

WizardAccountConclusionPage::WizardAccountConclusionPage(QWidget *parent)
     : QWizardPage(parent)
{
   setTitle(   i18n( "This assistant is now finished."));
   setSubTitle(i18n( "After checking the settings you chose, click \"Finish\" to create the account. This may take a while to complete."));

   QVBoxLayout *layout = new QVBoxLayout;
   setLayout(layout);
}

WizardAccountConclusionPage::~WizardAccountConclusionPage()
{
   
}

int WizardAccountConclusionPage::nextId() const
{
   return -1;
}
