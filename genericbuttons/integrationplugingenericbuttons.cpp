// SPDX-License-Identifier: GPL-3.0-or-later

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*
* Copyright (C) 2013 - 2024, nymea GmbH
* Copyright (C) 2024 - 2025, chargebyte austria GmbH
*
* This file is part of nymea-plugins-genericthings.
*
* nymea-plugins-genericthings is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* nymea-plugins-genericthings is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with nymea-plugins-genericthings. If not, see <https://www.gnu.org/licenses/>.
*
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "integrationplugingenericbuttons.h"
#include "plugininfo.h"

#include <QDebug>

IntegrationPluginGenericButtons::IntegrationPluginGenericButtons()
{
}

void IntegrationPluginGenericButtons::setupThing(ThingSetupInfo *info)
{
    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericButtons::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == buttonThingClassId) {
        if (action.actionTypeId() == buttonPressActionTypeId) {
            thing->emitEvent(buttonPressedEventTypeId);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    if (thing->thingClassId() == virtualButtonThingClassId) {
        if (action.actionTypeId() == virtualButtonPressActionTypeId) {
            thing->emitEvent(virtualButtonPressedEventTypeId);
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    if (thing->thingClassId() == powerSwitchThingClassId) {
        if (action.actionTypeId() == powerSwitchPowerActionTypeId) {
            thing->setStateValue(powerSwitchPowerStateTypeId, action.param(powerSwitchPowerActionPowerParamTypeId).value());
            thing->emitEvent(powerSwitchPressedEventTypeId, {
                                 Param(powerSwitchPowerActionPowerParamTypeId,
                                 action.paramValue(powerSwitchPowerActionPowerParamTypeId).toBool())
                             });
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    if (thing->thingClassId() == virtualPowerSwitchThingClassId) {
        if (action.actionTypeId() == virtualPowerSwitchPowerActionTypeId) {
            thing->setStateValue(virtualPowerSwitchPowerStateTypeId, action.param(virtualPowerSwitchPowerActionPowerParamTypeId).value());
            thing->emitEvent(virtualPowerSwitchPressedEventTypeId, {
                                 Param(virtualPowerSwitchPowerActionPowerParamTypeId,
                                 action.paramValue(virtualPowerSwitchPowerActionPowerParamTypeId).toBool())
                             });
            info->finish(Thing::ThingErrorNoError);
            return;
        }
    }

    Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
}
