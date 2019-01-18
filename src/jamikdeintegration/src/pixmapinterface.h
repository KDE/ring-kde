/****************************************************************************
 *   Copyright (C) 2013-2014 by Savoir-Faire Linux                          *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                    *
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
#ifndef KDEPIXMAPMANIPULATION_H
#define KDEPIXMAPMANIPULATION_H

#include <interfaces/pixmapmanipulatori.h>
#include <call.h>
// #include "matrixutils.h"

class Person;


class PixmapInterface final : public Interfaces::PixmapManipulatorI
{
public:
    explicit PixmapInterface();
    QVariant contactPhoto(Person* c, const QSize& size, bool displayPresence = true) override;

    virtual QVariant callPhoto(const ContactMethod* n, const QSize& size, bool displayPresence = true) override;

    virtual QVariant callPhoto(Call* c, const QSize& size, bool displayPresence = true) override;

    virtual QByteArray toByteArray(const QVariant& pxm, const QString& type = QStringLiteral("PNG")) override;

    virtual QVariant personPhoto(const QByteArray& data, const QString& type = QStringLiteral("PNG")) override;

    QVariant numberCategoryIcon(const QVariant& p, const QSize& size, bool displayPresence = false, bool isPresent = false) override;

    virtual QVariant securityIssueIcon(const QModelIndex& index) override;

    virtual QVariant userActionIcon(const UserActionElement& state) const override;

    virtual QVariant collectionIcon(const CollectionInterface* interface, Interfaces::PixmapManipulatorI::CollectionIconHint hint = Interfaces::PixmapManipulatorI::CollectionIconHint::NONE) const override;

    virtual QVariant securityLevelIcon(const SecurityEvaluationModel::SecurityLevel level) const override;

    virtual QVariant historySortingCategoryIcon(const CallHistoryModel::SortedProxy::Categories cat) const override;

    virtual QVariant contactSortingCategoryIcon(const ContactModel::SortedProxy::Categories cat) const override;

    virtual QVariant videoDeviceIcon(const QModelIndex& idx) const override;

    virtual QString  takeSnapshot(Call* call) override;

    virtual QVariant decorationRole(const QModelIndex& index) override;

    virtual QVariant decorationRole(const Call*              c    ) override;

    virtual QVariant decorationRole(const ContactMethod*     cm   ) override;

    virtual QVariant decorationRole(const Person*            p    ) override;

    virtual QVariant decorationRole(const Account*           a    ) override;

    static const char* icnPath[2][2];
};

#endif // KDEPIXMAPMANIPULATION_H

// kate: space-indent on; indent-width 4; replace-tabs on;
