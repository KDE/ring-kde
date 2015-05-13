/***************************************************************************
 *   Copyright (C) 2015 by Savoir-Faire Linux                              *
 *   Author : Emmanuel Lepage Vallee <emmanuel.lepage@savoirfairelinux.com>*
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
#include "audiorecordingconfigurator.h"
#include "ui_dlgaudiorecording.h"

//KDE
#include <KLineEdit>

//Ring
#include <media/recordingmodel.h>

AudioRecordingConfigurator::AudioRecordingConfigurator(QObject* parent) : CollectionConfigurationInterface(parent),m_pDialog(nullptr),m_HasChanged(false)
{

}

QByteArray AudioRecordingConfigurator::id() const
{
   return "audioRecordingConfigurator";
}

QString AudioRecordingConfigurator::name() const
{
   return tr("Audio recording configurator");
}

QVariant AudioRecordingConfigurator::icon() const
{
   return QVariant();
}

void AudioRecordingConfigurator::loadCollection(CollectionInterface* col, QObject* parent)
{
   Q_UNUSED(col)
   if (parent && qobject_cast<QWidget*>(parent)) {
      QWidget* w = qobject_cast<QWidget*>(parent);
      if (!m_pDialog) {
         m_pDialog = new QWidget();
         ui = new Ui_DlgAudioRecording();
         ui->setupUi(m_pDialog);
         QHBoxLayout* l = new QHBoxLayout(w);
         l->addWidget(m_pDialog);

         ui->m_pAlwaysRecordCK->setChecked(Media::RecordingModel::instance()->isAlwaysRecording());

         ui->KUrlRequester_destinationFolder->setMode(KFile::Directory|KFile::ExistingOnly|KFile::LocalOnly);
         ui->KUrlRequester_destinationFolder->setUrl(QUrl(Media::RecordingModel::instance()->recordPath()));
         ui->KUrlRequester_destinationFolder->lineEdit()->setReadOnly(true);

         connect( ui->KUrlRequester_destinationFolder , SIGNAL(textChanged(QString)) , this , SLOT(slotChanged()));
         connect( ui->m_pAlwaysRecordCK               , SIGNAL(clicked(bool))        , this , SLOT(slotChanged()));
      }
   }
}

void AudioRecordingConfigurator::save()
{
   Media::RecordingModel::instance()->setRecordPath     ( ui->KUrlRequester_destinationFolder->lineEdit()->text() );
   Media::RecordingModel::instance()->setAlwaysRecording( ui->m_pAlwaysRecordCK->isChecked()                      );
   m_HasChanged = false;
}

bool AudioRecordingConfigurator::hasChanged()
{
   return m_HasChanged;
}

void AudioRecordingConfigurator::slotChanged()
{
   m_HasChanged = true;
   emit changed();
}
