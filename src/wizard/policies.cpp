/************************************************************************************
 *   Copyright (C) 2019 by BlueSystems GmbH                                         *
 *   Author : Emmanuel Lepage Vallee <elv1313@gmail.com>                            *
 *                                                                                  *
 *   This library is free software; you can redistribute it and/or                  *
 *   modify it under the terms of the GNU Lesser General Public                     *
 *   License as published by the Free Software Foundation; either                   *
 *   version 2.1 of the License, or (at your option) any later version.             *
 *                                                                                  *
 *   This library is distributed in the hope that it will be useful,                *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU              *
 *   Lesser General Public License for more details.                                *
 *                                                                                  *
 *   You should have received a copy of the GNU Lesser General Public               *
 *   License along with this library; if not, write to the Free Software            *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
 ***********************************************************************************/
#include "policies.h"

// LibRingQt
#include <session.h>
#include <accountmodel.h>

// JamiKDEIntegration
#include "../jamikdeintegration/kcfg_settings.h" //FIXME export this
#include "../jamikdeintegration/src/windowevent.h" //FIXME export this

// If there's more than 1 instance, this is worth fixing.
static enum {
    UNDECIDED,
    DISCARDED,
    YES,
} s_DisplayWizard;

WizardPolicies::WizardPolicies(QObject* parent) : QObject(parent)
{}

WizardPolicies::~WizardPolicies()
{

}

bool WizardPolicies::displayWizard() const
{
    static bool dw  = ConfigurationSkeleton::enableWizard()
        || ConfigurationSkeleton::showSplash();

    const  bool dos = ConfigurationSkeleton::displayOnStart()
        && !WindowEvent::instance()->startIconified();

        // The first run wizard
    if (dos && dw) {

        if (ConfigurationSkeleton::enableWizard())
            WindowEvent::instance()->showWizard();

        if (!Session::instance()->accountModel()->size())
            WindowEvent::instance()->showWizard();

        ConfigurationSkeleton::setEnableWizard(false);

        s_DisplayWizard = YES;
    }

    return s_DisplayWizard == YES;
}

void WizardPolicies::setWizardFinished(bool f)
{
    s_DisplayWizard = DISCARDED;
}
