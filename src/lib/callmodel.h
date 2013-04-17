/****************************************************************************
 *   Copyright (C) 2009-2013 by Savoir-Faire Linux                          *
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

#ifndef CALL_MODEL2_H
#define CALL_MODEL2_H

#include <QAbstractItemModel>
#include <QMap>
#include "typedefs.h"

//SFLPhone
class Call;

//class AccountList;
class Account;

//Typedef
typedef QMap<QString, Call*>  CallMap;
typedef QList<Call*>          CallList;
struct InternalStruct;

///CallModelBase: Base class for the central model/frontend
///This class need to exist because template classes can't have signals and
///slots because Qt MOC generator can't guess the type at precompilation   
class LIB_EXPORT CallModelBase : public QAbstractItemModel
{
   Q_OBJECT
public:
   ~CallModelBase();
   virtual Call* getCall              ( const QString& callId                       ) const = 0;

   bool isValid();
   
private Q_SLOTS:
   void callStateChanged      ( const QString& callID    , const QString &state   );
   void incomingCall          ( const QString& accountID , const QString & callID );
   void incomingConference    ( const QString& confID                             );
   void changingConference    ( const QString& confID    , const QString &state   );
   void conferenceRemovedSlot ( const QString& confId                             );
   void voiceMailNotifySlot   ( const QString& accountID , int count              );
   void volumeChangedSlot     ( const QString& device    , double value           );
   void addPrivateCall        ( Call* call                                        );
   void newRecordingAvail     ( const QString& callId    , const QString& filePath);
   void callChanged           ( Call* call                                        );
   #ifdef ENABLE_VIDEO
   void startedDecoding       ( const QString& callId    , const QString& shmKey  );
   void stoppedDecoding       ( const QString& callId    , const QString& shmKey  );
   #endif

protected:
   explicit CallModelBase(QObject* parent = nullptr);
   virtual Call* addCall          ( Call* call    , Call* parent = nullptr      );
   virtual bool changeConference  ( const QString& confId, const QString &state ) = 0;
   virtual void removeConference  ( const QString& confId                       ) = 0;
   virtual Call* addConference    ( const QString& confID                       ) = 0;
   virtual Call* addRingingCall   ( const QString& callId                       ) = 0;
   virtual Call* addIncomingCall  ( const QString& callId                       ) = 0;
   void          removeCall       ( Call* call                                 );

   //Struct
   QList<InternalStruct*> m_lInternalModel;
   QHash< Call*       , InternalStruct* > m_sPrivateCallList_call   ;
   QHash< QString     , InternalStruct* > m_sPrivateCallList_callId ;

Q_SIGNALS:
   ///Emitted when a call state change
   void callStateChanged        ( Call* call                              );
   ///Emitted when a new call is incoming
   void incomingCall            ( Call* call                              );
   ///Emitted when a conference is created
   void conferenceCreated       ( Call* conf                              );
   ///Emitted when a conference change state or participant
   void conferenceChanged       ( Call* conf                              );
   ///Emitted when a conference is removed
   void conferenceRemoved       ( Call* conf                              );
   ///Emitted just before a conference is removed
   void aboutToRemoveConference ( Call* conf                              );
   ///Emitted when a new voice mail is available
   void voiceMailNotify         ( const QString& accountID , int    count );
   ///Emitted when the volume change
   void volumeChanged           ( const QString& device    , double value );
   ///Emitted when a call is added
   void callAdded               ( Call* call               , Call* parent );
   ///Emitted when an account state change
   //void accountStateChanged     ( Account* account, QString state         );
};

///CallModel: Central model/frontend to deal with sflphoned
class LIB_EXPORT CallModel : public CallModelBase {
   Q_OBJECT
   public:
      //Constructors, initializer and destructors
      virtual ~CallModel( );

      //Call related
      Call* addDialingCall   ( const QString& peerName=QString(), Account* account=nullptr );
      void  attendedTransfer ( Call* toTransfer          , Call* target             );
      void  transfer         ( Call* toTransfer          , QString target           );

      //Conference related
      bool createConferenceFromCall  ( Call* call1, Call* call2      );
      bool mergeConferences          ( Call* conf1, Call* conf2      );
      bool addParticipant            ( Call* call2, Call* conference );
      bool detachParticipant         ( Call* call                    );
      void removeConference          ( Call* conf                    );

      //Getters
      int size                   ();
      CallList getCallList       ();
      CallList getConferenceList ();

      //Model implementation
      virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );
      virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
      virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
      virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
      virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
      virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
      virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
      virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

      //Singleton
      static CallModel* instance() {
         if (!m_spInstance)
            m_spInstance = new CallModel();
         return m_spInstance;
      }

      Call* getCall ( const QString& callId  ) const;
      Call* getCall ( const QModelIndex& idx ) const;

   protected:
      virtual Call*  addCall          ( Call* call                , Call* parent = nullptr );
      virtual Call*  addConference    ( const QString& confID                              );
      virtual bool   changeConference ( const QString& confId, const QString& state        );
      virtual void   removeConference ( const QString& confId                              );
      Call*          addIncomingCall  ( const QString& callId                              );
      Call*          addRingingCall   ( const QString& callId                              );

   private:
      explicit CallModel();

      //Singleton
      static CallModel* m_spInstance;
};

#endif
