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

#include <QObject>
#include <QVector>
#include <QWidget>
#include <QModelIndex>
#include <QMap>
#include "typedefs.h"

//Qt
class QModelIndex;

//SFLPhone
class Call;
//class AccountList;
class Account;

//Typedef
typedef QMap<QString, Call*>  CallMap;
typedef QList<Call*>          CallList;

///CallModelBase: Base class for the central model/frontend
///This class need to exist because template classes can't have signals and
///slots because Qt MOC generator can't guess the type at precompilation   
class LIB_EXPORT CallModelBase : public QAbstractItemModel
{
   Q_OBJECT
public:
   explicit CallModelBase(QObject* parent = nullptr);
   ~CallModelBase();
   virtual Call* addCall              ( Call* call           , Call* parent =0      );
   virtual Call* getCall              ( const QString& callId                       ) const = 0;
   Call*   addConferenceS             ( Call* conf                                  );

   bool isValid();
   
private Q_SLOTS:
   void callStateChanged      ( const QString& callID    , const QString &state   );
   void incomingCall          ( const QString& accountID , const QString & callID );
   void incomingConference    ( const QString& confID                             );
   void changingConference    ( const QString& confID    , const QString &state   );
   void conferenceRemovedSlot ( const QString& confId                             );
   void voiceMailNotifySlot   ( const QString& accountID , int count              );
   void volumeChangedSlot     ( const QString& device    , double value           );
   void removeActiveCall      ( Call* call                                        );
   void addPrivateCall        ( Call* call                                        );
   void newRecordingAvail     ( const QString& callId    , const QString& filePath);
   #ifdef ENABLE_VIDEO
   void startedDecoding       ( const QString& callId    , const QString& shmKey  );
   void stoppedDecoding       ( const QString& callId    , const QString& shmKey  );
   #endif

protected:
   virtual Call* findCallByCallId ( const QString& callId                       ) = 0;
   virtual bool changeConference  ( const QString& confId, const QString &state ) = 0;
   virtual void removeConference  ( const QString& confId                       ) = 0;
   virtual Call* addConference    ( const QString& confID                       ) = 0;
   virtual Call* addRingingCall   ( const QString& callId                       ) = 0;
   virtual Call* addIncomingCall  ( const QString& callId                       ) = 0;

   //Attributes
   static CallMap m_sActiveCalls;
   
   //Struct
   struct InternalStruct;
   typedef QList<InternalStruct*> InternalCallList;
   ///InternalStruct: internal representation of a call
   struct InternalStruct {
      InternalStruct() : m_pParent(nullptr){}
      Call*            call_real  ;
      QModelIndex            index      ;
      InternalCallList m_lChildren   ;
      bool             conference ;
      InternalStruct* m_pParent;
   };
   typedef QHash< Call*      , InternalStruct* > InternalCall  ;
   typedef QHash< QString    , InternalStruct* > InternalCallId;
   typedef QHash< QModelIndex      , InternalStruct* > InternalIndex ;
   
   InternalCallList m_lInternalModel;

   //Static attributes
   static InternalCall   m_sPrivateCallList_call  ;
   static InternalCallId m_sPrivateCallList_callId;
   static InternalIndex  m_sPrivateCallList_index ;

   static CallMap        m_lConfList;
   static bool           m_sCallInit;

private:
   static bool dbusInit;

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

/**
 * Using QAbstractModel resulted in a failure. Managing all corner case bloated the code to the point of no
 * return. This frontend may not be cleaner from a design point of view, but it is from a code point of view
 */
///CallModel: Central model/frontend to deal with sflphoned
class LIB_EXPORT CallModel : public CallModelBase {
   Q_OBJECT
   public:

      //Constructors, initializer and destructors
      CallModel                (                    );
      virtual ~CallModel       (                    );
      virtual bool initCall    (                    );
      static  void destroy     (                    );

      //Call related
      virtual Call*  addCall          ( Call* call                , Call* parent =0          );
      Call*          addDialingCall   ( const QString& peerName="", Account* account=nullptr );
      static QString generateCallId   (                                                      );
      void           removeCall       ( Call* call                                           );
      void           attendedTransfer ( Call* toTransfer          , Call* target             );
      void           transfer         ( Call* toTransfer          , QString target           );

      //Conference related
      bool createConferenceFromCall  ( Call* call1, Call* call2                    );
      bool mergeConferences          ( Call* conf1, Call* conf2                    );
      bool addParticipant            ( Call* call2, Call* conference               );
      bool detachParticipant         ( Call* call                                  );
      void removeConference          ( Call* conf                                  );

      //Getters
      int size                                        ();
      CallList                 getCallList            ();
      CallList                 getConferenceList      ();
      Call*                    getCurrentCall         (){return nullptr;}

      //Setters
      void                     setCurrentCall         (Call* current){}
      void                     setCurrentCall         (const QModelIndex& current){}

      //Model implementation
      virtual bool          setData     ( const QModelIndex& index, const QVariant &value, int role   );
      virtual QVariant      data        ( const QModelIndex& index, int role = Qt::DisplayRole        ) const;
      virtual int           rowCount    ( const QModelIndex& parent = QModelIndex()                   ) const;
      virtual Qt::ItemFlags flags       ( const QModelIndex& index                                    ) const;
      virtual int           columnCount ( const QModelIndex& parent = QModelIndex()                   ) const;
      virtual QModelIndex   parent      ( const QModelIndex& index                                    ) const;
      virtual QModelIndex   index       ( int row, int column, const QModelIndex& parent=QModelIndex()) const;
      virtual QVariant      headerData  ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

      //Connection related
      static bool init();
      
      //Singleton
      static CallModel* instance() {
         return m_spInstance;
      }

      //Magic dispatcher
      CallList getCalls     (                         );
      CallList getCalls     ( const QString& callId   ) const;
      CallList getCalls     ( const Call* call        ) const;
      CallList getCalls     ( const QModelIndex& idx  ) const;

      bool isConference     ( const Call* call        ) const;
      bool isConference     ( const QString& callId   ) const;
      bool isConference     ( const QModelIndex& idx  ) const;

      Call* getCall         ( const QString& callId   ) const;
      Call* getCall         ( const QModelIndex& idx  ) const;
      Call* getCall         ( const Call* call        ) const;

      QModelIndex getIndex  ( const Call* call        ) const;
      QModelIndex getIndex  ( const QModelIndex& idx  ) const;
      QModelIndex getIndex  ( const QString& callId   ) const;

      bool updateIndex      ( Call* call, const QModelIndex& value      );

   protected:
      virtual Call* findCallByCallId ( const QString& callId                       );
      virtual Call* addConference    ( const QString& confID                       );
      virtual bool  changeConference ( const QString& confId, const QString& state );
      virtual void  removeConference ( const QString& confId                       );
      Call*         addIncomingCall  ( const QString& callId                       );
      Call*         addRingingCall   ( const QString& callId                       );

   private:
      static bool m_sInstanceInit;
      //Singleton
      static CallModel* m_spInstance;

      //Helpers
      Call* addCallCommon(Call* call);
      bool  updateCommon (Call* call);
};

#endif
