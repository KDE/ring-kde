#pragma once

#include <QtCore/QObject>

class WelcomeDialog : public QObject
{
   Q_OBJECT
public:
   Q_PROPERTY(QString defaultUserName READ defaultUserName)

   explicit WelcomeDialog(QObject* parent = nullptr);
   virtual ~WelcomeDialog();

   QString defaultUserName() const;
};

Q_DECLARE_METATYPE(WelcomeDialog*)

// kate: space-indent on; indent-width 3; replace-tabs on;
