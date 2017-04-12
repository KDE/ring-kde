#pragma once

#include <QtCore/QObject>

class WelcomeDialog : public QObject
{
    Q_OBJECT
public:
    explicit WelcomeDialog(QObject* parent = nullptr);
    virtual ~WelcomeDialog();

    void show();
};
