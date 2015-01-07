/****************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                          *
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
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore/QAbstractListModel>

//SFLPhone
#include "../typedefs.h"


namespace Audio {

class AlsaPluginModel    ;
class InputDeviceModel   ;
class OutputDeviceModel  ;
class ManagerModel       ;
class RingtoneDeviceModel;

class SettingsPrivate;

/**
 * This class group all ComboBox models used by audio settings dialogs
 */
class LIB_EXPORT Settings : public QObject {
   Q_OBJECT
public:

   enum class DeviceIndex {
      OUTPUT   = 0,
      INPUT    = 1,
      RINGTONE = 2,
   };

   virtual ~Settings();
   static Settings* instance();

   //Getters
   Audio::AlsaPluginModel*     alsaPluginModel    () const;
   Audio::InputDeviceModel*    inputDeviceModel   () const;
   Audio::OutputDeviceModel*   outputDeviceModel  () const;
   Audio::ManagerModel*        managerModel       () const;
   Audio::RingtoneDeviceModel* ringtoneDeviceModel() const;
   bool                 isRoomToneEnabled  ();
   bool                 isNoiseSuppressEnabled () const;
   bool                 isPlaybackMuted  () const;
   bool                 isCaptureMuted   () const;
   bool                 isAlwaysRecording() const;
   bool                 areDTMFMuted     () const;
   int                  playbackVolume   () const;
   int                  captureVolume    () const;
   QUrl                 recordPath       () const;

   //Setters
   void setEnableRoomTone    ( bool        enable  );
   void setNoiseSuppressState( bool        enabled );
   void setRecordPath        ( const QUrl& path    );
   void setAlwaysRecording   ( bool        record  );

   //Room tone type
   enum class ToneType {
      WITHOUT_MESSAGE = 0,
      WITH_MESSAGE    = 1,
   };

   class DeviceKey {
   public:
      constexpr static const char* CAPTURE  = "mic"    ;
      constexpr static const char* PLAYBACK = "speaker";
   };

   //Mutator
   ToneType playRoomTone() const;
   void     stopRoomTone() const;

public Q_SLOTS:
   void reload           (              );
   void mutePlayback     ( bool m       );
   void muteCapture      ( bool m       );
   void setPlaybackVolume( int  volume  );
   void setCaptureVolume ( int  volume  );
   void setDTMFMuted     ( bool muted   );

Q_SIGNALS:
   void captureMuted(bool);
   void playbackMuted(bool);
   void playbackVolumeChanged(int);
   void captureVolumeChanged(int);
   void DTMFMutedChanged(bool);

private:
   //Constructor
   explicit Settings();
   QScopedPointer<SettingsPrivate> d_ptr;
   Q_DECLARE_PRIVATE(Settings)

   //Singleton
   static Settings* m_spInstance;
};

}

#endif