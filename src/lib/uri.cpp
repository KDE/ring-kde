/****************************************************************************
 *   Copyright (C) 2014-2015 by Savoir-Faire Linux                          *
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
#include "uri.h"

constexpr const char* URI::schemeNames[];

class URIPrivate
{
public:
   URIPrivate(QString* uri);
   //Attributes
   QString          m_Hostname    ;
   QString          m_Userinfo    ;
   QStringList      m_lAttributes ;
   QString          m_Stripped    ;
   URI::SchemeType  m_HeaderType  ;
   bool             m_hasChevrons ;
   bool             m_Parsed      ;

   //Helper
   static QString strip(const QString& uri, URI::SchemeType& sheme);
   void parse();
private:
   QString* q_ptr;
};

URIPrivate::URIPrivate(QString* uri) : m_Parsed(false),m_HeaderType(URI::SchemeType::NONE),q_ptr(uri)
{
}

URI::URI(const QString& other):QString(), d_ptr(new URIPrivate(this))
{
   d_ptr->m_Stripped              = URIPrivate::strip(other,d_ptr->m_HeaderType);
   (*static_cast<QString*>(this)) = d_ptr->m_Stripped                           ;
}

URI::URI(const URI& o):QString(), d_ptr(new URIPrivate(this))
{
   d_ptr->m_Parsed     = o.d_ptr->m_Parsed    ;
   d_ptr->m_Hostname   = o.d_ptr->m_Hostname  ;
   d_ptr->m_HeaderType = o.d_ptr->m_HeaderType;
   d_ptr->m_Userinfo   = o.d_ptr->m_Userinfo  ;
   d_ptr->m_Stripped   = o.d_ptr->m_Stripped  ;

   (*static_cast<QString*>(this)) = o.d_ptr->m_Stripped;
}

URI::~URI()
{
   (*static_cast<QString*>(this)) = QString();
   d_ptr->m_Stripped = QString();
   delete d_ptr;
}

URI& URI::operator=(const URI& o)
{
   d_ptr->m_Parsed     = o.d_ptr->m_Parsed    ;
   d_ptr->m_Hostname   = o.d_ptr->m_Hostname  ;
   d_ptr->m_HeaderType = o.d_ptr->m_HeaderType;
   d_ptr->m_Userinfo   = o.d_ptr->m_Userinfo  ;
   d_ptr->m_Stripped   = o.d_ptr->m_Stripped  ;

   (*static_cast<QString*>(this)) = o.d_ptr->m_Stripped;
   return (*this);
}

///Strip out <sip:****> from the URI
QString URIPrivate::strip(const QString& uri, URI::SchemeType& sheme)
{
   if (uri.isEmpty())
      return QString();
   int start(0),end(uri.size()-1); //Other type of comparisons were too slow
   if (end > 5 && uri[0] == '<' ) {
      if (uri[4] == ':') {
         sheme = uri[1] == 's'?URI::SchemeType::SIP : URI::SchemeType::IAX;
         start = 5;
      }
      else if (uri[5] == ':') {
         sheme = URI::SchemeType::SIPS;
         start = 6;
      }
   }
   if (end && uri[end] == '>')
      end--;
   else if (start) {
      //TODO there may be a ';' section with arguments, check
   }
   return uri.mid(start,end-start+1);
}

///Return the domaine of an URI (<sip:12345@example.com>)
QString URI::hostname() const
{
   if (!d_ptr->m_Parsed)
      const_cast<URI*>(this)->d_ptr->parse();
   return d_ptr->m_Hostname;
}

bool URI::hasHostname() const
{
   if (!d_ptr->m_Parsed)
      const_cast<URI*>(this)->d_ptr->parse();
   return !d_ptr->m_Hostname.isEmpty();
}

///Keep a cache of the values to avoid re-parsing them
void URIPrivate::parse()
{
   if (q_ptr->indexOf('@') != -1) {
      const QStringList splitted = q_ptr->split('@');
      m_Hostname = splitted[1];//splitted[1].left(splitted[1].size())
      m_Userinfo = splitted[0];
      m_Parsed = true;
   }
}

QString URI::userinfo() const
{
   if (!d_ptr->m_Parsed)
      const_cast<URI*>(this)->d_ptr->parse();
   return d_ptr->m_Userinfo;
}

/**
 * Some feature, like SIP presence, require a properly formatted USI
 */
QString URI::fullUri() const
{
   return QString("<%1%2>")
      .arg(schemeNames[static_cast<int>(d_ptr->m_HeaderType == SchemeType::NONE?SchemeType::SIP:d_ptr->m_HeaderType)])
      .arg(*this);
}