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

#include "integrationplugingenericlights.h"
#include "plugininfo.h"

#include <QDebug>
#include <QtMath>

IntegrationPluginGenericLights::IntegrationPluginGenericLights()
{

}

void IntegrationPluginGenericLights::setupThing(ThingSetupInfo *info)
{
    info->finish(Thing::ThingErrorNoError);
}

void IntegrationPluginGenericLights::executeAction(ThingActionInfo *info)
{
    Thing *thing = info->thing();
    Action action = info->action();

    if (thing->thingClassId() == lightThingClassId) {
        if (action.actionTypeId() == lightPowerActionTypeId) {
            thing->setStateValue(lightPowerStateTypeId, action.param(lightPowerActionPowerParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }
    } else if (thing->thingClassId() == dimmableLightThingClassId) {
        if (action.actionTypeId() == dimmableLightPowerActionTypeId) {
            thing->setStateValue(dimmableLightPowerStateTypeId, action.param(dimmableLightPowerActionPowerParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == dimmableLightBrightnessActionTypeId) {
            thing->setStateValue(dimmableLightBrightnessStateTypeId, action.param(dimmableLightBrightnessActionBrightnessParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else if (thing->thingClassId() == colorTemperatureLightThingClassId) {
        if (action.actionTypeId() == colorTemperatureLightPowerActionTypeId) {
            thing->setStateValue(colorTemperatureLightPowerStateTypeId, action.param(colorTemperatureLightPowerActionPowerParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == colorTemperatureLightBrightnessActionTypeId) {
            thing->setStateValue(colorTemperatureLightBrightnessStateTypeId, action.param(colorTemperatureLightBrightnessActionBrightnessParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == colorTemperatureLightColorTemperatureActionTypeId) {
            thing->setStateValue(colorTemperatureLightColorTemperatureStateTypeId, action.param(colorTemperatureLightColorTemperatureActionColorTemperatureParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else if (thing->thingClassId() == colorLightThingClassId) {
        if (action.actionTypeId() == colorLightPowerActionTypeId) {
            thing->setStateValue(colorLightPowerStateTypeId, action.param(colorLightPowerActionPowerParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == colorLightBrightnessActionTypeId) {
            thing->setStateValue(colorLightBrightnessStateTypeId, action.param(colorLightBrightnessActionBrightnessParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == colorLightColorTemperatureActionTypeId) {
            thing->setStateValue(colorLightColorTemperatureStateTypeId, action.param(colorLightColorTemperatureActionColorTemperatureParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else if (action.actionTypeId() == colorLightColorActionTypeId) {
            thing->setStateValue(colorLightColorStateTypeId, action.param(colorLightColorActionColorParamTypeId).value());
            return info->finish(Thing::ThingErrorNoError);
        } else {
            Q_ASSERT_X(false, "executeAction", QString("Unhandled actionTypeId: %1").arg(action.actionTypeId().toString()).toUtf8());
        }

    } else {
        Q_ASSERT_X(false, "executeAction", QString("Unhandled thingClassId: %1").arg(thing->thingClassId().toString()).toUtf8());
    }
}
