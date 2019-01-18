/***************************************************************************
 *   Copyright (C) 2013-2015 by Savoir-Faire Linux                         *
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
#pragma once

#include <interfaces/actionextenderi.h>

class ActionInterface final : public Interfaces::ActionExtenderI
{
public:
    explicit ActionInterface() = default;
    ActionInterface(const ActionInterface&) = delete;
    ActionInterface& operator=(const ActionInterface&) = delete;

    virtual void editPerson(Person* p) override;
    virtual void viewChatHistory(ContactMethod* cm) override;
    virtual void viewChatHistory(Person* p) override;
    virtual void copyInformation(QMimeData* data) override;
    virtual bool warnDeletePerson(Person* p) override;
    virtual bool warnDeleteCall(Call* c) override;
    virtual Person* selectPerson(FlagPack<SelectPersonHint> hints, const QVariant& hintVar) const override;
    virtual ContactMethod* selectContactMethod(FlagPack<ActionExtenderI::SelectContactMethodHint>, const QVariant& hintVar) const override;
};
